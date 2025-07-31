#ifdef _WIN32
	#include <windows.h>

#else
	#include <limits.h>
	#include <unistd.h>
#endif

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

#include "json.hpp"

#include "clock.h"
#include "time_account.h"
#include "json_handler.h"
#include "translator.h"
#include "cmd_ctrl.h"
#include "arg_manager.h"
#include "cli_ui.h"

// SIMPLE TIME DOCUMENTATION /* github.com/eichi150/std */

using json = nlohmann::json;

int main(int argc, char* argv[]){
	
	if(argc > 1){
		std::shared_ptr<Cmd_Ctrl> ctrl = std::make_shared<Cmd_Ctrl>();
		try{
			
			//Argumente entgegen nehmen und Parsen
			std::vector<std::string> str_argv = ctrl->parse_argv(argc, argv);
			if(ctrl->is_argument_valid(str_argv)){
				
				std::shared_ptr<JSON_Handler> jsonH = std::make_shared<JSON_Handler>();

		#ifdef __linux__
				auto regex_pattern = ctrl->get_regex_pattern();
				
				//Vorher Automation ausführen
				if(	str_argv.size() >= 4 
					&& std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
					&& std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor))
				){
					Device_Ctrl device{ctrl->get_str_error().at(error::unknown)};
					
					std::cout << device.process_automation(jsonH, str_argv[2]) << std::endl;
					
					return 0;
				}

		#endif //__linux__
				
				//Init Argument Manager
				std::shared_ptr<Arg_Manager> arg_man = std::make_shared<Arg_Manager>(jsonH, ctrl);
				
				arg_man->proceed_inputs(argc, str_argv);
				//CLI UserInterface & ConsoleOutput				
				std::unique_ptr<CLI_UI> cli = std::make_unique<CLI_UI>(arg_man, ctrl->get_log());
				cli->update();
				
				return 0;
			}
			
			throw std::runtime_error{ctrl->get_str_error().at(error::unknown)};
			
		//Error Output	
		}catch(const std::runtime_error& re){
			std::cerr << "**" << re.what() << "\n" << ctrl->get_log() << std::endl;
		}
		
	}
	
	std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
	
	return 0;
}
