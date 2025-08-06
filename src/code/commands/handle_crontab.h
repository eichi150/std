#ifndef HANDLE_CRONTAB_H
#define HANDLE_CRONTAB_H

#include "interaction.h"
#include "../exception/exception.h"

#ifdef __linux__
#include <functional>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <cstdlib>
#include <regex>
#include <bitset>

#include "../control/device_ctrl.h"
#include "listbox.h"

class Crontab : public Interaction{
protected:
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
	
	enum class to_do{
		erase_all = 0
		, erase_detail
	};
	
	
public:
	Crontab(
		std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
		, std::vector<Automation_Config>& automations
	);
	
	
	void interact() override = 0;
	
	void finalize();
	
protected:
	//container
	std::vector<std::string>& str_argv;
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<JSON_Handler> jsonH;
	
	//__container
	std::regex integer_pattern;
	std::map<weekday, std::string> str_weekday;
	std::map<months, std::string> str_months;
	
	//func ptr
	std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line;
	
	//loaded data
	std::vector<Automation_Config>& automation_config;
	std::vector<std::string> current_Crontab;
	//created data
	std::string cronLine;
	std::string logLine;
	
	bool crontab_contains(const std::vector<std::string>& crontabLines, const std::vector<std::string>& searched_targets);
	
	std::string convert_crontabLine_to_speaking_str(const std::string& crontab_line);
	
private:

	std::vector<std::string> get_current_Crontab();
	
};

class delete_task_from_Crontab : public Crontab{
public:
	delete_task_from_Crontab(
		std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
		, std::vector<Automation_Config>& automations
	);
	
	void interact() override;
	
private:
	void process_delete();
	void erase_detail_out_automation_config();
	void erase_complete_alias_out_automation_config();
	void check_is_crontab_task_used();
	std::vector<std::string> erase_out_crontab();
	void write_cronLine();
	
private:
	std::bitset<4> to_do_flags;
	std::string alias;
	std::vector<std::string> str_command;
	bool is_cut_out_automations{false};
	bool is_crontab_command_still_needed{false};
	std::vector<std::string> obsolete_cmds;
	std::vector<std::string> output_vector;
};

class write_into_Crontab : public Crontab{
public:
	write_into_Crontab(
		std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
		, std::vector<Automation_Config>& automations
		
	);
	
	void interact() override;

private:
	void set_user_automation_crontab();
	
	std::pair<std::string, bool> get_user_crontab_line();
	
	std::string check_that_between_A_B(const std::string& str
		, int A
		, int B
		, const std::string& error_prompt);

	bool write_Crontab(const std::shared_ptr<JSON_Handler>& jsonH
		, const std::string& str_command,
		 const std::string& alias
		 , bool logfile);
		 
private:
	std::string connection_type;
	std::string executioner;
	std::string entity;
	std::string alias;
	std::string device_name;
	std::pair<std::string, bool> crontab_line;
	
};
#endif // __linux__

#endif // HANDLE_CRONTAB_H
