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
bool Cmd_Ctrl::is_argument_valid(int& argc, std::vector<std::string>& str_argv){
	
	check_debug_mode(argc, str_argv);
	
	//commands regex_pattern
	for(const auto& pattern : regex_pattern){
		for(size_t i{0}; i < str_argv.size(); ++i){
			if(std::regex_match(str_argv[i], pattern.second)){
				
				return  true;
			}	
		}
	}
	
	//device_regex_pattern
	for(const auto& pattern : device_regex_pattern){
		for(size_t i{0}; i < str_argv.size(); ++i){
			if(std::regex_match(str_argv[i], pattern.second)){

				return  true;
			}	
		}
	}
	
	return false;
}

void Cmd_Ctrl::check_debug_mode(int& argc, std::vector<std::string>& str_argv){
	//enable show_logfile, erase '-prolog' cmd out of str_argv, argc -=1
    std::vector<std::string> str_argv_without_LOG_cmd;
    std::copy_if(
        str_argv.begin(), str_argv.end(),
        std::back_inserter(str_argv_without_LOG_cmd),
        [this, &argc](const std::string& str) {
	    if(!std::regex_match(str, this->regex_pattern.at(command::debug))){
		return true;
	    }
	    
	    //this->output_flags.set(static_cast<size_t>(OutputType::show_all_log));
	    argc -= 1;
		debug_enable = true;
	    return false;
        }
    );
    str_argv = str_argv_without_LOG_cmd;
}
