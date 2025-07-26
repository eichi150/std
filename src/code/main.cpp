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
#include "arg_manager.h"

// SIMPLE TIME DOCUMENTATION /* github.com/eichi150/std */

using json = nlohmann::json;

#include <vector>
#include <string>
#include <sstream>
#include <regex>

// Gibt eine Liste von Tokens zurück, getrennt durch Leerzeichen oder Komma
std::vector<std::string> split_input(const std::string& input) {
    std::vector<std::string> result;
    std::regex re(R"([\s]+)"); // regex: trennt an Leerzeichen ODER Kommas
    std::sregex_token_iterator it(input.begin(), input.end(), re, -1);
    std::sregex_token_iterator end;

    for (; it != end; ++it) {
        if (!it->str().empty()) {
            result.push_back(it->str());
        }
    }

    return result;
}


int main(int argc, char* argv[]){
	
	//Argumente entgegen nehmen und in vector<string> speichern
	std::vector<std::string> str_argv;
	for(int i{0}; i < argc; ++i){

		std::string arg = argv[i];
		auto it = split_input(arg);
		if(!it.empty()){
			for(const auto& split : it){
				str_argv.push_back(split);
			}
		}
	}
	argv = {};
	
	/*for(const auto& str : str_argv){
		std::cout << str << '\n';
	}
	std::cout << argc << std::endl;*/
	
	/*if(static_cast<size_t>(argc) != str_argv.size()){
		std::cout << "!!Argument Error" << std::endl;
		return 1;
	}*/
	
	if(argc > 1){
		try{
			Cmd_Ctrl ctrl{};
			
			if(ctrl.is_argument_valid(str_argv)){
			
				std::shared_ptr<JSON_Handler> jsonH = std::make_shared<JSON_Handler>();

		#ifdef __linux__
				//Vorher Automation ausführen
				auto regex_pattern = ctrl.get_regex_pattern();
				if(std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
					&& std::regex_match(str_argv[3], regex_pattern.at(command::messure_sensor))
				){
					Device_Ctrl device{ctrl.get_str_error().at(error::unknown)};
					device.process_automation(jsonH, str_argv[2]);
					
					return 0;
				}
		#endif //__linux__

			
				//Init Argument Manager
				Arg_Manager arg_man{jsonH, std::make_shared<Cmd_Ctrl>(ctrl)};
				
				arg_man.proceed_inputs(argc, str_argv);
				
				if(arg_man.run_environment()){
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
							
							new_argv = split_input(input);
							new_argv.insert(new_argv.begin(), "std");

							arg_valid = ctrl.is_argument_valid(new_argv);
							
						}while(!arg_valid);
						
						str_argv = new_argv;
						argc = static_cast<int>(str_argv.size());

						try{
				
							arg_man.proceed_inputs(argc, str_argv);
							
						}catch(const std::runtime_error& rt){
							std::cout << "**" << rt.what() << std::endl;
						}
					}while(arg_man.run_environment());
				}
				
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
