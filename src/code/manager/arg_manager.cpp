#include "arg_manager.h"

void Manager::log(const std::string& msg){
    if(logger){
		logger->log(msg);
    }
}
	
void Manager::add_output(const std::string& output){
    if(output_logger){
		output_logger->log(output);
    }
}


Arg_Manager::Arg_Manager(
    std::shared_ptr<ErrorLogger> _logger
    , std::shared_ptr<ErrorLogger> _output_logger
    , std::shared_ptr<JSON_Handler> _jsonH
    , std::shared_ptr<Cmd_Ctrl> _ctrl
    , int _argc
    , std::vector<std::string> _str_argv

) : Manager(
		std::move(_logger)
		, _output_logger
		, std::move(_jsonH)
		, std::move(_ctrl)
		
	) , argc(_argc)
		, str_argv(_str_argv)
{
    log("===== Arg_Manager_Log: =====");
    log(std::string{__FILE__} + " - Arg_Manager");
    
	log("===== JsonHandler_Log: =====");
	if(!jsonH){
		throw std::invalid_argument{"JSON Handler cannot be null"};
	}
	jsonH->read_all_accounts(all_accounts);		
};

void Arg_Manager::manage(){
    //Zeige Hilfe an
    //help
    if(std::regex_match(str_argv[1], regex_pattern.at(command::help))){
		output_flags.set(static_cast<size_t>(OutputType::show_help));
		log("show help");
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
			
		log("show command");
			
		cmd = std::make_shared<Show_Command>(
			all_accounts
			, str_argv
			, jsonH
			, argc
			, ctrl->get_specific_regex_pattern(commands)
			, output_flags
			, logger
		);
		
		if(cmd){
			log("show flags got set");
		}
	}
    
    if(!cmd){
		switch(argc){
		//1.argc == std
			case 2:
			{
				if(check_two_args()){
					break;
				}
				log("error: Syntax wrong");
				throw SyntaxError{"currently unavailable"};
			}
				
			case 3:
			{	
				if(check_three_args()){
					break;
				}
				std::stringstream syntax;
				syntax 
					<< "> <alias> -delete\n> <alias> -measure\n"
					<< "> -delete <entity>\n"
					<< "> -language <language>\n> -touch BME280";
				
				throw SyntaxError{syntax.str()};
			}

			case 4:
			{
				if(check_four_args()){
					break;
				}
				log("error: Syntax wrong");
				std::stringstream syntax;
				syntax 
					<< "> -add <entity> <alias>\n> <alias> [ ] -h/ -m\n"
					<< "> <alias> -tag [ ]\n"
					<< "> -f <entityFilepath> <accountsFilepath>";
				throw SyntaxError{syntax.str()};
			}
				
			case 5:
			{
				if(check_five_args()){
					break;
				}
				log("error: Syntax wrong");
				std::stringstream syntax;
				syntax 
					<< "> <alias> [time value] -h -m [ ]\n"
					<< "> <alias> -activate -measure [time_value]\n"
					<< "> <alias> -deactivate -measure -all/ -detail\n"
					<< "> -cf <configFilepath> <entityFilepath> <accountsFilepath>";
				throw SyntaxError{syntax.str()};
			}

			case 6:
			{	
				if(check_six_args()){
					break;
				}
				log("syntax wrong");
				throw SyntaxError{"> -add <entity> <alias> -tag [ ]"};
			}

			
			default:
			{	
				log("switch case ArgManager Error");
				throw Logged_Error("Untitled Error", logger);
			}
		};
    }
    
    if(cmd){
	
		cmd->set_output_logger(output_logger);
		
		cmd->execute();

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
			log("error: unknown_flag, no command to execute");
			throw Logged_Error("No Flag set. No Command", logger);
		}
    }
}

bool Arg_Manager::check_two_args(){
    
    if(std::regex_match(str_argv[1], regex_pattern.at(command::environment))){
		log("Warning: start environment out of arg_manager");
		return true;
    }
    
    return false;
}

