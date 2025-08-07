#include "cli_ui.h"

CLI_UI::CLI_UI(
		std::shared_ptr<ErrorLogger> _output_logger
		, std::shared_ptr<Manager> _manager
		, std::shared_ptr<JSON_Handler> _jsonH
		, bool _run_env
		, std::shared_ptr<Cmd_Ctrl> _ctrl
	) : output_logger(std::move(_output_logger))
		, jsonH(std::move(_jsonH))
		, run_env(_run_env)
		, ctrl(_ctrl)
		, _rx(ctrl) // Replxx
	{
		
		if(_manager){
			translator = _manager->get_translator_ptr();
			if(run_env){
				env_man = std::dynamic_pointer_cast<Env_Manager>(_manager);
			}else{
				arg_man = std::dynamic_pointer_cast<Arg_Manager>(_manager);
			}
		}
	};

void CLI_UI::update(){
	
	if(ctrl && jsonH && output_logger){
		standard();

		if(run_env && env_man){
			run_environment();
			env_man->clear_output_flags();
			env_man->hold_env_running();
		}
		if(arg_man){
			arg_man->clear_output_flags();
		}
	}
}

bool CLI_UI::is_env_running() const {
	return run_env;
}
void CLI_UI::pass_tabcompletions_to_rx(){
	//collect alias names as strings for tab insertion
	std::vector<std::string> alias_strings;
	for(const auto& acc : env_man->get_all_accounts()){
		alias_strings.push_back(acc.get_alias());
	}
	//collect entity names as strings for tab insertion
	std::vector<std::string> entity_strings;
	std::set<std::string> unique_entities;
	for (const auto& acc : env_man->get_all_accounts()) {
		unique_entities.insert(acc.get_entity());
	}
	entity_strings.assign(unique_entities.begin(), unique_entities.end());

	_rx.set_tab_completion(alias_strings, entity_strings);
};

void CLI_UI::run_environment(){
	
	if(!first_run){
		std::cout << print_centered_message("Type exit to Quit", 30, '-');
		first_run = true;
	}else{
		std::cout << print_centered_message("", 30, '~');
	}
	//Reset Env_Manager Argc/Str_Argv
	env_man->reset_args();
	//collect alias & entity strings for tab completion in replexx
	pass_tabcompletions_to_rx();

	//while input != exit || quit => run_environment = true
	std::pair<int, std::vector<std::string>> argc_input_buffer;
    run_env =_rx.run_replxx(argc_input_buffer);

	//Pass input to Env_Manager if Environment should further run
	if(run_env){
		env_man->setup_next_iteration(argc_input_buffer);
	}else{
		std::cout << print_centered_message("Goodbye", 30, '-');
	}
}

void CLI_UI::standard(){
	
	if(!translator){
		throw std::runtime_error{"translator missing\n"};
	}

	OutputBitset flags;

	if(arg_man){
		flags = arg_man->get_output_flags();
	}
	if(env_man){
		flags = env_man->get_output_flags();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_help)) ){
		
		//ui_interface_log <<  "show help\n";
		show_help();
	}
	if(flags.test(static_cast<size_t>(OutputType::show_filepaths)) ){
		
		//ui_interface_log <<  "show filepaths\n";
		show_filepaths();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_language)) ){
		
		//ui_interface_log <<  "show language\n";
		show_language();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_alias_table)) ){
		
		//ui_interface_log <<  "show alias Table\n";
		show_alias_table();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_alias_automation)) ){
		
		//ui_interface_log <<  "show alias automation table\n";
		show_alias_automation_table();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_all_accounts))){
		
		//ui_interface_log <<  "show all_accounts\n";
		show_all_accounts();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_entity))){
		//ui_interface_log << "show_entitiy";
		show_entity_table();
	}
	if(flags.test(static_cast<size_t>(OutputType::show_user_output_log))){
		
		//ui_interface_log << "show cmd user output\n";
		std::cout << output_logger->get_logs() << std::flush;
	}
}

