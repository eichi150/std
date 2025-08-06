#include "change_command.h"

//==========================//
//==Change_Command==========//
//==========================//

Change_Command::Change_Command( 
	std::shared_ptr<JSON_Handler> jsonH
	, std::shared_ptr<ErrorLogger> logger
) : Command(std::move(logger))
	, jsonH(jsonH)
{
	log(std::string{__FILE__} + " - Change_Command");
};

void Change_Command::finalize() {
	log("change finalize");
	if(!jsonH){
		throw Logged_Error("JSON Handler not initialized", logger);
	}
	try{
		jsonH->save_config_file(new_data);
	}catch(const std::exception& e){
		throw Logged_Error(std::string{"Failed to save config: "} + e.what(), logger);
	}
	
	
}


//==========================//
//==UserFilepath_Change_Command//
//==========================//

UserFilepath_Change_Command::UserFilepath_Change_Command(
	std::shared_ptr<JSON_Handler> jsonH
	, std::shared_ptr<Translator> translator
	, const std::vector<std::string>& str_argv
	, std::shared_ptr<ErrorLogger> logger
	
) : Change_Command(jsonH, std::move(logger))
	, translator(translator)
	, str_argv(str_argv)
{
	log(std::string{__FILE__} + " - UserFilepath_Change_Command");
};

void UserFilepath_Change_Command::execute() {
	
	if(str_argv.size() > 4){
		log("Change: config, entity, accounts filepath");
		new_data = {
			  {"config_filepath", str_argv[2]} // <- Change Config_Filepath
			, {"entity_filepath", str_argv[3]} // <- Change Entity_Filepath
			, {"accounts_filepath", str_argv[4]} // <- Change Account_Filepath
			, {"language", translator->get_str_language()}
			, {"automation_filepath", std::string{str_argv[3] + "/automation_config.json"} } // <- Change Automation_Config_Filepath
		};
		std::stringstream ss;
		ss  << str_argv[2] << "\n"
			<< str_argv[3] << "\n" 
			<< str_argv[4] << "\n"
			<< std::string{str_argv[3] + "/automation_config.json"}
			<< "\n";
			
		log(ss.str());
		add_output(ss.str());
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
			<< std::string{str_argv[3] + "/automation_config.json"}
			<< "\n";
			
		log(ss.str());
		add_output(ss.str());
	}
	
	finalize();
}

//==========================//
//==Language_Change_Command=//
//==========================//

Language_Change_Command::Language_Change_Command(
	std::shared_ptr<JSON_Handler> jsonH
	, std::shared_ptr<Translator> translator
	, const std::string& change_to_language
	, std::shared_ptr<ErrorLogger> logger
	
) : Change_Command(jsonH, std::move(logger))
	, translator(translator)
	, change_to_language(change_to_language)
{
	log(std::string{__FILE__} + " - Language_Change_Command");
};


void Language_Change_Command::execute() {
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
		add_output(ss.str());
		log("Unknown Language: " + change_to_language);                   
		throw Logged_Error("Unknown language: " + change_to_language, logger);
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
