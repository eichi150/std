#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../code/device_ctrl.h"

	
TEST_CASE("Crontab Argument Parsing") {
    Device_Ctrl device{"Error"};

    // alle 15 Minuten
    {
        std::string result = device.get_user_crontag_line({"std", "-a", "-mes", "15", "-m", "february"});
        CHECK(result == "*/15 * * 2 *");
    }

    // jede Stunde
    {
        std::string result = device.get_user_crontag_line({"std", "-a", "-mes", "1", "-h"});
        CHECK(result == "* */1 * * *");
    }

    // jede Woche am Montag um 5 Uhr
    {
        std::string result = device.get_user_crontag_line({"std", "-a", "-mes", "5", "-clock", "monday", "-log"});
        CHECK(result == "0 5 * * 1");
    }

    // jeden Monat am 1. Tag um 3 Uhr
    {
        std::string result = device.get_user_crontag_line({"std", "-a", "-mes", "3", "-clock", "1", "-month"});
        CHECK(result == "0 3 1 * *");
    }

    // jeden Freitag um 22:30 Uhr
    {
        std::string result = device.get_user_crontag_line({"std", "-a", "-mes", "22:30", "-clock", "friday"});
        CHECK(result == "30 22 * * 5");
    }

    // täglicher Task um 00:01
    {
        std::string result = device.get_user_crontag_line({"std", "-a", "-mes", "00:01", "-clock"});
        CHECK(result == "1 0 * * *");
    }
	
	// täglicher Task um 70:01
    
    {
        CHECK_THROWS_WITH( device.get_user_crontag_line({"std", "-a", "-mes", "70:01", "-clock"}),
			"Clock Hours allowed between 0 - 23" );
    }
    // jeden 15. um 6:15 Uhr
    {
        std::string result = device.get_user_crontag_line({"std", "-a", "-mes", "6:15", "-clock", "15", "-month"});
        CHECK(result == "15 6 15 * *");
    }

    // 3 stündlich mit log
    {
        std::string result = device.get_user_crontag_line({"std", "-a", "-mes", "3", "-h", "-log"});
        CHECK(result == "* */3 * * *");
    }

}