void CLI_UI::show_help(){
	std::cout << help << std::flush;
}

void CLI_UI::show_filepaths() {
	std::cout 
        << "Config: " << jsonH->get_config_filepath() << '\n'
        << translator->language_pack.at("entity") << ": " << jsonH->get_entity_filepath() << '\n' 
        << "Accounts: " << jsonH->get_accounts_filepath() << std::flush;
}

void CLI_UI::show_language() {
    std::cout 
		<< translator->language_pack.at("str_language")
		<< ": " 
		<< translator->language_pack.at("language") << std::flush;
}

void CLI_UI::show_all_accounts(){

	std::cout << create_all_accounts_table() << std::flush;
}

void CLI_UI::show_alias_automation_table(){
	if(arg_man){
		std::cout << create_automation_table(arg_man->get_str_argv()[2]) << std::flush;
	}
	if(env_man){
		std::cout << create_automation_table(env_man->get_str_argv()[2]) << std::flush;
	}
}

void CLI_UI::show_alias_table(){
	std::cout << create_alias_table() << std::flush;
}
void CLI_UI::show_entity_table(){
	std::cout << create_entity_table() << std::flush;
}

std::string CLI_UI::print_centered_message(const std::string& msg, int total_width, char fill_char){
	int msg_len = static_cast<int>(msg.size());
	int padding = (total_width - msg_len) / 2;

	if(padding < 0){
		padding = 0;
	}

	std::ostringstream output_stream;
	//Trennlinie 
	output_stream 
		<< std::setfill(fill_char) << std::setw(padding) << ""
		<< msg
		<< std::setw(total_width - padding - msg_len) << "" << std::setfill(' ') 
		<< '\n';
	return output_stream.str();
}
std::string CLI_UI::create_line(int width, const char& symbol){
	std::ostringstream output_stream;
	//Trennlinie 
    output_stream 
		<< std::setfill(symbol) << std::setw(width) 
        << std::string{symbol} << std::setfill(' ') << '\n';
       
     return output_stream.str();
}


