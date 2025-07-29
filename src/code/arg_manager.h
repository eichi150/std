#ifndef ARG_MANAGER_H
#define ARG_MANAGER_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>

#include "json_handler.h"
#include "translator.h"
#include "clock.h"
#include "cmd_ctrl.h"
#include "enum_class.h"

#ifdef __linux__
	#include "device_ctrl.h"
#endif

class Command{
public:
	
	virtual ~Command() = default;
	
	virtual void execute() = 0;
	
	virtual std::string get_log() const = 0;

};

class Delete_Command : public Command{
public:
	Delete_Command(
		std::vector<Time_Account>& all_accounts
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
		, const std::string& alias_to_delete
	
	) : all_accounts(all_accounts),
		jsonH(std::move(jsonH)),
		str_error(str_error),
		alias_to_delete(alias_to_delete) {};
	
	void execute() override{
		delete_cmd_log << "execute Delete_Command\n";
		if(all_accounts.empty()){
			throw std::runtime_error{str_error.at(error::untitled_error)};
		}
		
		bool found_alias = false;
		std::string entity;
		std::string alias;
	
		for(const auto& account : all_accounts){
			if(account.get_alias() == alias_to_delete){
				entity = account.get_entity();
				alias = account.get_alias();
				found_alias = true;
				break;
			}
		}
		if(!found_alias){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		std::vector<Time_Account> adjusted_accounts;
		
		delete_cmd_log << "account_size: " << all_accounts.size() << '\n';
		//remove out of all_accounts
		std::copy_if(all_accounts.begin(), all_accounts.end(),
			std::back_inserter(adjusted_accounts),
			[this](const Time_Account& account){
				if(account.get_alias() == this->alias_to_delete){
					return false;
				}
				return true;
			}
		);
		delete_cmd_log << "after_copy account_size: " << adjusted_accounts.size() << '\n';
		
		//update Files
		jsonH->save_json_accounts(adjusted_accounts);
		jsonH->save_json_entity(adjusted_accounts, entity);
		
		all_accounts = adjusted_accounts;
		delete_cmd_log << alias_to_delete << " deleted\n";//translator.language_pack.at("deleted_out_of_accounts.json") << std::endl;
	}
	
	std::string get_log() const override {
		return delete_cmd_log.str();
	}
	
private:
	std::stringstream delete_cmd_log;
	
	std::vector<Time_Account>& all_accounts;
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<error, std::string> str_error;
	std::string alias_to_delete;
};



struct Add_account{
	std::string entity;
	std::string alias;
	std::string tag;
	
};

class Add_Command : public Command{
public:
	Add_Command(
		std::vector<Time_Account>& all_accounts
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
		, const Add_account& add
		, const std::regex& pattern
	) 
		
		: all_accounts(all_accounts),
		jsonH(jsonH),
		str_error(str_error),
		add(add),
		regex_pattern(pattern) 
		{
			add_cmd_log << "+Add_Command\n";
		};
	
	void execute() override{
		add_cmd_log << "execute Add_Command\n";
		std::stringstream ss;
		//Entity or Alias cant be named as a command
		
		if(std::regex_match(add.entity, regex_pattern)){
			throw std::runtime_error{str_error.at(error::user_input_is_command)};
				
		}	
		if(std::regex_match(add.alias, regex_pattern)){
			throw std::runtime_error{str_error.at(error::user_input_is_command)};
				
		}
		
		//Entity or Alias already taken?
		for(const auto& account : all_accounts){
			if(account.get_alias() == add.alias){
				throw std::runtime_error{str_error.at(error::double_alias)};
			}
			if(account.get_alias() == add.entity){
				throw std::runtime_error{str_error.at(error::alias_equal_entity)};
			}
		}
		
		Time_Account new_account{add.entity, add.alias};
		
		all_accounts.push_back(new_account);

		jsonH->save_json_accounts(all_accounts);
		
		jsonH->save_json_entity(all_accounts, add.entity);	
		
		ss << add.entity << "-> " << add.alias;
		if(!add.tag.empty()){
			new_account.set_tag(add.tag);
			ss <<  " -tag= " << add.tag;
		}
		ss << " Saved";
		add_cmd_log << ss.str() << std::endl;	
	};
	
