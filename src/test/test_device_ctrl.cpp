#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../code/arg_manager.h"

TEST_CASE("split_line funktioniert korrekt"){
	Device_Ctrl ctrl{"Fehler"};
	std::string input = "*/15 * * * * /usr/bin/std -auto test -mes";
	auto result = ctrl.split_line(input);

	CHECK(result.size() >= 6);
	CHECK(result[0] == "*/15");
	CHECK(result[5] == "/usr/bin/std");
}

TEST_CASE("crontab_contains erkennt alias"){
	Device_Ctrl ctrl{"Fehler"};
	std::vector<std::string> lines = {
		"*/15 * * * * /usr/bin/std -auto test -mes"	
	};

	CHECK(ctrl.crontab_contains(lines, "usr/bin/std -auto", "test") == true);
	CHECK(ctrl.crontab_contains(lines, "usr/bin/std -auto", "andere") == false);
}
