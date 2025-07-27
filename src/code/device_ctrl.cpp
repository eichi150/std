#ifdef __linux__
#include "device_ctrl.h"

Device_Ctrl::Device_Ctrl(const std::string& error_prompt) : error_prompt(error_prompt){};


void Device_Ctrl::process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& alias){

	//auto Accounts lesen
	all_automations = jsonH->read_automation_config_file();
	
	//auto Accounts erstellen
	for(const auto& data : all_automations){
		Time_Account new_account{data.entity, data.alias};
		all_accounts.push_back(new_account);
	}
	
	//auto Accounts <entity>.json lesen
	jsonH->read_json_entity(all_accounts);

	//Sensor Werte abfragen
	std::vector<float> output_sensor = check_device();
	if(output_sensor.empty()){
		throw std::runtime_error{"§§ No Sensor Output"};
	}
	std::stringstream ss;
	ss << "Temperature: " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
		<< "Pressure: " << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
		<< "Humidity: " << std::fixed << std::setprecision(2) << output_sensor[2] << " %";
		
	std::tm localTime = clock.get_time();
	
	//auto Accounts speichern
	for(auto& account : all_accounts){
		if(account.get_alias() == alias){
			Entry entry{0.f, ss.str(), localTime};
			account.add_entry(entry);
			jsonH->save_json_entity(all_accounts, account.get_entity(), {});
			break;
		}
	}

	std::cout 
		<< std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << '\n'
		<< ss.str() << '\n' 
		<< std::endl;
}

std::vector<float> Device_Ctrl::check_device(){
	
	std::vector<float> output_sensor;
	
	if(sensor.scan_sensor(output_sensor) == 1){
		throw std::runtime_error{error_prompt};
	}

	return output_sensor;
}


void Device_Ctrl::write_Crontab(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command, const std::string& alias, bool logfile){
	
	std::string exe_filepath = jsonH->getExecutableDir() + "/";
	std::cout << "Crontab Exe Filepath " << exe_filepath << std::endl;
	
	std::string cronLine = command + " " + exe_filepath + "std -auto " + alias + " -mes";
	std::string logLine = " >> " + exe_filepath + "std.log 2>&1";

	//alte Crontab sichern
	system("crontab -l > /tmp/mycron");


	std::vector<std::string> current_Crontab = get_current_Crontab();
	
	if(crontab_contains(current_Crontab, cronLine, alias)){
		std::cout << cronLine << '\n' 
			 << "Crontab existiert bereits. Kein neuer Eintrag erforderlich." << std::endl; 
		return;
	}
	
	//neue Zeile anhängen
	std::ofstream out("/tmp/mycron", std::ios::app);
	out << cronLine;
	if(logfile){
		out << logLine;
	}
	out << '\n';
	out.close();

	//Neue Crontab setzen
	system("crontab /tmp/mycron");

	//aufräumen
	system("rm /tmp/mycron");

	std::cout << "Crontab written" << std::endl;
}


std::string Device_Ctrl::get_user_crontag_line(const std::vector<std::string>& str_argv){

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
		if(str_argv[i] == "-log"){
			with_logfile = true;
			continue;
		}

		//alle X minuten
		if(str_argv[i] == "-m"){
			
			if(!std::regex_match(parameter, integer_pattern)){
				throw std::runtime_error{"Number for Minutes required"};			
			}
			
			crontab[0].append("/" + check_that_between_A_B(parameter, 0, 59, "Minutes"));
			found_command = true;
			continue;
		}

		//alle X stunden
		if(str_argv[i] == "-h"){
		
		 	found_command = true;
		 	crontab[0] = "0";
		 	
			if(!std::regex_match(parameter, integer_pattern)){
				continue;
			}
			
			crontab[1].append("/" + check_that_between_A_B(parameter, 0, 23, "Hours"));
			continue;
		}

		//zur X uhrzeit
		if(str_argv[i] == "-clock"){
			
			std::vector<std::string> splitted_line = split_line(parameter, std::regex{R"([:])"});

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
		if(str_argv[i] == "-day"){
		
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
		throw std::runtime_error{"Syntax Error"};
	}
	
	std::string crontab_line;
	for(const auto& c : crontab){
		crontab_line.append(c + ' ');
	}
	crontab_line.pop_back();
	
	//std::cout << "Result: " << crontab_line << " " << (with_logfile ? "withLog" : "noLog") << std::endl;
	
	return crontab_line;
}

std::string Device_Ctrl::convert_crontabLine_to_speeking_str(const std::string& crontab_line){
	std::string result;
	// 0 */1 * * *
	// Every @1 hour @0 minutes 

	std::regex integer_pattern{R"(^\d+$)"};
	
	std::vector<std::string> splitted_crontab = split_line(crontab_line, std::regex{R"([\s]+)"});

	for(size_t i{0}; i < splitted_crontab.size(); ++i){

		if(splitted_crontab[i] == "*"){
			continue;
		}

		if(std::regex_match(splitted_crontab[i], integer_pattern)){
			result.append("@ ");
		}else{
			
			for(const auto& c : splitted_crontab[i]){
			
				if(c == '/'){

					splitted_crontab[i] = splitted_crontab[i].back();
					
					result.append("Every ");
					break;
				}
			}
		}
		
		result.append(splitted_crontab[i]);
		
		if(i == 0){
			result.append(" minutes ");			
		}
		if(i == 1){
			result.append(" hour ");			
		}
		if(i == 2){
			result.append(" day of month ");			
		}
		if(i == 3){
			result.append(" month ");			
		}
		if(i == 4){
		
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

std::string Device_Ctrl::check_that_between_A_B(const std::string& str, int A, int B, const std::string& error_prompt){
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

	
std::vector<std::string> Device_Ctrl::get_current_Crontab(){

	std::vector<std::string> lines;
	std::string cmd = "crontab -l 2>/dev/null";
	std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);

	if(!pipe){
		throw std::runtime_error{"popen() failed"};
	}

	char buffer[128];
	while(fgets(buffer, sizeof(buffer), pipe.get()) != nullptr){
		lines.push_back(std::string(buffer));
	}
	
	return lines;
}

//split string an leerzeichen
std::vector<std::string> Device_Ctrl::split_line(const std::string& line,const std::regex& re){
	std::vector<std::string> result;
	//std::regex re{R"([\s]+)"};
	std::sregex_token_iterator it(line.begin(), line.end(), re, -1);
	std::sregex_token_iterator end;

	for(; it != end; ++it){
		if(!it->str().empty()){
			result.push_back(it->str());
		}
	}

	return result;
}

bool Device_Ctrl::crontab_contains(const std::vector<std::string>& crontabLines, const std::string& targetLine, const std::string& targetChar){
	for(const auto& line : crontabLines){
		if(line.find(targetLine) != std::string::npos){
			// TargetLine = CrontabLine => True
			if(targetChar.empty()){
				return true;
			}
			//TargetChar in line vorhanden??
			std::vector<std::string> splitted_line = split_line(line, std::regex{R"([\s]+)"});

			for(const auto& splitted : splitted_line){
				if(splitted == targetChar){
					std::cout << splitted << " bereits vorhanden" << std::endl;
					return true;
				}
			}
			
		}
	}
	return false;
}

#endif // __linux__
