// Auto-generated single-file version of std

// -------- /home/eichi/Dev/Projekte/std/src/code/time_account.h ---------
#ifndef TIME_ACCOUNT_H
#define TIME_ACCOUNT_H

#include <string>
#include <vector>
#include <ctime>

struct Entry{
	float hours;
	std::string description;
	std::tm time_point;
};
	

class Time_Account{
public:
	Time_Account(const std::string& ent, const std::string& al) : entity(ent), alias(al){};
	
	void set_hours(const float& amount);
	
	std::string get_entity() const;
	std::string get_alias() const;
	std::vector<Entry> get_entry() const;
	
	float get_hours() const;
	
	void add_json_read_entry(const Entry& read_entry);
	
	void add_entry(const Entry& new_entry);
private:
	std::string entity{};
	std::string alias{};
	
	float hours{0.f};
	std::vector<Entry> entry;
};

#endif // TIME_ACCOUNT_H

// -------- /home/eichi/Dev/Projekte/std/src/code/time_account.cpp ---------

void Time_Account::set_hours(const float& amount){
    hours = amount;
}

std::string Time_Account::get_entity()const {
    return entity;
}
std::string Time_Account::get_alias()const {
    return alias;
}
std::vector<Entry> Time_Account::get_entry()const {
    return entry;
}

float Time_Account::get_hours()const{
    return hours;
}

void Time_Account::add_json_read_entry(const Entry& read_entry){
    entry.push_back(read_entry);
}

void Time_Account::add_entry(const Entry& new_entry){
    hours += new_entry.hours;
    entry.push_back(new_entry);
}


// -------- /home/eichi/Dev/Projekte/std/src/code/json_handler.h ---------
#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#ifdef _WIN32
	#include <windows.h>

#else
	#include <limits.h>
	#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <fstream>


class JSON_Handler{
public:

	JSON_Handler(std::vector<Time_Account>& all_accounts);

	std::string get_config_filepath() const;

	std::string get_entity_filepath() const;

	std::string get_accounts_filepath() const;

	Language get_config_language() const;
	
	//Search Filepath in home Directory
	std::string getExecutableDir();
	
	std::string getConfigFilePath();

	void save_config_file(std::map<std::string, std::string>& save_to_config);

	void read_config_file();
	
	//Sammle alle Accounts mit dieser Entity
	std::vector<Time_Account> collect_accounts_with_entity(const std::vector<Time_Account>& search_in, const std::string& search_for_entity);
	
	void save_json_entity(const std::vector<Time_Account>& all_accounts, const std::string& entity_to_save, const std::string& alias);
	
	void save_json_accounts(const std::vector<Time_Account>& all_accounts);

	void read_json_entity(std::vector<Time_Account>& all_accounts);

	void read_json_accounts(std::vector<Time_Account>& all_accounts);

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
};

#endif // JSON_HANDLER_H

// -------- /home/eichi/Dev/Projekte/std/src/code/json_handler.cpp ---------

using json = nlohmann::json;

JSON_Handler::JSON_Handler(std::vector<Time_Account>& all_accounts){

    read_config_file();
    
    read_json_accounts(all_accounts);
    read_json_entity(all_accounts);
};

std::string JSON_Handler::get_config_filepath() const { 
    return config_filepath;
}
std::string JSON_Handler::get_entity_filepath() const {
    return entity_filepath;
}
std::string JSON_Handler::get_accounts_filepath() const {
    return accounts_filepath;
}

Language JSON_Handler::get_config_language() const {
    return config_language;
}

std::string JSON_Handler::getExecutableDir(){
    char path[PATH_MAX];
#ifdef _WIN32
    
    DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
    if(length == 0){
        return "";
    }
    std::string exePath(path, length);
    return exePath.substr(0, exePath.find_last_of("\\/"));
    
#else
    
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if(count == -1){
        return "";
    }
    std::string exePath(path, count);
    return exePath.substr(0, exePath.find_last_of('/'));
    
#endif
}


std::string JSON_Handler::getConfigFilePath() {

    std::string exeDir = getExecutableDir();
    if(exeDir.empty()){
        return "";
    }
    size_t lastSlash = exeDir.find_last_of("/\\");
    std::string baseDir = exeDir.substr(0, lastSlash);

    #ifdef _WIN32
        return baseDir + "\\config.json";
    #else
        return baseDir + "/config.json";
    #endif
}

