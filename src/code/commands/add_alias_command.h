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
		, const std::map<error, std::string>& str_error
		
	) : Alias_Command(std::move(str_argv), all_accounts)
		, jsonH(jsonH)
		, str_error(str_error)
	{};
	
	std::string get_log() const override = 0;
	std::string get_user_log() const override = 0;
	void execute() override = 0;
	
	//Save to file
	void finalize() {
		if(account){
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
			jsonH->save_json_entity(all_accounts, account->get_entity());
		}else{
			
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
	}
	
protected:
	std::map<error, std::string> str_error;
	std::shared_ptr<JSON_Handler> jsonH;
}; // Add_Alias_Command


class Hours_Add_Alias_Command : public Add_Alias_Command{
public:
	
	Hours_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<Translator> translator
		
	) : Add_Alias_Command(all_accounts, str_argv, jsonH, str_error)
		, regex_pattern(regex_pattern)
		, translator(translator)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override {
		return user_output_log.str();
	}
	
	void execute() override {
		
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		if(!std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
			user_output_log << "insert number for time value\n";
			cmd_log << "insert number for time value\n";
			
			throw std::runtime_error{"insert number for time value"};
		}
		
		Clock clock{};
		auto localTime = clock.get_time();
		
		float time_float;
		try{
			time_float = stof(str_argv[2]);
			if (std::regex_match(str_argv[3], regex_pattern.at(command::minutes))){
				time_float /= 60.f;
			}
		}catch(const std::runtime_error& re){
			cmd_log << "error addhours string_to_float "<< re.what() << "\n";
		}
				
		std::stringstream description;
		if(str_argv.size() > 4){
			for(size_t i{4}; i < str_argv.size(); ++i){
				description << str_argv[i] << " ";
			}
		}
				
		Entry entry{time_float, description.str(), localTime};
		account->add_entry(entry);
		
		//Save to file
		finalize();
		
		//Save All_Accounts
		jsonH->save_json_accounts(all_accounts);
		
		cmd_log 
			<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
			<< translator->language_pack.at("time_saved")
			<< std::endl;
			
		user_output_log << cmd_log.str();
    }
    
private:
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<Translator> translator;
	
}; // AddHours_Alias_Command


class Tag_Add_Alias_Command : public Add_Alias_Command{
public:
	Tag_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
	
	) : Add_Alias_Command(all_accounts, str_argv, jsonH, str_error)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override {
		return user_output_log.str();
	}
	
	void execute() override{
	
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		account->set_tag(str_argv[3]);
		
		//Save to file
		finalize();
		
		user_output_log << "Tag: " << account->get_tag() << " to " << account->get_alias() << " added" << std::endl;
		cmd_log << "Tag: " << account->get_tag() << " to " << account->get_alias() << " added" << std::endl;
		
	};
	
}; // Tag_Add_Alias_Command


#ifdef __linux__
#include "../device_ctrl.h"

class SensorData_Add_Alias_Command : public Add_Alias_Command{
public:
	SensorData_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		
	) : Add_Alias_Command(all_accounts, str_argv, jsonH, str_error)
		, translator(translator)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override {
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		Clock clock{};
		Device_Ctrl device{str_error.at(error::sensor)};
		
		auto localTime = clock.get_time();
		std::stringstream output_str;

		std::vector<float> output_sensor = device.check_device("BME280");
				
		output_str 
			<< translator->language_pack.at("Temperature") << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
			<< translator->language_pack.at("Pressure") << ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
			<< translator->language_pack.at("Humidity") << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
					
		Entry entry{0.f, output_str.str(), localTime};
		account->add_entry(entry);
		
		//Save to file
		finalize();
		
		std::stringstream output;
		output
			<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
			<< translator->language_pack.at("time_saved") << '\n'
			<< output_str.str()
			<< std::setfill('=') << std::setw(10) << "=" << std::setfill(' ') << '\n';

		cmd_log << output.str() << device.get_name() << " Connection closed\n";
		user_output_log << output.str();
	};
	
private:
	std::shared_ptr<Translator> translator;
	
}; // SensorData_Add_Alias_Command
#endif // __linux__

#endif // ADD_ALIAS_COMMAND_H
