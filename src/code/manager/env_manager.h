#ifndef ENV_MANAGER_H
#define ENV_MANAGER_H
#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>
#include <functional>

#include "../exception/exception.h"

#include "../json_handler.h"
#include "../interface/translator.h"

#include "../control/clock.h"
#include "../control/cmd_ctrl.h"

#include "../commands/command.h"
#include "../commands/show_command.h"
#include "../commands/alias_command.h"
#include "../commands/add_alias_command.h"
#include "../commands/change_command.h"
#include "../commands/entity_command.h"

#include "manager.h"

// ====================== Env_Manager ===========================

class Env_Manager : public Manager{
public:
	Env_Manager(
		std::shared_ptr<ErrorLogger> _logger
		, std::shared_ptr<ErrorLogger> _output_logger
		, std::shared_ptr<JSON_Handler> _jsonH
		, std::shared_ptr<Cmd_Ctrl> _ctrl
		, int _argc
		, const std::vector<std::string>& _str_argv
	);

	void manage() override ;

	void reset_args();
	void set_str_argv(std::vector<std::string>& _str_argv);
	std::vector<std::string>& get_str_argv();
	int get_argc()const;
	void set_argc();
	
	void set_output_flag(OutputType flag, bool value);
	OutputBitset get_output_flags() const;
	void clear_output_flags();

	std::vector<Time_Account> get_all_accounts() const;
	std::shared_ptr<Time_Account> get_account_with_alias(const std::string& alias);
	
private:
	//arguments
	int argc;
	std::vector<std::string> str_argv;
	//process args
	bool check_two_args() override;
	bool check_three_args() override;

	bool check_four_args() override;
	bool check_five_args() override;
	bool check_six_args() override;
}; // Env_Manager

#endif // ENV_MANAGER_H