#ifdef _WIN32
	#include <windows.h>
#endif // _WIN32

#ifdef __linux__
	#include <limits.h>
	#include <unistd.h>
#endif // __linux

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <regex>
#include <map>
#include <cstdlib>

#include "json.hpp"

#include "clock.h"
#include "time_account.h"
#include "json_handler.h"
#include "translator.h"
#include "cmd_ctrl.h"
#include "arg_manager.h"
#include "cli_ui.h"
#include "./exception/exception.h"


// SIMPLE TIME DOCUMENTATION /* github.com/eichi150/std */

using json = nlohmann::json;

class STD_Master{
public:
	STD_Master(
		const int _argc
		, char* argv[]
	
	) : argc(_argc)
	{
		
		logger = std::make_shared<Default_Logger>();
		output_logger = std::make_shared<User_Logger>();
		
		ctrl = std::make_shared<Cmd_Ctrl>(logger);
		jsonH = std::make_shared<JSON_Handler>(logger);
		
		if(argc > 1 && ctrl){
			str_argv = ctrl->parse_argv(argc, argv);
		}
		if(logger){
			log(std::string{__FILE__} + " - STD_Master");
		}
	};
	
	void run_cli(){
		if(argc == 1){
			std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
			return;
		}
		
		if(!logger || !output_logger || !ctrl || !jsonH){
			throw std::runtime_error{"Pointer initiation failed"};
		}
		
		try{
			log("run CLI");
				
			if(ctrl->is_argument_valid(argc, str_argv)){
				#ifdef __linux_
					log("run automated Crontab Task");
					if(run_linux_Crontab_Automation()){
						return;
					}
				#endif // __linux__
					
				//Init Argument Manager
				arg_man = std::make_shared<Arg_Manager>(
					logger
					, output_logger
					, jsonH
					, ctrl
				);
				arg_man->proceed_inputs(argc, str_argv);
					
				//CLI UserInterface & ConsoleOutput				
				cli = std::make_unique<CLI_UI>(
					output_logger
					, arg_man
					, jsonH
					, arg_man->get_translator_ptr()
				);
				cli->update();
					
				//Debug Output
				logger->dump_log_for_Mode(logger, ErrorLogger::Mode::debug);
				
			}else{
				throw std::invalid_argument{"Invalid Arguments"};
			}		
				
		//Error Output
		}catch(const Logged_Error& le){
				
			LOG_INFO(le.what());

			ErrorLogger::dump_log_for_Mode(logger, ErrorLogger::Mode::error);
				
				
		}catch(const SyntaxError& se){
				
			LOG_INFO(se.what());
			
		}catch(const DeviceConnectionError& de){
				
			std::cerr << "**" << de.what() << std::endl;
				
		}catch(const std::runtime_error& re){
				
			std::cerr << "**" << re.what() << std::endl;
				
		}catch(const std::invalid_argument& ia){
				
			std::cerr << "**" << ia.what() << std::endl;
				
		}
	}
	
#ifdef __linux__
	bool run_linux_Crontab_Automation(){
		
		auto regex_pattern = ctrl->get_regex_pattern();
		//Vorher Automation ausführen
		if(	str_argv.size() >= 4 
			&& std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
			&& std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor))
		){
			Device_Ctrl device{};
			log("go into automation process");
			std::cout << device.process_automation(jsonH, str_argv[2]) << std::endl;
					
			return true;
		}
		
	};
#endif //__linux__
	
private:

	void log(const std::string& new_log) {
		logger->log(new_log);
	};

private:
	//logs
	std::shared_ptr<ErrorLogger> logger;
	std::shared_ptr<ErrorLogger> output_logger;
	
	//pointer
	std::shared_ptr<Cmd_Ctrl> ctrl;
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<Arg_Manager> arg_man;
	std::unique_ptr<CLI_UI> cli;
	
	//arguments as strings
	std::vector<std::string> str_argv;
	int argc;
};


int main(int argc, char* argv[]){
	
	STD_Master master{argc, argv};
	master.run_cli();
	
	return 0;
}
