#ifdef __linux__
#include "handle_crontab.h"

//==========================//
//==Crontab=================//
//==========================//

Crontab::Crontab(
	std::vector<std::string>& str_argv
	, const std::map<command, std::regex>& regex_pattern
	, std::shared_ptr<JSON_Handler> jsonH
	, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
	, std::vector<Automation_Config>& automations
	
) : str_argv(str_argv)
	, regex_pattern(regex_pattern)
	, jsonH(jsonH)
	, _split_line(_split_line)
	, automation_config(automations)
	
{
	try{
		//automation_config = jsonH->read_automation_config_file();
		current_Crontab = get_current_Crontab();
	}catch(const std::runtime_error& re){
		log(re.what());
	}
	
	integer_pattern = regex_pattern.at(command::integer);
	
	str_weekday = {
		  {weekday::sunday, 	"sunday"}
		, {weekday::monday, 	"monday"}
		, {weekday::tuesday, 	"tuesday"}
		, {weekday::wednesday, 	"wednesday"}
		, {weekday::thursday, 	"thursday"}
		, {weekday::friday, 	"friday"}
		, {weekday::saturday, 	"saturday"}
	};
		
	str_months = {
		  {months::january, 	"january"}
		, {months::february,	"february"}
		, {months::march, 		"march"}
		, {months::april, 		"april"}
		, {months::may, 		"may"}
		, {months::june, 		"june"}
		, {months::july, 		"july"}
		, {months::august, 		"august"}
		, {months::september, 	"september"}
		, {months::october, 	"october"}
		, {months::november, 	"november"}
		, {months::december, 	"december"}
	};
	
};


void Crontab::finalize(){
	
	log("save automation_config.json");
	jsonH->save_automation_config_file(automation_config);
	 
	//neue Zeile anhängen
	std::ofstream out("/tmp/mycron", std::ios::app);
	out << cronLine;
	out << '\n';
	out.close();
	
	log("change crontab config");
	
	//set new crontab using popen
	std::string cmd = "crontab /tmp/mycron";
	FILE* pipe = popen(cmd.c_str(), "r");
	if(pipe){
		pclose(pipe);
	}
	//Clean up temp file
	std::remove("/tmp/mycron");
}

bool Crontab::crontab_contains(const std::vector<std::string>& crontabLines, const std::vector<std::string>& searched_targets){
	
	for(const auto& line : crontabLines){
		if(line.empty() || line.size() < 8 || line.front() == '\n' || line.front() == ' '){
			continue;
		}
		
		if(searched_targets.size() >= 1){
			//targetLine in crontab?
			if(line.find(searched_targets[1]) != std::string::npos){
				log("searched command: " + searched_targets[1]);
				return true;
			}
		}	
		
		if(line.find(searched_targets[0]) != std::string::npos){
			log("searched line: " + searched_targets[0]);
			return true;
		}
	}
	return false;
}

std::string Crontab::convert_crontabLine_to_speaking_str(const std::string& crontab_line){
	std::string result;
	// 0 */1 * * *
	// Every 1 hour @0 minutes 
	std::vector<std::string> splitted_crontab = _split_line(crontab_line, std::regex{R"([\s]+)"});
	
	for(size_t i{0}; i < splitted_crontab.size(); ++i){

		if(splitted_crontab[i] == "*"){
			continue;
		}
		
		bool every_x = false;
		
		if(std::regex_match(splitted_crontab[i], integer_pattern)){
			result.append("@ ");
		}else{

			std::string temp;
			bool found = false;
			for(const auto& c : splitted_crontab[i]){
				if(found){
					temp += c;
				}
				if(c == '/'){
					found = true;
					result.append("Every ");
					if(i == 0 || i == 1){
						every_x = true;
					}
				}
			}
			splitted_crontab[i] = temp;
								
		}
		//Uhrzeit //hours : minutes
		if(i == 1 && !every_x){
			result = {};	
			result.append(splitted_crontab[1] + ":" + splitted_crontab[0] + " o'clock");

		//Every or at X minutes
		}else if(i == 0){
			result.append(splitted_crontab[i]);
			result.append(" minutes ");	
		}
		//Every X hours
		else if(i == 1){
			result.append(splitted_crontab[i]);
			result.append(" hours ");
		}

		//Every or at X day of month
		if(i == 2){
			result.append(splitted_crontab[i]);
			result.append(" day of month ");			
		}
		//Every or at X months
		if(i == 3){
			result.append(splitted_crontab[i]);
			result.append(" month ");			
		}
		//Every or at X weekday
		if(i == 4){
			result.append(splitted_crontab[i]);
			
			int int_day;
			try{
				int_day = std::stoi(result.substr(result.size() -1));
				
				weekday day = static_cast<weekday>(int_day);
				result.pop_back();
				result.append(str_weekday.at(day));
				
			}catch(const std::invalid_argument& e){
				add_output("Invalid number format");
				log(e.what());
			}catch(const std::out_of_range& re){
				add_output("Insert valid number ");
				log(re.what());
			}
		}
	}
	return result;
}