std::string CLI_UI::create_entity_table(){
	
	// ------------------------------------------------------------
	// Table structure: Index | Alias
	// Dynamic width adjustment based on content
	// ------------------------------------------------------------

	const std::string INDEX_str = "Index";
	const std::string ALIAS_str = "Alias";
	const std::string SEP = "|";

	// Helper function: Sum of string lengths (including optional separators)
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

	
	// Determine maximum table width (initially with header)
	int max_data_width = get_sum_str_size({INDEX_str, ALIAS_str}, SEP) + 6; // +6 fr Puffer/Padding

	// BODY-Daten vorbereiten
	std::stringstream ss_body;
	int index = 0;
	std::vector<Time_Account> all_accounts;
	if(arg_man){
		all_accounts = arg_man->get_all_accounts();
	}
	if(env_man){
		all_accounts = env_man->get_all_accounts();
	}

	for (const auto& account : all_accounts) {
		std::string alias = account.get_alias();

		// Calculate required width for this row
		int body_data_width = get_sum_str_size({std::to_string(index), alias}, SEP);

		// Scale alias length slightly for visual adjustment
		int scaled_alias_width = static_cast<int>(static_cast<float>(alias.size()) * 1.5f);
		body_data_width += scaled_alias_width;

		// If larger than previously known width → adopt
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
			<< std::setw(table_width) << std::left << "📂 " + entity << '\n'
			<< std::string(table_width, '=') << '\n';

	//TABLE
	std::ostringstream table;
	table << ss_head.str()
		  << ss_body.str();

	return table.str();

}

std::string CLI_UI::create_alias_table(){
	//DATA
	std::string alias;
	if(arg_man){
		alias = arg_man->get_str_argv()[2];
		account = arg_man->get_account_with_alias(alias);
	}
	if(env_man){
		alias = env_man->get_str_argv()[2];
		account = env_man->get_account_with_alias(alias);
	}
	
	std::string t_alias = "@ " + alias;
	std::string entity = "<[- " + account->get_entity() + " -]>";
	std::string acc_tag = account->get_tag();
	std::string tag = (acc_tag.empty() ? "" : std::string{"[ Tag: " + acc_tag + " ]"});
	
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

	const int emoji_offset = 2;
	int col_t_alias_width  = get_max_width(t_alias, {t_alias}) + emoji_offset;
	int col_entity_width  = get_max_width(entity, {entity});
	int col_tag_width = get_max_width(tag, {tag});
	int col_t_automation_width  = get_max_width(t_automation, {t_automation});

	// Gesamtbreite + Trennzeichen
	table_width = col_t_alias_width + col_entity_width + col_tag_width + col_t_automation_width + 7;

	col_t_alias_width  = scale_int_to_float(col_t_alias_width, 1.5f);
	col_entity_width  = scale_int_to_float(col_entity_width, 1.5f);
	col_tag_width = scale_int_to_float(col_tag_width, 1.5f);
	col_t_automation_width  = scale_int_to_float(col_t_automation_width, 1.5f);
	table_width = scale_int_to_float(table_width, 1.5f);

 	//HEAD
	std::stringstream ss_head;
	ss_head << std::left
		<< std::setw(col_entity_width) << "📂 " + entity
		<< '\n'
		<< create_line(table_width, '=') 
		<< '\n'
		
		<< std::setw(col_t_alias_width) << t_alias
		<< std::setw(col_tag_width) << tag
		
		<< std::setw(col_t_automation_width) << t_automation
		<< '\n';
 	
 	//TABLE
	std::ostringstream table;
	table 
		<< ss_head.str()
		<< automation_table_str	
		<< data_table;
	
	return table.str();
}

int CLI_UI::get_max_width(const std::string& header, const std::vector<std::string>& values){
	int max_width = static_cast<int>(header.size());
	for(const auto& val : values){
		if(static_cast<int>(val.size()) > max_width){
			max_width = static_cast<int>(val.size());
		}
	}
	return max_width;
}

int CLI_UI::scale_int_to_float(int num, float scaling){
	float scale = static_cast<float>(num) * scaling;
	return static_cast<int>(scale);
};


std::string CLI_UI::create_data_table(const std::string& alias){

	if(arg_man && !account){
		account = arg_man->get_account_with_alias(alias);
	}	
	if(env_man && !account){
		account = env_man->get_account_with_alias(alias);
	}

	std::vector<std::string> indices;
	std::vector<std::string> timestamps;
	std::vector<std::string> hours;
	std::vector<std::string> comments;

	int index = 0;
	for(const auto& entry : account->get_entry()){
		
		indices.push_back(std::to_string(index++));

		std::stringstream ss_timepoint;
		std::string time_format = translator->language_pack.at("timepoint");
		ss_timepoint << std::put_time(&entry.time_point, time_format.c_str());
		timestamps.push_back(ss_timepoint.str());

		hours.push_back(std::to_string(static_cast<int>(entry.hours)));
		comments.push_back(entry.description);
		
	}

	const std::string INDEX_str{"#"};
	const std::string TIMESTAMP_str{"Timestamp"};
	const std::string HOURS_str{"Hours"};
	const std::string COMMENT_str{"Comment"};

	const int emoji_offset = 2;
	int col_index_width  = get_max_width(INDEX_str, indices) + emoji_offset;
	int col_timestamp_width  = get_max_width(TIMESTAMP_str, timestamps);
	int col_hours_width = get_max_width(HOURS_str, hours);
	int col_comment_width  = get_max_width(COMMENT_str, comments);

	// Gesamtbreite + Trennzeichen
	table_width = col_index_width + col_timestamp_width + col_hours_width + col_comment_width + 3;

	col_index_width  = scale_int_to_float(col_index_width, 1.5f);
	col_timestamp_width  = scale_int_to_float(col_timestamp_width, 1.5f);
	col_hours_width = scale_int_to_float(col_hours_width, 1.5f);
	col_comment_width  = scale_int_to_float(col_comment_width, 1.5f);
	table_width = scale_int_to_float(table_width, 1.5f);

	std::stringstream ss_body;
	index = 0;
	for(const auto& entry : account->get_entry()){
		std::stringstream ss_timepoint;
	
		std::string time_format = translator->language_pack.at("timepoint");
		ss_timepoint << std::put_time(&entry.time_point, time_format.c_str());
						
		ss_body << std::left
			<< std::setw(col_index_width) << index << '|' 
			<< std::setw(col_timestamp_width) << ss_timepoint.str() << '|' 
			<< std::setw(col_hours_width) << std::setprecision(3) << entry.hours << '|'
			<< std::setw(col_comment_width) << entry.description

			<< '\n'
			<< create_line(table_width, '-');                  
		++index;
	}
	
	//HEAD
	std::stringstream ss_head;
	ss_head 
		<< '\n'
		<< create_line(table_width, '=')
		<< '\n'
		
		<< "📄 Data: @ " + alias
		<< '\n'
		<< create_line(table_width, '~')
		<< '\n'

		<< std::left
		<< std::setw(col_index_width)  << INDEX_str  << " "   
		<< std::setw(col_timestamp_width)  << TIMESTAMP_str  << " "
		<< std::setw(col_hours_width) << HOURS_str << " "
		<< std::setw(col_comment_width)  << COMMENT_str << '\n';

	//TABLE
	std::ostringstream table;
	
	table
		<< ss_head.str()
		<< ss_body.str();
	
	return table.str();
}


std::string CLI_UI::create_all_accounts_table(){
	
	std::vector<Time_Account> all_accounts;
	if(arg_man){
		all_accounts = arg_man->get_all_accounts();
	}
	if(env_man){
		all_accounts = env_man->get_all_accounts();
	}
	std::vector<std::string> indices;
	std::vector<std::string> aliases;
	std::vector<std::string> entities;
	std::vector<std::string> hours;

	int index = 0;
	for(const auto& acc : all_accounts){
		aliases.push_back(acc.get_alias());
		entities.push_back(acc.get_entity());
		hours.push_back(std::to_string(static_cast<int>(acc.get_hours())));
		indices.push_back(std::to_string(index++));
	}
	const std::string INDEX_str{"#"};
	const std::string ALIAS_str{"Alias"};
	std::string entity_str = translator->language_pack.at("entity");
	std::string total_hours_str = translator->language_pack.at("total_hours");

	const int emoji_offset = 2;
	int col_index_width  = get_max_width(INDEX_str, indices) + emoji_offset;
	int col_alias_width  = get_max_width(ALIAS_str, aliases);
	int col_entity_width = get_max_width(entity_str, entities);
	int col_hours_width  = get_max_width(total_hours_str, hours);

	// Gesamtbreite + Trennzeichen
	table_width = col_index_width + col_alias_width + col_entity_width + col_hours_width + 3;

	col_index_width  = scale_int_to_float(col_index_width, 1.5f);
	col_alias_width  = scale_int_to_float(col_alias_width, 1.5f);
	col_entity_width = scale_int_to_float(col_entity_width, 1.5f);
	col_hours_width  = scale_int_to_float(col_hours_width, 1.5f);
	table_width = scale_int_to_float(table_width, 1.5f);
	//HEAD
	std::stringstream ss_head;
	ss_head 
		<< '\n'
		<< create_line(table_width, '=')
		<< '\n'
		<< std::left
		<< std::setw(col_index_width)  << INDEX_str  << " "   
		<< std::setw(col_alias_width)  << ALIAS_str  << " "
		<< std::setw(col_entity_width) << entity_str << " "
		<< std::setw(col_hours_width)  << total_hours_str;

	//BODY
	index = 0;
	std::stringstream ss_body;
	for(size_t i = 0; i < all_accounts.size(); ++i){
		const auto& acc = all_accounts[i];
		ss_body << '\n'
			<< create_line(table_width, '-')
			<< std::left
			<< std::setw(col_index_width)  << "🔍 " + std::to_string(i) << '|'
			<< std::setw(col_alias_width)  << acc.get_alias() << '|'
			<< std::setw(col_entity_width) << acc.get_entity() << '|'
			<< std::setw(col_hours_width)  << std::setprecision(3) << acc.get_hours();
	}
		
	//TABLE
	std::ostringstream table;
	table
		<< '\n'
		<< create_line(table_width, '-') 
		<< '\n'
		<< "📋 All Accounts:"
		<< ss_head.str()
		<< '\n'
		<< ss_body.str()
		<< '\n'
		<< create_line(table_width, '=')
		<< std::endl;
	
	return table.str();
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
	
	std::vector<std::string> indices;
	std::vector<std::string> devices;
	std::vector<std::string> exe_tasks;
	std::vector<std::string> cron_cmds;

	int index = 0;
	for(const auto& automation : all_automations){
		if(automation.alias != account_alias){
			continue;
		}
		devices.push_back(automation.device_name);
		exe_tasks.push_back(automation.executioner);
		cron_cmds.push_back(automation.crontab_command_speeking);
		
		indices.push_back(std::to_string(index++));		
	}
	//HEAD Parameter
	const std::string INDEX_str{"#"};
	const std::string DEVICE_str{"Device"};
	const std::string EXE_TASK_str{"Executioner"};
	const std::string CRON_CMD_str{"Crontab Command"};
	
	const int emoji_offset = 2;
	int col_index_width  = get_max_width(INDEX_str, indices) + emoji_offset;
	int col_device_width  = get_max_width(DEVICE_str, devices);
	int col_exe_task_width = get_max_width(EXE_TASK_str, exe_tasks);
	int col_cron_cmd_width  = get_max_width(CRON_CMD_str, cron_cmds);

	// Gesamtbreite + Trennzeichen
	table_width = col_index_width + col_device_width + col_exe_task_width + col_cron_cmd_width + 3;

	col_index_width  = scale_int_to_float(col_index_width, 1.5f);
	col_device_width  = scale_int_to_float(col_device_width, 1.5f);
	col_exe_task_width = scale_int_to_float(col_exe_task_width, 1.5f);
	col_cron_cmd_width  = scale_int_to_float(col_cron_cmd_width, 1.5f);
	table_width = scale_int_to_float(table_width, 1.5f);
	
	//BODY
	std::stringstream ss_body;
	index = 0;
	for(const auto& automation : all_automations){
		if(automation.alias != account_alias){
			continue;
		}
							  
		ss_body << std::left
			<< std::setw(col_index_width) << index << '|' 
			<< std::setw(col_device_width) << automation.device_name << '|' 
			<< std::setw(col_exe_task_width) << automation.executioner << '|'
			<< std::setw(col_cron_cmd_width) << automation.crontab_command_speeking
			
			<< '\n'
			<< create_line(table_width, '-') << '\n';
				                  
		++index;
	}
	
	//HEAD
	std::stringstream ss_head;
	ss_head << std::left
		<< '\n'
		<< create_line(table_width, '=') 
		<< '\n'
		
		<< "🛠️  Automations @ " << account_alias
		<< '\n'
		<< create_line(table_width, '-')
		
		//empty Strings
		<< std::setw(col_index_width) << std::string{" "} << std::setfill(' ') << '|' 
		<< std::setw(col_device_width) << std::string{" "} << std::setfill(' ') << '|'
		<< std::setw(col_exe_task_width) << std::string{" "} << std::setfill(' ') << '|'
		<< '\n'
		
		<< std::setw(col_index_width) << INDEX_str << " " 
		<< std::setw(col_device_width) << DEVICE_str << " "
		<< std::setw(col_exe_task_width) << EXE_TASK_str << " "
		<< std::setw(col_cron_cmd_width) << CRON_CMD_str
		
		<< '\n'
		<< create_line(table_width, '=') << '\n';
		
	//TABLE	
	std::ostringstream table;
	
	table
		<< ss_head.str()
		<< ss_body.str();
		
	return table.str();
}