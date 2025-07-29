#ifndef CLI_UI_H
#define CLI_UI_H

#include <iostream>
#include <iomanip>
#include <bitset>
#include <memory>
#include <ostream>
#include <vector>

#include "./abstract_class/ui_interface.h"
#include "arg_manager.h"

struct OutputTable{
	std::vector<std::string> head;
	std::vector<std::string> body;
	
};


//Console Line Interface - Konsolenausgaben
class CLI_UI : public UI_Interface{
public:
	explicit CLI_UI(std::shared_ptr<Arg_Manager> manager);

	void update() override;

private:
	std::shared_ptr<Arg_Manager> arg_man;
	
	int table_width = 0;
	//Build Tabellenausgabe
	std::string create_alias_table();
	std::string create_automation_table(const std::string& account_alias);
	std::string create_data_table(const std::string& alias);
	std::string create_all_accounts_table();
	
	std::string create_line(int width, const char& symbol);
	
	size_t scale_str_size(const std::string& str);
	int get_sum_str_size(const std::vector<std::string>& multiple_str);
	
	void show_all_accounts();
	
	void show_alias_table();
	void show_alias_automation_table();
	void show_filepaths();
	void show_language();
};

#endif // CLI_UI_H
