#ifndef ENTITY_COMMAND_H
#define ENTITY_COMMAND_H

#include <vector>
#include <memory>

#include "command.h"
#include "../time_account.h"
#include "../json_handler.h"

class Entity_Command : public Command{
public:
	Entity_Command(
		const std::vector<std::string>& str_argv
		, std::vector<Time_Account>& all_accounts
		, const std::string& arg_entity
		
	) : str_argv(str_argv)
		, all_accounts(all_accounts)
		, entity(arg_entity)
	{
		std::copy_if(
			all_accounts.begin(), all_accounts.end(),
			std::back_inserter(entity_accounts),
			[this](const auto& account){
			return account.get_entity() == entity;
			}
		);
		log("Accounts found: " + std::to_string(entity_accounts.size()) );	
	};
	
	void execute() override = 0;
	
	std::vector<Time_Account> get_entity_accounts() const {
		return entity_accounts;
	}
	
protected:
	std::vector<std::string> str_argv;
	std::vector<Time_Account>& all_accounts;
	
	//All Accounts from that Entity
	std::string entity;
	std::vector<Time_Account> entity_accounts;
	
}; // Entity_Command


class Delete_Entity_Command : public Entity_Command{
public:
	Delete_Entity_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::string& arg_entity
	
	) : Entity_Command(std::move(str_argv), all_accounts, arg_entity)
		, jsonH(jsonH)
	{};
	
    void execute() override {
		
		erase_entity_accounts_out();
		
		finalize();
		
		add_output("All Accounts from " + entity + " deleted");
    }
    
private:
	std::shared_ptr<JSON_Handler> jsonH;
	std::vector<Time_Account> entity_accounts;
	std::vector<Time_Account> fresh_vec;
	
	void erase_entity_accounts_out(){
	
		log("AllAccounts: " + std::to_string(all_accounts.size() ) );
		
		std::copy_if(
			all_accounts.begin(), all_accounts.end(),
			std::back_inserter(fresh_vec),
			[this](const auto& account){
					
				return account.get_entity() != this->entity;
			}
		);
		
		log("freshVec: " + std::to_string(fresh_vec.size() ) );
	}
	
	void finalize(){
		jsonH->save_json_accounts(fresh_vec);
		jsonH->save_json_entity(fresh_vec, this->entity);
		log("saved");
		all_accounts = fresh_vec;
	}
	
}; // Delete_Entity_Command


#endif // ENTITY_COMMAND_H
