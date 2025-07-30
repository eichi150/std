#include "arg_manager.h"

// ============= //
// == PUBLIC ==  //
// ============= //


Arg_Manager::Arg_Manager(std::shared_ptr<JSON_Handler> jH, std::shared_ptr<Cmd_Ctrl> ctrl_ptr): jsonH(jH), ctrl(ctrl_ptr)
{

    jsonH->read_all_accounts(all_accounts);
	
    str_error = ctrl->get_str_error();
    
    translator.set_language(jsonH->get_config_language());

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
	    jsonH
	    , str_argv
	    , argc
	    , ctrl->get_specific_regex_pattern(commands)
	    , output_flags
	    , all_accounts
	    , str_error
	);
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
                //del <alias>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::delete_))){
		    
		    cmd = std::make_unique<Delete_Command>(
			all_accounts
			, jsonH
			, str_error
			, str_argv[2]
		    );
                    
		    user_output_log << str_argv[2] << " Alias deleted\n";
		    arg_manager_log << str_argv[2] << " Alias deleted\n";
                    break;
                }
                //Language changeTo
                //-l ger
                if(std::regex_match(str_argv[1], regex_pattern.at(command::language))){
		    
		    
		    cmd = std::make_unique<ChangeLanguage_Command>(
			std::make_shared<Translator>(translator)
			, str_argv[2]
			, jsonH
			, str_error
		    );
		    
		    user_output_log << "Language changed to " << str_argv[2] << '\n';
		    arg_manager_log << "Language changed to " << str_argv[2] << '\n';
		    break;
                }
                

                //Connection abfrage
		    //-touch BME280
		    if(std::regex_match(str_argv[1], regex_pattern.at(command::touch_sensor))){
				
		#ifdef __linux__
		
			cmd = std::make_unique<TouchDevice_Command>(
			    str_error
			    , str_argv[2]
			   
			);
			arg_manager_log << "Device touched\n";
		#else
		
		    arg_manager_log << "-touch <device> Only available for Linux" << std::endl;
		    throw std::runtime_error{"Only available for Linux"};
		
		#endif // __linux__
		
			break;
		    }
                                
                //i2c Sensor Messwerte für <alias> speichern
                // <alias> -mes
                if(std::regex_match(str_argv[2], regex_pattern.at(command::measure_sensor))){
                
		#ifdef __linux__
    /* BUILD UP */
                    Device_Ctrl device{str_error.at(error::sensor)};
                    arg_manager_log 
			<< add_sensor_data(std::make_shared<Device_Ctrl>(device), all_accounts) 
			<< std::endl;
                    
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
    /* BUILD UP */
                //-f <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::user_filepath))){
                
                    user_change_filepaths(str_argv[2], str_argv[3]);
                    
                    arg_manager_log << str_argv[2] << '\n' << str_argv[3] << std::endl;
                    break;
                }
                //Neuen Account hinzufügen
                //add	
                if(std::regex_match(str_argv[1], regex_pattern.at(command::add))){
		    arg_manager_log << "add_account_noTag\n";
		    
		    cmd = std::make_unique<Add_Command>(
			all_accounts
			, jsonH
			, str_error
			, Add_account{str_argv[2], str_argv[3], ""}
			, regex_pattern.at(command::add)
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
    /* BUILD UP */
		    if( std::regex_match(str_argv[2], ctrl->get_integer_pattern()) ){
			add_hours(all_accounts, str_argv[2]);
			arg_manager_log << "add hours without comment\n";
		    }else{
			user_output_log << "insert number for time value\n";
			arg_manager_log << "insert number for time value\n";
		    }
                    break;
                }

                //tag nachträglich hinzufügen
                //<alias> -tag plant
		if(std::regex_match(str_argv[2], regex_pattern.at(command::tag))) 
               	{
    /* BUILD UP */         		                	
		    add_tag_to_account(all_accounts, str_argv[3]);
		    arg_manager_log << "add Tag to account\n";
		    break;
                }
		
		arg_manager_log << "error: Syntax wrong\n";
                throw std::runtime_error{str_error.at(error::synthax)};
            }
            
        case 5:
            {
                //-cf <configFilepath> <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::config_filepath))){
    /* BUILD UP */           
                    change_config_json_file(str_argv[2], str_argv[3], str_argv[4]);
                    
                    arg_manager_log << str_argv[2] << '\n' << str_argv[3] << '\n' << str_argv[4] << std::endl;
                    break;
                }
    
                //Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                    || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
    /* BUILD UP */
                    add_hours(all_accounts, str_argv[2]);
		    arg_manager_log << "add hours with comment\n";
                    break;
                }
		
		//Automation konfigurieren
		//<alias> -a -mes "time_config"
		//-activate -measure
		if( std::regex_match(str_argv[2], regex_pattern.at(command::activate)) ){
		    
		    cmd = std::make_unique<Activate_Command>(
			regex_pattern
			, all_accounts
			, str_error
			, jsonH
			, str_argv
		    );
		    arg_manager_log << "Activate_Command\n";
		    
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
		    	    
		    cmd = std::make_unique<Add_Command>(
			all_accounts
			, jsonH
			, str_error
			, Add_account{str_argv[2], str_argv[3], str_argv[5]}
			, regex_pattern.at(command::add)
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

// ============= //
// == PRIVATE == //
// ============= //

std::string Arg_Manager::get_log() const {
    std::ostringstream log;
    log 
    << "Arg_Manager:\n"
    << arg_manager_log.str()
    << "\nJSON_Handler:\n"
    << jsonH->get_log()
    << std::endl;
			
    return log.str();
}

std::string Arg_Manager::get_user_output_log() const {
    return user_output_log.str();
}
	
#ifdef __linux__
std::string Arg_Manager::add_sensor_data(const std::shared_ptr<Device_Ctrl>& device, std::vector<Time_Account>& all_accounts){

    bool found_alias = false;
    auto localTime = clock.get_time();
    std::stringstream output_str;
	
    for(auto& account : all_accounts){
        if(str_argv[1] == account.get_alias() ){

	    std::vector<float> output_sensor = device->check_device("BME280");
            
            output_str 
            	<< translator.language_pack.at("Temperature") << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
                << translator.language_pack.at("Pressure")<< ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
                << translator.language_pack.at("Humidity") << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
                
            Entry entry{0.f, output_str.str(), localTime};
            account.add_entry(entry);

            jsonH->save_json_accounts(all_accounts);
            jsonH->save_json_entity(all_accounts, account.get_entity());

            found_alias = true;
            break;	
        }
    }
    
    std::stringstream output;
    if(found_alias){
        output
            << std::put_time(&localTime, translator.language_pack.at("timepoint").c_str()) << '\n'
            << translator.language_pack.at("time_saved") << '\n'
            << output_str.str()
            << device->get_name() << " Connection closed\n"
            << std::setfill('=') << std::setw(10) << "=" << std::setfill(' ') << '\n';
    }else{
    
        throw std::runtime_error{str_error.at(error::not_found)};
    }

	arg_manager_log << output.str();
    return output.str();
}
#endif // __linux__


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


void Arg_Manager::change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath){
	
    std::map<std::string, std::string> new_data = {
          {"config_filepath", conf_filepath}
        , {"entity_filepath", ent_filepath}
        , {"accounts_filepath", acc_filepath}
        , {"language", translator.get_str_language()}
        , {"automation_filepath", std::string{ent_filepath + "automation_config.json"}}
    };
    jsonH->save_config_file(new_data);

}

void Arg_Manager::user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath){
    std::map<std::string, std::string> new_filepaths = {
          {"entity_filepath", ent_filepath}
        , {"accounts_filepath", acc_filepath}
    };
    jsonH->save_config_file(new_filepaths);

}


