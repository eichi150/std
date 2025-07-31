#include "arg_manager.h"

// ============= //
// == PUBLIC ==  //
// ============= //


Arg_Manager::Arg_Manager(std::shared_ptr<JSON_Handler> jH, std::shared_ptr<Cmd_Ctrl> ctrl_ptr): jsonH(jH), ctrl(ctrl_ptr)
{
    arg_manager_log << "read_all_accounts\n";
    jsonH->read_all_accounts(all_accounts);
    
    str_error = ctrl->get_str_error();
    
    arg_manager_log << "build Translator\n";
    translator = std::make_shared<Translator>();
    
    arg_manager_log << "set language\n";
    translator->set_language(jsonH->get_config_language());
    
};

void Arg_Manager::proceed_inputs(const int& _argc, const std::vector<std::string>& argv){

    this->str_argv = argv;
    this->argc = _argc;
    this->regex_pattern = ctrl->get_regex_pattern();
    
    //enable show_logfile, erase '-prolog' cmd out of str_argv, argc -=1
    std::vector<std::string> str_argv_without_LOG_cmd;
    std::copy_if(
        str_argv.begin(), str_argv.end(),
        std::back_inserter(str_argv_without_LOG_cmd),
        [this](const std::string& str) {
	    if(!std::regex_match(str, this->regex_pattern.at(command::show_all_log))){
	    
		return true;
	    }
	    this->output_flags.set(static_cast<size_t>(OutputType::show_all_log));
	    this->argc -= 1;
	    ctrl->show_log(true);
	    arg_manager_log << "show log\n";
	    return false;
        }
    );
    str_argv = str_argv_without_LOG_cmd;
    
    //Zeige Hilfe an
    //help
    if(std::regex_match(str_argv[1], regex_pattern.at(command::help))){
	output_flags.set(static_cast<size_t>(OutputType::show_help));
	arg_manager_log << "show help\n";
	return;
    }
    
    //show Möglichkeiten
    if(std::regex_match(str_argv[1], regex_pattern.at(command::show))){
	std::vector<command> commands = {
	    command::config_filepath
	    , command::language
	    , command::help
	    , command::show
	    , command::activate
	};
	    
	arg_manager_log << "show Command\n";
	    
	cmd = std::make_unique<Show_Command>(
	    all_accounts
	    , str_argv
	    , jsonH
	    , argc
	    , ctrl->get_specific_regex_pattern(commands)
	    , output_flags
	    , str_error
	);
	
	if(cmd){
	    arg_manager_log << "show flags got set\n";
	}
    }
    
    
    if(!cmd){
	switch(argc){
    
        case 2:
            {
                if(std::regex_match(str_argv[1], regex_pattern.at(command::environment))){
		    run_env = true;
		    arg_manager_log << "start environment\n";
		    break;
                }
                arg_manager_log << "error: syntax wrong\n";
                throw std::runtime_error{str_error.at(error::synthax)};
            }
            
        case 3:
            {	
                
                //Account löschen
                //<alias> -del
                if(std::regex_match(str_argv[2], regex_pattern.at(command::delete_))){
		    
		    arg_manager_log << "Delete Alias";
		    cmd = std::make_unique<Delete_Alias_Command>(
			all_accounts
			, str_argv
			, jsonH
			, str_error
		    );
                    
		    user_output_log << str_argv[1] << " Alias deleted\n";
		    arg_manager_log << str_argv[1] << " Alias deleted\n";
                    break;
                }
                //Language changeTo
                //-l ger
                if(std::regex_match(str_argv[1], regex_pattern.at(command::language))){
		    
		    arg_manager_log << "Change Language\n";
		    cmd = std::make_unique<Language_Change_Command>(
			jsonH
			, str_error
			, translator
			, str_argv[2]
		    );
		    
		    user_output_log << "Language changed to " << str_argv[2] << '\n';
		    arg_manager_log << "Language changed to " << str_argv[2] << '\n';
		    break;
                }

                //Connection abfrage
		    //-touch BME280
		if(std::regex_match(str_argv[1], regex_pattern.at(command::touch_sensor))){
				
		#ifdef __linux__
		    arg_manager_log << "Touch Device\n";
		    cmd = std::make_unique<TouchDevice_Command>(
			str_error
			, str_argv[2]
		    );
		#else
		    arg_manager_log << "-touch <device> Only available for Linux" << std::endl;
		    user_output_log << "Only available for Linux\n";
		#endif // __linux__
		
		    break;
		}
                                
                //i2c Sensor Messwerte für <alias> speichern
                // <alias> -mes
                if(std::regex_match(str_argv[2], regex_pattern.at(command::measure_sensor))){
                
		#ifdef __linux__
		    cmd = std::make_unique<SensorData_Add_Alias_Command>(
			all_accounts
			, str_argv
			, str_error
			, jsonH
			, translator
		    );
		#else
			arg_manager_log << "Only available for Linux\n";
			user_output_log << "Only available for Linux\n";
		#endif // __linux__
		
		    break;
                }
		arg_manager_log << "error: Syntax wrong\n";
                throw std::runtime_error{str_error.at(error::synthax)};
            }

        case 4:
            {
                //-f <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::user_filepath))){

		    arg_manager_log << "user change entity and account filepath";
		    
		    cmd = std::make_unique<UserFilepath_Change_Command>(
			jsonH
			, str_error
			, translator
			, str_argv
		    );
                    break;
                }
		
                //Neuen Account hinzufügen
                //add <entity> <alias> -tag ' '
                if(std::regex_match(str_argv[1], regex_pattern.at(command::add))){
		    
		    arg_manager_log << "add_account_noTag\n";
		    cmd = std::make_unique<Add_Alias>(
			all_accounts
			, jsonH
			, str_error
			, Add_account{str_argv[2], str_argv[3], ""}
			, regex_pattern
		    );
                    
		    user_output_log << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << '\n';
		    arg_manager_log << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << '\n';
                    break;
                }
        
                //Für Alias Stunden h oder Minuten m hinzufügen	OHNE Kommentar	
                //-h -m
                if( std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                   || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)) )
                {
		    
		    if( std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
			
			std::vector<command> commands = {
			    command::hours
			    , command::minutes
			    , command::integer
			};
			
			arg_manager_log << "add hours with comment\n";
			cmd = std::make_unique<Hours_Add_Alias_Command>(
			    all_accounts
			    , str_argv
			    , jsonH
			    , ctrl->get_specific_regex_pattern(commands)
			    , str_error
			    , translator
			);
			break;
		    }
		    user_output_log << "insert number for time value\n";
		    arg_manager_log << "insert number for time value\n";
		    throw std::runtime_error{"insert number for time value"};
                }

                //tag nachträglich hinzufügen
                //<alias> -tag plant
		if(std::regex_match(str_argv[2], regex_pattern.at(command::tag))) 
               	{

		    arg_manager_log << "add Tag to account\n"; 
		    cmd = std::make_unique<Tag_Add_Alias_Command>(
			all_accounts
			, str_argv
			, jsonH
			, str_error
		    );
		    
		    break;
                }
		
		arg_manager_log << "error: Syntax wrong\n";
                throw std::runtime_error{str_error.at(error::synthax)};
            }
            
        case 5:
            {
                //-cf <configFilepath> <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::config_filepath))){
		    
		    arg_manager_log << "user change entity and account filepath";
		    
		    cmd = std::make_unique<UserFilepath_Change_Command>(
			jsonH
			, str_error
			, translator
			, str_argv
		    );
                    
                    arg_manager_log 
			<< str_argv[2] << '\n' 
			<< str_argv[3] << '\n' 
			<< str_argv[4] 
			<< std::endl;
                    break;
                }
    
                //Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
                //<alias> 'time' -h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                    || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
		    if( std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
			
			std::vector<command> commands = {
			    command::hours
			    , command::minutes
			    , command::integer
			};
			
			arg_manager_log << "add hours without comment\n";
			cmd = std::make_unique<Hours_Add_Alias_Command>(
			    all_accounts
			    , str_argv
			    , jsonH
			    , ctrl->get_specific_regex_pattern(commands)
			    , str_error
			    , translator
			);
			break;
		    }
		    user_output_log << "insert number for time value\n";
		    arg_manager_log << "insert number for time value\n";
		    throw std::runtime_error{"insert number for time value"};
                }
		
		//Automation konfigurieren
		//<alias> -a -mes "time_config"
		//<alias> -activate -measure
		if( std::regex_match(str_argv[2], regex_pattern.at(command::activate)) ){
		    
		    std::vector<command> commands = {
			command::logfile
			, command::minutes
			, command::hours
			, command::clock
			, command::day
			, command::measure_sensor
		    };
			
		    arg_manager_log << "Activate_Command\n";
		    cmd = std::make_unique<Activate_Alias_Command>(
			all_accounts
			, str_argv
			, ctrl->get_specific_regex_pattern(commands)
			, str_error
			, jsonH
		    );
		    
		    break;
		}
			
		arg_manager_log << "error: Syntax wrong\n";		
                throw std::runtime_error{str_error.at(error::synthax)};
            }

        case 6:
	    {	
		//Neuen Account mit tag hinzufügen
                //std add <entity> <alias> -tag plant
                if(std::regex_match(str_argv[1], regex_pattern.at(command::add))
		    && std::regex_match(str_argv[4], regex_pattern.at(command::tag)) )
               	{
		    arg_manager_log << "add_account_withTag " << all_accounts.size() << '\n';
		    	    
		    cmd = std::make_unique<Add_Alias>(
			all_accounts
			, jsonH
			, str_error
			, Add_account{str_argv[2], str_argv[3], str_argv[5]}
			, regex_pattern
		    );
		    arg_manager_log  << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << " | Tag: " << str_argv[5] << '\n';
		    user_output_log << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << " | Tag: " << str_argv[5] << '\n';
                    break;
                }	
		    arg_manager_log << "error: Syntax wrong\n";
		    throw std::runtime_error{str_error.at(error::synthax)};
	    }

        
        default:
            {	
		arg_manager_log << "error: switchCase default\n";
                throw std::runtime_error{str_error.at(error::untitled_error)};
	    }
	};
    }
    
    arg_manager_log
	<< "\n===== JsonHandler_Log: =====\n" 
	<< jsonH->get_log();
	    
    if(cmd){
	cmd->execute();
	arg_manager_log
	    << "\n===== Command_Log: =====\n" 
	    << cmd->get_log();
	
	user_output_log << cmd->get_user_log();
	output_flags.set(static_cast<size_t>(OutputType::show_user_output_log));
	
    }else{
	bool throw_error = true;
	for(int i{0}; i < static_cast<int>(OutputType::COUNT); ++i){
	    if(output_flags.test(i)){
		throw_error = false;
		break;
	    }
	}
	
	if(throw_error){
	    arg_manager_log << "error: unknown_flag\n";
	    throw std::runtime_error{str_error.at(error::unknown)};
	}
    }
}

