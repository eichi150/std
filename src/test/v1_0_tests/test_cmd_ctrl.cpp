// test_cmd_ctrl.cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../../code/bme280/sensor.h"
#include "../../code/control/ctrl.h"
#include "../../code/control/cmd_ctrl.h"

// Dummy-Logger für Tests
class FakeLogger : public ErrorLogger {
public:
    std::vector<std::string> logs;
    bool debug_enabled = false;

    void log(const std::string& msg) override {
        logs.push_back(msg);
    }

    void set_debug_to(bool enabled) {
        debug_enabled = enabled;
    }
    
    std::string get_logs() const override{
		return "dummy log";
	};
};


// Abgeleitete Klasse, um Zugriff auf geschätzte Member zu bekommen
class TestableCmdCtrl : public Cmd_Ctrl {
public:
    using Cmd_Ctrl::regex_pattern; // Zugriff ermöglichen

    TestableCmdCtrl(std::shared_ptr<ErrorLogger> logger)
        : Cmd_Ctrl(logger) {}
};

TEST_CASE("Cmd_Ctrl::split_input splits by whitespace") {
    auto logger = std::make_shared<FakeLogger>();
    Cmd_Ctrl ctrl(logger);

    std::string input = "show arbeit -debug";
    auto result = ctrl.split_input(input);

    CHECK(result.size() == 3);
    CHECK(result[0] == "show");
    CHECK(result[1] == "arbeit");
    CHECK(result[2] == "-debug");
};

TEST_CASE("Cmd_Ctrl::is_argument_valid returns true on matching command pattern") {
    auto logger = std::make_shared<FakeLogger>();
    TestableCmdCtrl ctrl(logger);

    std::vector<std::string> argv = {"show", "-debug"};
    int argc = static_cast<int>(argv.size());
	
	argv = ctrl.parse_argv(argc, argv);
	
    CHECK(ctrl.is_argument_valid(argc, argv) == true);
    CHECK(ctrl.args_valid == true);
};

TEST_CASE("Cmd_Ctrl::is_argument_valid returns false on no match") {
    auto logger = std::make_shared<FakeLogger>();
    TestableCmdCtrl ctrl(logger);

    std::vector<std::string> argv = {"invalid", "input"};
    int argc = static_cast<int>(argv.size());

    CHECK(ctrl.is_argument_valid(argc, argv) == false);
};

TEST_CASE("Cmd_Ctrl::check_debug_mode aktiviert Debug-Modus und entfernt -prolog") {
    auto logger = std::make_shared<FakeLogger>();
    TestableCmdCtrl ctrl(logger);

    // Muster für Debug-Argument setzen
    ctrl.regex_pattern[command::debug] = std::regex(R"(^-?debug$)");

    std::vector<std::string> argv = {"<alias>", "-activate", "-measure", "1 -h", "-debug"};
    int argc = static_cast<int>(argv.size());

    ctrl.check_debug_mode(argc, argv);

    // Erwartung: debug ist aktiv, '-debug' entfernt, "show" bleibt
    argv = ctrl.parse_argv(argc, argv);
    
    CHECK(argc == 4);
    CHECK(argv.size() == 5);
    CHECK(argv[0] == "<alias>");
    CHECK(argv[1] == "-activate");
    CHECK(argv[2] == "-measure");
    CHECK(argv[3] == "1");
    CHECK(argv[4] == "-h");
	
    auto fakeLogger = std::dynamic_pointer_cast<FakeLogger>(logger);
    REQUIRE(fakeLogger != nullptr);
    fakeLogger->set_debug_to(true);
    CHECK(fakeLogger->debug_enabled == true);
}

