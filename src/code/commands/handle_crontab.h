#ifndef HANDLE_CRONTAB_H
#define HANDLE_CRONTAB_H

#include "../abstract_class/interaction.h"
#ifdef __linux__
#include "../device_ctrl.h"

#include <functional>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <cstdlib>

enum class weekday{
	sunday = 0
	, monday
	, tuesday
	, wednesday
	, thursday
	, friday
	, saturday
};

enum class months{
	january = 1
	, february
	, march
	, april
	, may
	, june
	, july
	, august
	, september
	, october
	, november
	, december
};

class Crontab : public Interaction{
public:
	Crontab(
		std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
			
	) : str_argv(str_argv)
		, regex_pattern(regex_pattern)
		, str_error(str_error)
		, jsonH(jsonH)
		, _split_line(_split_line)
		
	{
		try{
			automation_config = jsonH->read_automation_config_file();
			current_Crontab = get_current_Crontab();
		}catch(const std::runtime_error& re){
			interaction_log << re.what();
		}
	};
	
	std::string get_log() const override {
		return interaction_log.str();
	}
	std::string get_user_output_log() const override {
		return user_output_log.str();
	};
	
	void interact() override = 0;
	
	void finalize(){
		
		interaction_log << "save automation_config.json\n";
		jsonH->save_automation_config_file(automation_config);
			
		//neue Zeile anhängen
		std::ofstream out("/tmp/mycron", std::ios::app);
		out << cronLine;
		out << '\n';
		out.close();
		
		interaction_log << "finalize change crontab config\n";
		//Neue Crontab setzen
		system("crontab /tmp/mycron");
		
		//aufräumen
		system("rm /tmp/mycron");
	}
	
protected:
	//container
	std::vector<std::string>& str_argv;
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	std::shared_ptr<JSON_Handler> jsonH;
	//func ptr
	std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line;
	
	//loaded data
	std::vector<Automation_Config> automation_config;
	std::vector<std::string> current_Crontab;
	//created data
	std::string cronLine;
	std::string logLine;
	
	bool crontab_contains(const std::vector<std::string>& crontabLines, const std::vector<std::string>& searched_targets){
		
		for(const auto& line : crontabLines){
			if(line.empty() || line.size() < 8 || line.front() == '\n' || line.front() == ' '){
				continue;
			}
			
			if(searched_targets.size() >= 1){
				//targetLine in crontab?
				if(line.find(searched_targets[1]) != std::string::npos){
					interaction_log << "searched command: " << searched_targets[1] << "\n";
					return true;
				}
			}	
			
			if(line.find(searched_targets[0]) != std::string::npos){
				interaction_log << "searched line: " << searched_targets[0] << "\n";
				return true;
			}
		}
		return false;
	}
	
private:

	std::vector<std::string> get_current_Crontab(){

		std::vector<std::string> lines;
		std::string cmd = "crontab -l 2>/dev/null";
		std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);

		if(!pipe){
			throw std::runtime_error{"popen() failed"};
		}

		char buffer[128];
		while(fgets(buffer, sizeof(buffer), pipe.get()) != nullptr){
			if(std::string(buffer).empty() || std::string(buffer) == "\n"){
				continue;
			}
			lines.push_back(std::string(buffer));
		}
		
		return lines;
	}
	
};

enum class to_do{
	erase_all = 0
	, erase_detail
};

class delete_task_from_Crontab : public Crontab{
public:
	delete_task_from_Crontab(
		std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
	) : Crontab(
		  str_argv
		, regex_pattern
		, str_error
		, jsonH
		, _split_line
		)
	{
		alias = str_argv[1];
		to_do_flags.reset();
	};
	
	std::string get_log() const override {
		return interaction_log.str();
	}
	std::string get_user_output_log() const override{
		return user_output_log.str();
	}
		
