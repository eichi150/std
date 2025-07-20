#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <ostream>
#include <fstream>
#include <memory>
#include <regex>
#include <map>
#include <limits.h>
#include <unistd.h>

#include "json.hpp"

// SIMPLE TIME DOCUMENTATION == std

using json = nlohmann::json;

class Clock{
public:
	std::tm get_time(){
		std::time_t now = std::time(nullptr);
		std::tm localTime = *std::localtime(&now);

		return localTime;
	}
};

enum class errors{
	ok = 0,
	unknown,
	not_found,
	synthax,
	untitled_error,
	double_entity,
	double_alias,
	unknown_alias
};
enum class command{
	  help = 0
	, add
	, delete_
	, show
	, hours
	, minutes 
	, config_filepath
	, user_filepath
	, language
};

enum class Language{
	english = 0,
	german 
};

struct Entry{
	float hours;
	std::string description;
	std::tm time_point;
};

class Time_Account{
public:
	Time_Account(const std::string& ent, const std::string& al) : entity(ent), alias(al){};
	
	void set_hours(const float& amount){
		hours = amount;
	}
	
	std::string get_entity()const {return entity;}
	std::string get_alias()const {return alias;}
	std::vector<Entry> get_entry()const {return entry;}
	
	float get_hours()const{return hours;}
	
	void add_json_read_entry(const Entry& read_entry){
		entry.push_back(read_entry);
	}
	
	void add_entry(const Entry& new_entry){
		hours += new_entry.hours;
		entry.push_back(new_entry);
	}
	
private:
	std::string entity{};
	std::string alias{};
	float hours{0.f};
	std::vector<Entry> entry;
};

class JSON_Handler{
private:
	std::string config_filepath{"../config.json"};
	
	const std::vector<std::string> allowed_keys = {
		"config_filepath"
		, "entity_filepath"
		, "accounts_filepath"
		, "language"
	};
	
	std::string entity_filepath{"../files/"};
	std::string accounts_filepath{"../files/accounts.json"};

	Language config_language = Language::english;
public:

	JSON_Handler(std::vector<Time_Account>& all_accounts){
	
		read_config_file();
		
		read_json_accounts(all_accounts);
		read_json_entity(all_accounts);
	};

	std::string get_config_filepath() const { 
		return config_filepath;
	}
	std::string get_entity_filepath() const {
		return entity_filepath;
	}
	std::string get_accounts_filepath() const {
		return accounts_filepath;
	}

	Language get_config_language() const {
		return config_language;
	}
	
	//Search Filepath in home Directory
	std::string getExecutableDir(){
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		if(count == -1){
			return "";
		}
		std::string path(result, count);
		return path.substr(0, path.find_last_of('/'));
	}
	
	std::string getConfigFilePath() {
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		if (count == -1 || count == PATH_MAX) {
			return "";
		}

		std::string exePath(result, count);                    // /home/eichi/bin/std/bin/std
		std::string exeDir = exePath.substr(0, exePath.find_last_of('/'));  // ? /home/eichi/bin/std/bin
		std::string baseDir = exeDir.substr(0, exeDir.find_last_of('/'));   // ? /home/eichi/bin/std

		return baseDir + "/config.json";
	}


	void save_config_file(std::map<std::string, std::string>& save_to_config){
		json config;
		bool has_data = false;

		for(const auto& key : allowed_keys){
			auto it = save_to_config.find(key);
			if(it != save_to_config.end()){
				config[key] = it->second;
				has_data = true;
			}
		}
		if(has_data){
			std::ofstream config_file(config_filepath);
			if(config_file.is_open()){
				config_file << config.dump(4);
				config_file.close();
				std::cout << "**Config_File saved" << std::endl;
			}else{
				std::cout << "**Cant open Config_File!" << std::endl;
			}
		}else{
			std::cout << "No new valid config entries to save" << std::endl;
		}
	}

