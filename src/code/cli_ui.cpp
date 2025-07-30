#include "cli_ui.h"

CLI_UI::CLI_UI(std::shared_ptr<Arg_Manager> manager) : arg_man(std::move(manager)){};

void CLI_UI::update(){
	
	const auto flags = arg_man->get_output_flags();
	
	if(flags.test(static_cast<size_t>(OutputType::show_help)) ){
		
		show_help();
	}
	if(flags.test(static_cast<size_t>(OutputType::show_filepaths)) ){
		
		show_filepaths();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_language)) ){
		
		show_language();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_alias_table)) ){
		
		show_alias_table();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_alias_automation)) ){
		
		show_alias_automation_table();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_all_accounts))){
		
		show_all_accounts();
		
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_all_log))){
		
		std::cout 
			<< "\n===== Arg_Manager_Log: =====\n"
			<< arg_man->get_log() 
			<< "\n===== UI_Interface_Log: =====\n"
			<< ui_interface_log.str()
			<< "\n===== Log_Ende ====="
			<< std::endl;
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_user_output_log))){
		
		std::cout << arg_man->get_user_output_log() << std::endl;
	}
	
	arg_man->clear_output_flags();
}

void CLI_UI::show_help(){
	std::cout << help << std::endl;
}

void CLI_UI::show_filepaths() {
	std::cout 
        << "Config: " << arg_man->jsonH->get_config_filepath() << '\n'
        << arg_man->translator.language_pack.at("entity") << ": " << arg_man->jsonH->get_entity_filepath() << '\n' 
        << "Accounts: " << arg_man->jsonH->get_accounts_filepath() << std::endl;
}


void CLI_UI::show_language() {
    std::cout 
		<< arg_man->translator.language_pack.at("str_language")
		<< ": " 
		<< arg_man->translator.language_pack.at("language") << std::endl;
}

void CLI_UI::show_all_accounts(){

	std::cout << create_all_accounts_table() << std::endl;

}

void CLI_UI::show_alias_automation_table(){
	
	std::cout << create_automation_table(arg_man->get_str_argv()[2]) << std::endl;
	
}

void CLI_UI::show_alias_table(){
	std::cout << create_alias_table() << std::endl;
}


std::string CLI_UI::create_line(int width, const char& symbol){
	std::ostringstream output_stream;
	//Trennlinie 
    output_stream 
		<< '\n' << std::setfill(symbol) << std::setw(width) 
        << std::string{symbol} << std::setfill(' ') << '\n';
       
     return output_stream.str();
}
size_t CLI_UI::scale_str_size(const std::string& str){
	
	float scaled_comment = static_cast<float>(str.size()) * 1.5f;
	
	return static_cast<int>(scaled_comment);
}

int CLI_UI::get_sum_str_size(const std::vector<std::string>& multiple_str){
	int sum = 0;
	for(const auto& str : multiple_str){
		sum += static_cast<int>(scale_str_size(str));
	}
	return sum;
}

std::string CLI_UI::create_automation_table(const std::string& account_alias){
	
	try{
	//Read Automation_Config.json
	if(all_automations.empty()){
		all_automations = arg_man->jsonH->read_automation_config_file();
	}
	
	}catch(const std::runtime_error& re){
		ui_interface_log << "CLI_UI AutomationTable " << re.what() << std::endl;
	}
	
	//HEAD Parameter
	const std::string INDEX_str{"Index"};
	const std::string DEVICE_str{"Device"};
	const std::string CRON_TASK_str{"Crontab Task"};
	const std::string CRON_CMD_str{"Crontab Command"};
	
	if(table_width == 0){
		table_width = get_sum_str_size({INDEX_str, DEVICE_str, CRON_TASK_str, CRON_CMD_str});
	}
	//BODY
	std::stringstream ss_body;
	
	int index = 0;
	
	for(const auto& automation : all_automations){
		if(automation.alias != account_alias){
			continue;
		}
		
		std::string crontab_task = "Crontab_Task";
			
							  
		ss_body << std::left
			<< std::setw(scale_str_size(INDEX_str)) << index << '|' 
			<< std::setw(scale_str_size(DEVICE_str)) << automation.device_name << '|' 
			<< std::setw(scale_str_size(CRON_TASK_str)) << crontab_task << '|'
			<< std::setw(scale_str_size(CRON_CMD_str)) << automation.crontab_command
			
			<< create_line(table_width, '-') << '\n';
				                  
		++index;
	}
	
	
	//HEAD
	std::stringstream ss_head;
	ss_head << std::left
		<< create_line(table_width, '=') 
		<< '\n'
		
		<< "Automations @ " + account_alias
		<< create_line(table_width, '-')
		
		//empty Strings
		<< std::setw(scale_str_size(INDEX_str)) << std::string{" "} << std::setfill(' ') << '|' 
		<< std::setw(scale_str_size(DEVICE_str)) << std::string{" "} << std::setfill(' ') << '|'
		<< std::setw(scale_str_size(CRON_TASK_str)) << std::string{" "} << std::setfill(' ') << '|'
		<< '\n'
		
		<< std::setw(scale_str_size(INDEX_str)) << INDEX_str << '|' 
		<< std::setw(scale_str_size(DEVICE_str)) << DEVICE_str << '|'
		<< std::setw(scale_str_size(CRON_TASK_str)) << CRON_TASK_str << '|'
		<< std::setw(scale_str_size(CRON_CMD_str)) << CRON_CMD_str
		
		<< create_line(table_width, '=') << '\n';
		
	//TABLE	
	std::ostringstream table;
	
	table 
		<< ss_head.str()
		<< ss_body.str();
		
	return table.str();
}

