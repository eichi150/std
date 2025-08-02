#include "arg_manager.h"

Arg_Manager::Arg_Manager(
    std::shared_ptr<ErrorLogger> logger
    , std::shared_ptr<ErrorLogger> output_logger
    , std::shared_ptr<JSON_Handler> jH
    , std::shared_ptr<Cmd_Ctrl> ctrl_ptr
) : logger(logger)
    , output_logger(output_logger)
    , jsonH(jH)
    , ctrl(ctrl_ptr)
{
    set_logger(logger);
    set_output_logger(output_logger);
    
    //log("\n\n===== LOG =====");
    jsonH->read_all_accounts(all_accounts);
    log( std::string{"===== JsonHandler_Log: =====\n"  + jsonH->get_log()} );
    
    translator = std::make_shared<Translator>();
    log("set language");
    translator->set_language(jsonH->get_config_language());
    
};

void Arg_Manager::proceed_inputs(const int& _argc, const std::vector<std::string>& argv){

    this->str_argv = argv;
    this->argc = _argc;
    this->regex_pattern = ctrl->get_regex_pattern();
 
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
	);
	
	if(cmd){
	    log("show flags got set");
	}
    }
    
    
    if(!cmd){
	switch(argc){
    
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
		//arg_manager_log << "error: Syntax wrong\n";
		std::stringstream syntax;
		syntax 
		    << "> <alias> -delete\n> <alias> -measure\n"
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
	cmd->set_logger(logger);
	cmd->execute();
	//log( std::string{"===== Command_Log: =====\n"  + cmd->get_logs()} );
	
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
	    log("error: unknown_flag, no command to execute");
	    throw Logged_Error("Untitled Error", logger);
	}
    }
}

bool Arg_Manager::check_two_args(){
    
    /*if(std::regex_match(str_argv[1], regex_pattern.at(command::environment))){
	run_env = true;
	arg_manager_log << "start environment\n";
	return true;
    }*/
    
    return false;
}

bool Arg_Manager::check_three_args(){
    
    //Account löschen
    //<alias> -del
    if(std::regex_match(str_argv[2], regex_pattern.at(command::delete_)) ){

	log("<alias> -delete");
	cmd = std::make_shared<Delete_Alias_Command>(
	    all_accounts
	    , str_argv
	    , jsonH
	);
	return true;
	
    }else if(std::regex_match(str_argv[1], regex_pattern.at(command::delete_)) ){
	
	log("<entity> -delete");
	std::vector<Time_Account> entity_accounts;
	std::string entity = str_argv[2];
		
	std::copy_if(
	    all_accounts.begin(), all_accounts.end(),
	    std::back_inserter(entity_accounts),
	    [&entity](const auto& account){
		return account.get_entity() == entity;
	    }
	);
	
	log("Accounts found: " + std::to_string(entity_accounts.size()) );
	
	return true;
    }else{
	throw Logged_Error("No Accounts found", logger);
    }
    
    //Language changeTo
    //-l ger
    if(std::regex_match(str_argv[1], regex_pattern.at(command::language))){
		    
	log("-l " + str_argv[2]);
	cmd = std::make_shared<Language_Change_Command>(
	    jsonH
	    , translator
	    , str_argv[2]
	);
		    
	user_output_log << "Language changed to " << str_argv[2] << '\n';
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
	);
    #else
	log("-touch <device> Only available for Linux");
	user_output_log << "Only available for Linux\n";
    #endif // __linux__
		
	return true;
    }
                                
    //Sensor Messwerte für <alias> speichern
    // <alias> -mes
    if(std::regex_match(str_argv[2], regex_pattern.at(command::measure_sensor))){
                
    #ifdef __linux__
	cmd = std::make_shared<SensorData_Add_Alias_Command>(
	    all_accounts
	    , str_argv
	    , jsonH
	    , translator
	);
    #else
	log("<alias> -measure Only available for Linux");
	user_output_log << "Only available for Linux\n";
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
	    , Add_account{str_argv[2], str_argv[3], ""}
	    , regex_pattern
	);
                    
	user_output_log << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << '\n';
	log( std::string{"Account added" + str_argv[2] + " -> " + str_argv[3]} );
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
	    );
	    return true;
	}
	user_output_log << "insert number for time value\n";
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
	    );
	    return true;
	}
	user_output_log << "insert number for time value\n";
	log("insert number for time value");
	throw Logged_Error("insert number for time value", logger);
    }
    
    //Automation konfigurieren
    //<alias> -activate -measure [time_value]
    //<alias> -deactivate -measure -all/ -detail 
    #ifdef __linux__
    if( std::regex_match(str_argv[2], regex_pattern.at(command::activate)) 
	|| std::regex_match(str_argv[2], regex_pattern.at(command::deactivate)) )
    {
	log("Interact: Activate/ Deactivate");
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
	);
	return true;
    }
    #else
	log("Only for Linux");
	user_output_log << "Only for Linux\n";
    #endif // __linux
    
    return false;
}

bool Arg_Manager::check_six_args(){
    
    //Neuen Account mit tag hinzufügen
    //std add <entity> <alias> -tag plant
    if(std::regex_match(str_argv[1], regex_pattern.at(command::add))
	&& std::regex_match(str_argv[4], regex_pattern.at(command::tag)) )
    {
	log("add_account_withTag " + all_accounts.size());
		    	    
	cmd = std::make_shared<Add_Alias>(
	    all_accounts
	    , jsonH
	    , Add_account{str_argv[2], str_argv[3], str_argv[5]}
	    , regex_pattern
	);
	std::stringstream ss;
	ss << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << " | Tag: " << str_argv[5];
	user_output_log << ss.str() << "\n";
	log(ss.str());
	return true;
    }
    return false;
}

std::string Arg_Manager::get_user_output_log() const {
    return user_output_log.str();
}

std::shared_ptr<Time_Account> Arg_Manager::get_account_with_alias(const std::string& alias){
    log("get account Pointer with name");
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

void Arg_Manager::set_logger(std::shared_ptr<ErrorLogger> log){
    logger = std::move(log);
}
void Arg_Manager::log(const std::string& msg){
    if(logger){
	logger->log(msg);
    }
    throw std::runtime_error{"No Logger given"};
}


void Arg_Manager::set_output_logger(std::shared_ptr<ErrorLogger> log){
    output_logger = std::move(log);
}	
void Arg_Manager::add_output(const std::string& output){
    if(output_logger){
	output_logger->log(output);
    }
    throw std::runtime_error{"No Output Logger given"};
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

std::shared_ptr<Translator> Arg_Manager::get_translator_ptr() const {
    return translator;
}

std::vector<std::string> Arg_Manager::get_str_argv() const {
    return str_argv;
}
