#ifndef CLI_UI_H
#define CLI_UI_H

#include <iostream>
#include <iomanip>
#include <bitset>
#include <memory>
#include <ostream>
#include <vector>
#include <string>

#include "./abstract_class/ui_interface.h"
#include "arg_manager.h"


//Console Line Interface - Konsolenausgaben
class CLI_UI : public UI::UI_Interface{
public:
	explicit CLI_UI(
		std::shared_ptr<ErrorLogger> output_logger
		, std::shared_ptr<Arg_Manager> manager
		, std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
	);

	void update() override;
	
private:
	//Build Tabellenausgabe
	std::string create_alias_table();
	std::string create_automation_table(const std::string& account_alias);
	std::string create_data_table(const std::string& alias);
	std::string create_all_accounts_table();
	
	std::string create_line(int width, const char& symbol);
	
	size_t scale_str_size(const std::string& str);
	int get_sum_str_size(const std::vector<std::string>& multiple_str);
	
	void show_help();
	void show_all_accounts();
	void show_alias_table();
	void show_alias_automation_table();
	void show_filepaths();
	void show_language();
	
private:
	std::shared_ptr<ErrorLogger> output_logger;
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<Translator> translator;
	
	std::shared_ptr<Time_Account> account;
	
	std::shared_ptr<Arg_Manager> arg_man;
	std::vector<Automation_Config> all_automations;
	
	int table_width = 0;
	
	const std::string help = {
    "add 			Add new Entity give it a Alias\n"
    "-h -m  		Time to save in Hours or Minutes\n"
    "show 			Show all Entitys and Alias's\n"
    "sh 			Short Form of show\n"
    "show 'ALIAS' 	show specific Entity's Time Account\n\n"
    "For more Information have a look at README.md on github.com/eichi150/std\n"
    };
};

#endif // CLI_UI_H