void Arg_Manager::add_hours(std::vector<Time_Account>& all_accounts, const std::string& amount){
	
    bool found_alias = false;
    auto localTime = clock.get_time();
    
    for(auto& account : all_accounts){
                        
        if(str_argv[1] == account.get_alias() ){
	    float time_float = stof(amount);
           
            if (std::regex_match(str_argv[3], regex_pattern.at(command::minutes))){
                time_float /= 60.f;
            }
            
            std::string description;
            if(argc > 4){
                description = str_argv[4];
            }
            
            Entry entry{time_float, description, localTime};
            account.add_entry(entry);

            jsonH->save_json_accounts(all_accounts);
            jsonH->save_json_entity(all_accounts, account.get_entity());

            found_alias = true;

            break;	
        }
    }
    
    if(found_alias){
        arg_manager_log 
            << std::put_time(&localTime, translator.language_pack.at("timepoint").c_str()) << '\n'
            << translator.language_pack.at("time_saved")
            << std::endl;
    }else{
    
        throw std::runtime_error{str_error.at(error::not_found)};
    }
}


void Arg_Manager::add_tag_to_account(std::vector<Time_Account>& all_accounts, const std::string& tag){

	bool found_alias = false;
	std::string entity;
	for(auto& account : all_accounts){
		if(account.get_alias() == str_argv[1]){
		    account.set_tag(tag);
		    found_alias = true;
		    entity = account.get_entity();
			
		    arg_manager_log << "Tag: " << tag << " to " << account.get_alias() << " added" << std::endl;
		    break;
		}
	}
	if(!found_alias){
	    throw std::runtime_error{str_error.at(error::not_found)};
	}
	
	jsonH->save_json_accounts(all_accounts);
	jsonH->save_json_entity(all_accounts, entity);
    	
}




