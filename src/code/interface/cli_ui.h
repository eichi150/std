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
	);

	void update() override;
	
private:	
	void standard();
	
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
	void write_env_header_footer();
	//Build Tabellenausgabe
	std::string create_entity_table(const std::vector<Time_Account>& all_accounts);
	std::string create_alias_table(
		const std::vector<Automation_Config>& all_automations
		, const std::string& alias
		, std::shared_ptr<Time_Account> account
		, const std::string& time_format
	);


	std::string create_automation_table(const std::string& account_alias, const std::vector<Automation_Config>& all_automations);
	std::string create_data_table(std::shared_ptr<Time_Account> account, const std::string& alias, const std::string& time_format);
	std::string create_all_accounts_table(const std::vector<Time_Account>& all_accounts);
	
	std::string create_line(int width, const char& symbol);
	
	int get_max_width(const std::string& header, const std::vector<std::string>& values);
	int scale_int_to_float(int num, float scaling);

	void log(const std::string& msg);
private:
	bool first_run = false;
	bool run_env = false;

	std::shared_ptr<ErrorLogger> output_logger;
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<Manager> manager;
	std::shared_ptr<Translator> translator;

private:
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
