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

enum class OutputType{
	show_all_accounts = 0
	, show_help
	, show_filepaths
	, show_language
	, show_specific_table
	, show_alias_table
	, show_alias_automation
	, arg_manager_log
	, COUNT //maxSize Bitset
};


using OutputBitset = std::bitset<static_cast<size_t>(OutputType::COUNT)>;

class Command{
public:
	
	virtual ~Command() = default;
	
	virtual void execute() = 0;
	
	virtual std::string get_log() const = 0;

protected:
	std::stringstream cmd_log;
};

class Device_Command : public Command{
public:
	Device_Command(
		const std::map<error, std::string>& str_error
		, const std::string& device_name
		
		
	) : str_error(str_error)
		, arg(device_name)
		
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	}
	
	void execute(){
		
		Device_Ctrl device{str_error.at(error::sensor)};
		
		auto it = device.device_regex_pattern.find(arg);
		
		if(it == device.device_regex_pattern.end()){
		    std::stringstream ss;
		    ss << "\nPossible Connections:\n";
			for(const auto& name : device.device_regex_pattern){
				ss << " > " << name.first << '\n';
			}
		    throw std::runtime_error{str_error.at(error::synthax) + ss.str()};
		}
				    
		if(std::regex_match(arg, device.device_regex_pattern.at(arg)) ){

		    
		    std::vector<float> output_sensor = device.check_device(arg);

		    std::stringstream output_str;
					    
			output_str 
			<< "Temperature" << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
			<< "Pressure" << ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
			<< "Humidity" << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
					        
			cmd_log << output_str.str() << std::endl;
					    
		}
	};
	

	
private:
	std::map<error, std::string> str_error;
	std::string arg;
	
};

class Show_Command : public Command{
public:
	
	Show_Command(
		std::shared_ptr<JSON_Handler> jsonH
		, const std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, OutputBitset& output_flags
		, const std::vector<Time_Account>& all_accounts
		, const std::map<error, std::string>& str_error
	
	) : jsonH(jsonH)
		, str_argv(str_argv)
		, regex_pattern(regex_pattern)
		, output_flags(output_flags)
		, all_accounts(all_accounts)
		, str_error(str_error)
	{};
	
	std::string get_log() const override{
		return cmd_log.str();
	}
	
	void execute() override{
		cmd_log << "execute Show_Command\n";
		
		//Zeige Filepaths, Language Setting, Alias Tabelle an
        //show filepath
        if(std::regex_match(str_argv[2], regex_pattern.at(command::config_filepath))){
                    
            output_flags.set(static_cast<size_t>(OutputType::show_filepaths));
            cmd_log << str_argv[2] << " show activated\n";
			return;
        }
        //language anzeigen
        if(std::regex_match(str_argv[2], regex_pattern.at(command::language))){
                                       
			output_flags.set(static_cast<size_t>(OutputType::show_language));
			cmd_log << str_argv[2] << " show activated\n";
            return;    
        }
        //Zeige spezifischen Account an
        //show <alias>
        std::vector<Time_Account> matching_accounts;
        matching_accounts = check_for_alias(str_argv[2]);
                    
        if(matching_accounts.empty()){
			throw std::runtime_error{str_error.at(error::unknown_alias_or_entity)};
        }
	 
		output_flags.set(static_cast<size_t>(OutputType::show_alias_table));
		
		cmd_log << str_argv[2] << " show activated\n";
	};
	
	std::vector<Time_Account> check_for_alias(const std::string& alias){
		std::vector<Time_Account> matching_accounts;
			
		std::copy_if(
			all_accounts.begin(), all_accounts.end(),
			std::back_inserter(matching_accounts),
			[&alias](const Time_Account& acc) {
				return acc.get_alias() == alias;
			}
		);
		
		return matching_accounts;
	}

private:
	std::shared_ptr<JSON_Handler> jsonH;
	std::vector<std::string> str_argv;
	std::map<command, std::regex> regex_pattern;
	OutputBitset& output_flags;
	std::vector<Time_Account> all_accounts;
	std::map<error, std::string> str_error;
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
		cmd_log << "execute Delete_Command\n";
		if(all_accounts.empty()){
			throw std::runtime_error{str_error.at(error::untitled_error)};
		}
		
		std::string entity;
		std::string alias;
		
		bool found_alias = std::any_of(all_accounts.begin(), all_accounts.end(),
			[this, &entity, &alias](const Time_Account& account){
				if(account.get_alias() == this->alias_to_delete){
					entity = account.get_entity();
					alias = account.get_alias();
					return true;
				}
				return false;
			}
		);
		
		if(!found_alias){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		std::vector<Time_Account> adjusted_accounts;
		
		cmd_log << "account_size: " << all_accounts.size() << '\n';
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
		cmd_log << "after_copy account_size: " << adjusted_accounts.size() << '\n';
		
		//update Files
		jsonH->save_json_accounts(adjusted_accounts);
		jsonH->save_json_entity(adjusted_accounts, entity);
		
		all_accounts = adjusted_accounts;
		cmd_log << alias_to_delete << " deleted\n";//translator.language_pack.at("deleted_out_of_accounts.json") << std::endl;
	}
	
	std::string get_log() const override {
		return cmd_log.str();
	}
	
private:
	
	
	std::vector<Time_Account>& all_accounts;
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<error, std::string> str_error;
	std::string alias_to_delete;
}; // Delete_Command



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
	
	) : all_accounts(all_accounts),
		jsonH(jsonH),
		str_error(str_error),
		add(add),
		regex_pattern(pattern) 
	{
		cmd_log << "+Add_Command\n";
	};
	
	void execute() override{
		cmd_log << "execute Add_Command\n";
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
		cmd_log << ss.str() << std::endl;	
	};
	
	std::string get_log() const override{
		return cmd_log.str();
	}
	
private:
	
	std::vector<Time_Account> all_accounts;
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<error, std::string> str_error;
	Add_account add;
	std::regex regex_pattern;
}; // Add_Command



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
	
	std::string get_log() const;
	
private:

#ifdef __linux__
	std::string add_sensor_data(const std::shared_ptr<Device_Ctrl>& device, std::vector<Time_Account>& all_accounts);
#endif // __linux__
	
	std::unique_ptr<Command> cmd;
	OutputBitset output_flags;
	std::stringstream arg_manager_log;
	
	std::shared_ptr<Cmd_Ctrl> ctrl;
	std::vector<Time_Account> all_accounts;
	int argc;
	std::vector<std::string> str_argv;
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
/*====*/
	bool run_env = false;
	
	Clock clock{};

	void change_config_json_language(const std::string& to_language);

	void change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath);

	void user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath);
	
	void add_tag_to_account(std::vector<Time_Account>& all_accounts, const std::string& tag);
	
	void add_hours(std::vector<Time_Account>& all_accounts, const std::string& amount);
	    
	
};//Arg_Manager


#endif // ARG_MANAGER_H
