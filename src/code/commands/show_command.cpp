#include "show_command.h"

//==========================//
//==Show_Command============//
//==========================//

Show_Command::Show_Command(
	std::vector<Time_Account>& _all_accounts
	, std::vector<std::string>& _str_argv
	, std::shared_ptr<JSON_Handler> _jsonH
	, int _argc
	, const std::map<command, std::regex>& _regex_pattern
	, OutputBitset& _output_flags
	, std::shared_ptr<ErrorLogger> _logger
	
) : Command(std::move(_logger))
	, str_argv(_str_argv)
	, all_accounts(_all_accounts)
	, jsonH(_jsonH)
	, argc(_argc)
	, regex_pattern(_regex_pattern)
	, output_flags(_output_flags)
{
	log(std::string{__FILE__} + " - Show_Command");
};

void Show_Command::execute() {
	log("execute Show_Command");
	
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
		throw Logged_Error("Nothing chosen", logger);
	}
	};
}

void Show_Command::with_2_args(){
//Zeige alle <entity> und <alias> an
      //show
    if (std::regex_match(str_argv[1], regex_pattern.at(command::show))) {
		output_flags.set(static_cast<size_t>(OutputType::show_all_accounts));
		log("show all accounts");
        return;
    }
    throw SyntaxError{"Show all Accounts"};
}

void Show_Command::with_3_args(){
//Zeige Filepaths, Language Setting, Alias Tabelle an
     //show filepath
	if(std::regex_match(str_argv[2], regex_pattern.at(command::config_filepath))){
                  
        output_flags.set(static_cast<size_t>(OutputType::show_filepaths));
        log(str_argv[2] + " show config_filepath.json");
		return;
    }
    //language anzeigen
    if(std::regex_match(str_argv[2], regex_pattern.at(command::language))){
                                     
	output_flags.set(static_cast<size_t>(OutputType::show_language));
	log(str_argv[2] + " show language_setting");
    return;    
    }
    //Zeige spezifischen Account an
    //show <alias>
    if(find_alias(str_argv[2])){
		output_flags.set(static_cast<size_t>(OutputType::show_alias_table));
	
		log(str_argv[2] + " show alias table");
		return;

    }else if(find_entity(str_argv[2])){
		output_flags.set(static_cast<size_t>(OutputType::show_entity));
		log(str_argv[2] + " show entity table");
	
		log("all_accounts: " + std::to_string(all_accounts.size()));
		Entity_Command sh_entity{
			str_argv
			, all_accounts
			, str_argv[2]
			, logger
		};
	
		all_accounts = sh_entity.get_entity_accounts();
		log("all_accounts: " + std::to_string(all_accounts.size()));
		return;
	
    }else{
		throw Logged_Error("Unknown Entity | Unknown Alias", logger);
    }	
}

void Show_Command::with_4_args(){
//show automation für alias
//.std sh <alias> -activate
    if( std::regex_match(str_argv[3], regex_pattern.at(command::activate)) 
		&& find_alias(str_argv[2]) ) 
    {  
		output_flags.set(static_cast<size_t>(OutputType::show_alias_automation));
		log("show automation from alias table");
		return;
	}
	throw SyntaxError{""};
}

bool Show_Command::find_entity(const std::string& __entity){
	std::string entity = __entity;
	auto it = std::find_if(
		all_accounts.begin(), all_accounts.end(),
		[&entity](const Time_Account& acc){
			return acc.get_entity() == entity;
		}
	);
	if(it != all_accounts.end()){
		
		return true;
	}
	return false;
}

bool Show_Command::find_alias(const std::string& __alias){
	std::string alias = __alias;
	auto it = std::find_if(
		all_accounts.begin(), all_accounts.end(),
		[&alias](const Time_Account& acc){
			return acc.get_alias() == alias;
		}
	);
	if(it != all_accounts.end()){
		return true;
	}
	return false;
}
