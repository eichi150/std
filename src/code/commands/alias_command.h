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
#include <functional>

#include "../time_account.h"
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

//Delete Alias out of Registry
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
	std::map<error, std::string> str_error;
	
}; // Delete_Alias_Command

#ifdef __linux__
	#include "handle_crontab.h"
#else
	#include "interaction.h"
#endif // __linux__

class Interact_Alias_Command : public Alias_Command{
public:
	Interact_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> callback

	) : Alias_Command(std::move(str_argv), all_accounts)
		, regex_pattern(regex_pattern)
		, str_error(str_error)
		, jsonH(jsonH)
		, _split_line(callback)
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
			
		    interact_with_Crontab();
		#else
		    cmd_log << "Only available for Linux" << std::endl;
		    user_output_log << "Only available for Linux" << std::endl;
		    
		#endif // __linux__
		}
		
		if(interaction){
			interaction->interact();
			cmd_log 
				<< "\n===== Interaction_Log: =====\n" 
				<< interaction->get_log();
			user_output_log << interaction->get_user_output_log();
		}else{
			cmd_log << str_error.at(error::synthax) << "\n";
			throw std::runtime_error{str_error.at(error::synthax)};
		}	
	};
	
	
private:
	std::stringstream user_output_log;
	std::unique_ptr<Interaction> interaction;
	
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	std::shared_ptr<JSON_Handler> jsonH;
	std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line;
	
	#ifdef __linux__
	void interact_with_Crontab(){
		
		if( std::regex_match(str_argv[2], regex_pattern.at(command::activate)) ){
			
			cmd_log << "Activate Crontab  - - ";
			str_argv[2] = "BME280";
			if(account){
				//an den beginn von str_argv die entity speichern -> für automation_config file
				str_argv[0] = account->get_entity();
			}else{
				throw std::runtime_error{ str_error.at(error::unknown_alias) };
			}
			
			cmd_log << "- WriteInto Crontab - configuration BME280\n";
			interaction = std::make_unique<write_into_Crontab>(
				str_argv
				, regex_pattern
				, str_error
				, jsonH
				, _split_line
			);
			return;
		}
		
		if( std::regex_match(str_argv[2], regex_pattern.at(command::deactivate)) ){
			
			cmd_log << "deactivate Crontab\n";
			interaction = std::make_unique<delete_task_from_Crontab>(
				str_argv
				, regex_pattern
				, str_error
				, jsonH
				, _split_line
			);
			return;
		}
	}
	#endif // __linux__
	
}; // Interact_Alias_Command

#endif // COMMAND_ALIAS_H