void JSON_Handler::save_config_file(std::map<std::string, std::string>& save_to_config){
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
            std::cout << "##Config_File saved" << std::endl;
        }else{
            std::cout << "##Cant open Config_File!" << std::endl;
        }
    }else{
        std::cout << "##No new valid config entries to save" << std::endl;
    }
}


void JSON_Handler::read_config_file(){
    config_filepath = getConfigFilePath();
    std::ifstream config_file(config_filepath);
    
    if(!config_file.is_open()){
        std::cerr << "##Cant open " << config_filepath  << std::endl;
        return;
    }

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

std::vector<Time_Account> JSON_Handler::collect_accounts_with_entity(const std::vector<Time_Account>& search_in, const std::string& search_for_entity){
    std::vector<Time_Account> matching_accounts;
    std::copy_if(
        search_in.begin(), search_in.end(),
        std::back_inserter(matching_accounts),
        [&search_for_entity](const Time_Account& acc) {
            return acc.get_entity() == search_for_entity;
        }
    );
    return matching_accounts;
}

void JSON_Handler::save_json_entity(const std::vector<Time_Account>& all_accounts, const std::string& entity_to_save, const std::string& alias){
	
    std::vector<Time_Account> matching_accounts = collect_accounts_with_entity(all_accounts, entity_to_save);

    // Keine passenden Accounts?
    if (matching_accounts.empty()) {
        std::cerr << "##Keine Eintäge für Entity '" << entity_to_save << "' gefunden." << std::endl;
        return;
    }

    std::cout << "##matching accounts: "<< matching_accounts.size() << std::endl;

    // 2. Struktur aufbauen
    json eintraege;
    eintraege["entity"] = entity_to_save;

    json alias_list = json::array();

    for (const auto& acc : matching_accounts) {
        if(!alias.empty() && acc.get_alias() == alias){
            continue;
        }
        json alias_entry;
        alias_entry["alias"] = acc.get_alias();
        alias_entry["total_hours"] = acc.get_hours();
        alias_entry["entries"] = json::array();

        for (const auto& entry : acc.get_entry()) {
            json eintrag;
            eintrag["hours"] = entry.hours;
            eintrag["description"] = entry.description;

            std::stringstream ss;
            ss << std::put_time(&entry.time_point, "%Y-%m-%d %H:%M:%S");
            eintrag["timepoint"] = ss.str();

            alias_entry["entries"].push_back(eintrag);
        }

        alias_list.push_back(alias_entry);
    }

    eintraege["alias"] = alias_list;  // ACHTUNG: Kein Slash in "alias"

    // 3. Datei schreiben
    std::string entity_filepath_total = entity_filepath + entity_to_save + ".json";
    std::ofstream file_entry(entity_filepath_total);
    if (file_entry.is_open()) {
        file_entry << eintraege.dump(4);
        file_entry.close();
        std::cout << "##Debug: Entity-Datei gespeichert unter " << entity_filepath_total << std::endl;
    } else {
        std::cerr << "##Fehler beim Öffnen der Datei: " << entity_filepath_total << std::endl;
    }


}


void JSON_Handler::save_json_accounts(const std::vector<Time_Account>& all_accounts){
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
        std::cout << "##Debug: Accounts-Datei gespeichert." << std::endl;
    } else {
        std::cerr << "##Fehler beim oeffnen der Datei." << std::endl;
    }
}


