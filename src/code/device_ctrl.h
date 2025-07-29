#ifdef __linux__ // LINUX ONLY
#ifndef DEVICE_CTRL_H
#define DEVICE_CTRL_H

#include <iostream>
#include <iomanip>
#include <memory>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <cstdio>

#include "./abstract_class/ctrl.h"

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


class Device_Ctrl : public Ctrl{
public:
	Device_Ctrl(const std::string& error_prompt);
	
	std::string process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command);
	
	std::string set_user_automation_crontab(const std::vector<std::string>& str_argv
		, const std::shared_ptr<JSON_Handler>& jsonH
		, const std::map<command, std::regex>& regex_pattern
	);
	
	std::vector<float> check_device(const std::string& name) override;

	std::string get_name() const { return name; }
	
	
private:
	std::string name;
	std::string error_prompt;
	std::vector<Automation_Config> all_automations;
	std::vector<Time_Account> all_accounts;
	
	std::map<weekday, std::string> str_weekday;
	std::map<months, std::string> str_months;
	
	std::pair<std::string, bool> get_user_crontag_line(const std::vector<std::string>& str_argv, const std::map<command, std::regex>& regex_pattern);

	bool write_Crontab(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command, const std::string& alias, bool logfile);

	std::string convert_crontabLine_to_speeking_str(const std::string& crontab_line);
	
	std::string check_that_between_A_B(const std::string& str, int A, int B, const std::string& error_prompt);
		
	std::vector<std::string> get_current_Crontab();
	
	bool crontab_contains(const std::vector<std::string>& crontabLines, const std::vector<std::string>& searched_targets);
};

#endif // DEVICE_CTRL_H
#endif // __linux__
