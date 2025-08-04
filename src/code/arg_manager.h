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
#include "./interface/translator.h"
#include "./control/clock.h"
#include "./control/cmd_ctrl.h"

#include "./exception/exception.h"

#include "./commands/command.h"
#include "./commands/show_command.h"
#include "./commands/alias_command.h"
#include "./commands/add_alias_command.h"
#include "./commands/change_command.h"
#include "./commands/entity_command.h"


class Arg_Manager{
public:
	Arg_Manager(
		std::shared_ptr<ErrorLogger> logger
		, std::shared_ptr<ErrorLogger> output_logger
		, std::shared_ptr<JSON_Handler> jH
		, std::shared_ptr<Cmd_Ctrl> ctrl_ptr
		, const int& argc
		, const std::vector<std::string>& str_argv
		, std::map<command, std::regex> _regex_pattern
	);
	
	//Methods
	void process();
	
	void set_output_flag(OutputType flag, bool value = true); 
	OutputBitset get_output_flags() const;
	void clear_output_flags();
	std::vector<Time_Account> get_all_accounts() const;
	std::vector<std::string> get_str_argv() const;

	std::shared_ptr<Time_Account> get_account_with_alias(const std::string& alias);
	std::shared_ptr<Translator> get_translator_ptr() const;
	
private:
	//logger
	void log(const std::string& msg);
	void add_output(const std::string& output);
	//process args
	bool check_two_args();
	bool check_three_args();
	bool check_four_args();
	bool check_five_args();
	bool check_six_args();

private:
	//Log Pointer
	std::shared_ptr<ErrorLogger> logger;
	std::shared_ptr<ErrorLogger> output_logger;
	//Pointer
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<Translator> translator;

	//arguments
	int argc;
	std::vector<std::string> str_argv;
	
	//controle instance
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<Cmd_Ctrl> ctrl;
	
	//All Accounts
	std::vector<Time_Account> all_accounts;
	
	//cmd to be executed
	std::shared_ptr<Command> cmd;
	
	//OutputFlags
	OutputBitset output_flags;
	
};//Arg_Manager
#endif // ARG_MANAGER_H