std::vector<std::string> Crontab::get_current_Crontab(){

	std::vector<std::string> lines;
	std::string cmd = "crontab -l 2>/dev/null";
	std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);

	if(!pipe){
		throw std::runtime_error{"popen() failed"};
	}

	char buffer[128];
	while(fgets(buffer, sizeof(buffer), pipe.get()) != nullptr){
		/*if(std::string(buffer).empty() || std::string(buffer) == "\n"){
			continue;
		}*/
		lines.push_back(std::string(buffer));
	}
	
	return lines;
}

//==========================//
//==delete_task_from_Crontab//
//==========================//


delete_task_from_Crontab::delete_task_from_Crontab(
	std::vector<std::string>& str_argv
	, const std::map<command, std::regex>& regex_pattern
	, std::shared_ptr<JSON_Handler> jsonH
	, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
	, std::vector<Automation_Config>& automations
) : Crontab(
	  str_argv
	, regex_pattern
	, jsonH
	, _split_line
	, automations)
{
	alias = str_argv[1];
	to_do_flags.reset();
};


void delete_task_from_Crontab::interact() {
	
	bool found_alias = std::any_of(
		automation_config.begin(), automation_config.end(),
		[this](const auto& config){
			return config.alias == this->alias;
		}
	);
	if(!found_alias){
		throw Logged_Error("Alias has no Crontab automations", logger);
	}
	
	// <alias> -deactivate -measure -all
	if( std::regex_match(str_argv[4], regex_pattern.at(command::all)) ){
		to_do_flags.set(static_cast<int>(to_do::erase_all));
		log("Delete All " + alias + " from Crontab and automation_config");
	}
	
	
	// <alias> -deactivate -measure -detail
	if( std::regex_match(str_argv[4], regex_pattern.at(command::detail)) ){
		to_do_flags.set(static_cast<int>(to_do::erase_detail));				
		log("Delete Detail of " + alias + " from Crontab and automation_config");
	}
	
	if( to_do_flags.test(static_cast<int>(to_do::erase_all))
		|| to_do_flags.test(static_cast<int>(to_do::erase_detail)) )
	{
		//process delete
		process_delete();
	
		//Collect Output
		std::stringstream ss;
		ss << "Crontab Task is ";
		if(automation_config.empty()){		
			ss << "NOT in use\n"
				<< "Deleted " << alias << " from "
				<< (is_cut_out_automations ? "automation_config.json " : "")
				<< "and Crontab automated measuring";
			add_output(ss.str());	
			return;
		}
		ss << (is_crontab_command_still_needed ? "NOT in use\n" : "in use\n")
			<< alias << " from "
			<< (is_cut_out_automations ? "automation_config.json " : "")
			<< (is_crontab_command_still_needed ? "and Crontab " : "")
			<< "automated measuring deleted";
		
		add_output(ss.str());
	}else{
		throw SyntaxError{
			"> <alias> -deactivate -measure -all\n> <alias> -deactivate -measure -detail"
		};
	}
};

void delete_task_from_Crontab::process_delete(){
	
	//Process ALL
	if( to_do_flags.test(static_cast<int>(to_do::erase_all)) ){
		erase_complete_alias_out_automation_config();
		check_is_crontab_task_used();
		if(is_crontab_command_still_needed){
				
			log("only delete out of automation_config.json");
			output_vector = current_Crontab;
		}else{
			output_vector = erase_out_crontab();
		}
	}
	//Process DETAIL
	if( to_do_flags.test(static_cast<int>(to_do::erase_detail)) ){
		erase_detail_out_automation_config();
		check_is_crontab_task_used();
		
		output_vector = erase_out_crontab();	
	}
	
	//push crontab vector in a single string
	write_cronLine();
	
	//finalize -> Save Files	
	finalize();
}