bool Arg_Manager::check_three_args(){
    
    //Account löschen
    //delete <alias> 
    if(std::regex_match(str_argv[1], regex_pattern.at(command::delete_)) ){

	log("delete <alias>");
	cmd = std::make_shared<Delete_Alias_Command>(
	    all_accounts
	    , str_argv
	    , jsonH
	    , str_argv[2]
	    , logger
	);
	return true;
	
    }
    //Entität und zugehörige Alias löschen
    //delete <entity>
    if(std::regex_match(str_argv[1], regex_pattern.at(command::delete_)) ){
	
	log("-delete <entity>");
	cmd = std::make_shared<Delete_Entity_Command>(
	    all_accounts
	    , str_argv
	    , jsonH
	    , str_argv[2]
	    , logger
	);
	
	return true;
    }
    
    //Language changeTo
    //-l ger
    if(std::regex_match(str_argv[1], regex_pattern.at(command::language))){
		    
	log("-l " + str_argv[2]);
	cmd = std::make_shared<Language_Change_Command>(
	    jsonH
	    , translator
	    , str_argv[2]
	    , logger
	);
		    
	add_output("Language changed to " + str_argv[2]);
	log("Language changed to " + str_argv[2]);
	return true;
    }

    //Connection abfrage
    //-touch BME280
    if(std::regex_match(str_argv[1], regex_pattern.at(command::touch_sensor))){
				
#ifdef __linux__
	log("-touch" + str_argv[2]);
	cmd = std::make_shared<TouchDevice_Command>(
	    str_argv[2]
	    , logger
	);
#else
log("-touch <device> Only available for Linux");
add_output("Only available for Linux");
#endif // __linux__
		
	return true;
    }
                                
    //Sensor Messwerte für <alias> speichern
    // measure <alias>
    if(std::regex_match(str_argv[1], regex_pattern.at(command::measure_sensor))){
                
#ifdef __linux__
	cmd = std::make_shared<SensorData_Add_Alias_Command>(
	    all_accounts
	    , str_argv
	    , jsonH
	    , translator
	    , str_argv[2]
	    , logger
	);
#else
log("<alias> -measure Only available for Linux");
add_output("Only available for Linux");
#endif // __linux__
		
	return true;
    }

    return false;
}

bool Arg_Manager::check_four_args(){
    //-f <entityFilepath> <accountsFilepath>
    if(std::regex_match(str_argv[1], regex_pattern.at(command::user_filepath))){

	log("user change entity and account filepath");
	cmd = std::make_shared<UserFilepath_Change_Command>(
	    jsonH
	    , translator
	    , str_argv
	    , logger
	);
	return true;
    }
		
    //Neuen Account hinzufügen
    //add <entity> <alias>
    if(std::regex_match(str_argv[1], regex_pattern.at(command::add))){
		    
		log("add_account_noTag");
		cmd = std::make_shared<Add_Alias>(
			all_accounts
			, jsonH
			, Add_Alias::Add_account{str_argv[2], str_argv[3], ""}
			, regex_pattern
			, logger
		);

		log( std::string{ "NewAccount for: " + str_argv[2] + " " + str_argv[3]} );
		return true;
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
			
	    log("add hours with comment");
	    cmd = std::make_shared<Hours_Add_Alias_Command>(
		all_accounts
		, str_argv
		, jsonH
		, ctrl->get_specific_regex_pattern(commands)
		, translator
		, str_argv[1]
		, logger
	    );
	    return true;
	}
	add_output("insert number for time value");
	log("insert number for time value");
	throw std::runtime_error{"insert number for time value"};
    }

    //tag nachträglich hinzufügen
    //<alias> -tag plant
    if(std::regex_match(str_argv[2], regex_pattern.at(command::tag))) 
    {

	log("add Tag to account"); 
	cmd = std::make_shared<Tag_Add_Alias_Command>(
	    all_accounts
	    , str_argv
	    , jsonH
	    , str_argv[1]
	    , logger
	);
	return true;
    }
    
    return false;
}

