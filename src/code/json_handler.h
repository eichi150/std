#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#ifdef _WIN32
	#include <windows.h>

#else
	#include <limits.h>
	#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <fstream>

#include "json.hpp"
#include "translator.h"
#include "time_account.h"
#include "./exception/exception.h"

class JSON_Handler{
public:
	JSON_Handler(std::shared_ptr<ErrorLogger> logger_);
	
	void read_all_accounts(std::vector<Time_Account>& all_accounts);
	
	std::string get_config_filepath() const;

	std::string get_entity_filepath() const;

	std::string get_accounts_filepath() const;

	Language get_config_language() const;

	std::string get_automatic_config_filepath() const { return automation_config_filepath; }
	//Search Filepath in home Directory
	std::string getExecutableDir();
	
	std::string getConfigFilePath();

	void save_config_file(std::map<std::string, std::string>& save_to_config);

	void read_config_file();
	
	//Sammle alle Accounts mit dieser Entity
	std::vector<Time_Account> collect_accounts_with_entity(const std::vector<Time_Account>& search_in, const std::string& search_for_entity);
	
	void save_json_entity(const std::vector<Time_Account>& all_accounts, const std::string& entity_to_save);
	
	void save_json_accounts(const std::vector<Time_Account>& all_accounts);

	void read_json_entity(std::vector<Time_Account>& all_accounts);

	void read_json_accounts(std::vector<Time_Account>& all_accounts);

	void save_automation_config_file(const std::vector<Automation_Config>& automation_config);
	std::vector<Automation_Config> read_automation_config_file();
	

private:
	void log(const std::string& msg) const;

private:
	std::shared_ptr<ErrorLogger> logger;
	
	std::string config_filepath{"../config.json"};
	
	const std::vector<std::string> allowed_keys = {
		"config_filepath"
		, "entity_filepath"
		, "accounts_filepath"
		, "language"
		, "automation_filepath"
	};
	
	std::string entity_filepath{"../files/"};
	std::string accounts_filepath{"../files/accounts.json"};
	std::string automation_config_filepath{"../files/automation_config.json"};
	
	Language config_language = Language::english;
};

#endif // JSON_HANDLER_H