void delete_task_from_Crontab::erase_detail_out_automation_config(){
	
	std::vector<Automation_Config> vec = automation_config;

	// Auswahl vorbereiten
	std::vector<std::string> choose_one;
	std::vector<std::string> chosen_as_cmd;
	for (const auto& cfg : vec) {
		if (cfg.alias == alias) {
			choose_one.push_back(convert_crontabLine_to_speaking_str(cfg.crontab_command));
			chosen_as_cmd.push_back(cfg.crontab_command);
		}
	}

	if (choose_one.empty()) {
		throw std::invalid_argument{"No entry with this alias exists"};
	}
	//create Listbox for User
	Listbox listbox{choose_one};
	int index = listbox.select_option();
	std::string chosen = choose_one[index];
	std::string chosen_cmd = chosen_as_cmd[index];

	add_output(std::string{"You selected: (" + chosen + ')' });
	log(std::string{chosen_cmd + " [Index " + std::to_string(index) + "]"});
	
	// Command merken, um aus Crontab zu löschen
	str_command.push_back(chosen_cmd);

	// Aus automation_config löschen
	auto it = std::remove_if(vec.begin(), vec.end(),
		[&](const Automation_Config& config){
			return config.alias == alias && config.crontab_command == chosen_cmd;
		});

	if (it != vec.end()) {
		vec.erase(it, vec.end());
		is_cut_out_automations = true;
		automation_config = vec;
	} else {
		throw std::runtime_error{"Nothing to delete"};
	}
}


void delete_task_from_Crontab::erase_complete_alias_out_automation_config(){
	
	//erase Alias Config out of automation_config.json
	std::vector<Automation_Config> fresh_automation_config;
	log("Automation size before delete: " + std::to_string(automation_config.size()));		
	
	std::remove_copy_if(
		automation_config.begin(), automation_config.end(),
		std::back_inserter(fresh_automation_config),
		[this](const auto& config){
			if(config.alias == this->alias){
				
				this->str_command.push_back(config.crontab_command);
				return true;
			}
			
			return false;
		}
	);
	log("Automation size after delete: " + std::to_string(fresh_automation_config.size()) );
	
	if(fresh_automation_config.size() < automation_config.size()){
		is_cut_out_automations = true;
		automation_config = fresh_automation_config;
	}else{
		throw std::runtime_error{"Nothing to delete"};
	}
}

void delete_task_from_Crontab::check_is_crontab_task_used(){
	//MUSS DER CRONTAB GELÖSCHT WERDEN?? oder gibts andere alias die den crontabcommand nutzen?
	
	std::vector<std::string> still_used;
	for (const auto& cmd : str_command) {
		//auch den letzten eitnrag entfernen
		if(automation_config.empty()){
			is_crontab_command_still_needed = false;
			obsolete_cmds.push_back(cmd);
			break;
		}
		bool used = std::any_of(
			automation_config.begin(), automation_config.end(),
			[&cmd](const auto& conf) {
				return conf.crontab_command == cmd;
			}
		);
		if (used) {
			still_used.push_back(cmd);
		} else {
			obsolete_cmds.push_back(cmd);
		}
	}
	
	if(!automation_config.empty()){
		is_crontab_command_still_needed = !obsolete_cmds.empty() && still_used.empty();
	}
	std::stringstream ss_log;
	ss_log << "Obsolete commands: " << obsolete_cmds.size() << "\n"
		<< "Still used commands: " << still_used.size() << "\n"
		<< "Crontab Task is " << (is_crontab_command_still_needed ? "NOT in use" : "in use");
	
	log(ss_log.str());
}

std::vector<std::string> delete_task_from_Crontab::erase_out_crontab(){
		
	//erase out of crontab config
	log("Crontab size before delete: " + std::to_string(current_Crontab.size()) );
	
	std::vector<std::string> vec = current_Crontab;
	vec.erase(
		std::remove_if(
			vec.begin(),
			vec.end(),
			[this](const std::string& line) {
				return std::any_of(
					this->obsolete_cmds.begin(), this->obsolete_cmds.end(),
					[&line](const std::string& cmd_to_check) {
						return line.find(cmd_to_check) != std::string::npos;
					}
				);
			}
		),
		vec.end()
	);
	log("Crontab size after: " +  std::to_string(vec.size()) );
		
	return vec;
}

