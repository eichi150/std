#ifndef CMD_CTRL_H
#define CMD_CTRL_H

#include <regex>
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "./abstract_class/ctrl.h"
#include "./exception/exception.h"

class Cmd_Ctrl : public CTRL::Ctrl{
public:
	
	Cmd_Ctrl(std::shared_ptr<ErrorLogger> logger_);
	
	// Gibt eine Liste von Tokens zurück, getrennt durch Leerzeichen
	std::vector<std::string> split_input(const std::string& input);
	//Check for valid Arguments
	bool is_argument_valid(int& argc, std::vector<std::string>& str_argv);
	void check_debug_mode(int& argc, std::vector<std::string>& str_argv);
	
	template <typename T>
	std::vector<std::string> parse_argv(int argc, T& argv){
		std::vector<std::string> str_argv;
		for(int i{0}; i < argc; ++i){
		
				std::string arg = argv[i];

				//Crontab Command nicht parsen bei automatischer abfrage
				if( i + 1 <= argc
					&& std::regex_match(arg, regex_pattern.at(command::automatic)))
				{
					str_argv.push_back(arg);
					
					std::string crontab_command = argv[i + 1];
					str_argv.push_back(crontab_command);
					++i;
					continue;
				}
				
				auto it = split_input(arg);
				if(!it.empty()){
					for(const auto& split : it){
						str_argv.push_back(split);
					}
				}
			}
			
			argv = {};
			
			//Log Output
			ctrl_log << std::to_string(argc) << "\n";
			for(const auto& str : str_argv){
				ctrl_log << str << "\n";
			}
		return str_argv;
	}

private:
	std::shared_ptr<ErrorLogger> logger;
	
	void log(const std::string& msg){
		if(logger){
			logger->log(msg);
		}
	}
};

#endif // CMD_CTRL_H
