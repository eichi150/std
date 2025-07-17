#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>

#include <fstream>
#include "json.hpp"
// SIMPLE TIME DOCUMENTATION == std


//Entity hinzufügen mit Alias
//./std add BuendnisDepression BD

//BuendisDepression = BD 2Stunden zum aktuellen Zeitpunkt eintragen
//./std BD 2 -n 

//Zeige alle Entitys und Alias
//./std show

//Zeige Stundenkonto an
//./std show BD 

using json = nlohmann::json;


class Clock{
public:
	std::tm* get_time(){
		
		std::time_t now = std::time(nullptr);

		std::tm* localTime = std::localtime(&now);

		
		std::cout << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << std::endl;

		return localTime;
	}
};


struct Entry{
	float hours;
	std::string description;
	std::tm* time_point;
};


class Time_Account{
public:
	Time_Account(const std::string& ent, const std::string& al) : entity(ent), alias(al){};

	std::string get_entity()const {return entity;}
	std::string get_alias()const {return alias;}
	std::vector<Entry> get_entry()const {return entry;}
	
	float get_hours()const{return hours;}
	void add_entry(const Entry& new_entry){
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
	std::string accounts_filepath{"./accounts.json"};
public:
	JSON_Handler(std::vector<Time_Account>& all_accounts){
		read_json_accounts(all_accounts);
	};
	
	void save_json_accounts(const std::vector<Time_Account>& all_accounts){
		json daten = json::array();  // Hauptliste aller Accounts

		for(const auto& account : all_accounts){
			json account_json;
			account_json["entity"] = account.get_entity();
			account_json["alias"] = account.get_alias();

			// Liste der Einträge
			json eintraege = json::array();
			for(const auto& entry : account.get_entry()){
				json eintrag;
				eintrag["hours"] = entry.hours;
				eintrag["description"] = entry.description;
				// eintrag["timepoint"] = entry.time_point;  // ggf. konvertieren
				eintraege.push_back(eintrag);
			}

			account_json["entries"] = eintraege;
			daten.push_back(account_json);
		}

		std::ofstream file(accounts_filepath);
		if (file.is_open()) {
			file << daten.dump(4);
			file.close();
			std::cout << "Accounts gespeichert!" << std::endl;
		} else {
			std::cerr << "Fehler beim öffnen der Datei." << std::endl;
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

				for (const auto& entry_json : account_json["entries"]) {
					float hours = entry_json.value("hours", 0.0f);
					std::string description = entry_json.value("description", "");
					Entry entry{hours, description, nullptr};  // Zeit ignoriert
					account.add_entry(entry);
				}

				all_accounts.push_back(account);
			}
		} catch (const json::parse_error& e) {
			std::cerr << "JSON-Fehler: " << e.what() << std::endl;
		}
	}
};


enum class errors{
	double_entity = 1,
	double_alias = 2,
	untitled_error = 9
};


int main(int argc, char* argv[]){

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
	
	//Neuen Account hinzufügen	
	if(str_argv[1] == "add" && argc == 4){
		std::string entity = str_argv[2];
		std::string alias = str_argv[3];
		
		for(const auto& account : all_accounts){
			if(account.get_entity() == entity){
				std::cout << "Entity bereits vergeben" << std::endl;
				return static_cast<int>(errors::double_entity);
			}
			if(account.get_alias() == alias){
				std::cout << "Alias bereits vergeben" << std::endl;
				return static_cast<int>(errors::double_alias);
			}
		}
		Time_Account new_account{entity, alias};
		all_accounts.push_back(new_account);
		
		jsonH.save_json_accounts(all_accounts);
		
		return 0;
	}
	
	//Zeige alle Entity und Alias an
	if(argc == 2){
		if(str_argv[1] == "show"){
			int index{0};
			for(const auto& account : all_accounts){
				std::cout << index << ") " << account.get_alias() << " | " << account.get_entity() << std::endl;
				++index;
			}
			return 0;
		}
	}
	//Zeige spezifischen Account an
	if(argc > 2 && str_argv[1] == "show"){
		for(const auto& account : all_accounts){
			if(account.get_alias() == str_argv[2] || account.get_entity() == str_argv[2]){
				std::cout << "Bei " << account.get_entity() << " bisher " << account.get_hours() << " Stunden geleistet." << std::endl;
				return 0;
			}
		}
	}
	/*for(const auto& account : all_accounts){
		//Alias Stunden oder Minuten hinzufügen
		if(str_argv[1] == account.get_alias() && argc == 4){
			std::cout << "+ " << str_argv[2];
			std::string einheit;
			
			if (str_argv[3] == "h"){
				einheit = "Std";
			}else if(str_argv[3] == "m"){
				einheit = "Min";
			}
			std::cout << " " << einheit << " für "  <<  " eintragen - " << clock.get_time() << std::endl;
		}
	}*/
	
	
	return 0;
}
