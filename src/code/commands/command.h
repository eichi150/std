#ifndef COMMAND_H
#define COMMAND_H

#include <sstream>
#include <memory>
#include <vector>
#include <map>
#include "../json_handler.h"
#include "../time_account.h"

#include "../exception/exception.h"


class Command{
public:
	virtual ~Command() = default;
	
	virtual void execute() = 0;
	
	void set_logger(std::shared_ptr<ErrorLogger> log){
		logger = std::move(log);
	}
	void log(const std::string& msg){
		if(logger){
			logger->log(msg);
		}
	}
	std::string get_logs() const {
		return logger->get_logs();
	}
	
	
	void set_output_logger(std::shared_ptr<ErrorLogger> log){
		output_logger = std::move(log);
	}
	void add_output(const std::string& msg){
		if(output_logger){
			output_logger->log(msg);
		}
	}
	std::string get_output_logs() const {
		return output_logger->get_logs();
	}
	
protected:
	std::shared_ptr<ErrorLogger> output_logger;
	std::shared_ptr<ErrorLogger> logger;
	
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
		, const Add_account& add
		, const std::map<command, std::regex>& pattern
	
	) : all_accounts(all_accounts),
		jsonH(jsonH),
		add(add),
		regex_pattern(pattern) 
	{
		log("Add_Command");
	};
	
	void execute() override{
	
		log("execute Add_Command");
		
		check_input();
		
		create_new_account();

		jsonH->save_json_accounts(all_accounts);
		jsonH->save_json_entity(all_accounts, add.entity);
	};

private:

	void check_input(){
		log("check input");
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
			
			log("Names like commands blocked");
			throw Logged_Error("Add_Alias failed", logger);
		}	
		
		//Entity or Alias already taken?
		for(const auto& account : all_accounts){
			if(account.get_alias() == add.alias){
				
				log("Alias already taken");
				throw Logged_Error("Alias already taken", logger);
			}
			if(account.get_alias() == add.entity){
				
				log("Alias names equal to an Entity");
				throw Logged_Error("Alias names equal to an Entity", logger);
			}
		}
	}
	
	void create_new_account(){
		log("create_new_account");
		std::stringstream ss;
		
		Time_Account new_account{add.entity, add.alias};
		ss << add.entity << "-> " 
			<< add.alias;
		
		if(!add.tag.empty()){
			new_account.set_tag(add.tag);
			ss <<  " -tag= " << add.tag;
		}
		ss << " Saved";
		log(ss.str());
		add_output(ss.str());
		
		log("save to all_accounts and files");
		all_accounts.push_back(new_account);
	}

private:
	
	std::vector<Time_Account>& all_accounts;
	std::shared_ptr<JSON_Handler> jsonH;
	Add_account add;
	const std::map<command, std::regex>& regex_pattern;
		
}; // Add_Alias


#ifdef __linux__
#include "../device_ctrl.h"
class TouchDevice_Command : public Command{
public:
	TouchDevice_Command(const std::string& device_name) : arg(device_name) {};
	
	void execute() override {
		
		Device_Ctrl device{};
		auto it = device.device_regex_pattern.find(arg);
		
		if(it == device.device_regex_pattern.end()){
		    std::stringstream ss;
		    ss << "\nPossible Connections:\n";
			for(const auto& name : device.device_regex_pattern){
				ss << " > " << name.first << '\n';
			}
			log("syntax error");
		    throw SyntaxError{ss.str()};
		}
				    
		if(std::regex_match(arg, device.device_regex_pattern.at(arg)) ){

		    
		    std::vector<float> output_sensor = device.check_device(arg);
			if(output_sensor.empty()){
				throw DeviceConnectionError{"got no data back"};
			}
			
		    std::stringstream output_str; 
			output_str 
				<< "Temperature"<< ": "  << std::fixed 	<< std::setprecision(2) << output_sensor[0] << " °C || "
				<< "Pressure" 	<< ": "  << std::fixed 	<< std::setprecision(2) << output_sensor[1] << " hPa || "
				<< "Humidity" 	<< ": "  << std::fixed 	<< std::setprecision(2) << output_sensor[2] << " %\n";
					        
			
			std::stringstream ss;
			ss << ( output_sensor.empty() ? "Device touched\n" : output_str.str() )
				<< std::endl;
			
			add_output(ss.str());
			log("Device touched\n" + output_str.str());	    
		}
	};
	
private:
	std::string arg;
	
}; // TouchDevice_Command
#endif // __linux__

#endif // COMMAND_H