void JSON_Handler::read_json_entity(std::vector<Time_Account>& all_accounts) {
    for (auto& account : all_accounts) {
        std::string filename = entity_filepath + account.get_entity() + ".json";
        std::ifstream entry_file(filename);
        if (!entry_file.is_open()) {
            std::cerr << "##Eintragsdatei konnte nicht geöffnet werden: " << filename << std::endl;
            continue;
        }

        json entry_data;
        try {
            entry_file >> entry_data;
        } catch (const json::parse_error& e) {
            std::cerr << "##JSON-Parse-Fehler: " << e.what() << std::endl;
            continue;
        }

        if (entry_data.contains("alias")) {
            for (const auto& alias : entry_data["alias"]) {
                std::string str_alias = alias.value("alias", "");
                if (account.get_alias() != str_alias)
                    continue;

                float total_hours = alias.value("total_hours", 0.0f);
                account.set_hours(total_hours);

                if (alias.contains("entries")) {
                    for (const auto& entry_json : alias["entries"]) {
                        float hours = entry_json.value("hours", 0.0f);
                        std::string description = entry_json.value("description", "");

                        std::tm time_point{};
                        std::istringstream ss(entry_json.value("timepoint", ""));
                        ss >> std::get_time(&time_point, "%Y-%m-%d %H:%M:%S");

                        if (ss.fail()) {
                            std::cerr << "##Zeit konnte nicht gelesen werden für Alias: " << str_alias << std::endl;
                            time_point = std::tm{};
                        }

                        Entry entry{hours, description, time_point};
                        account.add_json_read_entry(entry);
                    }
                }
            }
        }
    }
}

void JSON_Handler::read_json_accounts(std::vector<Time_Account>& all_accounts) {
    std::ifstream eingabe(accounts_filepath);
    if (!eingabe.is_open()) {
        std::cerr << "##Datei konnte nicht geöffnet werden. " << accounts_filepath << std::endl;
        return;
    }

    if (eingabe.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "##Datei ist leer.\n";
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
        std::cerr << "##JSON-Fehler: " << e.what() << std::endl;
    }
}

// -------- /home/eichi/Dev/Projekte/std/src/code/clock.h ---------
#ifndef CLOCK_H
#define CLOCK_H

#include <ctime>

class Clock{
public:
	std::tm get_time(){
		std::time_t now = std::time(nullptr);
		std::tm localTime = *std::localtime(&now);

		return localTime;
	}
};

#endif // CLOCK_H

// -------- /home/eichi/Dev/Projekte/std/src/code/enum_class.h ---------
#ifndef ENUM_CLASS_H
#define ENUM_CLASS_H

enum class error{
	  ok = 0
	, unknown
	, not_found
	, synthax
	, untitled_error
	, double_entity
	, double_alias
	, unknown_alias
	, unknown_alias_or_entity
	, alias_equal_entity
	, user_input_is_command
	, unknown_language
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
	  english = 0
	, german 
};

#endif //ENUM_CLASS_H

// -------- /home/eichi/Dev/Projekte/std/src/code/translator.h ---------
#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <map>
#include <string>

class Translator{
public:
	std::map<std::string, std::string> language_pack{};
	
    static std::map<Language, std::string> dict_language;
    static std::map<std::string, std::string> english_pack;
    static std::map<std::string, std::string> german_pack;
    static std::map<Language, std::map<std::string, std::string>> all_packs;

	void set_language(const Language& language);
	
	std::map<std::string, std::string> which_language_pack();

	std::string get_str_language();

private:
	Language language;
	
};

#endif // TRANSLATOR_H


// -------- /home/eichi/Dev/Projekte/std/src/code/translator.cpp ---------

std::map<std::string, std::string> Translator::english_pack = {
    {"language", "english"}
    ,{"str_language", "Language"}
    ,{"timepoint", "%Y-%m-%d %H:%M:%S"}
    ,{"deleted_out_of_accounts.json", " got deleted. File is still available for Export."}
    ,{"total_hours", "Total Hours"}
    ,{"entity", "Entity"}
    ,{"hours", "Hours"}
    ,{"timestamp", "Timestamp"}
    ,{"comment", "Comment"}
    ,{"time_saved", "Time saved"}
    ,{"saved", " saved"}
    ,{"No_Entrys_available", "No Entrys available"}
};

std::map<std::string, std::string> Translator::german_pack = {
    {"language", "deutsch"}
    ,{"str_language", "Sprache"}
    ,{"timepoint", "%d-%m-%Y %H:%M:%S"}
    ,{"deleted_out_of_accounts.json", " wurde gelöscht. Die Datei zu exportieren ist weiterhin möglich."}
    ,{"total_hours", "Stunden gesamt"}
    ,{"entity", "Entität"}
    ,{"hours", "Stunden"}
    ,{"timestamp", "Zeitstempel"}
    ,{"comment", "Kommentar"}
    ,{"time_saved", "Zeitstempel gespeichert"}
    ,{"saved", " gespeichert"}
    ,{"No_Entrys_available", "Keine Einträge verfügbar"}
};