void delete_task_from_Crontab::write_cronLine(){
	std::ostringstream cron;
	for(const auto& line : output_vector){
		if(line.empty() || line[0] == '\n'){
			continue;
		}
		cron << line;
	}
	cronLine = cron.str();
}


//==========================//
//==write_into_Crontab======//
//==========================//

write_into_Crontab::write_into_Crontab(
	std::vector<std::string>& str_argv
	, const std::map<command, std::regex>& regex_pattern
	, std::shared_ptr<JSON_Handler> jsonH
	, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
	, std::vector<Automation_Config>& automations
	
) : Crontab(
	  str_argv
	, regex_pattern
	, jsonH
	, _split_line
	, automations)
{
	connection_type = "I2C";
	executioner = "Crontab";
	entity = str_argv[0];
	alias = str_argv[1];
	device_name = str_argv[2];
};

void write_into_Crontab::interact() {
	log("Crontab Interaktion");		
	set_user_automation_crontab();
	//save files
	finalize();
};

void write_into_Crontab::set_user_automation_crontab(){
	
	//Verarbeite User Arguments
	
	crontab_line = get_user_crontab_line();
	
	//write Command into Crontab
	if( write_Crontab(jsonH, crontab_line.first, alias, crontab_line.second) ){
		
		log("Crontab written");
		add_output("Crontab written");
	}else{
		
		log("Crontab existiert bereits");
		add_output("Crontab existiert bereits. Kein neuer Eintrag in Crontab erforderlich");
	}
	
	Automation_Config new_cfg{
		  connection_type
		, entity, alias 
		, executioner
		, crontab_line.first
		, convert_crontabLine_to_speaking_str(crontab_line.first)
		,(crontab_line.second ? "true" : "false")
		, device_name
	};
	bool already_in = std::any_of(
		automation_config.begin(), automation_config.end(),
		[&new_cfg](const Automation_Config& cfg){
			return cfg == new_cfg;
		}
	);
	if(already_in){
		throw std::runtime_error{"Automation Config already set"};
	}
	//automation hinzufügen
	automation_config.push_back(new_cfg);
	
	std::stringstream output;
	output 
		<< device_name << " measures for " << alias << " from "<< entity << " over " << connection_type
		<< "\n"
		<< "Time for " << executioner << " to execute: "
		<< convert_crontabLine_to_speaking_str(crontab_line.first)
		<< (crontab_line.second ? " with Logfile" : "");
			
	//Print
	add_output(output.str());
}