	void read_config_file(){
		std::ifstream try_config_file(config_filepath);
		if(!try_config_file.is_open()){
			std::cerr << "**Cant open " << config_filepath  << std::endl;

			config_filepath = getConfigFilePath(); 
			
			std::ifstream try_config_file_2(config_filepath);
			if(!try_config_file_2.is_open()){
				std::cerr << "**Cant open " << config_filepath << std::endl;
				return;
			}
		}

		std::ifstream config_file(config_filepath);	
		json config_data;
		config_file >> config_data;

		config_filepath = config_data.value("config_filepath", "");
		entity_filepath = config_data.value("entity_filepath", "");
		accounts_filepath = config_data.value("accounts_filepath", "");
		std::string str_config_language = config_data.value("language", "");

		if(!str_config_language.empty()){
			if(str_config_language == "english"){
				config_language = Language::english;
			}else
			if(str_config_language == "german"){
				config_language = Language::german;
			}
		}else{
			config_language = Language::english;
		}
	}
		
	void save_json_entity(const std::vector<Time_Account>& all_accounts, const std::string& entity_to_save){
		for(const auto& account : all_accounts){
			if(entity_to_save == account.get_entity()){
				json eintraege;
				eintraege["entity"] = account.get_entity();
				eintraege["alias"] = account.get_alias();
				eintraege["total_hours"] = account.get_hours();
				eintraege["entries"] = json::array();

				for(const auto& entry : account.get_entry()){
					json eintrag;
					eintrag["hours"] = entry.hours;
					eintrag["description"] = entry.description;

					std::stringstream ss;
					ss << std::put_time(&entry.time_point, "%Y-%m-%d %H:%M:%S");
					eintrag["timepoint"] = ss.str();

					eintraege["entries"].push_back(eintrag);
				}

				std::string entity_filepath_total = entity_filepath + account.get_entity() + ".json";
				std::ofstream file_entry(entity_filepath_total);
				if (file_entry.is_open()) {
					file_entry << eintraege.dump(4);
					file_entry.close();
					std::cout << "**Debug: Entity-Datei gespeichert." << std::endl;
				} else {
					std::cerr << "**Fehler beim oeffnen der Datei." << std::endl;
				}
			}
		}
	}

	
	void save_json_accounts(const std::vector<Time_Account>& all_accounts){
		json daten = json::array();

		for(const auto& account : all_accounts){
			json account_json;
			account_json["entity"] = account.get_entity();
			account_json["alias"] = account.get_alias();
			account_json["total_hours"] = account.get_hours();
			
			daten.push_back(account_json);
		}

		std::ofstream file(accounts_filepath);
		if (file.is_open()) {
			file << daten.dump(4);
			file.close();
			std::cout << "**Debug: Accounts-Datei gespeichert." << std::endl;
		} else {
			std::cerr << "**Fehler beim oeffnen der Datei." << std::endl;
		}
	}

	void read_json_entity(std::vector<Time_Account>& all_accounts){
		for (auto& account : all_accounts) {
			std::string filename = entity_filepath + account.get_entity() + ".json";
			std::ifstream entry_file(filename);
			if (!entry_file.is_open()) {
				std::cerr << "**Eintragsdatei konnte nicht geoeffnet werden: " << filename << std::endl;
				continue;
			}

			json entry_data;
			entry_file >> entry_data;

			if (entry_data.contains("entries")) {
				for (const auto& entry_json : entry_data["entries"]) {
					float hours = entry_json.value("hours", 0.0f);
					std::string description = entry_json.value("description", "");
					
					std::tm time_point{};
					std::istringstream ss(entry_json.value("timepoint", ""));
					ss >> std::get_time(&time_point, "%Y-%m-%d %H:%M:%S");
					
					if(ss.fail()){
						std::cerr << "Zeit konnte nicht gelesen werden.\n";
						time_point = std::tm{};
					}
					
					Entry entry{hours, description, time_point};
					account.add_json_read_entry(entry);
				}
			}
		}
	}


	void read_json_accounts(std::vector<Time_Account>& all_accounts) {
		std::ifstream eingabe(accounts_filepath);
		if (!eingabe.is_open()) {
			std::cerr << "**Datei konnte nicht geöffnet werden. " << accounts_filepath << std::endl;
			return;
		}

		if (eingabe.peek() == std::ifstream::traits_type::eof()) {
			std::cerr << "**Datei ist leer.\n";
			return;
		}

		try {
			json gelesene_daten;
			eingabe >> gelesene_daten;

			for (const auto& account_json : gelesene_daten) {
				std::string entity = account_json.value("entity", "");
				std::string alias = account_json.value("alias", "");
				
				Time_Account account(entity, alias);
				float total_hours = account_json.value("total_hours", 0.0f);
				account.set_hours(total_hours);
				
				all_accounts.push_back(account);
			}
			
		} catch (const json::parse_error& e) {
			std::cerr << "**JSON-Fehler: " << e.what() << std::endl;
		}
	}
};