std::map<Language, std::map<std::string, std::string>> Translator::all_packs = {
    {Language::english, english_pack}
    , {Language::german, german_pack}
};

std::map<Language, std::string> Translator::dict_language = {
    {Language::english, "english"}
    , {Language::german, "german"}
};

void Translator::set_language(const Language& language){
    this->language = language;
    language_pack = which_language_pack();
}

std::map<std::string, std::string> Translator::which_language_pack(){

    auto pack = all_packs.at(Language::english);
    if(english_pack.size() != german_pack.size()){
        pack = all_packs.at(Language::english);
        all_packs.clear();
        return pack;
    }
    
    bool same_keys = std::equal(
        english_pack.begin(), english_pack.end(),
        german_pack.begin(),
        [](const auto& a, const auto& b){
            return a.first == b.first;
        }
    );
        
    if(!same_keys){
        pack = all_packs.at(Language::english);
        all_packs.clear();
        return pack;
    }
    pack = all_packs.at(language);
    all_packs.clear();
    return pack;
}

std::string Translator::get_str_language(){
    return dict_language.at(language);
}

// -------- /home/eichi/Dev/Projekte/std/src/code/arg_manager.h ---------
#ifndef ARG_MANAGER_H
#define ARG_MANAGER_H


#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <regex>

class Arg_Manager{
public:
	Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::vector<std::string>& argv, const int& argc, const std::map<command, std::regex>& pattern);
	
	void proceed_inputs(std::vector<Time_Account>& all_accounts, const std::map<error, std::string>& str_error);
	
private:

	std::shared_ptr<JSON_Handler> jsonH;
	Translator translator{};
	Clock clock{};
		
	std::vector<std::string> str_argv;
	int argc;
	
	//show Tabellen setw(max_length[]) 
	std::vector<int> max_length;

	std::map<command, std::regex> regex_pattern;

	std::map<error, std::string> str_error;
		
	const std::string help = {
    "add 			Add new Entity give it a Alias\n"
    "-h -m  		Time to save in Hours or Minutes\n"
    "show 			Show all Entitys and Alias's\n"
    "sh 			Short Form of show\n"
    "show 'ALIAS' 	show specific Entity's Time Account\n\n"
    "For more Information have a look at README.md on github.com/eichi150/std\n"
    };

	std::vector<Time_Account> check_for_alias_or_entity(const std::vector<Time_Account>& all_accounts, const std::string& alias_or_entity);
	
	//Sammle alle Accounts mit diesem Alias
	std::vector<Time_Account> collect_accounts_with_alias(const std::vector<Time_Account>& search_in, const std::string& search_for_alias);

	void change_config_json_language(const std::string& to_language);

	void change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath);

	void user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath);
	
	void delete_account(std::vector<Time_Account>& all_accounts, const std::string& alias_to_delete);
	
	void add_account(std::vector<Time_Account>& all_accounts);
	
	void add_hours(std::vector<Time_Account>& all_accounts);

	void set_table_width(const std::vector<Time_Account>& all_accounts, std::vector<int>& max_length);

	void show_filepaths() const;

	void show_language()const;
	
	void show_specific_table(const std::vector<Time_Account>& show_accounts);
	
	void show_all(const std::vector<Time_Account>& all_accounts);
	
};//Arg_Manager

#endif // ARG_MANAGER_H

// -------- /home/eichi/Dev/Projekte/std/src/code/arg_manager.cpp ---------
// ============= //
// == PUBLIC ==  //
// ============= //
Arg_Manager::Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::vector<std::string>& argv, const int& argc, const std::map<command, std::regex>& pattern)
    : jsonH(jH), str_argv(argv), argc(argc), regex_pattern(pattern)
{
    
    translator.set_language(jsonH->get_config_language());
    
    max_length = {
            10 //Index Standard	
        , 10 //Alias Standard
        , 15 //Entity Standard
        , static_cast<int>(translator.language_pack.at("total_hours").size())
        , static_cast<int>(translator.language_pack.at("hours").size() + 5)
        , static_cast<int>(translator.language_pack.at("timestamp").size() + 10)
        , static_cast<int>(translator.language_pack.at("comment").size() + 10)
    };
};

