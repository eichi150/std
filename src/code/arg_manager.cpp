#include "arg_manager.h"
// ============= //
// == PUBLIC ==  //
// ============= //
Arg_Manager::Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::shared_ptr<Cmd_Ctrl>& ctrl, const std::vector<std::string>& argv, const int& argc)
    : jsonH(jH), str_argv(argv), argc(argc)
{

    regex_pattern = ctrl->get_regex_pattern();
    str_error = ctrl->get_str_error();
    
    translator.set_language(jsonH->get_config_language());
    
    max_length = {
        10 //Index Standard
        , 10 //Alias Standard
        , 15 //Entity Standard
        , static_cast<int>(translator.language_pack.at("total_hours").size())
        , static_cast<int>(translator.language_pack.at("hours").size() + 5)
        , static_cast<int>(translator.language_pack.at("timestamp").size() + 10)
        , static_cast<int>(translator.language_pack.at("comment").size() + 10)
    };
};

void Arg_Manager::proceed_inputs(std::vector<Time_Account>& all_accounts){
    
    switch(argc){
    
        case 2:
            {
                //Zeige Hilfe an
                //help
                if(std::regex_match(str_argv[1], regex_pattern.at(command::help))){
                
                    std::cout << help << std::endl;
                    break;
                }
                
                //Zeige alle Entity und Alias an
                //show
                if(std::regex_match(str_argv[1], regex_pattern.at(command::show))){
                
                        show_all(all_accounts);
                        break;
                }
                
                //i2c Sensor Abfrage
                if(std::regex_match(str_argv[1], regex_pattern.at(command::sensor))){
                    
                    std::string output_sensor;
                    BME_Sensor sensor{};
                    if(sensor.scan_sensor(output_sensor) == 1){
                        throw std::runtime_error(str_error.at(error::sensor));
                    }
                    
                    std::cout << output_sensor << std::endl;
                    break;

                }
                throw std::runtime_error{str_error.at(error::synthax)};
            };
            
        case 3:
            {	
                //show Möglichkeiten
                if(std::regex_match(str_argv[1], regex_pattern.at(command::show))){
                
                    //Zeige entity, accounts, config filepaths an
                    //show filepath
                    if(std::regex_match(str_argv[2], regex_pattern.at(command::config_filepath))){
                    
                        show_filepaths();
                        break;
                    }
                    //language anzeigen
                    if(std::regex_match(str_argv[2], regex_pattern.at(command::language))){
                                            
                        show_language();
                        break;
                    }
                    //Zeige spezifischen Account an
                    //show <entity> ODER show <alias>	
                        //entity oder alias ?? -> entsprechende accounts erhalten
                    std::vector<Time_Account> matching_accounts;
                    matching_accounts = check_for_alias_or_entity(all_accounts, str_argv[2]);
                    
                    if(matching_accounts.empty()){
                        throw std::runtime_error{str_error.at(error::unknown_alias_or_entity)};
                    }
                    
                    show_specific_table(matching_accounts);
                    break;
                }
                //Account löschen
                //del
                if(std::regex_match(str_argv[1], regex_pattern.at(command::delete_))){
                
                    delete_account(all_accounts, str_argv[2]);
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
                
                //i2c Sensor Abfrage
                if(std::regex_match(str_argv[2], regex_pattern.at(command::save_sensor_data))){
                    
                    std::string output_sensor;
                    BME_Sensor sensor{};
                    if(sensor.scan_sensor(output_sensor) == 1){
                        throw std::runtime_error(str_error.at(error::sensor));
                    }
                    
                    add_sensor_data(all_accounts);
                    break;

                }
                
                throw std::runtime_error{str_error.at(error::synthax)};
            };

        case 4:
            {
                //-f <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::user_filepath))){
                
                    user_change_filepaths(str_argv[2], str_argv[3]);
                    
                    std::cout << str_argv[2] << '\n' << str_argv[3] << std::endl;
                    break;
                }
                //Neuen Account hinzufügen
                //add	
                if(std::regex_match(str_argv[1], regex_pattern.at(command::add))){
                
                    add_account(all_accounts);
                    break;
                }
        
                //Für Alias Stunden h oder Minuten m hinzufügen	OHNE Kommentar	
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                        || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
                    add_hours(all_accounts);
                    break;
                }
                
                throw std::runtime_error{str_error.at(error::synthax)};
            };
            
        case 5:
            {
                //-cf <configFilepath> <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::config_filepath))){
                
                    change_config_json_file(str_argv[2], str_argv[3], str_argv[4]);
                    
                    std::cout << str_argv[2] << '\n' << str_argv[3] << '\n' << str_argv[4] << std::endl;
                    break;
                }
    
                //Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                        || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
                    add_hours(all_accounts);
                    break;
                }
                throw std::runtime_error{str_error.at(error::synthax)};
            };
            
        default:
            {
                throw std::runtime_error{str_error.at(error::untitled_error)};
            };
    };
}
// ============= //
// == PRIVATE == //
// ============= //

