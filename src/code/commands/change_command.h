#ifndef CHANGE_COMMAND_H
#define CHANGE_COMMAND_H

#include <vector>
#include <memory>
#include <regex>
#include <map>
#include <sstream>

#include "../json_handler.h"
#include "../interface/translator.h"
#include "../control/cmd_ctrl.h"

#include "command.h"

class Change_Command : public Command{
public:
	Change_Command( 
		std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<ErrorLogger> logger
	);

	void execute() override = 0;
	
	void finalize();
	
protected:
	std::map<std::string, std::string> new_data;
	
	std::shared_ptr<JSON_Handler> jsonH;

}; // Change_Command

class UserFilepath_Change_Command : public Change_Command{
public:
	UserFilepath_Change_Command(
		std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<ErrorLogger> logger
		
	);
	
	void execute() override;
	
private:
	std::shared_ptr<Translator> translator;
	std::vector<std::string> str_argv;
	
}; // UserFilepath_Change_Command

class Language_Change_Command : public Change_Command{
public:
	Language_Change_Command(
		std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		, const std::string& change_to_language
		, std::shared_ptr<ErrorLogger> logger
		
	);

	void execute() override;

private:
	std::shared_ptr<Translator> translator;
	std::string change_to_language;
	
}; // Language_Change_Command

#endif // CHANGE_COMMAND_H
