#ifndef ADD_ALIAS_COMMAND_H
#define ADD_ALIAS_COMMAND_H

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
#include "alias_command.h"

class Add_Alias_Command : public Alias_Command{
public:
	Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		
	) : Alias_Command(std::move(str_argv), all_accounts)
		, jsonH(jsonH)
	{
		log("Add--Alias_Command BaseClass for adding Data to Alias");
	};
	
	std::string get_user_log() const override = 0;
	void execute() override = 0;
	
	//Save to file
	void finalize() {
		if(!account){
			throw Logged_Error("Unknown Alias", logger);	
		}
		add_alias();
	}
	
protected:
	std::shared_ptr<JSON_Handler> jsonH;
	
private:

	void add_alias(){
		std::any_of(
			all_accounts.begin(), all_accounts.end(),
			[this](auto& acc){
				if(acc.get_alias() == account->get_alias()){
					acc = *account;
					return true;
				}
				return false;
			}
		);
		//Save Account Entity File
		if(!jsonH){
			throw Logged_Error("jsonH missing", logger);
		}
		jsonH->save_json_entity(all_accounts, account->get_entity());
	}
	
}; // Add_Alias_Command


class Hours_Add_Alias_Command : public Add_Alias_Command{
public:
	
	Hours_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<command, std::regex>& regex_pattern
		, std::shared_ptr<Translator> translator
		
	) : Add_Alias_Command(all_accounts, str_argv, jsonH)
		, regex_pattern(regex_pattern)
		, translator(translator)
	{
		log("add Hours to Alias");
		Clock clock{};
		localTime = clock.get_time();
	};
	
	std::string get_user_log() const override {
		return user_output_log.str();
	}
	
	void execute() override {
		
		if(!account){
			throw Logged_Error("Unknown Alias", logger);
		}
		
		if(!std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
			user_output_log << "insert number for time value\n";
			log("insert number for time value");
			
			throw Logged_Error("insert number for time value", logger);
		}
		
		add_hours();
		
		//Save to file
		finalize();
		//Save All_Accounts
		
		jsonH->save_json_accounts(all_accounts);
		
		if(!translator){
			throw Logged_Error("Translator missing", logger);
		}
		user_output_log 
			<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
			<< translator->language_pack.at("time_saved")
			<< std::endl;
    }
    
private:

	void add_hours(){
		
		float time_float;
		try{
			time_float = stof(str_argv[2]);
			if (std::regex_match(str_argv[3], regex_pattern.at(command::minutes))){
				time_float /= 60.f;
			}
		}catch(const std::out_of_range& re){
			log("error addhours string_to_float " + std::string{re.what()} );
			throw Logged_Error("insert number for time value", logger);
		}
				
		std::stringstream description;
		if(str_argv.size() > 4){
			for(size_t i{4}; i < str_argv.size(); ++i){
				description << str_argv[i] << " ";
			}
		}
				
		Entry entry{time_float, description.str(), localTime};
		account->add_entry(entry);
	}

private:
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<Translator> translator;
	std::tm localTime;
	
}; // AddHours_Alias_Command


class Tag_Add_Alias_Command : public Add_Alias_Command{
public:
	Tag_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
	
	) : Add_Alias_Command(all_accounts, str_argv, jsonH)
	{
		log("add Tag to Alias");
	};
	
	
	std::string get_user_log() const override {
		return user_output_log.str();
	}
	
	void execute() override{
	
		if(!account){
			throw Logged_Error("Unknown Alias", logger);
		}
		
		account->set_tag(str_argv[3]);
		
		//Save to file
		finalize();
		
		user_output_log 
			<< "Tag: " 
			<< account->get_tag() << " to " 
			<< account->get_alias() << " added" 
			<< std::endl;
		log(user_output_log.str());
		
	};
	
}; // Tag_Add_Alias_Command


#ifdef __linux__
#include "../device_ctrl.h"

class SensorData_Add_Alias_Command : public Add_Alias_Command{
public:
	SensorData_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		
	) : Add_Alias_Command(all_accounts, str_argv, jsonH)
		, translator(translator)
		
	{
		log("add Sensordata to Alias");
		Clock clock{};
		localTime = clock.get_time();
	};
	
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override {
		
		if(!account){
			throw Logged_Error("Unknown Alias", logger);
		}
		//add data
		add_sensor_data();
		
		//Save to file
		finalize();
		
		if(!translator){
			throw Logged_Error("Translator missing", logger);
		}
		std::stringstream output;
		output
			<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
			<< translator->language_pack.at("time_saved") << '\n'
			<< output_str.str()
			<< std::setfill('=') << std::setw(10) << "=" << std::setfill(' ') << '\n';

		log(output.str() + device.get_name() + " Connection closed");
		user_output_log << output.str();
	};

private:
	
	void add_sensor_data(){
		
		log("check Sensor for Data");
		
		std::vector<float> output_sensor = device.check_device("BME280");
		
		if(output_sensor.size() < 3){
			throw Logged_Error("No Sensor output", logger);
		}
		output_str 
			<< translator->language_pack.at("Temperature") << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
			<< translator->language_pack.at("Pressure") << ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
			<< translator->language_pack.at("Humidity") << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
					
		Entry entry{0.f, output_str.str(), localTime};
		account->add_entry(entry);
	}

private:
	std::shared_ptr<Translator> translator;
	std::tm localTime;
	Device_Ctrl device{};
	std::stringstream output_str;
	
}; // SensorData_Add_Alias_Command
#endif // __linux__

#endif // ADD_ALIAS_COMMAND_H
