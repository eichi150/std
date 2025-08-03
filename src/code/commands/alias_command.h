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

#include "command.h"
#include "../time_account.h"
#include "../json_handler.h"
#include "../translator.h"
#include "../clock.h"
//#include "../cmd_ctrl.h"

class Alias_Command : public Command{
public:
	Alias_Command(
		const std::vector<std::string>& str_argv
		, std::vector<Time_Account>& all_accounts
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
		
	) : Command(std::move(logger))
		, str_argv(str_argv)
		, all_accounts(all_accounts)
		, alias(arg_alias)
	
	{
		
		log(std::string{__FILE__} + " - Alias_Command");
	
		log("search account with alias");
		get_account_from_alias();
	};
	
	void execute() override = 0;
	
	std::shared_ptr<Time_Account> get_account(){
		return account;
	}
	
protected:
	//Alias Account
	std::shared_ptr<Time_Account> account;
	std::vector<Time_Account>& all_accounts;
	std::vector<std::string> str_argv;	
	
	std::string alias;
	std::string entity;
	
private:
	
	void get_account_from_alias(){
		
		//get Account from Alias
		bool found_alias = std::any_of( 
			all_accounts.begin(), all_accounts.end(),
			[this](const Time_Account& acc){
				if(this->alias == acc.get_alias()){
					account = std::make_shared<Time_Account>(acc);
					return true;
				}
			return false;
		});
		
		if(account){
			alias = account->get_alias();
			entity = account->get_entity();
		}
	}
};

//Delete Alias out of Registry
class Delete_Alias_Command : public Alias_Command{
public:
	Delete_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
		
	) : Alias_Command(std::move(str_argv), all_accounts, arg_alias, std::move(logger))
		, jsonH(jsonH)
	{
		log(std::string{__FILE__} + " - Delete_Alias_Command");
	};
	
	
	void execute() override{
		log("execute Delete_Command");
		if(all_accounts.empty()){
			throw Logged_Error("No Accounts found", logger);
		}
		if(!account){
			throw Logged_Error("Unknown Alias", logger);
		}
		
		delete_alias();	
	}
	
private:
	
	/*void delete_all_or_detailed(){
			
		if( std::regex_match(str_argv[3], regex_pattern.at(command::all)) ){
			
			log("delete all from this entity");
			
		}
		if( std::regex_match(str_argv[2], regex_pattern.at(command::detail)) ){
				
			log("delete detail from this entity");
		}
		
	}*/
	
	void delete_alias(){
		
		std::vector<Time_Account> adjusted_accounts;
		
		log("account_size: " + std::to_string(all_accounts.size()) );
		//remove out of all_accounts
		std::copy_if(all_accounts.begin(), all_accounts.end(),
			std::back_inserter(adjusted_accounts),
			[this](const Time_Account& acc){
				if(acc.get_alias() == this->alias){
					log(this->alias); 
					return false;
				}
				
				return true;
			}
		);
		log( "after_copy account_size: " + std::to_string(adjusted_accounts.size()) );
		
		if(adjusted_accounts.size() < all_accounts.size()){
			
			//update Files
			if(!jsonH){
				throw Logged_Error("jsonH Ptr error", logger);
			}
			jsonH->save_json_accounts(adjusted_accounts);
			jsonH->save_json_entity(adjusted_accounts, this->entity);
			
			all_accounts = adjusted_accounts;
			log(" deleted");
			add_output(alias + " Alias deleted");
		}
	}
	
private:
	std::shared_ptr<JSON_Handler> jsonH;
	
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
		, std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> callback
		, const std::string& arg_alias
		, std::shared_ptr<ErrorLogger> logger
		
	) : Alias_Command(std::move(str_argv), all_accounts, arg_alias, std::move(logger))
		, regex_pattern(regex_pattern)
		, jsonH(jsonH)
		, _split_line(callback)
	{
		log(std::string{__FILE__} + " - Interact_Alias_Command");
	};
	void execute() override{
		
		if( std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor)) ){
			log("measure_sensor");
			
			log("read automation_config");
			automation_config = jsonH->read_automation_config_file();
			
			if(automation_config.empty()){
				add_output("No Automations saved");
				log("No Automations saved");
			} 
			
		#ifdef __linux__  
			
			std::string executioner = "Crontab";
			
			if(executioner == "Crontab"){
				
				interact_with_Crontab();
			}else{
				throw Logged_Error("Executioner " + executioner + " doesnt match", logger);
			}
			
		#else
		    log("Only available for Linux");
		    add_output("Only available for Linux");
		    
		#endif // __linux__
		}
		
		if(interaction){
			interaction->set_logger(logger);
			interaction->set_output_logger(output_logger);
			
			log( "===== Interaction_Log: =====");
			interaction->interact();
			
		}else{
			log("No Interaction to execute");
			throw Logged_Error("No Interaction to execute", logger);
		}	
	};
	
private:
#ifdef __linux__

	void interact_with_Crontab(){
		
		if( std::regex_match(str_argv[2], regex_pattern.at(command::activate)) ){
			
			log("Activate Crontab  - - ");
			str_argv[2] = "BME280";
			if(account){
				//an den beginn von str_argv die entity speichern -> für automation_config file
				str_argv[0] = entity;
			}else{
				throw Logged_Error("Unknown Alias", logger);
			}
			
			log("- WriteInto Crontab - configuration BME280");
			interaction = std::make_shared<write_into_Crontab>(
				str_argv
				, regex_pattern
				, jsonH
				, _split_line
				, automation_config
			);
			
		}
		
		if( std::regex_match(str_argv[2], regex_pattern.at(command::deactivate)) ){
			
			if(automation_config.empty()){
				throw Logged_Error("No Automations saved", logger);
			}
			
			log("deactivate Crontab");
			interaction = std::make_shared<delete_task_from_Crontab>(
				str_argv
				, regex_pattern
				, jsonH
				, _split_line
				, automation_config
			);
			
		}
		
	}
	
#endif // __linux__

private:
	std::shared_ptr<Interaction> interaction;
	std::vector<Automation_Config> automation_config;
	std::vector<Automation_Config> only_crontabs;
	
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<JSON_Handler> jsonH;
	std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line;
	
}; // Interact_Alias_Command

#endif // COMMAND_ALIAS_H
