#ifndef ARG_MANAGER_H
#define ARG_MANAGER_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>

#include "json_handler.h"
#include "translator.h"
#include "clock.h"

#ifdef __linux__
	#include "device_ctrl.h"
#endif

class Cmd_Ctrl{
public:
	
	//Check for valid Arguments
	bool is_argument_valid(const std::vector<std::string>& str_argv){
		
		for(const auto& pattern : regex_pattern){
			for(size_t i{1}; i < str_argv.size(); ++i){
				if(std::regex_match(str_argv[i], pattern.second)){
				
					return  true;
				}	
			}
		}
		
		return false;
	}

	std::map<error, std::string> get_str_error() const { return str_error; }
	std::map<command, std::regex> get_regex_pattern() const { return regex_pattern; }
private:

	std::map<command, std::regex> regex_pattern = {
		  { command::help,      		std::regex{R"(^--?help$)", std::regex_constants::icase } }
		, { command::add,       		std::regex{R"(^--?add$)", std::regex_constants::icase } }
		, { command::show,      		std::regex{R"(^(--?sh(ow)?|sh|show)$)", std::regex_constants::icase } }
		, { command::delete_,   		std::regex{R"(^(--?del(ete)?)$)", std::regex_constants::icase } }
		, { command::hours, 			std::regex{R"(^(--?h(ours)?|h|hours)$)", std::regex_constants::icase } }
		, { command::minutes, 			std::regex{R"(^(--?m(inutes)?|m|minutes)$)", std::regex_constants::icase} }
		, { command::config_filepath, 	std::regex{R"(^--?cf$)", std::regex_constants::icase } }
		, { command::user_filepath,  	std::regex{R"(^(--?f(ilepath)?|filepath)$)", std::regex_constants::icase } }
		, { command::language,  		std::regex{R"(^(--?l(anguage)?|language)$)", std::regex_constants::icase } }
		, { command::tag,				std::regex{R"(^--?tag$)", std::regex_constants::icase } }
		, { command::touch_sensor, 		std::regex{R"(^--?touch$)", std::regex_constants::icase } }
		, { command::messure_sensor,	std::regex{R"(^(--?mes(sure)?)$)", std::regex_constants::icase } }
	    , { command::activate,			std::regex{R"(^(--?a(ctivate)?)$)", std::regex_constants::icase } }
	    , { command::i2c, 				std::regex{R"(^--?i2c$)", std::regex_constants::icase } }
		, { command::automatic, 		std::regex{R"(^--?auto$)", std::regex_constants::icase } }
		, { command::environment, 		std::regex{R"(^--?env$)", std::regex_constants::icase } }
	};

	std::map<error, std::string> str_error{
		  {error::double_entity, "Entity already taken"}
		, {error::double_alias, "Alias already taken"}
		, {error::alias_equal_entity, "Alias cant be equal to any Entity"}
		, {error::unknown_alias_or_entity, "Alias or Entity not found"}
		, {error::user_input_is_command, "Rejected Input"}
		, {error::not_found, "Not found"}
		, {error::synthax, "Synthax wrong"}
		, {error::untitled_error,"Untitled Error"}
		, {error::unknown, "Unknown Command"}
		, {error::unknown_alias, "Unknown Alias"}
		, {error::unknown_language, "Unknown Language"}
		, {error::sensor, "Sensor Error. Make sure you installed i2c.\nExecute on Command Line: 'sudo apt-get install i2c-tools'\nand try 'sudo i2cdetect -y 1'\nPort: 76 should be active. Succesfully installed."}
		, {error::tag_not_found, "Unknown Tag"}
	};
	
};


class Arg_Manager{
public:
	Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::shared_ptr<Cmd_Ctrl>& ctrl);
	
	void proceed_inputs(const int& argc, const std::vector<std::string>& argv);

	bool run_environment() const { return run_env; }
	
private:
	bool run_env = false;
	
	std::shared_ptr<JSON_Handler> jsonH;
	std::vector<Time_Account> all_accounts;
	Translator translator{};
	Clock clock{};	
	std::vector<std::string> str_argv;
	int argc;
	//show Tabellen setw(max_length[]) 
	std::vector<int> max_length;
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	
	const std::string help = {
    "add 			Add new Entity give it a Alias\n"
    "-h -m  		Time to save in Hours or Minutes\n"
    "show 			Show all Entitys and Alias's\n"
    "sh 			Short Form of show\n"
    "show 'ALIAS' 	show specific Entity's Time Account\n\n"
    "For more Information have a look at README.md on github.com/eichi150/std\n"
    };
	

	std::vector<Time_Account> check_for_alias_or_entity(const std::vector<Time_Account>& all_accounts, const std::string& alias_or_entity);
	
	//Sammle alle Accounts mit diesem Alias
	std::vector<Time_Account> collect_accounts_with_alias(const std::vector<Time_Account>& search_in, const std::string& search_for_alias);

	void change_config_json_language(const std::string& to_language);

	void change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath);

	void user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath);
	
	void delete_account(std::vector<Time_Account>& all_accounts, const std::string& alias_to_delete);
	
	void add_account(std::vector<Time_Account>& all_accounts, const std::string& tag);

	void add_tag_to_account(std::vector<Time_Account>& all_accounts, const std::string& tag);

#ifdef __linux__
	void add_sensor_data(std::vector<Time_Account>& all_accounts);
#endif // __linux__
	
	void add_hours(std::vector<Time_Account>& all_accounts, const std::string& amount);
	
	void set_table_width(const std::vector<Time_Account>& all_accounts, std::vector<int>& max_length);

	void show_filepaths() const;

	void show_language()const;
	
	void show_specific_table(const std::vector<Time_Account>& show_accounts);
	
	void show_all(const std::vector<Time_Account>& all_accounts);
	
};//Arg_Manager

#endif // ARG_MANAGER_H
