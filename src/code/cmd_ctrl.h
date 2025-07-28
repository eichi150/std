#ifndef CMD_CTRL_H
#define CMD_CTRL_H

#include <regex>
#include <vector>
#include <map>
#include <string>
#include "enum_class.h"
#include "control.h"

class Cmd_Ctrl : public Ctrl{
public:
	Cmd_Ctrl(){};
	~Cmd_Ctrl(){};
	
	// Gibt eine Liste von Tokens zurück, getrennt durch Leerzeichen
	std::vector<std::string> split_input(const std::string& input);
	
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
			/*for(const auto& str : str_argv){
				std::cout << argc << " "  << str << std::endl;
			}*/
		return str_argv;
	}
	
	//Check for valid Arguments
	bool is_argument_valid(const std::vector<std::string>& str_argv);
	
};

#endif // CMD_CTRL_H
