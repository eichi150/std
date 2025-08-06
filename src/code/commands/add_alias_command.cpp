#include "add_alias_command.h"

//==========================//
//==Add_Alias_Command=======//
//==========================//

Add_Alias_Command::Add_Alias_Command(
	std::vector<Time_Account>& all_accounts
	, const std::vector<std::string>& str_argv
	, std::shared_ptr<JSON_Handler> jsonH
	, const std::string& arg_alias
	, std::shared_ptr<ErrorLogger> logger
	
) : Alias_Command(std::move(str_argv), all_accounts, arg_alias, std::move(logger))
	, jsonH(jsonH)
{
	log("Add--Alias_Command BaseClass for adding Data to Alias");
};

void Add_Alias_Command::finalize() {
	if(!account){
		throw Logged_Error("Unknown Alias", logger);	
	}
	add_alias();
}

void Add_Alias_Command::add_alias(){
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
	//Save Account Entity File
	if(!jsonH){
		throw Logged_Error("jsonH missing", logger);
	}
	jsonH->save_json_entity(all_accounts, account->get_entity());
}

//==========================//
//==Hours_Add_Alias_Command=//
//==========================//

Hours_Add_Alias_Command::Hours_Add_Alias_Command(
	std::vector<Time_Account>& all_accounts
	, const std::vector<std::string>& str_argv
	, std::shared_ptr<JSON_Handler> jsonH
	, const std::map<command, std::regex>& regex_pattern
	, std::shared_ptr<Translator> translator
	, const std::string& arg_alias
	, std::shared_ptr<ErrorLogger> logger
	
) : Add_Alias_Command(all_accounts, str_argv, jsonH, arg_alias, std::move(logger))
	, regex_pattern(regex_pattern)
	, translator(translator)
{
	log("add Hours to Alias");
	Clock clock{};
	localTime = clock.get_time();
};

void Hours_Add_Alias_Command::execute() {
	
	if(!account){
		throw Logged_Error("Unknown Alias", logger);
	}
	if(str_argv.size() < 3){
		throw Logged_Error("Insufficient arguments", logger);
	}

	if(!std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
		add_output("insert number for time value");
		log("insert number for time value");
		
		throw Logged_Error("insert number for time value", logger);
	}
	
	add_hours();
	
	//Save to file
	finalize();
	//Save All_Accounts
	
	jsonH->save_json_accounts(all_accounts);
	
	if(!translator){
		throw Logged_Error("Translator missing", logger);
	}
	std::stringstream ss;
	ss
		<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
		<< translator->language_pack.at("time_saved");
	add_output(ss.str());
   }	

void Hours_Add_Alias_Command::add_hours(){
	
	float time_float;
	try{
		time_float = stof(str_argv[2]);
		if (str_argv.size() > 3
			&& std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
		{
			time_float /= 60.f;
		}
	}catch(const std::out_of_range& re){
		log("error addhours string_to_float " + std::string{re.what()} );
		throw Logged_Error("insert number for time value", logger);
	}
			
	std::stringstream description;
	if(str_argv.size() > 4){
		for(size_t i{4}; i < str_argv.size(); ++i){
			description << str_argv[i] << " ";
		}
	}
			
	Entry entry{time_float, description.str(), localTime};
	account->add_entry(entry);
}

//==========================//
//==Tag_Add_Alias_Command===//
//==========================//

Tag_Add_Alias_Command::Tag_Add_Alias_Command(
	std::vector<Time_Account>& all_accounts
	, const std::vector<std::string>& str_argv
	, std::shared_ptr<JSON_Handler> jsonH
	, const std::string& arg_alias
	, std::shared_ptr<ErrorLogger> logger
	
) : Add_Alias_Command(all_accounts, str_argv, jsonH, arg_alias, std::move(logger))
{
	log("add Tag to Alias");
};

void Tag_Add_Alias_Command::execute() {

	if(!account){
		throw Logged_Error("Unknown Alias", logger);
	}
	if(str_argv.size() < 4){
		throw Logged_Error("Tag value not provided", logger);
	}
	account->set_tag(str_argv[3]);
	
	//Save to file
	finalize();
	
	std::stringstream ss;
	ss 
		<< "Tag: " 
		<< account->get_tag() << " to " 
		<< account->get_alias() << " added" 
		<< std::endl;
		
	log(ss.str());
	add_output(ss.str());
};

//==========================//
//SensorData_Add_Alias_Command//
//==========================//

#ifdef __linux__
SensorData_Add_Alias_Command::SensorData_Add_Alias_Command(
	std::vector<Time_Account>& all_accounts
	, const std::vector<std::string>& str_argv
	, std::shared_ptr<JSON_Handler> jsonH
	, std::shared_ptr<Translator> translator
	, const std::string& arg_alias
	, std::shared_ptr<ErrorLogger> logger
	
) : Add_Alias_Command(all_accounts, str_argv, jsonH, arg_alias, std::move(logger))
	, translator(translator)
	
{
	log("add Sensordata to Alias");
	Clock clock{};
	localTime = clock.get_time();
};

void SensorData_Add_Alias_Command::execute() {
	
	if(!account){
		throw Logged_Error("Unknown Alias", logger);
	}
	//add data
	add_sensor_data();
	
	//Save to file
	finalize();
	
	if(!translator){
		throw Logged_Error("Translator missing", logger);
	}
	std::stringstream output;
	output
		<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
		<< translator->language_pack.at("time_saved") << '\n'
		<< output_str.str()
		<< std::setfill('=') << std::setw(10) << "=" << std::setfill(' ');

	log(output.str() + device.get_name() + " Connection closed");
	add_output(output.str());
};

void SensorData_Add_Alias_Command::add_sensor_data(){
	
	log("check Sensor for Data");
	
	std::vector<float> output_sensor = device.check_device("BME280");
	
	if(output_sensor.size() < 3){
		throw Logged_Error("No Sensor output", logger);
	}
	
	output_str 
		<< translator->language_pack.at("Temperature") << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
		<< translator->language_pack.at("Pressure") << ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
		<< translator->language_pack.at("Humidity") << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
				
	Entry entry{0.f, output_str.str(), localTime};
	account->add_entry(entry);
}

#endif // __linux__