class Translator{
public:

	void set_language(const Language& language){
		this->language = language;
	}
	
	std::map<std::string, std::string> which_language_pack(){
	
		std::map<std::string, std::string> english_pack{
			{"timepoint", "%Y-%m-%d %H:%M:%S"}
			,{"deleted_out_of_accounts.json", " got deleted. File is still available for Export."}
			,{"total_hours", "Total Hours"}
			,{"entity", "Entity"}
		};
		
		std::map<std::string, std::string> german_pack{
			{"timepoint", "%d-%m-%Y %H:%M:%S"}
			,{"deleted_out_of_accounts.json", " wurde gelöscht. Die Datei zu exportieren ist weiterhin möglich."}
			,{"total_hours", "Stunden gesamt"}
			,{"entity", "Entität"}
		};

		//Default = English
		auto language_pack = english_pack;
		
		if(english_pack.size() != german_pack.size()){
			return english_pack;
			
		}
		
		bool same_keys = std::equal(
			english_pack.begin(), english_pack.end(),
			german_pack.begin(),
			[](const auto& a, const auto& b){
				return a.first == b.first;
			}
		);
			
		if(!same_keys){
			return english_pack;
		}
		
		//Possible to change Language
		if(static_cast<int>(language) == static_cast<int>(Language::german)){

			language_pack = german_pack;
		}
	
		return language_pack;
	}

private:
	Language language;
};

class Arg_Manager{
public:
	Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::vector<std::string>& argv, const int& argc, const std::map<int, std::regex>& pattern)
		 : jsonH(jH), str_argv(argv), argc(argc)
	 {
	 	
		regex_pattern = pattern;

		init_language();
				 	
	 	max_length = {
 			  10 //Index Standard	
 			, 10 //Alias Standard
 			, 15 //Entity Standard
 			, static_cast<int>(language_pack.at("total_hours").size()) //TotalHours Standard
 		};
	};
	
	int proceed_inputs(std::vector<Time_Account>& all_accounts){
	
		int method_responce{static_cast<int>(errors::unknown)};

		switch(argc){
			case 1:
				{
					std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
					method_responce = static_cast<int>(errors::ok);
					break;
				};
				

			case 2:
				{
					//Zeige Hilfe an
					//help
					if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::help)))){
						std::cout << help << std::endl;
						method_responce = static_cast<int>(errors::ok);
					}
					
					//Zeige alle Entity und Alias an
					//show
					if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::show)))){
						method_responce = show_all(all_accounts);
					}
					break;
				};
			
				
			case 3:
				{	
					//show ++
					if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::show)))){
					
						//Zeige entity, accounts, config filepaths an
						//show filepath
						if(std::regex_match(str_argv[2], regex_pattern.at(static_cast<int>(command::config_filepath)))){
						
							method_responce = show_filepaths();
		
						//Zeige spezifischen Account an
						//show ALIAS
						}else{
						
							method_responce = show_specific_entity(all_accounts, method_responce);
						}
						
					}else
					
					//Account löschen
					//del
					if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::delete_)))){
					
						method_responce = delete_account(all_accounts, str_argv[2]);
					}else
		
					//Language changeTo
					//-l ger
					if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::language)))){
					
						method_responce = change_config_json_language(str_argv[2]);
					}
					break;
				};

				
			case 4:
				{
					//-f <entityFilepath> <accountsFilepath>
					if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::user_filepath)))){
						method_responce = user_change_filepaths(str_argv[2], str_argv[3]);
						
						std::cout << str_argv[2] << '\n' << str_argv[3] << std::endl;
						
					}else
					//Neuen Account hinzufügen
					//add	
					if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::add)))){
					
						method_responce = add_account(all_accounts);
					}else
			
					//Für Alias Stunden h oder Minuten m hinzufügen	OHNE Kommentar	
					//-h -m
					if(std::regex_match(str_argv[3], regex_pattern.at(static_cast<int>(command::hours)))
						 || std::regex_match(str_argv[3], regex_pattern.at(static_cast<int>(command::minutes))))
				 	{
						method_responce = add_hours(all_accounts);
			
					}else{
						method_responce = static_cast<int>(errors::synthax);
					}
					break;
				};
				
				
			case 5:
				{
					//-cf <configFilepath> <entityFilepath> <accountsFilepath>
					if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::config_filepath)))){
					
						method_responce = change_config_json_file(str_argv[2], str_argv[3], str_argv[4]);
						
						std::cout << str_argv[2] << '\n' << str_argv[3] << '\n' << str_argv[4] << std::endl;
						
					}else
		
								
					//Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
					//-h -m
					if(std::regex_match(str_argv[3], regex_pattern.at(static_cast<int>(command::hours)))
						 || std::regex_match(str_argv[3], regex_pattern.at(static_cast<int>(command::minutes))))
				 	{
						method_responce = add_hours(all_accounts);
						
					}else{
						method_responce = static_cast<int>(errors::synthax);
					}
					break;
				};
				
				
			default:
				{
					method_responce = static_cast<int>(errors::untitled_error);
					break;
				};
		};

		return method_responce;
	}
	
