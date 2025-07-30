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
#include "enum_class.h"
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
		try{
			Cmd_Ctrl ctrl{};
			
			//Argumente entgegen nehmen und Parsen
			
			std::vector<std::string> str_argv = ctrl.parse_argv(argc, argv);

			if(ctrl.is_argument_valid(str_argv)){
				
				std::shared_ptr<JSON_Handler> jsonH = std::make_shared<JSON_Handler>();

		#ifdef __linux__
				auto regex_pattern = ctrl.get_regex_pattern();
				
				//Vorher Automation ausführen
				if(	str_argv.size() >= 4 
					&& std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
					&& std::regex_match(str_argv[3], regex_pattern.at(command::messure_sensor))
				){
					Device_Ctrl device{ctrl.get_str_error().at(error::unknown)};
					
					std::cout << device.process_automation(jsonH, str_argv[2]) << std::endl;
					
					return 0;
				}

		#endif //__linux__
				
				//Init Argument Manager
				std::shared_ptr<Arg_Manager> arg_man = std::make_shared<Arg_Manager>(jsonH, std::make_shared<Cmd_Ctrl>(ctrl));
				
				arg_man->proceed_inputs(argc, str_argv);

				std::unique_ptr<CLI_UI> cli = std::make_unique<CLI_UI>(arg_man);

				//Environment
				if(arg_man->run_environment()){
					std::cout << "std - Environment started... |-- Close with 'exit'" << std::endl;
					do{
						bool arg_valid = false;
						std::vector<std::string> new_argv{"std"};
						
						do{
							//New Inputs parsen
							std::string input;
							std::cout << "@std > ";
							std::getline(std::cin, input);
												
							if(input == "exit"){
								return 0;
							}
							
							new_argv = ctrl.split_input(input);
							new_argv.insert(new_argv.begin(), "std");
							new_argv = ctrl.parse_argv(new_argv.size(), new_argv);
							arg_valid = ctrl.is_argument_valid(new_argv);
							
							for(const auto& arg : new_argv){
								std::cout << arg << '\n';
							}
							
						}while(!arg_valid);
						
						str_argv = new_argv;
						argc = static_cast<int>(str_argv.size());

						try{
				
							arg_man->proceed_inputs(argc, str_argv);
							
						}catch(const std::runtime_error& rt){
							std::cout << "**" << rt.what() << std::endl;
						}
					}while(arg_man->run_environment());
				}

				//CLI UserInterface & ConsoleOutput				
				cli->update();

				
			}else{
			
				throw std::runtime_error{ctrl.get_str_error().at(error::unknown)};
			}
		//Error Output	
		}catch(const std::runtime_error& re){
			std::cerr << "**" << re.what() << std::endl;
		}
		
	}else{
	
		std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
	}
	
	return 0;
}