std::vector<Time_Account> Arg_Manager::check_for_alias_or_entity(const std::vector<Time_Account>& all_accounts, const std::string& alias_or_entity){
    std::vector<Time_Account> matching_accounts;
    bool alias_found = false;
    bool entity_found = false;
    for(auto& account : all_accounts){
    
        if(account.get_alias() == alias_or_entity){
            alias_found = true;
            matching_accounts = collect_accounts_with_alias(all_accounts, str_argv[2]);
            break;
        }
        if(account.get_entity() == alias_or_entity){
            entity_found = true;
            matching_accounts = jsonH->collect_accounts_with_entity(all_accounts, str_argv[2]);
            break;
        }
    }

    if( (!alias_found && !entity_found) || matching_accounts.empty()){
        throw std::runtime_error{str_error.at(error::unknown_alias_or_entity)};
    }
    
    return matching_accounts;
}


std::vector<Time_Account> Arg_Manager::collect_accounts_with_alias(const std::vector<Time_Account>& search_in, const std::string& search_for_alias){
    std::vector<Time_Account> matching_accounts;
    std::copy_if(
        search_in.begin(), search_in.end(),
        std::back_inserter(matching_accounts),
        [&search_for_alias](const Time_Account& acc) {
            return acc.get_alias() == search_for_alias;
        }
    );
    return matching_accounts;
}

void Arg_Manager::change_config_json_language(const std::string& to_language){
    std::map<std::string, std::string> new_data = {
            {"config_filepath", jsonH->get_config_filepath()}
        , {"entity_filepath", jsonH->get_entity_filepath()}
        , {"accounts_filepath", jsonH->get_accounts_filepath()}
        , {"language", to_language}
    };
    jsonH->save_config_file(new_data);
    
}