	std::string get_log() const override{
		return add_cmd_log.str();
	}
	
private:
	std::stringstream add_cmd_log;
	
	std::vector<Time_Account> all_accounts;
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<error, std::string> str_error;
	Add_account add;
	std::regex regex_pattern;
};


enum class OutputType{
	show_all_accounts = 0
	, show_filepaths
	, show_language
	, show_specific_table
	, show_alias_table
	, show_alias_automation
	, arg_manager_log
	, COUNT //maxSize Bitset
};

using OutputBitset = std::bitset<static_cast<size_t>(OutputType::COUNT)>;

class Arg_Manager{
public:
	Arg_Manager(std::shared_ptr<JSON_Handler> jH, std::shared_ptr<Cmd_Ctrl> ctrl_ptr);
	
	void proceed_inputs(const int& _argc, const std::vector<std::string>& argv);

	bool run_environment() const { return run_env; }
	
	std::shared_ptr<JSON_Handler> jsonH;
	Translator translator{};
	
	void set_output_flag(OutputType flag, bool value = true){
		output_flags.set(static_cast<size_t>(flag), value);
	}
	OutputBitset get_output_flags() const {
		return output_flags;
	}
	void clear_output_flags(){
		output_flags.reset();
	}
	
	std::vector<Time_Account> get_all_accounts() const {
		return all_accounts;
	}
	
	std::shared_ptr<Time_Account> get_account_with_alias(const std::string& alias);
	
	std::vector<std::string> get_str_argv() const {
		return str_argv;
	}
	
	std::string get_log() const {
		std::ostringstream log;
		log 
			<< "Process Log:\nArg_Manager:\n"
			<< arg_manager_log.str()
			<< "\nJSON_Handler:\n"
			<< jsonH->get_log()
			<< std::endl;
			
		return log.str();
	}
	
private:

#ifdef __linux__
	std::string add_sensor_data(const std::shared_ptr<Device_Ctrl>& device, std::vector<Time_Account>& all_accounts);
#endif // __linux__
	
	std::unique_ptr<Command> cmd;
	
	OutputBitset output_flags;
	
	std::stringstream arg_manager_log;
	
	bool run_env = false;
	
	std::shared_ptr<Cmd_Ctrl> ctrl;
	
	std::vector<Time_Account> all_accounts;
	
	Clock clock{};
	
	std::vector<std::string> str_argv;
	int argc;
	
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;

	std::vector<Time_Account> check_for_alias_or_entity(const std::vector<Time_Account>& all_accounts, const std::string& alias_or_entity);
	
	//Sammle alle Accounts mit diesem Alias
	std::vector<Time_Account> collect_accounts_with_alias(const std::vector<Time_Account>& search_in, const std::string& search_for_alias);

	void change_config_json_language(const std::string& to_language);

	void change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath);

	void user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath);
	
	//void delete_account(std::vector<Time_Account>& all_accounts, const std::string& alias_to_delete);
	

	void add_tag_to_account(std::vector<Time_Account>& all_accounts, const std::string& tag);
	
	void add_hours(std::vector<Time_Account>& all_accounts, const std::string& amount);



	const std::string help = {
    "add 			Add new Entity give it a Alias\n"
    "-h -m  		Time to save in Hours or Minutes\n"
    "show 			Show all Entitys and Alias's\n"
    "sh 			Short Form of show\n"
    "show 'ALIAS' 	show specific Entity's Time Account\n\n"
    "For more Information have a look at README.md on github.com/eichi150/std\n"
    };
	    
	
};//Arg_Manager

#endif // ARG_MANAGER_H
