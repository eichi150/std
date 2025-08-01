#ifndef COMMAND_H
#define COMMAND_H

#include <sstream>

class Command{
public:
	virtual ~Command() = default;
	
	virtual void execute() = 0;
	virtual std::string get_log() const = 0;
	virtual std::string get_user_log() const = 0;
protected:
	std::stringstream cmd_log;
	std::stringstream user_output_log;
}; // Command

struct Add_account{
	std::string entity;
	std::string alias;
	std::string tag;
};

class Add_Alias : public Command{
public:
	Add_Alias(
		std::vector<Time_Account>& all_accounts
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
		, const Add_account& add
		, const std::map<command, std::regex>& pattern
	
	) : all_accounts(all_accounts),
		jsonH(jsonH),
		str_error(str_error),
		add(add),
		regex_pattern(pattern) 
	{
		cmd_log << "Add_Command\n";
	};
	std::string get_log() const override{
		return cmd_log.str();
	}
	
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		cmd_log << "execute Add_Command\n";
		std::stringstream ss;
		//Entity or Alias cant be named as a command
		bool is_entity = std::any_of(
			regex_pattern.begin(), regex_pattern.end(),
			[this](const auto& pair){
				return std::regex_match(this->add.entity, pair.second);
			}
		);
		bool is_alias = std::any_of(
			regex_pattern.begin(), regex_pattern.end(),
			[this](const auto& pair){
				return std::regex_match(this->add.alias, pair.second);
			}
		);
		if(is_entity || is_alias){
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
private:
	
	std::vector<Time_Account>& all_accounts;
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<error, std::string> str_error;
	Add_account add;
	const std::map<command, std::regex>& regex_pattern;
	
}; // Add_Alias


#ifdef __linux__
#include "../device_ctrl.h"
class TouchDevice_Command : public Command{
public:
	TouchDevice_Command(
		const std::map<error, std::string>& str_error
		, const std::string& device_name
		
		
	) : str_error(str_error)
		, arg(device_name)
		
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	}
	
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override {
		
		Device_Ctrl device{str_error.at(error::sensor)};
		
		auto it = device.device_regex_pattern.find(arg);
		
		if(it == device.device_regex_pattern.end()){
		    std::stringstream ss;
		    ss << "\nPossible Connections:\n";
			for(const auto& name : device.device_regex_pattern){
				ss << " > " << name.first << '\n';
			}
			cmd_log << str_error.at(error::synthax) << ss.str() << '\n';
		    throw std::runtime_error{str_error.at(error::synthax) + ss.str()};
		}
				    
		if(std::regex_match(arg, device.device_regex_pattern.at(arg)) ){

		    
		    std::vector<float> output_sensor = device.check_device(arg);
			if(output_sensor.empty()){
				throw std::runtime_error{"TouchDevice_Command " + str_error.at(error::sensor)};
			}
			
		    std::stringstream output_str; 
			output_str 
				<< "Temperature"<< ": " << std::fixed 	<< std::setprecision(2) << output_sensor[0] << " °C || "
				<< "Pressure" 	<< ": "  << std::fixed 	<< std::setprecision(2) << output_sensor[1] << " hPa || "
				<< "Humidity" 	<< ": "  << std::fixed 	<< std::setprecision(2) << output_sensor[2] << " %\n";
					        
			cmd_log << "Device touched\n" << output_str.str() << std::endl;
			user_output_log 
			<< ( output_sensor.empty() ? "Device touched\n" : output_str.str() )
			<< std::endl;	    
		}
		
		cmd_log << "\n===== Command_Log: =====\n" << device.get_log();
	};
	
private:
	std::map<error, std::string> str_error;
	std::string arg;
	
}; // TouchDevice_Command
#endif // __linux__

#endif // COMMAND_H
