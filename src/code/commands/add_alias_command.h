#ifndef ADD_ALIAS_COMMAND_H
#define ADD_ALIAS_COMMAND_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>

#include "../json_handler.h"
#include "../interface/translator.h"
#include "../control/clock.h"
#include "../control/cmd_ctrl.h"

#include "command.h"
#include "alias_command.h"

class Add_Alias_Command : public Alias_Command{
public:
	Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
		
	);
	
	void execute() override = 0;
	
	//Save to file
	void finalize();
	
protected:
	std::shared_ptr<JSON_Handler> jsonH;
	
private:

	void add_alias();
	
}; // Add_Alias_Command


class Hours_Add_Alias_Command : public Add_Alias_Command{
public:
	Hours_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<command, std::regex>& regex_pattern
		, std::shared_ptr<Translator> translator
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
		
	);

	void execute() override;
    
private:

	void add_hours();

private:
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<Translator> translator;
	std::tm localTime;
	
}; // AddHours_Alias_Command


class Tag_Add_Alias_Command : public Add_Alias_Command{
public:
	Tag_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
		
	);
	
	
	void execute() override;
	
}; // Tag_Add_Alias_Command


#ifdef __linux__
#include "../control/device_ctrl.h"

class SensorData_Add_Alias_Command : public Add_Alias_Command{
public:
	SensorData_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
	);

	void execute() override;

private:
	
	void add_sensor_data();

private:
	std::shared_ptr<Translator> translator;
	std::tm localTime;
	Device_Ctrl device{};
	std::stringstream output_str;
	
}; // SensorData_Add_Alias_Command
#endif // __linux__

#endif // ADD_ALIAS_COMMAND_H
