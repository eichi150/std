#ifndef ARG_MANAGER_H
#define ARG_MANAGER_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>
#include <functional>

#include "json_handler.h"
#include "translator.h"
#include "clock.h"
#include "cmd_ctrl.h"

#include "./commands/command.h"
#include "./commands/show_command.h"
#include "./commands/alias_command.h"
#include "./commands/add_alias_command.h"
#include "./commands/change_command.h"

class Arg_Manager{
public:
	Arg_Manager(std::shared_ptr<JSON_Handler> jH, std::shared_ptr<Cmd_Ctrl> ctrl_ptr);
	
	//Variables
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<Translator> translator;
	
	//Methods
	void proceed_inputs(const int& _argc, const std::vector<std::string>& argv);
	void set_output_flag(OutputType flag, bool value = true); 
	OutputBitset get_output_flags() const;
	void clear_output_flags();
	std::vector<Time_Account> get_all_accounts() const;
	std::vector<std::string> get_str_argv() const;
	std::string get_log() const;
	std::string get_user_output_log() const;
	std::shared_ptr<Time_Account> get_account_with_alias(const std::string& alias);
	
	//TO_DO
	//bool run_environment() const { return run_env; }
	//bool run_env = false;
private:
	//log
	std::stringstream arg_manager_log;
	std::stringstream user_output_log;

	//arguments
	int argc;
	std::vector<std::string> str_argv;
	
	//controle instance
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	std::shared_ptr<Cmd_Ctrl> ctrl;
	
	bool check_two_args();
	bool check_three_args();
	bool check_four_args();
	bool check_five_args();
	bool check_six_args();
	
	//All Accounts
	std::vector<Time_Account> all_accounts;
	
	//cmd to be executed
	std::unique_ptr<Command> cmd;
	
	//OutputFlags
	OutputBitset output_flags;
	
};//Arg_Manager
#endif // ARG_MANAGER_H
