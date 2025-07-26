#ifdef __linux__
#ifndef DEVICE_CTRL_H
#define DEVICE_CTRL_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include "bme280/bme280_sensor.h"
#include "json_handler.h"
#include "time_account.h"
#include "clock.h"

enum class weekday{
	sunday = 0
	, monday
	, tuesday
	, wednesday
	, thursday
	, friday
	, saturday
};

enum class months{
	january = 1
	, february
	, march
	, april
	, may
	, june
	, july
	, august
	, september
	, october
	, november
	, december
};

class Device_Ctrl{
public:
	Device_Ctrl(const std::string& error_prompt);
	
	void process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& alias);

	
	void write_Crontab(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& alias, bool logfile);

	
	std::string get_user_crontag_line(const std::vector<std::string>& str_argv);
	
	std::vector<float> check_device();
		
private:
	Clock clock{};
	BME_Sensor sensor{};
	std::string error_prompt;
	std::vector<Automation_Config> all_automations;
	std::vector<Time_Account> all_accounts;

	
	std::string check_that_between_A_B(const std::string& str, int A, int B, const std::string& error_prompt);

		
	std::vector<std::string> get_current_Crontab();
	
	//split string an leerzeichen
	std::vector<std::string> split_line(const std::string& line,const std::regex& re);
	
	bool crontab_contains(const std::vector<std::string>& crontabLines, const std::string& targetLine, const std::string& targetChar);
};

#endif // DEVICE_CTRL_H
#endif // __linux__
