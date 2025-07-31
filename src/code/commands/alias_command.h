#ifndef COMMAND_ALIAS_H
#define COMMAND_ALIAS_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>

#include "../json_handler.h"
#include "../translator.h"
#include "../clock.h"
#include "../cmd_ctrl.h"

#include "command.h"

class Alias_Command : public Command{
public:
	Alias_Command(
		const std::vector<std::string>& str_argv
		, std::vector<Time_Account>& all_accounts
	
	) : str_argv(str_argv)
		, all_accounts(all_accounts)
	
	{
		//get Account from Alias
		std::any_of( all_accounts.begin(), all_accounts.end(),
			[this](const Time_Account& acc){
				if(this->str_argv[1] == acc.get_alias()){
					account = std::make_shared<Time_Account>(acc);
					return true;
				}
			return false;
		});
	};
	
	std::string get_log() const override = 0;
	std::string get_user_log() const override = 0;
	void execute() override = 0;
	
	std::shared_ptr<Time_Account> get_account(){
		return account;
	}
	
protected:
	//Alias Account
	std::shared_ptr<Time_Account> account;
	std::vector<Time_Account>& all_accounts;
	std::vector<std::string> str_argv;	
};


class Delete_Alias_Command : public Alias_Command{
public:
	Delete_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
	
	) : Alias_Command(std::move(str_argv), all_accounts)
		, jsonH(jsonH)
		, str_error(str_error)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	}
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		cmd_log << "execute Delete_Command\n";
		if(all_accounts.empty()){
			throw std::runtime_error{str_error.at(error::untitled_error)};
		}
		
		std::string entity;
		std::string alias;
		
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		std::vector<Time_Account> adjusted_accounts;
		
		cmd_log << "account_size: " << all_accounts.size() << '\n';
		//remove out of all_accounts
		std::copy_if(all_accounts.begin(), all_accounts.end(),
			std::back_inserter(adjusted_accounts),
			[this](const Time_Account& acc){
				if(acc.get_alias() == this->account->get_alias()){
					cmd_log << this->account->get_alias(); 
					return false;
				}
				
				return true;
			}
		);
		cmd_log << "after_copy account_size: " << adjusted_accounts.size() << '\n';
		
		//update Files
		jsonH->save_json_accounts(adjusted_accounts);
		jsonH->save_json_entity(adjusted_accounts, entity);
		
		all_accounts = adjusted_accounts;
		cmd_log << " deleted\n";//translator.language_pack.at("deleted_out_of_accounts.json") << std::endl;
	}
	
	
private:

	std::shared_ptr<JSON_Handler> jsonH;
	const std::map<error, std::string>& str_error;
	
}; // Delete_Alias_Command

#ifdef __linux__
#include "../device_ctrl.h"
class Activate_Alias_Command : public Alias_Command{
public:
	Activate_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH

	) : Alias_Command(std::move(str_argv), all_accounts)
		, regex_pattern(regex_pattern)
		, str_error(str_error)
		, jsonH(jsonH)
		
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		
		if( std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor)) ){
		#ifdef __linux__   
			cmd_log << "measure_sensor\n";
			configure_BME280();
		    
		#else
		    cmd_log << "Only available for Linux" << std::endl;
		    user_output_log << "Only available for Linux" << std::endl;
		    
		#endif // __linux__
		}else{
			cmd_log << str_error.at(error::synthax) << "\n";
			throw std::runtime_error{str_error.at(error::synthax)};
		}	
	};
	
	void configure_BME280(){
		
		cmd_log << "start configuration BME280";
		
		str_argv[2] = "BME280";
		
		Device_Ctrl device{str_error.at(error::sensor)};
		
		if(account){
			//an den beginn von str_argv die entity speichern -> für automation_config file
			this->str_argv[0] = account->get_entity();
		}else{
			throw std::runtime_error{ str_error.at(error::unknown_alias) };
		}
		    
		std::vector<command> commands = {
			command::logfile
			, command::minutes
			, command::hours
			, command::clock
			, command::day
		};
		
		//SET Device Settings
		std::stringstream ss;
		ss << device.set_user_automation_crontab(
		    str_argv
		    , jsonH
		    , device.get_specific_regex_pattern(commands)
		);
		
		user_output_log << ss.str();
		cmd_log << ss.str() << "configuration end";
	}
private:

	const std::map<command, std::regex>& regex_pattern;
	const std::map<error, std::string>& str_error;
	std::shared_ptr<JSON_Handler> jsonH;
	
}; // Activate_Alias_Command
#endif // __linux__



#endif // COMMAND_ALIAS_H
