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
#include "json.hpp"

// SIMPLE TIME DOCUMENTATION == std

using json = nlohmann::json;

class Clock{
public:
	std::tm get_time(){
		
		std::time_t now = std::time(nullptr);
		std::tm localTime = *std::localtime(&now);

		std::cout << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << std::endl;
		return localTime;
	}
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
	const std::string entity_filepath{"../files/"};
	const std::string accounts_filepath{"../files/accounts.json"};
public:
	JSON_Handler(std::vector<Time_Account>& all_accounts){
		read_json_accounts(all_accounts);
		read_json_entity(all_accounts);
	};

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
					std::cerr << "Fehler beim oeffnen der Datei." << std::endl;
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
			std::cerr << "Fehler beim oeffnen der Datei." << std::endl;
		}
	}

	void read_json_entity(std::vector<Time_Account>& all_accounts){
		for (auto& account : all_accounts) {
			std::string filename = entity_filepath + account.get_entity() + ".json";
			std::ifstream entry_file(filename);
			if (!entry_file.is_open()) {
				std::cerr << "Eintragsdatei konnte nicht geoeffnet werden: " << filename << std::endl;
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
			std::cerr << "Datei konnte nicht geöffnet werden.\n";
			return;
		}

		if (eingabe.peek() == std::ifstream::traits_type::eof()) {
			std::cerr << "Datei ist leer.\n";
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
			std::cerr << "JSON-Fehler: " << e.what() << std::endl;
		}
	}
};

enum class errors{
	ok = 0,
	double_pair = 1,
	not_found = 2,
	synthax = 3,
	untitled_error = 9
};
enum class command{
	  help = 0
	, add
	, delete_
	, show
	, time_unit 
};

enum class Language{
	english = 0,
	german 
};


class Arg_Manager{
public:
	Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::vector<std::string>& argv, const int& argc, Language language)
		 : jsonH(jH), str_argv(argv), argc(argc), language(language){};

	int proceed_inputs(std::vector<Time_Account>& all_accounts){
	
		int method_responce{-1};
			
		if(argc == 1){
			std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
			method_responce = static_cast<int>(errors::ok);
		}else
		
		if(argc == 2){
		
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
			
		}else
	
		if(argc == 3){
			//Zeige spezifischen Account an
			//show ALIAS
			if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::show)))){
				method_responce = show_specific_entity(all_accounts);
			}else
					
			//Account löschen
			//del
			if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::delete_)))){
				method_responce = delete_account(all_accounts, str_argv[2]);
			}
			
		}else
		
		if(argc == 4){
			//Neuen Account hinzufügen
			//add	
			if(std::regex_match(str_argv[1], regex_pattern.at(static_cast<int>(command::add)))){
				method_responce = add_account(all_accounts);
			}else
	
			//Für Alias Stunden h oder Minuten m hinzufügen	OHNE Kommentar	
			//-h -m
			if(std::regex_match(str_argv[3], regex_pattern.at(static_cast<int>(command::time_unit)))){
				method_responce = add_hours(all_accounts);
	
			}else{
				method_responce = static_cast<int>(errors::synthax);
			}
		}else
	
		if(argc == 5){
			//Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
			//-h -m
			if(std::regex_match(str_argv[3], regex_pattern.at(static_cast<int>(command::time_unit)))){
				method_responce = add_hours(all_accounts);
			}else{
				method_responce = static_cast<int>(errors::synthax);
			}
		}else{
			method_responce = static_cast<int>(errors::untitled_error);
		}

		return method_responce;
	}
	
