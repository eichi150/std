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

int main(int argc, char* argv[]){
	
	if(argc > 1){
		std::shared_ptr<ErrorLogger> logger = std::make_shared<Default_Logger>();
		std::shared_ptr<ErrorLogger> output_logger = std::make_shared<User_Logger>();
		
		
		std::shared_ptr<Cmd_Ctrl> ctrl = std::make_shared<Cmd_Ctrl>();
		
		try{
			//Argumente entgegen nehmen und Parsen
			std::vector<std::string> str_argv = ctrl->parse_argv(argc, argv);
			if(ctrl->is_argument_valid(argc, str_argv)){
				
				std::shared_ptr<JSON_Handler> jsonH = std::make_shared<JSON_Handler>();

		#ifdef __linux__
				auto regex_pattern = ctrl->get_regex_pattern();
				
				//Vorher Automation ausführen
				if(	str_argv.size() >= 4 
					&& std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
					&& std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor))
				){
					Device_Ctrl device{};
					
					std::cout << device.process_automation(jsonH, str_argv[2]) << std::endl;
					
					return 0;
				}
		#endif //__linux__
				
				//Init Argument Manager
				std::shared_ptr<Arg_Manager> arg_man = std::make_shared<Arg_Manager>(
					logger
					, output_logger
					, jsonH
					, ctrl
				);
				arg_man->proceed_inputs(argc, str_argv);
				
				//CLI UserInterface & ConsoleOutput				
				std::unique_ptr<CLI_UI> cli = std::make_unique<CLI_UI>(
					arg_man
					, jsonH
					, arg_man->get_translator_ptr()
				);
				cli->update();
				
				if(ctrl->debug_enable){
					std::cout << "===== LOG =====\n"
						<< logger->get_logs() 
						<< std::endl;
				}
				return 0;
			}
			throw std::invalid_argument{"Invalid Arguments"};
			
	//Error Output
		}catch(const Logged_Error& le){
			
			std::cerr << le.what() << std::endl;
			if(ctrl->debug_enable){
				auto logger = le.get_logger();
				if(logger){
					std::cout << "===== LOG =====\n"
						<< logger->get_logs()
						<< std::endl;
				}
			}
		}catch(const SyntaxError& se){
			
			std::cerr << "**" << se.what() << std::endl;
		
		}catch(const DeviceConnectionError& de){
			
			std::cerr << "**" << de.what() << std::endl;
			
		}catch(const std::runtime_error& re){
			
			std::cerr << "**" << re.what() << std::endl;
			
		}catch(const std::invalid_argument& ia){
			
			std::cerr << "**" << ia.what() << std::endl;
			
		}
		
		
	}else{
		std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
	}
	
	return 0;
}