private:

	std::shared_ptr<JSON_Handler> jsonH;
	Translator translator{};
	Clock clock{};
		
	std::vector<std::string> str_argv;
	int argc;
	
	//show Tabellen setw(max_length[]) 
	std::vector<int> max_length;

	std::map<int, std::regex> regex_pattern;

	std::map<std::string, std::string> language_pack;
	
	const std::string help = {
		"add 			Add new Entity give it a Alias\n"
		"-h -m  		Time to save in Hours or Minutes\n"
		"show 			Show all Entitys and Alias's\n"
		"sh 			Short Form of show\n"
		"show 'ALIAS' 	show specific Entity's Time Account\n\n"
		"For more Information read the README.md at github.com/eichi150/std\n"
	 };

	void init_language(){
		//Set Language_Pack - Standard english
		Language language = jsonH->get_config_language();
 			
	 	translator.set_language(language);
		language_pack = translator.which_language_pack();
	}
		
	std::string get_str_config_language(){
		std::string str_config_language = "english";
		if(static_cast<int>(jsonH->get_config_language()) == static_cast<int>(Language::english)){
			str_config_language = "english";
		}else
		if(static_cast<int>(jsonH->get_config_language()) == static_cast<int>(Language::german)){
			str_config_language == "german";
		}
		
		return str_config_language;
	}

	int change_config_json_language(const std::string& to_language){
		std::map<std::string, std::string> new_data = {
			  {"config_filepath", jsonH->get_config_filepath()}
			, {"entity_filepath", jsonH->get_entity_filepath()}
			, {"accounts_filepath", jsonH->get_accounts_filepath()}
			, {"language", to_language}
		};
		jsonH->save_config_file(new_data);
		
		return static_cast<int>(errors::ok);
	}

	int change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath){
	
		std::map<std::string, std::string> new_data = {
			  {"config_filepath", conf_filepath}
			, {"entity_filepath", ent_filepath}
			, {"accounts_filepath", acc_filepath}
			, {"language", get_str_config_language()}
		};
		jsonH->save_config_file(new_data);

		return static_cast<int>(errors::ok);
	}

	int user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath){
		std::map<std::string, std::string> new_filepaths = {
			  {"entity_filepath", ent_filepath}
			, {"accounts_filepath", acc_filepath}
		};
		jsonH->save_config_file(new_filepaths);

		return static_cast<int>(errors::ok);
	}
	
		
	int delete_account(std::vector<Time_Account>& all_accounts, const std::string& entity_to_delete){
		if(all_accounts.empty()){
			return static_cast<int>(errors::untitled_error);
		}
		
		bool same_keys = std::equal(
			all_accounts.begin(), all_accounts.end(),
			[&entity_to_delete](const auto& a){
				return a.get_entity() == entity_to_delete;
			}
		);
		if(!same_keys){
			return static_cast<int>(errors::not_found);
		}
		
		std::vector<Time_Account> adjusted_accounts;
	
		size_t size_before = all_accounts.size();
		
		std::remove_copy_if(all_accounts.begin(), all_accounts.end(),
			std::back_inserter(adjusted_accounts),
			[entity_to_delete](const Time_Account& account){
				return account.get_entity() == entity_to_delete;
			}
		);
		
		all_accounts = adjusted_accounts;
		
		if( all_accounts.size() < size_before){
			std::cout << entity_to_delete << language_pack.at("deleted_out_of_accounts.json") << std::endl;
		}

		jsonH->save_json_accounts(all_accounts);
		
		return static_cast<int>(errors::ok);
	}
	
	int add_account(std::vector<Time_Account>& all_accounts){
		std::string entity = str_argv[2];
		std::string alias = str_argv[3];

		//Entity or Alias already taken?
		for(const auto& account : all_accounts){
			if(account.get_entity() == entity){
				return static_cast<int>(errors::double_entity);
			}
			if(account.get_alias() == alias){
				return static_cast<int>(errors::double_alias);
			}
		}
		
		Time_Account new_account{entity, alias};
		all_accounts.push_back(new_account);
				
		jsonH->save_json_accounts(all_accounts);
		jsonH->save_json_entity(all_accounts, entity);
				
		std::cout << "-> " << alias << " | " << entity << " saved." << std::endl;
		
		return static_cast<int>(errors::ok);
	}
	
	int add_hours(std::vector<Time_Account>& all_accounts){
		for(auto& account : all_accounts){
			if(str_argv[1] == account.get_alias() ){
				float time_float = stof(str_argv[2]);
				if (std::regex_match(str_argv[3], regex_pattern.at(static_cast<int>(command::minutes)))){
					time_float /= 60.f;
				}
				
				std::string description;
				if(argc > 4){
					description = str_argv[4];
				}
				auto localTime = clock.get_time();
				Entry entry{time_float, description, localTime};
				account.add_entry(entry);
				
				jsonH->save_json_accounts(all_accounts);
				jsonH->save_json_entity(all_accounts, account.get_entity());
				
				std::cout 
					<< std::put_time(&localTime, language_pack.at("timepoint").c_str()) << '\n'
					<< "Time saved." 
					<< std::endl;
					
				return static_cast<int>(errors::ok);
			}
		}
		return static_cast<int>(errors::not_found);
	}


	void set_table_width(const std::vector<Time_Account>& all_accounts, std::vector<int>& max_length){
		
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

	int show_filepaths(){
		std::cout 
			<< "Config: " << jsonH->get_config_filepath() << '\n'
			<< language_pack.at("entity") << ": " << jsonH->get_entity_filepath() << '\n' 
			<< "Accounts: " << jsonH->get_accounts_filepath() << std::endl;
		return static_cast<int>(errors::ok);
	}
	
	
	int show_specific_entity(const std::vector<Time_Account>& all_accounts, int method_responce) {
		
		set_table_width(all_accounts, max_length);
		
		int index{0};
		
		for(const auto& account : all_accounts){

			if(account.get_alias() == str_argv[2] || account.get_entity() == str_argv[2]){
				std::cout << std::left 
					<< std::setw(max_length[0]) << "index"
					<< std::setw(max_length[1]) << "Alias"
					<< std::setw(max_length[2]) << language_pack.at("entity")
					<< std::setw(max_length[3]) << language_pack.at("total_hours")
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

				if(account.get_entry().empty()){
					std::cout << "No Entrys available" << std::endl;
					break;
				}
				
				std::cout << std::left
					<< std::setw( max_length[0]) << "index"
					<< std::setw(10) << "Hours"
					<< std::setw(25) << "Timestamp"
					<< std::setw(15) << "Comment"
					<< std::endl;
					
				index = 0;
				for(const auto& entry : account.get_entry()){
					std::stringstream ss;
					
					std::string time_format = language_pack.at("timepoint");
					ss << std::put_time(&entry.time_point, time_format.c_str());

					//Trennlinie 
					std::cout << std::setfill('-') << std::setw( max_length[0] + 10 + 25 + 15) << "-" << std::setfill(' ') << std::endl;
					
					std::cout << std::left
						<< std::setw( max_length[0]) << index
						<< std::setw(10) << entry.hours
						<< std::setw(25) << ss.str()
						<< std::setw(15) << entry.description
						<< std::endl;
										
					++index;
				}
				method_responce = static_cast<int>(errors::ok);
				break;
			}else{
				method_responce = static_cast<int>(errors::unknown_alias);
			}
		}
		return method_responce;
	}

	
	int show_all(const std::vector<Time_Account>& all_accounts) {

		set_table_width(all_accounts, max_length);
		
		std::cout << std::left 
			<< std::setw( max_length[0]) << "index"
			<< std::setw( max_length[1]) << "Alias"
			<< std::setw( max_length[2]) << language_pack.at("entity")
			<< std::setw( max_length[3]) << language_pack.at("total_hours")
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
		return static_cast<int>(errors::ok);
	}	
};

