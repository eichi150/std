#ifndef ARG_MANAGER_H
#define ARG_MANAGER_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>

#include "json_handler.h"
#include "translator.h"
#include "clock.h"
#include "cmd_ctrl.h"

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
	, show_all_log
	, show_user_output_log
	, COUNT //maxSize Bitset
};


using OutputBitset = std::bitset<static_cast<size_t>(OutputType::COUNT)>;

class Command{
public:
	
	virtual ~Command() = default;
	
	virtual void execute() = 0;
	
	virtual std::string get_log() const = 0;
	virtual std::string get_user_log() const = 0;
	
protected:
	std::stringstream cmd_log;
	std::stringstream user_output_log;
};


class Alias_Command : public Command{
public:
	Alias_Command(
		std::vector<std::string>&& str_argv
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
	
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		
		
	};
	
	std::shared_ptr<Time_Account> get_account(){
		return account;
	}
	
protected:
	std::vector<Time_Account>& all_accounts;
	std::shared_ptr<Time_Account> account;
	std::vector<std::string>& str_argv;	
};


class AddHours_Alias_Command : public Alias_Command{
public:

	AddHours_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<Translator> translator
		
	) : Alias_Command(std::move(str_argv), all_accounts)
		, jsonH(jsonH)
		, regex_pattern(regex_pattern)
		, str_error(str_error)
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
		
		jsonH->save_json_accounts(all_accounts);
		jsonH->save_json_entity(all_accounts, account->get_entity());
		 	
		cmd_log 
			<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
			<< translator->language_pack.at("time_saved")
			<< std::endl;
			
		user_output_log << cmd_log.str();
    }
    
private:
	Clock clock{};
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	std::shared_ptr<Translator> translator;
	
}; // AddHours_Alias_Command



class AddTag_Alias_Command : public Alias_Command{
public:
	AddTag_Alias_Command(
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
	};
	std::string get_user_log() const override {
		return user_output_log.str();
	}
	
	void execute() override{
	
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		account->set_tag(str_argv[3]);
		user_output_log << "Tag: " << account->get_tag() << " to " << account->get_alias() << " added" << std::endl;
		cmd_log << "Tag: " << account->get_tag() << " to " << account->get_alias() << " added" << std::endl;
		
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
		
		jsonH->save_json_entity(all_accounts, account->get_entity());
	};
	
private:
	std::shared_ptr<JSON_Handler> jsonH;
	const std::map<error, std::string>& str_error;
	
}; // AddTag_Alias_Command


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
	const std::map<error, std::string>& str_error;
	
}; // Delete_Alias_Command


#ifdef __linux__
class AddSensorData_Alias_Command : public Alias_Command{
public:
	AddSensorData_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		
	) : Alias_Command(std::move(str_argv), all_accounts)
		, str_error(str_error)
		, jsonH(jsonH)
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
		
		jsonH->save_json_entity(all_accounts, account->get_entity());

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
	std::shared_ptr<JSON_Handler> jsonH;
	const std::map<error, std::string>& str_error;
	
}; // AddSensorData_Alias_Command


class Activate_Alias_Command : public Alias_Command{
public:
	Activate_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH

	) : Alias_Command(std::move(str_argv), all_accounts)
		, regex_pattern(regex_pattern)
		, str_error(str_error)
		, jsonH(jsonH)
		
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
			configure_BME280();
		    
		#else
		    cmd_log << "Only available for Linux" << std::endl;
		    user_output_log << "Only available for Linux" << std::endl;
		    
		#endif // __linux__
		}else{
			cmd_log << str_error.at(error::synthax) << "\n";
			throw std::runtime_error{str_error.at(error::synthax)};
		}	
	};
	
	void configure_BME280(){
		
		cmd_log << "start configuration BME280";
		
		str_argv[2] = "BME280";
		
		Device_Ctrl device{str_error.at(error::sensor)};
		
		if(account){
			//an den beginn von str_argv die entity speichern -> für automation_config file
			this->str_argv[0] = account->get_entity();
		}else{
			throw std::runtime_error{ str_error.at(error::unknown_alias) };
		}
		    
		std::vector<command> commands = {
			command::logfile
			, command::minutes
			, command::hours
			, command::clock
			, command::day
		};
		
		//SET Device Settings
		std::stringstream ss;
		ss << device.set_user_automation_crontab(
		    str_argv
		    , jsonH
		    , device.get_specific_regex_pattern(commands)
		);
		
		user_output_log << ss.str();
		cmd_log << ss.str() << "configuration end";
	}
private:

	const std::map<command, std::regex>& regex_pattern;
	const std::map<error, std::string>& str_error;
	std::shared_ptr<JSON_Handler> jsonH;
	
}; // Activate_Alias_Command


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


class Show_Command : public Command{
public:
	
	Show_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, int argc
		, const std::map<command, std::regex>& regex_pattern
		, OutputBitset& output_flags
		, const std::map<error, std::string>& str_error
		
	) : str_argv(str_argv)
		, all_accounts(all_accounts)
		, jsonH(jsonH)
		, argc(argc)
		, regex_pattern(regex_pattern)
		, output_flags(output_flags)
		, str_error(str_error)	
	{};
	
	std::string get_log() const override{
		return cmd_log.str();
	}
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		cmd_log << "execute Show_Command\n";
		
		switch(argc){
			case 2:
				{
					with_2_args();
					break;
                }
			case 3:
				{
					with_3_args();
					break;
				}
			case 4:
				{
					with_4_args();
					break;
				}
			default:
				{
					cmd_log << "nothing activated to be shown\n";
					break;
				}
		};
	};
	

