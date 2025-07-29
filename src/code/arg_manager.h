#ifndef ARG_MANAGER_H
#define ARG_MANAGER_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>

#include "json_handler.h"
#include "translator.h"
#include "clock.h"
#include "cmd_ctrl.h"
#include "enum_class.h"

#ifdef __linux__
	#include "device_ctrl.h"
#endif

enum class OutputType{
	show_all_accounts = 0
	, show_filepaths
	, show_language
	, show_specific_table
	, show_alias_table
	, show_alias_automation
	, arg_manager_log
	, COUNT //maxSize Bitset
};

using OutputBitset = std::bitset<static_cast<size_t>(OutputType::COUNT)>;

class Arg_Manager{
public:
	Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::shared_ptr<Cmd_Ctrl>& ctrl_ptr);
	
	void proceed_inputs(const int& _argc, const std::vector<std::string>& argv);

	bool run_environment() const { return run_env; }
	
	std::shared_ptr<JSON_Handler> jsonH;
	Translator translator{};
	
	void set_output_flag(OutputType flag, bool value = true){
		output_flags.set(static_cast<size_t>(flag), value);
	}
	OutputBitset get_output_flags() const {
		return output_flags;
	}
	void clear_output_flags(){
		output_flags.reset();
	}
	
	std::vector<Time_Account> get_all_accounts() const {
		return all_accounts;
	}
	
	std::shared_ptr<Time_Account> get_account_with_alias(const std::string& alias);
	
	std::vector<std::string> get_str_argv() const {
		return str_argv;
	}
	
	std::string get_log() const {
		std::ostringstream log;
		log 
			<< "Process Log:\n"
			<< arg_manager_log.str()
			<< '\n'
			<< jsonH->get_log()
			<< std::endl;
			
		return log.str();
	}
	
private:

#ifdef __linux__
	std::string add_sensor_data(const std::shared_ptr<Device_Ctrl>& device, std::vector<Time_Account>& all_accounts);
#endif // __linux__
	
	OutputBitset output_flags;
	
	std::stringstream arg_manager_log;
	
	bool run_env = false;
	
	std::shared_ptr<Cmd_Ctrl> ctrl;
	
	std::vector<Time_Account> all_accounts;
	
	Clock clock{};
	
	std::vector<std::string> str_argv;
	int argc;
	
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	

	std::vector<Time_Account> check_for_alias_or_entity(const std::vector<Time_Account>& all_accounts, const std::string& alias_or_entity);
	
	//Sammle alle Accounts mit diesem Alias
	std::vector<Time_Account> collect_accounts_with_alias(const std::vector<Time_Account>& search_in, const std::string& search_for_alias);

	void change_config_json_language(const std::string& to_language);

	void change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath);

	void user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath);
	
	void delete_account(std::vector<Time_Account>& all_accounts, const std::string& alias_to_delete);
	
	void add_account(std::vector<Time_Account>& all_accounts, const std::string& tag);

	void add_tag_to_account(std::vector<Time_Account>& all_accounts, const std::string& tag);
	
	void add_hours(std::vector<Time_Account>& all_accounts, const std::string& amount);



	const std::string help = {
    "add 			Add new Entity give it a Alias\n"
    "-h -m  		Time to save in Hours or Minutes\n"
    "show 			Show all Entitys and Alias's\n"
    "sh 			Short Form of show\n"
    "show 'ALIAS' 	show specific Entity's Time Account\n\n"
    "For more Information have a look at README.md on github.com/eichi150/std\n"
    };
	    
	
};//Arg_Manager

#endif // ARG_MANAGER_H