bool check_for_valid_args(const std::vector<std::string>& str_argv, const std::map<int, std::regex>& regex_pattern ){
	//Argumente checken ob ein Command zulässig ist. Ansonsten Programm frühzeitig ende
	
	for(const auto& pattern : regex_pattern){
		for(size_t i{0}; i < str_argv.size(); ++i){
			if(std::regex_match(str_argv[i], pattern.second)){
				return  true;
			}	
		}
	}
	
	return false;
}
	
int main(int argc, char* argv[]){
	
	//Argumente entgegen nehmen und in vector<string> speichern
	std::vector<std::string> str_argv;
	for(int i{0}; i < argc; ++i){
		std::string arg = argv[i];
		str_argv.push_back(arg);
	}
	argv = {};

	std::map<int, std::regex> regex_pattern = {
		{ static_cast<int>(command::help),      		std::regex{R"(^(--?h(elp)?|help)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::add),       		std::regex{R"(^(--?a(dd)?|add)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::show),      		std::regex{R"(^(--?sh(ow)?|sh|show)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::delete_),   		std::regex{R"(^(--?d(elete)?|del(ete)?)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::hours), 			std::regex{R"(^(--?h(ours)?|h|hours)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::minutes), 			std::regex{R"(^(--?m(inutes)?|m|minutes)$)", std::regex_constants::icase} },
		{ static_cast<int>(command::config_filepath),  	std::regex{R"(^--?cf$)", std::regex_constants::icase } },
		{ static_cast<int>(command::user_filepath),  	std::regex{R"(^(--?f(ilepath)?|filepath)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::language),  		std::regex{R"(^(--?l(anguage)?|language)$)", std::regex_constants::icase } },
	};

	int method_responce{static_cast<int>(errors::unknown)};
	
	//Check for valid Arguments
	if(argc >= 2 && !check_for_valid_args(str_argv, regex_pattern)){
		method_responce = static_cast<int>(errors::unknown);
		
	}else{
		//Initalize
		std::vector<Time_Account> all_accounts{};
		JSON_Handler jsonH{all_accounts};
	
		//Init Argument Manager
		Arg_Manager arg_man{std::make_shared<JSON_Handler>(jsonH), str_argv, argc, regex_pattern};
	
		
		method_responce = arg_man.proceed_inputs(all_accounts);	
	}

	//Error Output
	switch (method_responce){
		
		case static_cast<int>(errors::double_entity):
			std::cout << "**Entity already taken\n";
			break;
		case static_cast<int>(errors::double_alias):
			std::cout << "**Alias already taken\n";
			break;
		case static_cast<int>(errors::not_found):
			std::cout << "**Not found\n";
			break;
		case static_cast<int>(errors::synthax):
			std::cout << "**Synthax wrong\n";
			break;
		case static_cast<int>(errors::untitled_error):
			std::cout << "**Untitled Error\n";
			break;
		case static_cast<int>(errors::ok):
			std::cout << "**ok\n";
			break;
		case static_cast<int>(errors::unknown):
			std::cout << "**unknown command\n";
			break;
		case static_cast<int>(errors::unknown_alias):
			std::cout << "**unknown Alias\n";
			break;
		default:
			std::cout << "**unknown error\n";
			break;
	}
	return 0;
}
