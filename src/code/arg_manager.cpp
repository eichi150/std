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
    
    regex_pattern = {
			  { command::help,      		std::regex{R"(^--?help$)", std::regex_constants::icase } }
			, { command::add,       		std::regex{R"(^--?add$)", std::regex_constants::icase } }
			, { command::show,      		std::regex{R"(^(--?show|--?sh|show|sh)$)" , std::regex_constants::icase }}
			, { command::delete_,   		std::regex{R"(^(--?del(ete)?)$)", std::regex_constants::icase } }
			, { command::hours, 			std::regex{R"(^(--?h(ours)?)$)", std::regex_constants::icase } }
			, { command::minutes, 			std::regex{R"(^(--?m(inutes)?)$)", std::regex_constants::icase } }
			, { command::day, 				std::regex{R"(^--?day$)", std::regex_constants::icase} }
			, { command::logfile, 			std::regex{R"(^--?log$)", std::regex_constants::icase} }
			, { command::clock, 			std::regex{R"(^--?clock$)", std::regex_constants::icase} }
			, { command::config_filepath, 	std::regex{R"(^--?cf$)", std::regex_constants::icase } }
			, { command::user_filepath,  	std::regex{R"(^(--?f(ilepath)?|filepath)$)", std::regex_constants::icase } }
			, { command::language,  		std::regex{R"(^(--?l(anguage)?|language)$)", std::regex_constants::icase } }
			, { command::tag,				std::regex{R"(^--?tag$)", std::regex_constants::icase } }
			, { command::touch_sensor, 		std::regex{R"(^--?touch$)", std::regex_constants::icase } }
			, { command::messure_sensor,	std::regex{R"(^(--?mes(sure)?)$)", std::regex_constants::icase } }
			, { command::activate,			std::regex{R"(^(--?a(ctivate)?)$)", std::regex_constants::icase } }
			, { command::i2c, 				std::regex{R"(^--?i2c$)", std::regex_constants::icase } }
			, { command::automatic, 		std::regex{R"(^--?auto$)", std::regex_constants::icase } }
			, { command::environment, 		std::regex{R"(^--?env$)", std::regex_constants::icase } }
			, { command::process_log, 		std::regex{R"(^--?prolog$)", std::regex_constants::icase} }
		};
    
    
    
    //enable show_logfile, erase '-prolog' cmd out of str_argv, argc -=1
    std::vector<std::string> str_argv_without_LOG_cmd;
    std::copy_if(
        str_argv.begin(), str_argv.end(),
        std::back_inserter(str_argv_without_LOG_cmd),
        [this](const std::string& str) {
	    if(!std::regex_match(str, this->regex_pattern.at(command::process_log))){
	    
		return true;
	    }
	    this->output_flags.set(static_cast<size_t>(OutputType::arg_manager_log));
	    this->argc -= 1;
	    return false;
        }
    );
    str_argv = str_argv_without_LOG_cmd;
    
    arg_manager_log << argc << " " << str_argv.size() << '\n';
    
    switch(argc){
    
        case 2:
            {
                //Zeige Hilfe an
                //help
                if(std::regex_match(str_argv[1], regex_pattern.at(command::help))){
		    output_flags.set(static_cast<size_t>(OutputType::show_help));
                    break;
                }

                //Zeige alle <entity> und <alias> an
                //show
                if (std::regex_match(str_argv[1], regex_pattern.at(command::show))) {
                    output_flags.set(static_cast<size_t>(OutputType::show_all_accounts));
                    break;
                }

                if(std::regex_match(str_argv[1], regex_pattern.at(command::environment))){
		    run_env = true;
		    break;
                }
                
                throw std::runtime_error{str_error.at(error::synthax)};
            }
            
        case 3:
            {	
                //show Möglichkeiten
                if(std::regex_match(str_argv[1], regex_pattern.at(command::show))){
		    std::vector<command> commands = {
			command::config_filepath
			, command::language
		    };
				    
		    std::map<command, std::regex> regex_pattern = ctrl->get_specific_regex_pattern(commands);
		    
		    cmd = std::make_unique<Show_Command>(
			jsonH
			, str_argv
			, regex_pattern
			, output_flags
			, all_accounts
			, str_error
		    );
		    
		    break;

                }
                
                //Account löschen
                //del <alias>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::delete_))){
		    
		    cmd = std::make_unique<Delete_Command>(
			all_accounts
			, jsonH
			, str_error
			, str_argv[2]
		    );
                    
                    break;
                }
                //Language changeTo
                //-l ger
                if(std::regex_match(str_argv[1], regex_pattern.at(command::language))){

                    bool is_possible_language = false;					
                    for(const auto& key : translator.dict_language){
                        if(key.second == str_argv[2]){
                            is_possible_language = true;
                            break;
                        }
                    }
                    
                    if(!is_possible_language){
                        std::stringstream ss;
                        ss << "\nPossible Languages:\n";
                        for(const auto& str : translator.dict_language){
                            ss << " > " << str.second << '\n';
                        }
                                                
                        throw std::runtime_error{str_error.at(error::unknown_language) + ss.str()};
                    }

                    change_config_json_language(str_argv[2]);
                    break;
                }
                

                //Connection abfrage
		    //-touch BME280
		    if(std::regex_match(str_argv[1], regex_pattern.at(command::touch_sensor))){
				
		#ifdef __linux__
				    
			std::string arg = str_argv[2];
			auto it = ctrl->device_regex_pattern.find(arg);
			if(it == ctrl->device_regex_pattern.end()){
			    std::stringstream ss;
			    ss << "\nPossible Connections:\n";
			    for(const auto& name : ctrl->device_regex_pattern){
				ss << " > " << name.first << '\n';
			    }
			    throw std::runtime_error{str_error.at(error::synthax) + ss.str()};
			}
				    
			if(std::regex_match(str_argv[2],  ctrl->device_regex_pattern.at(arg)) ){

			    Device_Ctrl device{str_error.at(error::sensor)};
			    std::vector<float> output_sensor = device.check_device(arg);

			    std::stringstream output_str;
					    
			    output_str 
				<< translator.language_pack.at("Temperature") << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
				<< translator.language_pack.at("Pressure")<< ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
				<< translator.language_pack.at("Humidity") << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
					        
			    arg_manager_log << output_str.str() << std::endl;
					    
			}
		#else
            arg_manager_log << "Only available for Linux" << std::endl;
			
		#endif // __linux__
		
			break;
		    }
                                
                //i2c Sensor Messwerte für <alias> speichern
                // <alias> -mes
                if(std::regex_match(str_argv[2], regex_pattern.at(command::messure_sensor))){
                
        #ifdef __linux__
                
                    Device_Ctrl device{str_error.at(error::sensor)};
                    arg_manager_log << add_sensor_data(std::make_shared<Device_Ctrl>(device), all_accounts) << std::endl;
                    
		#else
			arg_manager_log << "Only available for Linux" << std::endl;
		#endif // __linux__
		
		break;
                }
             
                throw std::runtime_error{str_error.at(error::synthax)};
            }

        case 4:
            {
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
                    
                    break;
                }
        
                //Für Alias Stunden h oder Minuten m hinzufügen	OHNE Kommentar	
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                   || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
                    add_hours(all_accounts, str_argv[2]);
                    break;
                }

                //tag nachträglich hinzufügen
                //<alias> -tag plant
		if(std::regex_match(str_argv[2], regex_pattern.at(command::tag))) 
               	{
               		                	
		    add_tag_to_account(all_accounts, str_argv[3]);
		    break;
                }
		//show automation für alias
		//.std sh <alias> -activate
                if(std::regex_match(str_argv[1], regex_pattern.at(command::show))
		    && std::regex_match(str_argv[3], regex_pattern.at(command::activate))
		) 
               	{
		    
		    output_flags.set(static_cast<size_t>(OutputType::show_alias_automation));
		    break;
		}
		
                throw std::runtime_error{str_error.at(error::synthax)};
            }
            
        case 5:
            {
                //-cf <configFilepath> <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::config_filepath))){
                
                    change_config_json_file(str_argv[2], str_argv[3], str_argv[4]);
                    
                    arg_manager_log << str_argv[2] << '\n' << str_argv[3] << '\n' << str_argv[4] << std::endl;
                    break;
                }
    
                //Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                    || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
                    add_hours(all_accounts, str_argv[2]);
                    break;
                }
		
			//Automation konfigurieren
			//<alias> -a -mes "time_config"
			bool is_true = false;
			bool cmd_messure = std::regex_match(str_argv[3], regex_pattern.at(command::messure_sensor));
			//-activate
			if(std::regex_match(str_argv[2], regex_pattern.at(command::activate)) && cmd_messure){
				
			    std::stringstream ss;
			    ss << "\nPossible Connections:\n";
			    for(const auto& name : ctrl->device_regex_pattern){
				ss << " > " << name.first << '\n';
			    }
			    arg_manager_log << ss.str();
					
			    str_argv[2] = "BME280";
			    is_true = true;
					    	
			}else{
			    std::string arg = str_argv[2];
			    auto it = ctrl->device_regex_pattern.find(arg);
			    if(it != ctrl->device_regex_pattern.end()){
			    //<device_name>
				if(std::regex_match(str_argv[2], ctrl->device_regex_pattern.at(str_argv[2])) && cmd_messure){
				    is_true = true;
				}
			    }
			}
				
		#ifdef __linux__
		   
       			if(is_true){  
                    
			    Device_Ctrl device{str_error.at(error::sensor)};
				
			//an den beginn von str_argv die entity speichern -> für automation_config file
			    
			    std::any_of( all_accounts.begin(), all_accounts.end(),
				[this](const Time_Account& account){
				    if(this->str_argv[1] == account.get_alias()){
					this->str_argv[0] = account.get_entity();
					return true;
				    }
				    return false;
				});
			    std::vector<command> commands = {
				command::logfile
				, command::minutes
				, command::hours
				, command::clock
				, command::day
			    };
				    
			    std::map<command, std::regex> regex_pattern = ctrl->get_specific_regex_pattern(commands);
					
			    arg_manager_log << device.set_user_automation_crontab(str_argv, jsonH, regex_pattern) << std::endl;
					    
			    break;
			}
		#else
			arg_manager_log << "Only available for Linux" << std::endl;
			break;
			
		#endif // __linux__
				
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
		    //std::vector<command> commands = {command::add};
		    //std::map<command, std::regex> special_regex_pattern = ctrl->get_specific_regex_pattern(commands);
				    
		    cmd = std::make_unique<Add_Command>(
			all_accounts
			, jsonH
			, str_error
			, Add_account{str_argv[2], str_argv[3], str_argv[5]}
			, regex_pattern.at(command::add)
		    );
		    
                    break;
                }	
				
		    throw std::runtime_error{str_error.at(error::synthax)};
	    }

        
        default:
            {
                throw std::runtime_error{str_error.at(error::untitled_error)};
		
	    }
	
    };
    
    if(cmd){
	cmd->execute();
	arg_manager_log << cmd->get_log();
    
    }else{
	bool throw_error = true;
	for(int i{0}; i < static_cast<int>(OutputType::COUNT); ++i){
	    if(output_flags.test(i)){
		throw_error = false;
		break;
	    }
	}
	if(throw_error){
	    throw std::runtime_error{str_error.at(error::unknown)};
	}
    }
	
}

// ============= //
// == PRIVATE == //
// ============= //



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

std::string Arg_Manager::get_log() const {
    std::ostringstream log;
    log 
    << "Process Log:\nArg_Manager:\n"
    << arg_manager_log.str()
    << "\nJSON_Handler:\n"
    << jsonH->get_log()
    << std::endl;
			
    return log.str();
}


std::shared_ptr<Time_Account> Arg_Manager::get_account_with_alias(const std::string& alias){
    for(const auto& account : all_accounts){
	if(account.get_alias() == alias){
	    return std::make_shared<Time_Account>(account);
	}
    }
    throw std::runtime_error{str_error.at(error::unknown_alias)};
}


void Arg_Manager::change_config_json_language(const std::string& to_language){
    std::map<std::string, std::string> new_data = {
          {"config_filepath", jsonH->get_config_filepath()}
        , {"entity_filepath", jsonH->get_entity_filepath()}
        , {"accounts_filepath", jsonH->get_accounts_filepath()}
        , {"language", to_language}
        , {"automation_filepath", jsonH->get_automatic_config_filepath()}
    };
    jsonH->save_config_file(new_data);
    
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