void Arg_Manager::proceed_inputs(std::vector<Time_Account>& all_accounts, const std::map<error, std::string>& str_error){
	
    this->str_error = str_error;
    
    switch(argc){
    
        case 2:
            {
                //Zeige Hilfe an
                //help
                if(std::regex_match(str_argv[1], regex_pattern.at(command::help))){
                
                    std::cout << help << std::endl;
                    break;
                }
                //Zeige alle Entity und Alias an
                //show
                if(std::regex_match(str_argv[1], regex_pattern.at(command::show))){
                
                        show_all(all_accounts);
                        break;
                }
                throw std::runtime_error{str_error.at(error::synthax)};
            };
            
        case 3:
            {	
                //show Möglichkeiten
                if(std::regex_match(str_argv[1], regex_pattern.at(command::show))){
                
                    //Zeige entity, accounts, config filepaths an
                    //show filepath
                    if(std::regex_match(str_argv[2], regex_pattern.at(command::config_filepath))){
                    
                        show_filepaths();
                        break;
                    }
                    //language anzeigen
                    if(std::regex_match(str_argv[2], regex_pattern.at(command::language))){
                                            
                        show_language();
                        break;
                    }
                    //Zeige spezifischen Account an
                    //show <entity> ODER show <alias>	
                        //entity oder alias ?? -> entsprechende accounts erhalten
                    std::vector<Time_Account> matching_accounts;
                    matching_accounts = check_for_alias_or_entity(all_accounts, str_argv[2]);
                    
                    if(matching_accounts.empty()){
                        throw std::runtime_error{str_error.at(error::unknown_alias_or_entity)};
                    }
                    
                    show_specific_table(matching_accounts);
                    break;
                }
                //Account löschen
                //del
                if(std::regex_match(str_argv[1], regex_pattern.at(command::delete_))){
                
                    delete_account(all_accounts, str_argv[2]);
                    break;
                }
                //Language changeTo
                //-l ger
                if(std::regex_match(str_argv[1], regex_pattern.at(command::language))){

                    bool is_possible_language = false;					
                    for(const auto& key : translator.dict_language){
                        if(key.second == str_argv[2]){
                            is_possible_language = true;
                            break;
                        }
                    }
                    
                    if(!is_possible_language){
                        std::stringstream ss;
                        ss << "\nPossible Languages:\n";
                        for(const auto& str : translator.dict_language){
                            ss << " > " << str.second << '\n';
                        }
                                                
                        throw std::runtime_error{str_error.at(error::unknown_language) + ss.str()};
                    }

                    change_config_json_language(str_argv[2]);
                    break;
                } 
                
                throw std::runtime_error{str_error.at(error::synthax)};
            };

        case 4:
            {
                //-f <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::user_filepath))){
                
                    user_change_filepaths(str_argv[2], str_argv[3]);
                    
                    std::cout << str_argv[2] << '\n' << str_argv[3] << std::endl;
                    break;
                }
                //Neuen Account hinzufügen
                //add	
                if(std::regex_match(str_argv[1], regex_pattern.at(command::add))){
                
                    add_account(all_accounts);
                    break;
                }
        
                //Für Alias Stunden h oder Minuten m hinzufügen	OHNE Kommentar	
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                        || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
                    add_hours(all_accounts);
                    break;
                }
                
                throw std::runtime_error{str_error.at(error::synthax)};
            };
            
        case 5:
            {
                //-cf <configFilepath> <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::config_filepath))){
                
                    change_config_json_file(str_argv[2], str_argv[3], str_argv[4]);
                    
                    std::cout << str_argv[2] << '\n' << str_argv[3] << '\n' << str_argv[4] << std::endl;
                    break;
                }
    
                //Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                        || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
                    add_hours(all_accounts);
                    break;
                }
                throw std::runtime_error{str_error.at(error::synthax)};
            };
            
        default:
            {
                throw std::runtime_error{str_error.at(error::untitled_error)};
            };
    };
}
// ============= //
// == PRIVATE == //
// ============= //

