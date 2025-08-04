#include "cli_ui.h"

CLI_UI::CLI_UI(
		std::shared_ptr<ErrorLogger> output_logger
		, std::shared_ptr<Arg_Manager> manager
		, std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
	
	) : output_logger(std::move(output_logger))
		, arg_man(std::move(manager))
		, jsonH(std::move(jsonH))
		, translator(std::move(translator))
	{};

void CLI_UI::update(){
	
	const auto flags = arg_man->get_output_flags();
	
	if(flags.test(static_cast<size_t>(OutputType::show_help)) ){
		
		ui_interface_log <<  "show help\n";
		show_help();
	}
	if(flags.test(static_cast<size_t>(OutputType::show_filepaths)) ){
		
		ui_interface_log <<  "show filepaths\n";
		show_filepaths();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_language)) ){
		
		ui_interface_log <<  "show language\n";
		show_language();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_alias_table)) ){
		
		ui_interface_log <<  "show alias Table\n";
		show_alias_table();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_alias_automation)) ){
		
		ui_interface_log <<  "show alias automation table\n";
		show_alias_automation_table();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_all_accounts))){
		
		ui_interface_log <<  "show all_accounts\n";
		show_all_accounts();
		
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_entity))){
		ui_interface_log << "show_entitiy";
		show_entity_table();
	}
	if(flags.test(static_cast<size_t>(OutputType::show_user_output_log))){
		
		ui_interface_log << "show cmd user output\n";
		std::cout << output_logger->get_logs() << std::endl;
	}
	
	/*if(flags.test(static_cast<size_t>(OutputType::show_all_log))){
		
		std::cout 
			<< "\n===== UI_Interface_Log: =====\n"
			<< ui_interface_log.str()
			<< std::endl;
	}*/
	
	arg_man->clear_output_flags();
}

void CLI_UI::show_help(){
	std::cout << help << std::endl;
}

void CLI_UI::show_filepaths() {
	std::cout 
        << "Config: " << jsonH->get_config_filepath() << '\n'
        << translator->language_pack.at("entity") << ": " << jsonH->get_entity_filepath() << '\n' 
        << "Accounts: " << jsonH->get_accounts_filepath() << std::endl;
}


