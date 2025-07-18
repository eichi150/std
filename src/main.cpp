#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <regex>
#include <ostream>
#include <fstream>
#include <memory>
#include "json.hpp"

// SIMPLE TIME DOCUMENTATION == std


//Entity hinzufügen mit Alias
//./std add BuendnisDepression BD

//BuendisDepression = BD 2Stunden zum aktuellen Zeitpunkt eintragen
//./std BD 2 -h "Comment"

//Zeige alle Entitys und Alias
//./std show

//Zeige Stundenkonto an
//./std show BD 

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
	std::string entity_filepath{"../files/"};
	std::string accounts_filepath{"../files/accounts.json"};
public:
	JSON_Handler(std::vector<Time_Account>& all_accounts){
		read_json_accounts(all_accounts);
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
					std::cerr << "Fehler beim �ffnen der Datei." << std::endl;
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
			std::cerr << "Fehler beim �ffnen der Datei." << std::endl;
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
				if (account_json.contains("entries")) {
					for (const auto& entry_json : account_json["entries"]) {
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

				all_accounts.push_back(account);
			}
		} catch (const json::parse_error& e) {
			std::cerr << "JSON-Fehler: " << e.what() << std::endl;
		}
	}
};


enum class errors{
	double_pair = 1,
	untitled_error = 9
};

class Arg_Manager{
public:
	Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::vector<std::string>& argv ) : jsonH(jH), str_argv(argv){};
	
	int add_account(std::vector<Time_Account>& all_accounts){
		std::string entity = str_argv[2];
		std::string alias = str_argv[3];
				
		for(const auto& account : all_accounts){
			if(account.get_entity() == entity){// && account.get_alias() == alias){
				std::cout << "Entity / Alias Paar bereits vergeben" << std::endl;
				return static_cast<int>(errors::double_pair);
			}
		}
		Time_Account new_account{entity, alias};
		all_accounts.push_back(new_account);
				
		jsonH->save_json_accounts(all_accounts);
		jsonH->save_json_entity(all_accounts, entity);
				
		std::cout << alias << " | " << entity << " angelegt." << std::endl;
		
		return 0;
	}
	
private:
	std::shared_ptr<JSON_Handler> jsonH;
	std::vector<std::string> str_argv;
};

int main(int argc, char* argv[]){

	const std::string help = {
		"add 		Add new Entity give it a Alias\n"
		"-h -m  		Time to save in Hours or Minutes\n"
		"show			Show all Entitys and Alias's\n"
		"show 'ALIAS' 	show specific Entity's Time Account\n\n"
		"For more Information read the README.md at github.com/eichi150/std\n"
	 };

	
	//Argumente entgegen nehmen und in vector<string> speichern
	std::vector<std::string> str_argv;
	for(int i{0}; i < argc; ++i){
		std::string arg = argv[i];
		//std::cout << "Argument " << i << ": " << arg << std::endl;
		str_argv.push_back(arg);
	}
	
	Clock clock{};
	std::vector<Time_Account> all_accounts{};
	JSON_Handler jsonH{all_accounts};

	Arg_Manager arg_man{std::make_shared<JSON_Handler>(jsonH), str_argv};

	int method_responce{-1};

	if(argc == 2 && (str_argv[1] == "-h" || str_argv[1] == "help")){
		std::cout << help << std::endl;
		method_responce = 0;
	}
	
	//Neuen Account hinzufügen	
	if(str_argv[1] == "add" && argc == 4){
		method_responce = arg_man.add_account(all_accounts);
	}
	
	//Zeige alle Entity und Alias an
	if(argc == 2){
		if(str_argv[1] == "show"){
			int index{0};
			for(const auto& account : all_accounts){
				std::cout << index << ") " << account.get_alias() << " | " << account.get_entity() << std::endl;
				++index;
			}
			method_responce = 0;
		}
	}
	//Zeige spezifischen Account an
	if(argc > 2 && str_argv[1] == "show"){
		for(const auto& account : all_accounts){
			if(account.get_alias() == str_argv[2] || account.get_entity() == str_argv[2]){
				std::cout << "Bei " << account.get_entity() << " bisher " << account.get_hours() << " Stunden geleistet." << std::endl;
				method_responce = 0;
			}
		}
	}
	
	//Für Alias Stunden h oder Minuten m hinzufügen
	for(auto& account : all_accounts){
		if(str_argv[1] == account.get_alias() && argc >= 4){
		
			if(str_argv[3] != "-h" && str_argv[3] != "-m"){
				method_responce = static_cast<int>(errors::untitled_error);
				break;
			}
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
			
			jsonH.save_json_accounts(all_accounts);
			jsonH.save_json_entity(all_accounts, account.get_entity());
			
			std::cout << "Zeit eingetragen" << std::endl;
			method_responce = 0;
		}
	}
	
		
	return method_responce;
}
