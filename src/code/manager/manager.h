#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <memory>
#include <regex>
#include <map>
#include <bitset>

#include "../exception/exception.h"
#include "../json_handler.h"
#include "../interface/translator.h"
#include "../control/cmd_ctrl.h"
#include "../commands/command.h"

// ========================= Manager ============================
class Manager{
public:
	Manager(
		std::shared_ptr<ErrorLogger> _logger
		, std::shared_ptr<ErrorLogger> _output_logger
		, std::shared_ptr<JSON_Handler> _jsonH
		, std::shared_ptr<Cmd_Ctrl> _ctrl

	) : jsonH(_jsonH)
		, logger(std::move(_logger))
		, output_logger(_output_logger)
		, ctrl(std::move(_ctrl))
		
	{
		if(ctrl){
			regex_pattern = ctrl->get_regex_pattern();
		}
		if(logger && jsonH){
			log(std::string{__FILE__} + " - Manager");
		
			translator = std::make_shared<Translator>();
			log("set language");
			translator->set_language(jsonH->get_config_language());
		}
	};

	virtual ~Manager() = default;

	virtual void manage() = 0;
	std::shared_ptr<Translator> get_translator_ptr() const{
		return translator;
	};
	virtual int get_argc() const = 0;
	virtual const std::vector<std::string>& get_str_argv() const = 0;
protected:
	//process args
	virtual bool check_two_args() = 0;
	virtual bool check_three_args() = 0;
	virtual bool check_four_args() = 0;
	virtual bool check_five_args() = 0;
	virtual bool check_six_args() = 0;

protected:

	//logger
	void log(const std::string& msg);
	void add_output(const std::string& output);
	//Log Pointer
	std::shared_ptr<ErrorLogger> logger;
	std::shared_ptr<ErrorLogger> output_logger;
	//Pointer
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<Translator> translator;
	
	//controle instance
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<Cmd_Ctrl> ctrl;
	
	//All Accounts
	std::vector<Time_Account> all_accounts;
	
	//cmd to be executed
	std::shared_ptr<Command> cmd;
	
	//OutputFlags
	OutputBitset output_flags;

}; // Manager

#endif // MANAGER_H