void CLI_UI::show_language() {
    std::cout 
		<< translator->language_pack.at("str_language")
		<< ": " 
		<< translator->language_pack.at("language") << std::endl;
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
void CLI_UI::show_entity_table(){
	std::cout << create_entity_table() << std::endl;
}

std::string CLI_UI::create_line(int width, const char& symbol){
	std::ostringstream output_stream;
	//Trennlinie 
    output_stream 
		<< '\n' << std::setfill(symbol) << std::setw(width) 
        << std::string{symbol} << std::setfill(' ') << '\n';
       
     return output_stream.str();
}
int CLI_UI::scale_str_size(const std::string& str){
	
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
		all_automations = jsonH->read_automation_config_file();
	}
	
	}catch(const std::runtime_error& re){
		ui_interface_log << "CLI_UI AutomationTable " << re.what() << std::endl;
	}
	
	//HEAD Parameter
	const std::string INDEX_str{"Index"};
	const std::string DEVICE_str{"Device"};
	const std::string EXE_TASK_str{"Executioner"};
	const std::string CRON_CMD_str{"Crontab Command"};
	
	table_width = get_sum_str_size({INDEX_str, DEVICE_str, EXE_TASK_str, CRON_CMD_str});
	
	//BODY
	std::stringstream ss_body;
	
	int index = 0;
	
	for(const auto& automation : all_automations){
		if(automation.alias != account_alias){
			continue;
		}
							  
		ss_body << std::left
			<< std::setw(scale_str_size(INDEX_str)) << index << '|' 
			<< std::setw(scale_str_size(DEVICE_str)) << automation.device_name << '|' 
			<< std::setw(scale_str_size(EXE_TASK_str)) << automation.executioner << '|'
			<< std::setw(scale_str_size(CRON_CMD_str)) << automation.crontab_command_speeking
			
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
		<< std::setw(scale_str_size(EXE_TASK_str)) << std::string{" "} << std::setfill(' ') << '|'
		<< '\n'
		
		<< std::setw(scale_str_size(INDEX_str)) << INDEX_str << '|' 
		<< std::setw(scale_str_size(DEVICE_str)) << DEVICE_str << '|'
		<< std::setw(scale_str_size(EXE_TASK_str)) << EXE_TASK_str << '|'
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
		
	if(!account){
		account = arg_man->get_account_with_alias(alias);
	}	
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
			
			std::string time_format = translator->language_pack.at("timepoint");
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
		
		std::string time_format = translator->language_pack.at("timepoint");
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
		
		<< "Data: @ " + alias
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

std::string CLI_UI::create_entity_table(){
	
	// ------------------------------------------------------------
	// Tabellenaufbau: Index | Alias
	// Dynamische Breitenanpassung je nach Inhalt
	// ------------------------------------------------------------

	const std::string INDEX_str = "Index";
	const std::string ALIAS_str = "Alias";
	const std::string SEP = "|";

	// Hilfsfunktion: Summe der Lngen von Strings (inkl. optionaler Separatoren)
	auto get_sum_str_size = [](const std::vector<std::string>& parts, const std::string& sep = "") -> int {
		int total = 0;
		for (const auto& part : parts) {
			total += static_cast<int>(part.size());
		}
		if (!sep.empty()) {
			total += static_cast<int>(sep.size()) * static_cast<int>(parts.size() - 1);
		}
		return total;
	};

	
	// Bestimme maximale Breite der Tabelle (Initial mit Header)
	int max_data_width = get_sum_str_size({INDEX_str, ALIAS_str}, SEP) + 6; // +6 fr Puffer/Padding

	// BODY-Daten vorbereiten
	std::stringstream ss_body;
	int index = 0;
	auto all_accounts = arg_man->get_all_accounts();

	for (const auto& account : all_accounts) {
		std::string alias = account.get_alias();

		// Berechne benätigte Breite fr diese Zeile
		int body_data_width = get_sum_str_size({std::to_string(index), alias}, SEP);

		// Skaliere Alias-L#nge leicht, um optische Anpassung zu simulieren
		int scaled_alias_width = static_cast<int>(static_cast<float>(alias.size()) * 1.5f);
		body_data_width += scaled_alias_width;

		// Falls grer als bisher bekannte Breite ? bernehmen
		max_data_width = std::max(max_data_width, body_data_width);

		++index;
	}

	// Tabelle entsprechend der maximalen Breite setzen
	int table_width = max_data_width;

	//BODY
	index = 0;
	for (const auto& account : all_accounts) {
		ss_body << std::left
				<< std::setw(8) << index          
				<< SEP << " "
				<< std::setw(table_width - 12) << account.get_alias() 
				<< SEP << '\n'
				<< std::string(table_width, '-') << '\n';
		++index;
	}

	//HEAD
	std::string entity = all_accounts.front().get_entity();
	std::stringstream ss_head;
	ss_head << std::string(table_width, '=') << '\n'
			<< std::setw(table_width) << std::left << entity << '\n'
			<< std::string(table_width, '=') << '\n';

	//TABLE
	std::ostringstream table;
	table << ss_head.str()
		  << ss_body.str();

	return table.str();

}

std::string CLI_UI::create_alias_table(){
	//DATA
	std::string alias = arg_man->get_str_argv()[2];
	if(!account){
		account = arg_man->get_account_with_alias(alias);
	}
	std::string t_alias = "@ " + alias;
	
	std::string entity = account->get_entity();
	std::string tag = " -|- Tag: " + account->get_tag();
	
	//Daten Tabelle bauen. Breite entscheidet table_width
 	std::string data_table = create_data_table(alias);
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

 	
 	/*int _width = get_sum_str_size(table_strings);
 	if(table_width < _width){
		table_width = _width;
	}*/
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
	std::string entity_str = translator->language_pack.at("entity");
	std::string total_hours_str = translator->language_pack.at("total_hours");
	
	int index{0};
	if(table_width == 0){
		
		int hold_width;
		for(const auto& account : all_accounts){
			hold_width = get_sum_str_size(
				{std::to_string(index)
					, account.get_alias()
					, account.get_entity()
					, std::to_string( static_cast<int>(account.get_hours()) )
					}
				);
			
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


