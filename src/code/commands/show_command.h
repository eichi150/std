#ifndef SHOW_COMMAND_H
#define SHOW_COMMAND_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>

#include "../time_account.h"
#include "../json_handler.h"
#include "../interface/translator.h"
#include "../control/cmd_ctrl.h"
#include "command.h"
#include "entity_command.h"

#include "../exception/exception.h"

enum class OutputType{
	show_all_accounts = 0
	, show_help
	, show_filepaths
	, show_language
	, show_specific_table
	, show_alias_table
	, show_alias_automation
	, show_user_output_log
	, show_entity
	, COUNT //maxSize Bitset
};


using OutputBitset = std::bitset<static_cast<size_t>(OutputType::COUNT)>;


class Show_Command : public Command{
public:
	Show_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, int argc
		, const std::map<command, std::regex>& regex_pattern
		, OutputBitset& output_flags
		, std::shared_ptr<ErrorLogger> logger
		
	);
	void execute() override;
	OutputBitset& output_flags;

private:

	void with_2_args();

	void with_3_args();
	
	void with_4_args();
	
	bool find_entity(const std::string& __entity);
	
	bool find_alias(const std::string& __alias);
	
private:
	std::vector<Time_Account>& all_accounts;
	std::vector<std::string> str_argv;
	std::shared_ptr<JSON_Handler> jsonH;
	int argc;
	std::map<command, std::regex> regex_pattern;
	
		
}; // Show_Command

#endif // SHOW_COMMAND_H