std::vector<Time_Account> Arg_Manager::check_for_alias_or_entity(const std::vector<Time_Account>& all_accounts, const std::string& alias_or_entity){
    std::vector<Time_Account> matching_accounts;
    bool alias_found = false;
    bool entity_found = false;
    for(auto& account : all_accounts){
    
        if(account.get_alias() == alias_or_entity){
            alias_found = true;
            matching_accounts = collect_accounts_with_alias(all_accounts, str_argv[2]);
            break;
        }
        if(account.get_entity() == alias_or_entity){
            entity_found = true;
            matching_accounts = jsonH->collect_accounts_with_entity(all_accounts, str_argv[2]);
            break;
        }
    }

    if( (!alias_found && !entity_found) || matching_accounts.empty()){
        throw std::runtime_error{str_error.at(error::unknown_alias_or_entity)};
    }
    
    return matching_accounts;
}


std::vector<Time_Account> Arg_Manager::collect_accounts_with_alias(const std::vector<Time_Account>& search_in, const std::string& search_for_alias){
    std::vector<Time_Account> matching_accounts;
    std::copy_if(
        search_in.begin(), search_in.end(),
        std::back_inserter(matching_accounts),
        [&search_for_alias](const Time_Account& acc) {
            return acc.get_alias() == search_for_alias;
        }
    );
    return matching_accounts;
}

void Arg_Manager::change_config_json_language(const std::string& to_language){
    std::map<std::string, std::string> new_data = {
            {"config_filepath", jsonH->get_config_filepath()}
        , {"entity_filepath", jsonH->get_entity_filepath()}
        , {"accounts_filepath", jsonH->get_accounts_filepath()}
        , {"language", to_language}
    };
    jsonH->save_config_file(new_data);
    
}

void Arg_Manager::change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath){
	
    std::map<std::string, std::string> new_data = {
            {"config_filepath", conf_filepath}
        , {"entity_filepath", ent_filepath}
        , {"accounts_filepath", acc_filepath}
        , {"language", translator.get_str_language()}
    };
    jsonH->save_config_file(new_data);

}

void Arg_Manager::user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath){
    std::map<std::string, std::string> new_filepaths = {
            {"entity_filepath", ent_filepath}
        , {"accounts_filepath", acc_filepath}
    };
    jsonH->save_config_file(new_filepaths);

}

void Arg_Manager::delete_account(std::vector<Time_Account>& all_accounts, const std::string& alias_to_delete){
    if(all_accounts.empty()){
        throw std::runtime_error{str_error.at(error::untitled_error)};
    }
    
    bool found_alias = false;
    std::string entity;
    std::string alias;
    
    for(const auto& account : all_accounts){
        if(account.get_alias() == alias_to_delete){
            entity = account.get_entity();
            alias = account.get_alias();
            found_alias = true;
            break;
        }
    }
    if(!found_alias){
        throw std::runtime_error{str_error.at(error::unknown_alias)};
    }
    
    std::vector<Time_Account> adjusted_accounts;

    //remove out of all_accounts
    std::remove_copy_if(all_accounts.begin(), all_accounts.end(),
        std::back_inserter(adjusted_accounts),
        [alias_to_delete](const Time_Account& account){
            return account.get_alias() == alias_to_delete;
        }
    );
    jsonH->save_json_entity(all_accounts, entity, alias);
    
    all_accounts = adjusted_accounts;

    //update Files
    jsonH->save_json_accounts(all_accounts);
    //jsonH->save_json_entity(all_accounts, entity, alias);
    
    std::cout << alias_to_delete << translator.language_pack.at("deleted_out_of_accounts.json") << std::endl;
}