bool Arg_Manager::check_five_args(){
    
    //-cf <configFilepath> <entityFilepath> <accountsFilepath>
    if(std::regex_match(str_argv[1], regex_pattern.at(command::config_filepath))){
		    
	log("user change Config Filepath & entity and account filepath");
	cmd = std::make_shared<UserFilepath_Change_Command>(
	    jsonH
	    , translator
	    , str_argv
	    , logger
	);
                    
	std::stringstream ss; 
	ss << str_argv[2] << '\n' 
	    << str_argv[3] << '\n' 
	    << str_argv[4] 
	    << std::endl;
	log(ss.str());
	return true;
    }
    
    //Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
    //<alias> 'time' -h -m [ ]
    if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
	|| std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
    {
	if( std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
			
	    std::vector<command> commands = {
		command::hours
		, command::minutes
		, command::integer
	    };
			
	    log("add hours without comment");
	    cmd = std::make_shared<Hours_Add_Alias_Command>(
		all_accounts
		, str_argv
		, jsonH
		, ctrl->get_specific_regex_pattern(commands)
		, translator
		, str_argv[1]
		, logger
	    );
	    return true;
	}
	
	log("insert number for time value");
	throw Logged_Error("insert number for time value", logger);
    }
    
    //Automation konfigurieren
    //<alias> -activate -measure [time_value]
    //<alias> -deactivate -measure -all/ -detail 
    if( std::regex_match(str_argv[2], regex_pattern.at(command::activate)) 
		|| std::regex_match(str_argv[2], regex_pattern.at(command::deactivate)) )
    {
		log("Interact: Activate/ Deactivate");
#ifdef __linux__
		std::vector<command> commands = {
			command::activate
			, command::deactivate
			, command::all
			, command::detail
			, command::logfile
			, command::minutes
			, command::hours
			, command::clock
			, command::day
			, command::measure_sensor
			, command::integer
		};
		
		//creates split_line() Pointer
		std::function<std::vector<std::string>(const std::string&, const std::regex&)> split_line = 
			[this](const std::string& s, const std::regex& r){
			return this->ctrl->split_line(s, r);
			};
		
		cmd = std::make_shared<Interact_Alias_Command>(
			all_accounts
			, str_argv
			, ctrl->get_specific_regex_pattern(commands)
			, jsonH
			, split_line
			, str_argv[1]
			, logger
		);
	
#else
log("Only for Linux");
add_output("Only for Linux");
#endif // __linux	
		return true;
    }
	//Neuen Account hinzufügen
    //add <entity> <alias> -tag <tag>
    if(std::regex_match(str_argv[1], regex_pattern.at(command::add))){
		    
		log("add_account_noTag");
		cmd = std::make_shared<Add_Alias>(
			all_accounts
			, jsonH
			, Add_Alias::Add_account{str_argv[2], str_argv[3], str_argv[5]}
			, regex_pattern
			, logger
		);

		log( std::string{ "NewAccount for: " + str_argv[2] + " " + str_argv[3]} );
		return true;
    }	
    return false;
}

bool Arg_Manager::check_six_args(){
    
    //Neuen Account mit tag hinzufügen
    //std add <entity> <alias> -tag plant
    if(std::regex_match(str_argv[1], regex_pattern.at(command::add))
	&& std::regex_match(str_argv[4], regex_pattern.at(command::tag)) )
    {
	log("add_account_withTag " + std::to_string(all_accounts.size()) );
		    	    
	cmd = std::make_shared<Add_Alias>(
	    all_accounts
	    , jsonH
	    , Add_Alias::Add_account{str_argv[2], str_argv[3], str_argv[5]}
	    , regex_pattern
	    , logger
	);
	std::stringstream ss;
	ss << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << " | Tag: " << str_argv[5];
	add_output(ss.str());
	log(ss.str());
	return true;
    }
    return false;
}

std::shared_ptr<Time_Account> Arg_Manager::get_account_with_alias(const std::string& alias){
    log("get account Pointer with Alias");
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
	throw Logged_Error("Unknown Alias", logger);
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
	
const std::vector<Time_Account>& Arg_Manager::get_all_accounts() const {
    return all_accounts;
}

const std::vector<std::string>& Arg_Manager::get_str_argv() const {
    return str_argv;
}
