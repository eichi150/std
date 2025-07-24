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

	
int main(int argc, char* argv[]){

	//Argumente entgegen nehmen und in vector<string> speichern
	std::vector<std::string> str_argv;
	for(int i{0}; i < argc; ++i){
		std::string arg = argv[i];
		str_argv.push_back(arg);
	}
	argv = {};
	
	if(argc > 1){
		try{
			Cmd_Ctrl ctrl{};
			if(ctrl.is_argument_valid(str_argv)){
				//Initalize
				std::vector<Time_Account> all_accounts{};
				JSON_Handler jsonH{all_accounts};

				//Init Argument Manager
				Arg_Manager arg_man{std::make_shared<JSON_Handler>(jsonH), std::make_shared<Cmd_Ctrl>(ctrl), str_argv, argc};
			
				arg_man.proceed_inputs(all_accounts);
				
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
