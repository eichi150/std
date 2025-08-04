#ifndef COMMAND_ALIAS_H
#define COMMAND_ALIAS_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>
#include <functional>

#include "command.h"
#include "../time_account.h"
#include "../json_handler.h"
#include "../interface/translator.h"
#include "../control/clock.h"

class Alias_Command : public Command{
public:
	Alias_Command(
		const std::vector<std::string>& str_argv
		, std::vector<Time_Account>& all_accounts
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
		
	);
	
	void execute() override = 0;
	
	std::shared_ptr<Time_Account> get_account();
	
protected:
	//Alias Account
	std::shared_ptr<Time_Account> account;
	std::vector<Time_Account>& all_accounts;
	std::vector<std::string> str_argv;	
	
	std::string alias;
	std::string entity;
	
private:
	
	void get_account_from_alias();
};

//Delete Alias out of Registry
class Delete_Alias_Command : public Alias_Command{
public:
	Delete_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
	);
	
	void execute() override;
	
private:
	
	void delete_alias();
	
private:
	std::shared_ptr<JSON_Handler> jsonH;
	
}; // Delete_Alias_Command

#ifdef __linux__
	#include "handle_crontab.h"
#else
	#include "interaction.h"
#endif // __linux__

class Interact_Alias_Command : public Alias_Command{
public:
	Interact_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> callback
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
		
	);
	void execute() override;
	
private:
#ifdef __linux__
	void interact_with_Crontab();
#endif // __linux__

private:
	std::shared_ptr<Interaction> interaction;
	std::vector<Automation_Config> automation_config;
	std::vector<Automation_Config> only_crontabs;
	
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<JSON_Handler> jsonH;
	std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line;
	
}; // Interact_Alias_Command

#endif // COMMAND_ALIAS_H