private:
	
	std::vector<Time_Account>& all_accounts;
	std::vector<std::string> str_argv;
	std::shared_ptr<JSON_Handler> jsonH;
	int argc;
	std::map<command, std::regex> regex_pattern;
	OutputBitset& output_flags;
	std::map<error, std::string> str_error;
	
	void with_2_args(){
		
		//Zeige alle <entity> und <alias> an
        //show
        if (std::regex_match(str_argv[1], regex_pattern.at(command::show))) {
			output_flags.set(static_cast<size_t>(OutputType::show_all_accounts));
		    cmd_log << "show all accounts\n";
            return;
        }
        throw std::runtime_error{str_error.at(error::synthax)};
    }

	void with_3_args(){
		//Zeige Filepaths, Language Setting, Alias Tabelle an
        //show filepath
        if(std::regex_match(str_argv[2], regex_pattern.at(command::config_filepath))){
                    
            output_flags.set(static_cast<size_t>(OutputType::show_filepaths));
            cmd_log << str_argv[2] << " show config_filepath.json\n";
			return;
        }
        //language anzeigen
        if(std::regex_match(str_argv[2], regex_pattern.at(command::language))){
                                       
			output_flags.set(static_cast<size_t>(OutputType::show_language));
			cmd_log << str_argv[2] << " show language_setting\n";
            return;    
        }
        //Zeige spezifischen Account an
        //show <alias>
        if(find_alias()){
			output_flags.set(static_cast<size_t>(OutputType::show_alias_table));
			
			cmd_log << str_argv[2] << " show alias table\n";
			return;
		}else{
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}	
	}
	
	
	void with_4_args(){
		//show automation für alias
		//.std sh <alias> -activate
        if( std::regex_match(str_argv[3], regex_pattern.at(command::activate)) 
			&& find_alias() ) 
        {  
			
			output_flags.set(static_cast<size_t>(OutputType::show_alias_automation));
			cmd_log << "show automation from alias table\n";
			return;
			
		}
		throw std::runtime_error{str_error.at(error::synthax)};
	}
	
	bool find_alias(){
		std::string alias = str_argv[2];
        auto it = std::find_if(
			all_accounts.begin(), all_accounts.end(),
			[&alias](const Time_Account& acc){
				return acc.get_alias() == alias;
			}
        );
        if(it != all_accounts.end()){
			return true;
		}
		throw std::runtime_error{str_error.at(error::unknown_alias)};
	}
		
}; // Show_Command



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
	
}; // Add_Command


class ChangeLanguage_Command : public Command{
public:
	ChangeLanguage_Command(
		std::shared_ptr<Translator> translator
		, const std::string& change_to_language
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
	
	) : translator(translator)
		, change_to_language(change_to_language)
		, jsonH(jsonH)
		, str_error(str_error)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		
		bool is_possible_language = std::any_of(
			translator->dict_language.begin(), translator->dict_language.end(),
			[this](const auto& language){
				return language.second == change_to_language;
			}
		);
		
                    
        if(!is_possible_language){
           std::stringstream ss;
           ss << "\nPossible Languages:\n";
			for(const auto& str : translator->dict_language){
				 ss << " > " << str.second << '\n';
			}                             
			throw std::runtime_error{str_error.at(error::unknown_language) + ss.str()};
        }

        change_config_json_language();
	};
	

private:
	std::shared_ptr<Translator> translator;
	std::string change_to_language;
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<error, std::string> str_error;
	
	void change_config_json_language(){
		std::map<std::string, std::string> new_data = {
			  {"config_filepath", jsonH->get_config_filepath()}
			, {"entity_filepath", jsonH->get_entity_filepath()}
			, {"accounts_filepath", jsonH->get_accounts_filepath()}
			, {"language", change_to_language} // <- Change Language
			, {"automation_filepath", jsonH->get_automatic_config_filepath()}
		};
		jsonH->save_config_file(new_data);
	}
	
}; // ChangeLanguage_Command


class Arg_Manager{
public:
	Arg_Manager(std::shared_ptr<JSON_Handler> jH, std::shared_ptr<Cmd_Ctrl> ctrl_ptr);
	
	//Variables
	std::shared_ptr<JSON_Handler> jsonH;
	Translator translator{};
	
	//Methods
	void proceed_inputs(const int& _argc, const std::vector<std::string>& argv);

	bool run_environment() const { return run_env; }
	void set_output_flag(OutputType flag, bool value = true); 
	
	OutputBitset get_output_flags() const;
	void clear_output_flags();
	std::vector<Time_Account> get_all_accounts() const;
	std::vector<std::string> get_str_argv() const;
	std::string get_log() const;
	std::string get_user_output_log() const;
	std::shared_ptr<Time_Account> get_account_with_alias(const std::string& alias);
	
private:
	//log
	std::stringstream arg_manager_log;
	std::stringstream user_output_log;

	//arguments
	int argc;
	std::vector<std::string> str_argv;
	//controle instance
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	std::shared_ptr<Cmd_Ctrl> ctrl;
	//All Accounts
	std::vector<Time_Account> all_accounts;
	//cmd to be executed
	std::unique_ptr<Command> cmd;
	//OutputFlags
	OutputBitset output_flags;
	
	
	void change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath);

	void user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath);
	
	bool run_env = false;
	
};//Arg_Manager


#endif // ARG_MANAGER_H