std::string CLI_UI::create_data_table(const std::string& alias){

	const std::string INDEX_str{"Index"};
	const std::string TIMESTAMP_str{"Timestamp"};
	const std::string HOURS_str{"Hours"};
	const std::string COMMENT_str{"Comment"};
	table_width = get_sum_str_size({INDEX_str, TIMESTAMP_str, HOURS_str, COMMENT_str});
		
	//BODY
	int body_data_width = 0;
	
	std::stringstream ss_body;
		
	std::shared_ptr<Time_Account> account = arg_man->get_account_with_alias(alias);
		
	int index = 0;	
	for(const auto& entry : account->get_entry()){
		
		//Ermittel die maximale Breite der Datentabelle. Ggf. Tabelle neu bauen wenn zu klein
		int max_data_width = 0;
		int max_length_comment = scale_str_size(COMMENT_str);
		
		for(const auto& entry : account->get_entry()){
			int d_str_size = scale_str_size(entry.description);
			if(d_str_size <= max_length_comment){
				continue;
			}
			max_length_comment = d_str_size;
			std::stringstream ss_timepoint;
			
			std::string time_format = arg_man->translator.language_pack.at("timepoint");
			ss_timepoint << std::put_time(&entry.time_point, time_format.c_str());
			std::stringstream ss_hours;
			ss_hours << std::setprecision(3) << entry.hours;
			
			//Calculate body_data_width
			body_data_width = get_sum_str_size({std::to_string(index), ss_timepoint.str(), ss_hours.str()});
			//scale Comment
			float scaled_comment = static_cast<float>(entry.description.size()) * 1.1f;
			body_data_width += static_cast<int>(scaled_comment);
			
			if(body_data_width > max_data_width){
				max_data_width = body_data_width;
			}
		}
				
		//Wenn body_data_width größer als die head_width wird die tabelle neu gebaut -> passende Größe
		table_width = max_data_width;
		
		std::stringstream ss_timepoint;
		
		std::string time_format = arg_man->translator.language_pack.at("timepoint");
		ss_timepoint << std::put_time(&entry.time_point, time_format.c_str());
						  
		ss_body << std::left
			<< std::setw(scale_str_size(INDEX_str)) << index << '|' 
			<< std::setw(scale_str_size(TIMESTAMP_str)) << ss_timepoint.str() << '|' 
			<< std::setw(scale_str_size(HOURS_str)) << std::setprecision(3) << entry.hours << '|'
			<< std::setw(max_length_comment) << entry.description
		
			<< create_line(table_width, '-') << '\n';                  
		++index;
	}
 	
 	//HEAD
 	std::stringstream ss_head_data;	
	ss_head_data << std::left
		<< create_line(table_width, '=')
		<< '\n'
		
		<< "Data: @" + alias
		<< create_line(table_width, '-')
		
		//empty Strings
		<< std::setw(scale_str_size(INDEX_str)) << std::string{" "} << std::setfill(' ') << '|' 
		<< std::setw(scale_str_size(TIMESTAMP_str)) << std::string{" "} << std::setfill(' ') << '|'
		<< std::setw(scale_str_size(HOURS_str)) << std::string{" "} << std::setfill(' ') << '|'
		<< '\n'
		
		<< std::setw(scale_str_size(INDEX_str)) << INDEX_str << '|' 
		<< std::setw(scale_str_size(TIMESTAMP_str)) << TIMESTAMP_str << '|'
		<< std::setw(scale_str_size(HOURS_str)) << HOURS_str << '|'
		<< std::setw(scale_str_size(COMMENT_str)) << COMMENT_str
		
		<< create_line(table_width, '=') 
		<< '\n';
	//TABLE
	std::ostringstream table;
	
	table
		<< ss_head_data.str()
		<< ss_body.str();
	
	return table.str();
}

