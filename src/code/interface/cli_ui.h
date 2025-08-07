#ifndef CLI_UI_H
#define CLI_UI_H

#include <iostream>
#include <iomanip>
#include <bitset>
#include <memory>
#include <ostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <set>
#include <regex>

#include "replex.h"
#include "ui_interface.h"
#include "../manager/arg_manager.h"
#include "../manager/env_manager.h"
#include "../time_account.h"

//Console Line Interface - Konsolenausgaben
class CLI_UI : public UI::UI_Interface{
public:
	explicit CLI_UI(
		std::shared_ptr<ErrorLogger> _output_logger
		, std::shared_ptr<Manager> _manager
		, std::shared_ptr<JSON_Handler> _jsonH
		, bool _run_env
		, std::shared_ptr<Cmd_Ctrl> _ctrl
	);

	void update() override;
	
	bool is_env_running() const;
	

private:	
	void standard();
	void run_environment();
	void pass_tabcompletions_to_rx();
private:
	void show_help();
	void show_all_accounts();
	void show_entity_table();
	void show_alias_table();
	void show_alias_automation_table();
	void show_filepaths();
	void show_language();
	
private:
	std::string print_centered_message(const std::string& msg, int total_width = 60, char fill_char = '-');
	//Build Tabellenausgabe
	std::string create_entity_table();
	std::string create_alias_table();
	std::string create_automation_table(const std::string& account_alias);
	std::string create_data_table(const std::string& alias);
	std::string create_all_accounts_table();
	
	std::string create_line(int width, const char& symbol);
	
	int get_max_width(const std::string& header, const std::vector<std::string>& values);
	int scale_int_to_float(int num, float scaling);
private:
	bool first_run = false;
	bool run_env = false;
	std::shared_ptr<Cmd_Ctrl> ctrl;
	myReplxx _rx{ctrl};

	std::shared_ptr<ErrorLogger> output_logger;
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<Translator> translator;
	
	std::shared_ptr<Time_Account> account;
	
	std::shared_ptr<Arg_Manager> arg_man;
	std::shared_ptr<Env_Manager> env_man;

	std::vector<Automation_Config> all_automations;
	
	int table_width = 0;
	
	const std::string help = {
	"Usage: std [command] [args...]\n"
    "-add <entity> <alias>			Add new Entity give it a Alias\n"
    "<alias> [time] -h / -m  		Time to save in Hours or Minutes\n"
    "sh / show 						Show all Entitys and Alias's\n"
    "sh <alias> / <entity>			Show specific Entity or Alias\n"
    "show <alias> -activate		 	show Alias Automations\n\n"
	"<alias> -activate -measure [time setting]\n"
    "For more Information have a look at README.md on github.com/eichi150/std\n"
    };
};

#endif // CLI_UI_H
