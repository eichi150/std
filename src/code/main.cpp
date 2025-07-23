#include "clock.h"
#include "enum_class.h"
#include "time_account.h"
#include "json_handler.h"
#include "translator.h"
#include "arg_manager.h"

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

// SIMPLE TIME DOCUMENTATION /* github.com/eichi150/std */

using json = nlohmann::json;

//Check for valid Arguments
bool is_argument_valid(const std::vector<std::string>& str_argv, const std::map<command, std::regex>& regex_pattern ){
	
	for(const auto& pattern : regex_pattern){
		for(size_t i{1}; i < str_argv.size(); ++i){
			if(std::regex_match(str_argv[i], pattern.second)){
			
				return  true;
			}	
		}
	}
	
	return false;
}

	
int main(int argc, char* argv[]){

	//Argumente entgegen nehmen und in vector<string> speichern
	std::vector<std::string> str_argv;
	for(int i{0}; i < argc; ++i){
		std::string arg = argv[i];
		str_argv.push_back(arg);
	}
	argv = {};

	std::map<command, std::regex> regex_pattern = {
		  { command::help,      		std::regex{R"(^(--?h(elp)?|help)$)", std::regex_constants::icase } }
		, { command::add,       		std::regex{R"(^(--?a(dd)?|add)$)", std::regex_constants::icase } }
		, { command::show,      		std::regex{R"(^(--?sh(ow)?|sh|show)$)", std::regex_constants::icase } }
		, { command::delete_,   		std::regex{R"(^(--?d(elete)?|del(ete)?)$)", std::regex_constants::icase } }
		, { command::hours, 			std::regex{R"(^(--?h(ours)?|h|hours)$)", std::regex_constants::icase } }
		, { command::minutes, 		std::regex{R"(^(--?m(inutes)?|m|minutes)$)", std::regex_constants::icase} }
		, { command::config_filepath, std::regex{R"(^--?cf$)", std::regex_constants::icase } }
		, { command::user_filepath,  	std::regex{R"(^(--?f(ilepath)?|filepath)$)", std::regex_constants::icase } }
		, { command::language,  		std::regex{R"(^(--?l(anguage)?|language)$)", std::regex_constants::icase } }
	};

	std::map<error, std::string> str_error{
		  {error::double_entity, "Entity already taken"}
		, {error::double_alias, "Alias already taken"}
		, {error::alias_equal_entity, "Alias cant be equal to any Entity"}
		, {error::unknown_alias_or_entity, "Alias or Entity not found"}
		, {error::user_input_is_command, "Rejected Input"}
		, {error::not_found, "Not found"}
		, {error::synthax, "Synthax wrong"}
		, {error::untitled_error,"Untitled Error"}
		, {error::unknown, "Unknown Command"}
		, {error::unknown_alias, "Unknown Alias"}
		, {error::unknown_language, "Unknown Language"}
	};
	
	if(argc > 1){
		try{
			if(is_argument_valid(str_argv, regex_pattern)){
				//Initalize
				std::vector<Time_Account> all_accounts{};
				JSON_Handler jsonH{all_accounts};

				//Init Argument Manager
				Arg_Manager arg_man{std::make_shared<JSON_Handler>(jsonH), str_argv, argc, regex_pattern};
			
				arg_man.proceed_inputs(all_accounts, str_error);
				
			}else{
			
				throw std::runtime_error{str_error.at(error::unknown)};
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