private:
	std::shared_ptr<JSON_Handler> jsonH;
	std::vector<std::string> str_argv;
	int argc;
	Clock clock{};

	Language language;
	const std::string total_hours = "Total Hours";
	
	//show Tabellen setw(max_length[]) 
	std::vector<int> max_length{
		  10 //Index Standard	
		, 10 //Alias Standard
		, 15 //Entity Standard
		, static_cast<int>(total_hours.size()) //TotalHours Standard
	};

	const std::map<int, std::regex> regex_pattern{
		{ static_cast<int>(command::help),      std::regex{R"(^(--?h(elp)?|help)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::add),       std::regex{R"(^(--?a(dd)?|add)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::show),      std::regex{R"(^(--?sh(ow)?|sh|show)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::delete_),   std::regex{R"(^(--?d(elete)?|del(ete)?)$)", std::regex_constants::icase } },
		{ static_cast<int>(command::time_unit), std::regex{R"(^(--?h(ours)?|--?m(inutes)?|h|m)$)", std::regex_constants::icase } }
	};

	const std::map<std::string, std::string> english_pack{
		{"timepoint", "%Y-%m-%d %H:%M:%S"}
	};
	
	const std::map<std::string, std::string> german_pack{
		{"timepoint", "%d-%m-%Y %H:%M:%S"}
	};
	
	const std::string help = {
		"add 			Add new Entity give it a Alias\n"
		"-h -m  		Time to save in Hours or Minutes\n"
		"show 			Show all Entitys and Alias's\n"
		"sh 			Short Form of show\n"
		"show 'ALIAS' 	show specific Entity's Time Account\n\n"
		"For more Information read the README.md at github.com/eichi150/std\n"
	 };
		 
	int delete_account(std::vector<Time_Account>& all_accounts, const std::string& entity_to_delete){
		if(all_accounts.empty()){
			return static_cast<int>(errors::untitled_error);
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
			std::cout << entity_to_delete << " wurde gelöscht." << std::endl;
		}

		jsonH->save_json_accounts(all_accounts);
		
		return static_cast<int>(errors::ok);
	}
	
	int add_account(std::vector<Time_Account>& all_accounts){
		std::string entity = str_argv[2];
		std::string alias = str_argv[3];
				
		for(const auto& account : all_accounts){
			if(account.get_entity() == entity){
				return static_cast<int>(errors::double_pair);
			}
		}
		Time_Account new_account{entity, alias};
		all_accounts.push_back(new_account);
				
		jsonH->save_json_accounts(all_accounts);
		jsonH->save_json_entity(all_accounts, entity);
				
		std::cout << "-> " << alias << " | " << entity << " angelegt." << std::endl;
		
		return static_cast<int>(errors::ok);
	}
	
	int add_hours(std::vector<Time_Account>& all_accounts){
		for(auto& account : all_accounts){
			if(str_argv[1] == account.get_alias() ){
				float time_float = stof(str_argv[2]);
				if (str_argv[3] == "-m"){
					time_float /= 60.f;
				}
				
				std::string description;
				if(argc > 4){
					description = str_argv[4];
				}
				Entry entry{time_float, description, clock.get_time()};
				account.add_entry(entry);
				
				jsonH->save_json_accounts(all_accounts);
				jsonH->save_json_entity(all_accounts, account.get_entity());
				
				std::cout << "Zeit eingetragen" << std::endl;
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
	
	int show_specific_entity(const std::vector<Time_Account>& all_accounts) {
		
		set_table_width(all_accounts, max_length);
		
		int index{0};
		
		for(const auto& account : all_accounts){

			if(account.get_alias() == str_argv[2] || account.get_entity() == str_argv[2]){
				std::cout << std::left 
					<< std::setw(max_length[0]) << "index"
					<< std::setw(max_length[1]) << "Alias"
					<< std::setw(max_length[2]) << "Entity"
					<< std::setw(max_length[3]) << total_hours
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
					
					std::string time_format = english_pack.at("timepoint");
					if(static_cast<int>(language) == static_cast<int>(Language::english)){
						time_format = english_pack.at("timepoint");
					}else if(static_cast<int>(language) == static_cast<int>(Language::german)){
						time_format = german_pack.at("timepoint");
					}
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

				break;
			}
		}
		return static_cast<int>(errors::ok);
	}

	
	int show_all(const std::vector<Time_Account>& all_accounts) {

		set_table_width(all_accounts, max_length);
		
		std::cout << std::left 
			<< std::setw( max_length[0]) << "index"
			<< std::setw( max_length[1]) << "Alias"
			<< std::setw( max_length[2]) << "Entity"
			<< std::setw( max_length[3]) << total_hours
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



int main(int argc, char* argv[]){

	Language language = Language::german;
	
	//Argumente entgegen nehmen und in vector<string> speichern
	std::vector<std::string> str_argv;
	for(int i{0}; i < argc; ++i){
		std::string arg = argv[i];
		str_argv.push_back(arg);
	}
	argv = {};
	
	std::vector<Time_Account> all_accounts{};
	JSON_Handler jsonH{all_accounts};

	Arg_Manager arg_man{std::make_shared<JSON_Handler>(jsonH), str_argv, argc, language};

	int method_responce = arg_man.proceed_inputs(all_accounts);

	//Error Output
	switch (method_responce){
		case static_cast<int>(errors::double_pair):
			std::cout << "Error1: Entity / Alias Paar bereits vergeben" << std::endl;
			break;
		case static_cast<int>(errors::not_found):
			std::cout << "Error2: Not found.\n";
			break;
		case static_cast<int>(errors::synthax):
			std::cout << "Error3: Synthax wrong\n";
			break;
		case static_cast<int>(errors::untitled_error):
			std::cout << "Error9: Untitled Error\n";
			break;
		case static_cast<int>(errors::ok):
			//std::cout << "ok\n";
			break;
		case -1:
			std::cout << "unknown command\n";
			break;
		default:
			std::cout << "unknown error\n";
			break;
	}
	return 0;
}
