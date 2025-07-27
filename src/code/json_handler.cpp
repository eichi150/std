#include "json_handler.h"

using json = nlohmann::json;


void JSON_Handler::read_all_accounts(std::vector<Time_Account>& all_accounts){

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


std::vector<Automation_Config> JSON_Handler::read_automation_config_file(){
	read_config_file();

	std::ifstream file(automation_config_filepath);
	if(!file.is_open()){
		throw std::runtime_error{"##Couldn't Open Automation Config"};
	}

	std::vector<Automation_Config> all_automations;
	
	json config_json;
	file >> config_json;

	if (!config_json.is_array()) {
		throw std::runtime_error{"##Invalid JSON format: expected array at root"};
	}

	for (const auto& con : config_json) {
		std::string connection = con.value("connection", "undefined");

		if (!con.contains("config") || !con["config"].is_array())
			continue;

		for (const auto& cfg : con["config"]) {
			all_automations.push_back(Automation_Config{
				connection,
				cfg.value("entity", ""),
				cfg.value("alias", ""),
				cfg.value("crontab_command", "* * * * *"),
				cfg.value("log_file", "")
			});
		}
	}

	return all_automations;
}


void JSON_Handler::save_automation_config_file(const std::vector<Automation_Config>& automation_config){
	if(automation_config.empty()){
		throw std::runtime_error{"##Automation_Config Error"};
	}

	json eintrag = json::array();
	std::map<std::string, json> grouped_configs;

	// Gruppieren nach "connection"
	for (const auto& auto_config : automation_config) {
		json cfg = {
			{"entity", auto_config.entity},
			{"alias", auto_config.alias},
			{"crontab_command", auto_config.crontab_command},
			{"log_file", auto_config.logfile}
		};

		grouped_configs[auto_config.connection]["connection"] = auto_config.connection;
		grouped_configs[auto_config.connection]["config"].push_back(cfg);
	}

	// In Array umwandeln
	for (const auto& [_, group] : grouped_configs) {
		eintrag.push_back(group);
	}

	
	std::ofstream config_file(automation_config_filepath);
	if(config_file.is_open()){
		config_file << eintrag.dump(4);
		config_file.close();
		std::cout << "##Automation Config File saved" << std::endl;
	} else {
		throw std::runtime_error{"##Cant open Automation_config_File!"};
	}
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
    automation_config_filepath = config_data.value("automation_filepath", "");

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

//wenn der Alias übergeben wird, wird er übersprungen und somit nicht in die Datei geschrieben -> delete Function
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
        alias_entry["tag"] = acc.get_tag();
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
                if (account.get_alias() != str_alias){
               		continue;
                }
				std::string tag = alias.value("tag", "");
				account.set_tag(tag);
				
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