void Arg_Manager::add_account(std::vector<Time_Account>& all_accounts){
    std::string entity = str_argv[2];
    std::string alias = str_argv[3];

    //Entity or Alias cant be named as a command
    bool is_command = false;
    for(const auto& pattern : regex_pattern){
        
        if(std::regex_match(entity, pattern.second)){
            is_command = true;
            break;
        }	
        if(std::regex_match(alias, pattern.second)){
            is_command = true;
            break;
        }
    }
    if(is_command){
        throw std::runtime_error{str_error.at(error::user_input_is_command)};
    }
        
    //Entity or Alias already taken?
    for(const auto& account : all_accounts){
        if(account.get_alias() == alias){
            throw std::runtime_error{str_error.at(error::double_alias)};
        }
        if(account.get_alias() == entity){
            throw std::runtime_error{str_error.at(error::alias_equal_entity)};
        }
    }
    
    Time_Account new_account{entity, alias};
    all_accounts.push_back(new_account);
            
    jsonH->save_json_accounts(all_accounts);
    jsonH->save_json_entity(all_accounts, entity, {});
            
    std::cout << "-> " << alias << " | " << entity << translator.language_pack.at("saved") << std::endl;
    
}

void Arg_Manager::add_hours(std::vector<Time_Account>& all_accounts){
	
    bool found_alias = false;
    auto localTime = clock.get_time();
    
    for(auto& account : all_accounts){
                        
        if(str_argv[1] == account.get_alias() ){
            float time_float = stof(str_argv[2]);
            if (std::regex_match(str_argv[3], regex_pattern.at(command::minutes))){
                time_float /= 60.f;
            }
            
            std::string description;
            if(argc > 4){
                description = str_argv[4];
            }
            
            Entry entry{time_float, description, localTime};
            account.add_entry(entry);

            jsonH->save_json_accounts(all_accounts);
            jsonH->save_json_entity(all_accounts, account.get_entity(), {});

            found_alias = true;

            break;	
        }
    }
    
    if(found_alias){
        std::cout 
            << std::put_time(&localTime, translator.language_pack.at("timepoint").c_str()) << '\n'
            << translator.language_pack.at("time_saved")
            << std::endl;
    }else{
    
        throw std::runtime_error{str_error.at(error::not_found)};
    }
}