	void interact() override {
		
		// <alias> -deactivate -measure -all
		if( std::regex_match(str_argv[4], regex_pattern.at(command::all)) ){
			to_do_flags.set(static_cast<int>(to_do::erase_all));
			interaction_log << "Delete All " << alias << " from Crontab and automation_config\n";
		}
		
		
		// <alias> -deactivate -measure -detail
		if( std::regex_match(str_argv[4], regex_pattern.at(command::detail)) ){
			to_do_flags.set(static_cast<int>(to_do::erase_detail));				
			interaction_log << "Delete Detail of " << alias << " from Crontab and automation_config\n";
		}
		
		
		
		if( to_do_flags.test(static_cast<int>(to_do::erase_all)) ){
			erase_complete_alias_out_automation_config();
			check_is_crontab_task_used();
			if(is_crontab_command_still_needed){
					
				interaction_log << "only delete out of automation_config.json\n";
				output_vector = current_Crontab;
			}else{
				output_vector = erase_out_crontab();
			}
			
			
		}else if( to_do_flags.test(static_cast<int>(to_do::erase_detail)) ){
			erase_detail_out_automation_config();
			check_is_crontab_task_used();
			
			output_vector = erase_out_crontab();
			
		}else{
			throw std::runtime_error{str_error.at(error::synthax)};
		}
		
		write_cronLine();
		user_output_log << "Crontab Task is " << (is_crontab_command_still_needed ? "in use\n" : "NOT in use\n")
			<< alias << " from "
			<< (is_cut_out_automations ? "automation_config.json " : "")
			<< (is_crontab_command_still_needed ? "" : "and Crontab ")
			<< "automated measuring deleted\n";
				
		finalize();
			
	};
	
private:
	std::string alias;
	std::vector<std::string> str_command;
	bool is_cut_out_automations{false};
	bool is_crontab_command_still_needed{false};
	std::vector<std::string> obsolete_cmds;
	std::vector<std::string> output_vector;
	
	std::bitset<4> to_do_flags;
	
	void erase_detail_out_automation_config(){
		
		std::vector<Automation_Config> vec = automation_config;
		
		//Get User Input
		std::string _input;
		std::cout << "Which one you like to delete?\n";
		for(size_t i{0}; i < vec.size(); ++i){
			if(vec[i].alias == alias){
				std::cout << i << " = " << vec[i].crontab_command << '\n';
			}
		}
		std::cin >> _input;
			
		int index;
		try{
			index = stoi(_input);
			if (index < vec.size()) {
				str_command.push_back(vec[index].crontab_command);
				vec.erase(vec.begin() + index);
				
			}
		}catch(const std::runtime_error& re){
			throw std::runtime_error{"insert number\n"};
		}
			
		interaction_log << "Automation_Config size: " << vec.size() << "\n";
			
		if(vec.size() < automation_config.size()){
			is_cut_out_automations = true;
			automation_config = vec;
		}else{
			throw std::runtime_error{"Nothing to delete\n"};
		}
	}
		
	void erase_complete_alias_out_automation_config(){
		
		//erase Alias Config out of automation_config.json
		std::vector<Automation_Config> fresh_automation_config;
		interaction_log << "Automation size before delete: " << automation_config.size() << "\n";		
		
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
		interaction_log << "Automation size after delete: " << fresh_automation_config.size() << "\n";
		
		if(fresh_automation_config.size() < automation_config.size()){
			is_cut_out_automations = true;
			automation_config = fresh_automation_config;
		}else{
			throw std::runtime_error{"Nothing to delete\n"};
		}
	}
	
	void check_is_crontab_task_used(){
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
		
		interaction_log << "Obsolete commands: " << obsolete_cmds.size() << "\n";
		interaction_log << "Still used commands: " << still_used.size() << "\n";
		interaction_log << "Crontab Task is " << (is_crontab_command_still_needed ? "in use\n" : "NOT in use\n");
	}
	
	std::vector<std::string> erase_out_crontab(){
			
		//erase out of crontab config
		interaction_log << "Crontab size before delete: " << current_Crontab.size() << "\n";
		
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
		interaction_log << "Crontab size after: " << vec.size() << "\n";
			
		return vec;
	}
	
	void write_cronLine(){
		std::ostringstream cron;
		for(const auto& line : output_vector){
			if(line.empty() || line[0] == '\n'){
				continue;
			}
			cron << line;
		}
		cronLine = cron.str();
	}
};

class write_into_Crontab : public Crontab{
public:
	write_into_Crontab(
		std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH
		, std::function<std::vector<std::string>(const std::string&, const std::regex&)> _split_line
		
	) : Crontab(
		  str_argv
		, regex_pattern
		, str_error
		, jsonH
		, _split_line)
		