std::string CLI_UI::create_alias_table(){
	//DATA
	std::string alias = arg_man->get_str_argv()[2];
	
	std::shared_ptr<Time_Account> account = arg_man->get_account_with_alias(alias);
	
	std::string t_alias = "@ " + alias;
	
	std::string entity = account->get_entity();
	std::string tag = account->get_tag();
	
	//Read Automation_Config.json File
	std::string automation_table_str = create_automation_table(alias);
	
	std::string device_name;
	bool is_automation = std::any_of(
		all_automations.begin(), all_automations.end(),
		[&alias, &device_name](const Automation_Config& automation){
			if(automation.alias != alias){
				return false;
			}
			device_name = automation.device_name;
			return true;
		}
	);
	
	
	std::string t_automation = "Automation: " + (is_automation ? device_name : "None");
	
	std::vector<std::string> table_strings = {
		t_alias, tag, entity, t_automation
	};
	//Daten Tabelle bauen. Breite entscheidet table_width
 	std::string data_table = create_data_table(alias);
 	
 	if(table_width == 0){
		table_width = get_sum_str_size(table_strings);
	}
 	//HEAD
	std::stringstream ss_head;
	ss_head << std::left
		<< create_line(table_width, '=') 
		<< '\n'
		
		<< std::setw(scale_str_size(t_alias)) << t_alias
		<< std::setw(scale_str_size(tag)) << tag
		<< std::setw(scale_str_size(entity)) << entity
		<< std::setw(scale_str_size(t_automation)) << t_automation
		<< '\n';
 	
 	//TABLE
	std::ostringstream table;
	table 
		<< ss_head.str()
		<< automation_table_str	
		<< data_table
		
		<< std::endl;
	
	return table.str();
}

std::string CLI_UI::create_all_accounts_table(){
	
	auto all_accounts = arg_man->get_all_accounts();
	
	const std::string INDEX_str{"Index"};
	const std::string ALIAS_str{"Alias"};
	std::string entity_str = arg_man->translator.language_pack.at("entity");
	std::string total_hours_str = arg_man->translator.language_pack.at("total_hours");
	
	int index{0};
	if(table_width == 0){
		
		int hold_width;
		for(const auto& account : all_accounts){
			hold_width = get_sum_str_size({std::to_string(index), account.get_alias(), account.get_entity(), std::to_string( static_cast<int>(account.get_hours()) )});
			
			if(hold_width > table_width){
				table_width = hold_width;
			}
		}
		
		hold_width = get_sum_str_size({INDEX_str, ALIAS_str, entity_str, total_hours_str});
			
		if(hold_width > table_width){
			table_width = hold_width;
		}
	}
	//BODY
	std::stringstream ss_body;
	
    for(const auto& account : all_accounts){
			
        //Datenzeilen
		ss_body << std::left
			<< create_line(table_width, '-')
            << std::setw( scale_str_size(INDEX_str)) << index << '|' 
            << std::setw( scale_str_size(ALIAS_str)) << account.get_alias() << '|' 
            << std::setw( scale_str_size(entity_str)) << account.get_entity() << '|' 
            << std::setw( scale_str_size(total_hours_str)) << std::setprecision(3) << account.get_hours()
            << std::endl;
        
        ++index;
    }
	
	//HEAD
	std::stringstream ss_head;
	ss_head << std::left
		<< create_line(table_width, '=') 
		<< '\n'
		
		//empty Strings
		<< std::setw(scale_str_size(INDEX_str)) << std::string{" "} << std::setfill(' ') << '|' 
		<< std::setw(scale_str_size(ALIAS_str)) << std::string{" "} << std::setfill(' ') << '|'
		<< std::setw(scale_str_size(entity_str))<< std::string{" "} << std::setfill(' ') << '|'
		<< '\n'
		
		<< std::setw(scale_str_size(INDEX_str)) << INDEX_str << '|' 
		<< std::setw(scale_str_size(ALIAS_str)) << ALIAS_str << '|' 
		<< std::setw(scale_str_size(entity_str)) << entity_str << '|' 
		<< std::setw(scale_str_size(total_hours_str)) << total_hours_str;
		
	//TABLE
	std::ostringstream table;
	table
		<< create_line(table_width, '-') 
		<< '\n'
		<< "All Accounts:"
		<< ss_head.str()
		<< '\n'
		<< ss_body.str()
		<< create_line(table_width, '=')
		<< std::endl;
	
	return table.str();
}