void Arg_Manager::set_table_width(const std::vector<Time_Account>& all_accounts, std::vector<int>& max_length){
		
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

void Arg_Manager::show_filepaths()const {
    std::cout 
        << "Config: " << jsonH->get_config_filepath() << '\n'
        << translator.language_pack.at("entity") << ": " << jsonH->get_entity_filepath() << '\n' 
        << "Accounts: " << jsonH->get_accounts_filepath() << std::endl;
}

void Arg_Manager::show_language() const{
    std::cout << translator.language_pack.at("str_language")<< ": " << translator.language_pack.at("language") << std::endl;
}

void Arg_Manager::show_specific_table(const std::vector<Time_Account>& show_accounts) {
		
    set_table_width(show_accounts, max_length);
    
    int index{0};
    int index_entrys = 0;
    
    //Trennlinie 
    std::cout << '\n' << std::setfill('=') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
        << "=" << std::setfill(' ') << '\n' << std::endl;
        
    for(const auto& account : show_accounts){
        
        std::cout << std::left 
            << std::setw(max_length[0]) << "index"
            << std::setw(max_length[1]) << "Alias"
            << std::setw(max_length[2]) << translator.language_pack.at("entity")
            << std::setw(max_length[3]) << translator.language_pack.at("total_hours")
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

        ++index;
        if(account.get_entry().empty()){
            std::cout << translator.language_pack.at("No_Entrys_available") << std::endl;
            continue;
        }
        
        std::cout << std::left
            << std::setw( max_length[0]) << "index"
            << std::setw(max_length[4]) << translator.language_pack.at("hours")
            << std::setw(max_length[5]) << translator.language_pack.at("timestamp")
            << std::setw(max_length[6]) << translator.language_pack.at("comment")
            << std::endl;
            
        for(const auto& entry : account.get_entry()){
            std::stringstream ss;
            
            std::string time_format = translator.language_pack.at("timepoint");
            ss << std::put_time(&entry.time_point, time_format.c_str());

            //Trennlinie 
            std::cout << std::setfill('-') << std::setw( max_length[0] + max_length[4] + max_length[5] + max_length[6]) << "-" << std::setfill(' ') << std::endl;
            
            std::cout << std::left
                << std::setw( max_length[0]) << index_entrys
                << std::setw(max_length[4]) << entry.hours
                << std::setw(max_length[5]) << ss.str()
                << std::setw(max_length[6]) << entry.description
                << std::endl;
                                
            ++index_entrys;
        }
        //Trennlinie 
        std::cout << '\n' << std::setfill('=') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
            << "=" << std::setfill(' ') << '\n' << std::endl;
    }
}


void Arg_Manager::show_all(const std::vector<Time_Account>& all_accounts) {
		
    set_table_width(all_accounts, max_length);
    //Trennlinie 
    std::cout << '\n' << std::setfill('=') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
        << "=" << std::setfill(' ') << '\n' << std::endl;
        
    std::cout << std::left 
        << std::setw( max_length[0]) << "index"
        << std::setw( max_length[1]) << "Alias"
        << std::setw( max_length[2]) << translator.language_pack.at("entity")
        << std::setw( max_length[3]) << translator.language_pack.at("total_hours")
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

    //Trennlinie 
    std::cout << '\n' << std::setfill('=') << std::setw(max_length[0] + max_length[1] + max_length[2] + max_length[3]) 
        << "=" << std::setfill(' ') << '\n' << std::endl;
}

// -------- /home/eichi/Dev/Projekte/std/src/code/main.cpp ---------

#ifdef _WIN32
	#include <windows.h>

#else
	#include <limits.h>
	#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <regex>
#include <map>


// SIMPLE TIME DOCUMENTATION /* github.com/eichi150/std */

using json = nlohmann::json;

//Check for valid Arguments
bool is_argument_valid(const std::vector<std::string>& str_argv, const std::map<command, std::regex>& regex_pattern ){
	
	for(const auto& pattern : regex_pattern){
		for(size_t i{1}; i < str_argv.size(); ++i){
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

	std::map<command, std::regex> regex_pattern = {
		  { command::help,      		std::regex{R"(^(--?h(elp)?|help)$)", std::regex_constants::icase } }
		, { command::add,       		std::regex{R"(^(--?a(dd)?|add)$)", std::regex_constants::icase } }
		, { command::show,      		std::regex{R"(^(--?sh(ow)?|sh|show)$)", std::regex_constants::icase } }
		, { command::delete_,   		std::regex{R"(^(--?d(elete)?|del(ete)?)$)", std::regex_constants::icase } }
		, { command::hours, 			std::regex{R"(^(--?h(ours)?|h|hours)$)", std::regex_constants::icase } }
		, { command::minutes, 		std::regex{R"(^(--?m(inutes)?|m|minutes)$)", std::regex_constants::icase} }
		, { command::config_filepath, std::regex{R"(^--?cf$)", std::regex_constants::icase } }
		, { command::user_filepath,  	std::regex{R"(^(--?f(ilepath)?|filepath)$)", std::regex_constants::icase } }
		, { command::language,  		std::regex{R"(^(--?l(anguage)?|language)$)", std::regex_constants::icase } }
	};

	std::map<error, std::string> str_error{
		  {error::double_entity, "Entity already taken"}
		, {error::double_alias, "Alias already taken"}
		, {error::alias_equal_entity, "Alias cant be equal to any Entity"}
		, {error::unknown_alias_or_entity, "Alias or Entity not found"}
		, {error::user_input_is_command, "Rejected Input"}
		, {error::not_found, "Not found"}
		, {error::synthax, "Synthax wrong"}
		, {error::untitled_error,"Untitled Error"}
		, {error::unknown, "Unknown Command"}
		, {error::unknown_alias, "Unknown Alias"}
		, {error::unknown_language, "Unknown Language"}
	};
	
	if(argc > 1){
		try{
			if(is_argument_valid(str_argv, regex_pattern)){
				//Initalize
				std::vector<Time_Account> all_accounts{};
				JSON_Handler jsonH{all_accounts};

				//Init Argument Manager
				Arg_Manager arg_man{std::make_shared<JSON_Handler>(jsonH), str_argv, argc, regex_pattern};
			
				arg_man.proceed_inputs(all_accounts, str_error);
				
			}else{
			
				throw std::runtime_error{str_error.at(error::unknown)};
			}
		//Error Output	
		}catch(const std::runtime_error& re){
			std::cerr << "**" << re.what() << std::endl;
		}
		
	}else{
	
		std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
	}
	
	return 0;
}


