#ifndef CHANGE_COMMAND_H
#define CHANGE_COMMAND_H

#include <vector>
#include <memory>
#include <regex>
#include <map>
#include <sstream>

#include "../json_handler.h"
#include "../translator.h"
#include "../cmd_ctrl.h"

#include "command.h"

class Change_Command : public Command{
public:
	Change_Command( 
		std::shared_ptr<JSON_Handler> jsonH
		
	) : jsonH(jsonH)
	{};

	std::string get_user_log() const override = 0;
	
	void execute() override = 0;
	
	void finalize() {
		log("change finalize");
		if(!jsonH){
			throw Logged_Error("Unknown Alias", logger);
		}
		jsonH->save_config_file(new_data);
		
	}
	
protected:
	std::map<std::string, std::string> new_data;
	
	std::shared_ptr<JSON_Handler> jsonH;

}; // Change_Command

class UserFilepath_Change_Command : public Change_Command{
public:
	UserFilepath_Change_Command(
		std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		, const std::vector<std::string>& str_argv
		
		
	) : Change_Command(jsonH)
		, translator(translator)
		, str_argv(str_argv)
	{};
	
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override {
		
		if(str_argv.size() > 4){
			log("Change: config, entity, accounts filepath");
			new_data = {
				  {"config_filepath", str_argv[2]} // <- Change Config_Filepath
				, {"entity_filepath", str_argv[3]} // <- Change Entity_Filepath
				, {"accounts_filepath", str_argv[4]} // <- Change Account_Filepath
				, {"language", translator->get_str_language()}
				, {"automation_filepath", std::string{str_argv[3] + "automation_config.json"} } // <- Change Automation_Config_Filepath
			};
			std::stringstream ss;
			ss  << str_argv[2] << "\n"
				<< str_argv[3] << "\n" 
				<< str_argv[4] << "\n"
				<< std::string{str_argv[3] + "automation_config.json"}
				<< "\n";
				
			log(ss.str());
			user_output_log	<< ss.str();
		}else{
			log("Change: entity, account filepath");
			new_data = {
				  {"config_filepath", jsonH->get_config_filepath()}
				, {"entity_filepath", str_argv[2]} // <- Change Entity_Filepath
				, {"accounts_filepath", str_argv[3]} // <- Change Account_Filepath
				, {"language", translator->get_str_language()} 
				, {"automation_filepath", jsonH->get_automatic_config_filepath()}
			};
			std::stringstream ss;
			ss  << str_argv[2] << "\n"
				<< str_argv[3] << "\n" 
				<< translator->get_str_language() << "\n"
				<< std::string{str_argv[3] + "automation_config.json"}
				<< "\n";
				
			log(ss.str());
			user_output_log	<< ss.str();
		}
		
		finalize();
	}
	
private:
	std::shared_ptr<Translator> translator;
	std::vector<std::string> str_argv;
	
}; // UserFilepath_Change_Command

class Language_Change_Command : public Change_Command{
public:
	Language_Change_Command(
		std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		, const std::string& change_to_language
		
	) : Change_Command(jsonH)
		, translator(translator)
		, change_to_language(change_to_language)
	{};

	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override {
		log("try to change language");
		bool is_possible_language = std::any_of(
			translator->dict_language.begin(), translator->dict_language.end(),
			[this](const auto& language){
				return language.second == change_to_language;
			}
		);
		     
        if(!is_possible_language){
			std::stringstream ss;
			ss << "Possible Languages:\n";
			for(const auto& str : translator->dict_language){
				 ss << " > " << str.second << '\n';
			}
			user_output_log << ss.str();
			log("Unknown Language: " + change_to_language);                   
			throw Logged_Error(ss.str(), logger);
        }
		
        new_data = {
			  {"config_filepath", jsonH->get_config_filepath()}
			, {"entity_filepath", jsonH->get_entity_filepath()}
			, {"accounts_filepath", jsonH->get_accounts_filepath()}
			, {"language", change_to_language} // <- Change Language
			, {"automation_filepath", jsonH->get_automatic_config_filepath()}
		};
		
		finalize();
	};

private:
	std::shared_ptr<Translator> translator;
	std::string change_to_language;
	
}; // Language_Change_Command

#endif // CHANGE_COMMAND_H
