#ifndef COMMAND_H
#define COMMAND_H

#include <sstream>
#include <memory>
#include <vector>
#include <map>
#include "../json_handler.h"
#include "../time_account.h"
#include "../control/ctrl.h"
#include "../exception/exception.h"


class Command{
public:
	Command(
		std::shared_ptr<ErrorLogger> set_log
		
	);
	
	virtual ~Command() = default;
	
	virtual void execute() = 0;
	
	void log(const std::string& msg);
	std::string get_logs() const;
	
	void set_output_logger(std::shared_ptr<ErrorLogger> log);
	
	void add_output(const std::string& msg);
	
	std::string get_output_logs() const;
	
protected:
	std::shared_ptr<ErrorLogger> output_logger;
	std::shared_ptr<ErrorLogger> logger;
	
}; // Command



class Add_Alias : public Command{
public:
	struct Add_account{
		std::string entity;
		std::string alias;
		std::string tag;
	};
	
	Add_Alias(
		std::vector<Time_Account>& all_accounts
		, std::shared_ptr<JSON_Handler> jsonH
		, const Add_account& add_info
		, const std::map<command, std::regex>& pattern
		, std::shared_ptr<ErrorLogger> logger
	
	);
	
	void execute() override;

private:
	void check_input();
	
	void create_new_account();

private:
	
	std::vector<Time_Account>& all_accounts;
	std::shared_ptr<JSON_Handler> jsonH;
	Add_account add;
	const std::map<command, std::regex>& regex_pattern;
		
}; // Add_Alias


#ifdef __linux__
#include "../control/device_ctrl.h"
class TouchDevice_Command : public Command{
public:
	TouchDevice_Command(
		const std::string& device_name
		, std::shared_ptr<ErrorLogger> logger
	
	);
	
	void execute() override;
	
private:
	std::string arg;
	
}; // TouchDevice_Command
#endif // __linux__

#endif // COMMAND_H
