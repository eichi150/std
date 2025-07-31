#ifndef SHOW_COMMAND_H
#define SHOW_COMMAND_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>

#include "../time_account.h"
#include "../json_handler.h"
#include "../translator.h"
#include "../cmd_ctrl.h"
#include "command.h"

enum class OutputType{
	show_all_accounts = 0
	, show_help
	, show_filepaths
	, show_language
	, show_specific_table
	, show_alias_table
	, show_alias_automation
	, show_all_log
	, show_user_output_log
	, COUNT //maxSize Bitset
};


using OutputBitset = std::bitset<static_cast<size_t>(OutputType::COUNT)>;


class Show_Command : public Command{
public:
	Show_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, int argc
		, const std::map<command, std::regex>& regex_pattern
		, OutputBitset& output_flags
		, const std::map<error, std::string>& str_error
		
	) : str_argv(str_argv)
	, all_accounts(all_accounts)
	, jsonH(jsonH)
	, argc(argc)
	, regex_pattern(regex_pattern)
	, output_flags(output_flags)
	, str_error(str_error)	
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	}
	std::string get_user_log() const override{	
		return user_output_log.str();
	}
	
void execute() override {
	cmd_log << "execute Show_Command\n";
		
	switch(argc){
		case 2:
		{
			with_2_args();
			break;
                }
		case 3:
		{
			with_3_args();
			break;
		}
		case 4:
		{
			with_4_args();
			break;
		}
		default:
		{
			cmd_log << "nothing activated to be shown\n";
			break;
		}
	};
};

private:
	std::vector<Time_Account>& all_accounts;
	std::vector<std::string> str_argv;
	std::shared_ptr<JSON_Handler> jsonH;
	int argc;
	std::map<command, std::regex> regex_pattern;
	OutputBitset& output_flags;
	std::map<error, std::string> str_error;
	
	void with_2_args(){
		//Zeige alle <entity> und <alias> an
        //show
        if (std::regex_match(str_argv[1], regex_pattern.at(command::show))) {
			output_flags.set(static_cast<size_t>(OutputType::show_all_accounts));
		    cmd_log << "show all accounts\n";
            return;
        }
        throw std::runtime_error{str_error.at(error::synthax)};
    }

	void with_3_args(){
		//Zeige Filepaths, Language Setting, Alias Tabelle an
        //show filepath
        if(std::regex_match(str_argv[2], regex_pattern.at(command::config_filepath))){
                    
            output_flags.set(static_cast<size_t>(OutputType::show_filepaths));
            cmd_log << str_argv[2] << " show config_filepath.json\n";
			return;
        }
        //language anzeigen
        if(std::regex_match(str_argv[2], regex_pattern.at(command::language))){
                                       
			output_flags.set(static_cast<size_t>(OutputType::show_language));
			cmd_log << str_argv[2] << " show language_setting\n";
            return;    
        }
        //Zeige spezifischen Account an
        //show <alias>
        if(find_alias()){
			output_flags.set(static_cast<size_t>(OutputType::show_alias_table));
			
			cmd_log << str_argv[2] << " show alias table\n";
			return;
		}else{
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}	
	}
	
	void with_4_args(){
		//show automation für alias
		//.std sh <alias> -activate
        if( std::regex_match(str_argv[3], regex_pattern.at(command::activate)) 
			&& find_alias() ) 
        {  
			
			output_flags.set(static_cast<size_t>(OutputType::show_alias_automation));
			cmd_log << "show automation from alias table\n";
			return;
			
		}
		throw std::runtime_error{str_error.at(error::synthax)};
	}
	
	bool find_alias(){
		std::string alias = str_argv[2];
        auto it = std::find_if(
			all_accounts.begin(), all_accounts.end(),
			[&alias](const Time_Account& acc){
				return acc.get_alias() == alias;
			}
        );
        if(it != all_accounts.end()){
			return true;
		}
		throw std::runtime_error{str_error.at(error::unknown_alias)};
	}
		
}; // Show_Command

#endif // SHOW_COMMAND_H
