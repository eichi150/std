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
		, std::shared_ptr<ErrorLogger> logger
		
	);
	
	void execute() override;
	
	std::vector<Time_Account> get_entity_accounts() const;
	
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
		, std::shared_ptr<ErrorLogger> logger
		
	);
	
    void execute() override;
    
private:
	std::shared_ptr<JSON_Handler> jsonH;
	std::vector<Time_Account> fresh_vec;
	
	void erase_entity_accounts_out();
	
	void finalize();
	
}; // Delete_Entity_Command


#endif // ENTITY_COMMAND_H