std::pair<std::string, bool> write_into_Crontab::get_user_crontab_line(){

	std::vector<std::string> crontab = {
		{
		  "*" //minutes
		, "*" //hours
		, "*" //day of month
		, "*" //month
		, "*" //day of week
		}
	};
	
	bool with_logfile = false;
	bool found_command = false;
			
	for(size_t i{1}; i < str_argv.size(); ++i){

		if(std::regex_match(str_argv[i], integer_pattern)){
			continue;
		}
		
		std::string parameter = str_argv[i - 1];
		
		//Logfile aktivieren/ deaktivieren
		if( std::regex_match(str_argv[i], regex_pattern.at(command::logfile)) ){
			with_logfile = true;
			continue;
		}

		//alle X minuten
		if(std::regex_match(str_argv[i], regex_pattern.at(command::minutes)) ){
			
			if(!std::regex_match(parameter, integer_pattern)){
				throw std::invalid_argument{"Number for Minutes required"};			
			}
			
			crontab[0].append("/" + check_that_between_A_B(parameter, 0, 59, "Minutes"));
			found_command = true;
			continue;
		}

		//alle X stunden
		if(std::regex_match(str_argv[i], regex_pattern.at(command::hours)) ){
		
			found_command = true;
			crontab[0] = "0";
			
			if(!std::regex_match(parameter, integer_pattern)){
				continue;
			}
			
			crontab[1].append("/" + check_that_between_A_B(parameter, 0, 23, "Hours"));
			continue;
		}

		//zur X uhrzeit
		if( std::regex_match(str_argv[i], regex_pattern.at(command::clock)) ){
			
			std::vector<std::string> splitted_line = _split_line(parameter, std::regex{R"([:])"});

			std::string hours;
			std::string minutes;
			
			if(splitted_line.size() == 1){
				hours = splitted_line[0];
				minutes = "0";
			}
			if(splitted_line.size() == 2){
				hours = splitted_line[0];
				minutes = splitted_line[1];
			}
			if( !std::regex_match(hours, integer_pattern) 
				|| !std::regex_match(minutes, integer_pattern) )
			{
				throw std::invalid_argument{"Number for time value required"};
			}
			
			if(hours.size() > 2 || minutes.size() > 2){
				throw std::invalid_argument{"Number to big for HH::MM format"};
			}
			
			crontab[0] = check_that_between_A_B(minutes, 0, 59, "Clock Minutes");
			crontab[1] = check_that_between_A_B(hours, 0, 23, "Clock Hours");
			
			found_command = true;
			continue;
		}

		//am X.ten Tag im monat
		if( std::regex_match(str_argv[i], regex_pattern.at(command::day)) ){
		
			if(!std::regex_match(parameter, integer_pattern)){
				continue;
			}
			
			found_command = true;
			crontab[2] = check_that_between_A_B(parameter, 1, 31, "Tag im Monat");
			continue;
		}

				
		//alle X monate
		for(size_t m{0}; m < str_months.size(); ++m){

			std::string pattern = str_months[static_cast<months>(m)];
			pattern = "\\b(" + pattern + ")\\b";	
			std::regex month_pattern{pattern};
			
			if(std::regex_match(str_argv[i], month_pattern)){
				for(const auto& key : str_months){
					if(key.second == str_months[static_cast<months>(m)]){
						int int_month = static_cast<int>(key.first);
						crontab[3] = std::to_string(int_month);
						found_command = true;

						break;
					}
				}
				break;
			}
		}
				

		//alle X wochentage
		for(size_t day{0}; day < str_weekday.size(); ++day){

			std::string pattern = str_weekday[static_cast<weekday>(day)];
			pattern = "\\b(" + pattern + ")\\b";	
			std::regex day_pattern{pattern};
			
			if(std::regex_match(str_argv[i], day_pattern)){
				for(const auto& key : str_weekday){
					if(key.second == str_weekday[static_cast<weekday>(day)]){
						int int_weekday = static_cast<int>(key.first);
						crontab[4] = std::to_string(int_weekday);
						found_command = true;

						break;
					}
				}
				break;
			}
		}
	}

	if(!found_command){
		throw SyntaxError{"[time_value] Syntax wrong"};
	}
	
	std::string crontab_line;
	for(const auto& c : crontab){
		crontab_line.append(c + ' ');
	}
	crontab_line.pop_back();
	
	return {crontab_line, with_logfile};
}

std::string write_into_Crontab::check_that_between_A_B(
	const std::string& str
	, int A
	, int B
	, const std::string& error_prompt)
{
	
	int num;
	try{
		num = stoi(str);
		
	}catch(const std::invalid_argument& e){
		throw std::invalid_argument{error_prompt + " isnt a number"};
	}catch(const std::out_of_range& e){
		throw std::invalid_argument{error_prompt + " is out of range"};
	}
	
	if(num < A || num > B){
		throw std::invalid_argument{error_prompt + " allowed between " + std::to_string(A) + " - " + std::to_string(B)};
	}
	return std::to_string(num);
}

bool write_into_Crontab::write_Crontab(
	const std::shared_ptr<JSON_Handler>& jsonH
	, const std::string& str_command
	, const std::string& alias
	, bool logfile)
{
		
	std::string exe_filepath = jsonH->getExecutableDir() + "/";
	log("Crontab Exe Filepath " + exe_filepath);
		
	std::string __cronLine = str_command + " " + exe_filepath + "std -auto " + "\"" + str_command + "\"" + " -me";
	logLine = " >> " + exe_filepath + alias + "_std.log 2>&1";
	log(__cronLine + "\n" + logLine);
			
	//alte Crontab sichern
	//system("crontab -l > /tmp/mycron");
	//Backup old crontab
	FILE* pipe = popen("crontab -l", "r");
	if(pipe){
		std::ofstream backup("/tmp/mycron");
		char buffer[128];
		while(fgets(buffer, sizeof(buffer), pipe) != nullptr){
			backup << buffer;
		}
		pclose(pipe);
	}
	
	if(crontab_contains(current_Crontab, {__cronLine, str_command}) ){
		return false;
	}
	
	cronLine = __cronLine;
	if(logfile){
		cronLine.append(logLine);
	}
	return true;
}

#endif // __linux__
