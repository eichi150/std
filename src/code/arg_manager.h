#ifndef ARG_MANAGER_H
#define ARG_MANAGER_H

#include "json_handler.h"
#include "translator.h"
#include "clock.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <regex>

class Arg_Manager{
public:
	Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::vector<std::string>& argv, const int& argc, const std::map<command, std::regex>& pattern);
	
	void proceed_inputs(std::vector<Time_Account>& all_accounts, const std::map<error, std::string>& str_error);
	
private:

	std::shared_ptr<JSON_Handler> jsonH;
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
	
	void add_account(std::vector<Time_Account>& all_accounts);
	
	void add_hours(std::vector<Time_Account>& all_accounts);

	void set_table_width(const std::vector<Time_Account>& all_accounts, std::vector<int>& max_length);

	void show_filepaths() const;

	void show_language()const;
	
	void show_specific_table(const std::vector<Time_Account>& show_accounts);
	
	void show_all(const std::vector<Time_Account>& all_accounts);
	
};//Arg_Manager

#endif // ARG_MANAGER_H