void Arg_Manager::change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath){
	
    std::map<std::string, std::string> new_data = {
            {"config_filepath", conf_filepath}
        , {"entity_filepath", ent_filepath}
        , {"accounts_filepath", acc_filepath}
        , {"language", translator.get_str_language()}
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

void Arg_Manager::delete_account(std::vector<Time_Account>& all_accounts, const std::string& alias_to_delete){
    if(all_accounts.empty()){
        throw std::runtime_error{str_error.at(error::untitled_error)};
    }
    
    bool found_alias = false;
    std::string entity;
    std::string alias;
    
    for(const auto& account : all_accounts){
        if(account.get_alias() == alias_to_delete){
            entity = account.get_entity();
            alias = account.get_alias();
            found_alias = true;
            break;
        }
    }
    if(!found_alias){
        throw std::runtime_error{str_error.at(error::unknown_alias)};
    }
    
    std::vector<Time_Account> adjusted_accounts;

    //remove out of all_accounts
    std::remove_copy_if(all_accounts.begin(), all_accounts.end(),
        std::back_inserter(adjusted_accounts),
        [alias_to_delete](const Time_Account& account){
            return account.get_alias() == alias_to_delete;
        }
    );
    jsonH->save_json_entity(all_accounts, entity, alias);
    
    all_accounts = adjusted_accounts;

    //update Files
    jsonH->save_json_accounts(all_accounts);
    //jsonH->save_json_entity(all_accounts, entity, alias);
    
    std::cout << alias_to_delete << translator.language_pack.at("deleted_out_of_accounts.json") << std::endl;
}


void Arg_Manager::add_account(std::vector<Time_Account>& all_accounts){
    std::string entity = str_argv[2];
    std::string alias = str_argv[3];

    //Entity or Alias cant be named as a command
    bool is_command = false;
    for(const auto& pattern : regex_pattern){
        
        if(std::regex_match(entity, pattern.second)){
            is_command = true;
            break;
        }	
        if(std::regex_match(alias, pattern.second)){
            is_command = true;
            break;
        }
    }
    if(is_command){
        throw std::runtime_error{str_error.at(error::user_input_is_command)};
    }
        
    //Entity or Alias already taken?
    for(const auto& account : all_accounts){
        if(account.get_alias() == alias){
            throw std::runtime_error{str_error.at(error::double_alias)};
        }
        if(account.get_alias() == entity){
            throw std::runtime_error{str_error.at(error::alias_equal_entity)};
        }
    }
    
    Time_Account new_account{entity, alias};
    all_accounts.push_back(new_account);
            
    jsonH->save_json_accounts(all_accounts);
    jsonH->save_json_entity(all_accounts, entity, {});
            
    std::cout << "-> " << alias << " | " << entity << translator.language_pack.at("saved") << std::endl;
    
}

void Arg_Manager::add_hours(std::vector<Time_Account>& all_accounts){
	
    bool found_alias = false;
    auto localTime = clock.get_time();
    
    for(auto& account : all_accounts){
                        
        if(str_argv[1] == account.get_alias() ){
            float time_float = stof(str_argv[2]);
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
            jsonH->save_json_entity(all_accounts, account.get_entity(), {});

            found_alias = true;

            break;	
        }
    }
    
    if(found_alias){
        std::cout 
            << std::put_time(&localTime, translator.language_pack.at("timepoint").c_str()) << '\n'
            << translator.language_pack.at("time_saved")
            << std::endl;
    }else{
    
        throw std::runtime_error{str_error.at(error::not_found)};
    }
}

void Arg_Manager::add_sensor_data(std::vector<Time_Account>& all_accounts){
    bool found_alias = false;
    auto localTime = clock.get_time();
    std::string output_sensor;

    for(auto& account : all_accounts){
        if(str_argv[1] == account.get_alias() ){
            
            BME_Sensor sensor{};
            if(sensor.scan_sensor(output_sensor) == 1){
                throw std::runtime_error(str_error.at(error::sensor));
            }

            Entry entry{0.f, output_sensor, localTime};
            account.add_entry(entry);

            jsonH->save_json_accounts(all_accounts);
            jsonH->save_json_entity(all_accounts, account.get_entity(), {});

            found_alias = true;

            break;	
        }
    }
    
    if(found_alias){
        std::cout 
            << std::put_time(&localTime, translator.language_pack.at("timepoint").c_str()) << '\n'
            << translator.language_pack.at("time_saved") << '\n'
            << output_sensor << '\n'
            << "BME Sensor Connection closed"
            << std::endl;
    }else{
    
        throw std::runtime_error{str_error.at(error::not_found)};
    }
}

void Arg_Manager::set_table_width(const std::vector<Time_Account>& all_accounts, std::vector<int>& max_length){
		
    for(const auto& account : all_accounts){

        int alias_size = account.get_alias().size();
        if(alias_size > max_length[0]){
            max_length[1] = alias_size;
        }
                    
        int entity_size = account.get_entity().size();
        if(entity_size > max_length[1]){
            max_length[2] = entity_size + 5;
        }
    }
}

void Arg_Manager::show_filepaths()const {
    std::cout 
        << "Config: " << jsonH->get_config_filepath() << '\n'
        << translator.language_pack.at("entity") << ": " << jsonH->get_entity_filepath() << '\n' 
        << "Accounts: " << jsonH->get_accounts_filepath() << std::endl;
}

void Arg_Manager::show_language() const{
    std::cout << translator.language_pack.at("str_language")<< ": " << translator.language_pack.at("language") << std::endl;
}

void Arg_Manager::show_specific_table(const std::vector<Time_Account>& show_accounts) {
		
    set_table_width(show_accounts, max_length);
    
    int index{0};
    int index_entrys = 0;
    
    //Trennlinie 
    std::cout << '\n' << std::setfill('=') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
        << "=" << std::setfill(' ') << '\n' << std::endl;
        
    for(const auto& account : show_accounts){
        
        std::cout << std::left 
            << std::setw(max_length[0]) << "index"
            << std::setw(max_length[1]) << "Alias"
            << std::setw(max_length[2]) << translator.language_pack.at("entity")
            << std::setw(max_length[3]) << translator.language_pack.at("total_hours")
            << std::endl;
            
        //Trennlinie 
        std::cout << std::setfill('-') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
            << "-" << std::setfill(' ') << std::endl;

            
        //Datenzeilen
        std::cout << std::left
            << std::setw( max_length[0]) << index
            << std::setw( max_length[1]) << account.get_alias()
            << std::setw( max_length[2]) << account.get_entity()
            << std::setprecision(3) << std::setw( max_length[3]) << account.get_hours()
            << std::endl;
            
        //Trennlinie 
        std::cout << std::setfill('-') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
            << "-" << std::setfill(' ') << '\n' << std::endl;

        ++index;
        if(account.get_entry().empty()){
            std::cout << translator.language_pack.at("No_Entrys_available") << std::endl;
            continue;
        }
        
        std::cout << std::left
            << std::setw( max_length[0]) << "index"
            << std::setw(max_length[4]) << translator.language_pack.at("hours")
            << std::setw(max_length[5]) << translator.language_pack.at("timestamp")
            << std::setw(max_length[6]) << translator.language_pack.at("comment")
            << std::endl;
            
        for(const auto& entry : account.get_entry()){
            std::stringstream ss;
            
            std::string time_format = translator.language_pack.at("timepoint");
            ss << std::put_time(&entry.time_point, time_format.c_str());

            //Trennlinie 
            std::cout << std::setfill('-') << std::setw( max_length[0] + max_length[4] + max_length[5] + max_length[6]) << "-" << std::setfill(' ') << std::endl;
            
            std::cout << std::left
                << std::setw( max_length[0]) << index_entrys
                << std::setw(max_length[4]) << entry.hours
                << std::setw(max_length[5]) << ss.str()
                << std::setw(max_length[6]) << entry.description
                << std::endl;
                                
            ++index_entrys;
        }
        //Trennlinie 
        std::cout << '\n' << std::setfill('=') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
            << "=" << std::setfill(' ') << '\n' << std::endl;
    }
}


void Arg_Manager::show_all(const std::vector<Time_Account>& all_accounts) {
		
    set_table_width(all_accounts, max_length);
    //Trennlinie 
    std::cout << '\n' << std::setfill('=') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
        << "=" << std::setfill(' ') << '\n' << std::endl;
        
    std::cout << std::left 
        << std::setw( max_length[0]) << "index"
        << std::setw( max_length[1]) << "Alias"
        << std::setw( max_length[2]) << translator.language_pack.at("entity")
        << std::setw( max_length[3]) << translator.language_pack.at("total_hours")
        << std::endl;
    
    int index{0};
    for(const auto& account : all_accounts){

        //Trennlinie 
        std::cout << std::setfill('-') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) << "-" << std::setfill(' ') << std::endl;

        //Datenzeilen
        std::cout << std::left
            << std::setw( max_length[0]) << index
            << std::setw( max_length[1]) << account.get_alias()
            << std::setw( max_length[2]) << account.get_entity()
            << std::setprecision(3) << std::setw( max_length[3]) << account.get_hours()
            << std::endl;
        
        ++index;
    }

    //Trennlinie 
    std::cout << '\n' << std::setfill('=') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
        << "=" << std::setfill(' ') << '\n' << std::endl;
}
