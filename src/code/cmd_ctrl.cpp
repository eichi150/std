#include "cmd_ctrl.h"

// Gibt eine Liste von Tokens zurück, getrennt durch Leerzeichen
	std::vector<std::string> Cmd_Ctrl::split_input(const std::string& input) {
		std::vector<std::string> result;
		std::regex re(R"([\s]+)"); // regex: trennt an Leerzeichen
		
		std::sregex_token_iterator it(input.begin(), input.end(), re, -1);
		std::sregex_token_iterator end;

		for (; it != end; ++it) {
			if (!it->str().empty()) {
				result.push_back(it->str());
			}
		}

		return result;
	}
	
	//Check for valid Arguments
	bool Cmd_Ctrl::is_argument_valid(const std::vector<std::string>& str_argv){
		
		for(const auto& pattern : regex_pattern){
			for(size_t i{1}; i < str_argv.size(); ++i){
				if(std::regex_match(str_argv[i], pattern.second)){
				
					return  true;
				}	
			}
		}
		
		return false;
	}
