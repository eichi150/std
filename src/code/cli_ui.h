#ifndef CLI_UI_H
#define CLI_UI_H

#include <iostream>
#include <bitset>
#include <memory>
#include <ostream>

#include "./abstract_class/ui_interface.h"
#include "arg_manager.h"

//Console Line Interface - Konsolenausgaben
class CLI_UI : public UI_Interface{
public:
	explicit CLI_UI(std::shared_ptr<Arg_Manager> manager);

	void update() override;

private:
	std::shared_ptr<Arg_Manager> arg_man;

	//Build Tabellenausgabe
	std::string create_table();
	std::string create_line(int width);
	
	void show_filepaths();
	void show_language();
};

#endif // CLI_UI_H