std::string Arg_Manager::get_log() const {
    std::ostringstream log;
    log 
    << "Arg_Manager:\n"
    << arg_manager_log.str();
    
    return log.str();
}

std::string Arg_Manager::get_user_output_log() const {
    return user_output_log.str();
}

std::shared_ptr<Time_Account> Arg_Manager::get_account_with_alias(const std::string& alias){
    
    std::shared_ptr<Time_Account> acc;
    std::any_of(
	all_accounts.begin(), all_accounts.end(),
	[&alias, &acc](const Time_Account& account){
	    if(account.get_alias() != alias){
		return false;
	    }
	    acc = std::make_shared<Time_Account>(account);
	    return true;
	}
    );
    if(!acc){
	throw std::runtime_error{str_error.at(error::unknown_alias)};
    }
    return acc;
}

void Arg_Manager::set_output_flag(OutputType flag, bool value){
    output_flags.set(static_cast<size_t>(flag), value);
}

OutputBitset Arg_Manager::get_output_flags() const {
    return output_flags;
}

void Arg_Manager::clear_output_flags(){
    output_flags.reset();
}
	
std::vector<Time_Account> Arg_Manager::get_all_accounts() const {
    return all_accounts;
}
	
std::vector<std::string> Arg_Manager::get_str_argv() const {
    return str_argv;
}