	{
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
		
		connection_type = "I2C";
		entity = str_argv[0];
		alias = str_argv[1];
		device_name = str_argv[2];
	};
		
	std::string get_log() const override {
		return interaction_log.str();
	}
	std::string get_user_output_log() const override{
		return user_output_log.str();
	}
		
	void interact() override {
		interaction_log << "Crontab Interaktion\n";		
		set_user_automation_crontab();
		
		finalize();
	};
		
private:
	std::string connection_type;
	std::string entity;
	std::string alias;
	std::string device_name;
	
	//container
	std::regex integer_pattern;
	std::map<weekday, std::string> str_weekday;
	std::map<months, std::string> str_months;
	
	void set_user_automation_crontab(){
		
		//Verarbeite User Arguments
		std::pair<std::string, bool> crontab_line;
		crontab_line = get_user_crontag_line();
		
		//write Command into Crontab
		if( write_Crontab(jsonH, crontab_line.first, alias, crontab_line.second) ){
			
			interaction_log << "Crontab written\n";
			user_output_log << "Crontab written\n";
		}else{
			
			interaction_log << "Crontab existiert bereits\n";
			user_output_log << "Crontab existiert bereits. Kein neuer Eintrag in Crontab erforderlich\n";
			
		}
		automation_config.push_back(
			Automation_Config{
				  connection_type
				, entity, alias 
				, crontab_line.first
				,(crontab_line.second ? "true" : "false")
				, device_name
			}
		);
		
		std::stringstream output;
		output 
			<< device_name << " measures for " << alias << " from "<< entity << " over " << connection_type
			<< "\n"
			<< "Time to execute: " 
			<< convert_crontabLine_to_speeking_str(crontab_line.first)
			<< (crontab_line.second ? " with Logfile\n" : "\n");
				
		//Print
		interaction_log << output.str();
		user_output_log << output.str();
	}
	
	std::pair<std::string, bool> get_user_crontag_line(){

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
					throw std::runtime_error{"Number for Minutes required"};			
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
					throw std::runtime_error{"Number for time value required"};
				}
				
				if(hours.size() > 2 || minutes.size() > 2){
					throw std::runtime_error{"Number to big for HH::MM format"};
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
			throw std::runtime_error{str_error.at(error::synthax)};
		}
		
		std::string crontab_line;
		for(const auto& c : crontab){
			crontab_line.append(c + ' ');
		}
		crontab_line.pop_back();
		
		return {crontab_line, with_logfile};
	}
	
	std::string check_that_between_A_B(const std::string& str, int A, int B, const std::string& error_prompt){
		int num;
		
		try{
			num = stoi(str);
			
		}catch(const std::runtime_error& re){
			throw std::runtime_error{error_prompt + " isnt a number"};
		}
		
		if(num < A || num > B){
			throw std::runtime_error{error_prompt + " allowed between " + std::to_string(A) + " - " + std::to_string(B)};
		}
		return std::to_string(num);
	}

	bool write_Crontab(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& str_command, const std::string& alias, bool logfile){
			
		std::string exe_filepath = jsonH->getExecutableDir() + "/";
		interaction_log << "Crontab Exe Filepath " << exe_filepath << std::endl;
			
		std::string __cronLine = str_command + " " + exe_filepath + "std -auto " + "\"" + str_command + "\"" + " -me";
		logLine = " >> " + exe_filepath + alias + "_std.log 2>&1";
		interaction_log << __cronLine << "\n"
			<< logLine << "\n";
				
		//alte Crontab sichern
		system("crontab -l > /tmp/mycron");

		if(crontab_contains(current_Crontab, {__cronLine, str_command}) ){
			return false;
		}
		
		cronLine = __cronLine;
		if(logfile){
			cronLine.append(logLine);
		}
		return true;
	}
	
	
	std::string convert_crontabLine_to_speeking_str(const std::string& crontab_line){
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
					
				}catch(const std::runtime_error& re){
					std::cerr << re.what();
				}
			}
		}
		return result;
	}
};
#endif // __linux__

#endif // HANDLE_CRONTAB_H
