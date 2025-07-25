// Auto-generated single-file version of std

// -------- /home/eichi/Dev/Projekte/std/src/code//bme280/bme280_sensor.h ---------
#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#ifdef __linux__
	#include <linux/i2c-dev.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	
#endif // __linux__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <vector>

class BME_Sensor {
public:
    int scan_sensor(std::vector<float>& float_data);

private:
    int i2c_fd;
	
    // Muss static sein, da Funktionszeiger in C keinen Zugriff auf `this` haben
    static BME280_INTF_RET_TYPE user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);

    static BME280_INTF_RET_TYPE user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

    static void user_delay_us(uint32_t period_us, void *intf_ptr);
};

#endif // BME280_SENSOR_H


// -------- /home/eichi/Dev/Projekte/std/src/code//bme280/bme280_sensor.cpp ---------
#ifdef __linux__


int BME_Sensor::scan_sensor(std::vector<float>& float_data) {
    const char *i2c_device = "/dev/i2c-1";
    static uint8_t dev_addr = BME280_I2C_ADDR_PRIM; // 0x76

    // I2C öffnen
    if ((i2c_fd = open(i2c_device, O_RDWR)) < 0) {
        perror("I2C open");
        return 1;
    }

    if (ioctl(i2c_fd, I2C_SLAVE, dev_addr) < 0) {
        perror("I2C ioctl");
        return 1;
    }

    struct bme280_dev dev;
    dev.intf = BME280_I2C_INTF;
    dev.intf_ptr = &i2c_fd;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = user_delay_us;

    if (bme280_init(&dev) != BME280_OK) {
        printf("Sensorinitialisierung fehlgeschlagen\n");
        return 1;
    }

    struct bme280_settings settings;
    settings.osr_h = BME280_OVERSAMPLING_1X;
    settings.osr_p = BME280_OVERSAMPLING_16X;
    settings.osr_t = BME280_OVERSAMPLING_2X;
    settings.filter = BME280_FILTER_COEFF_16;
    settings.standby_time = BME280_STANDBY_TIME_1000_MS;

    uint8_t settings_sel = BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP |
                           BME280_SEL_OSR_HUM | BME280_SEL_FILTER;

    if (bme280_set_sensor_settings(settings_sel, &settings, &dev) != BME280_OK) {
        printf("Sensor Settings fehlgeschlagen\n");
        return 1;
    }

    if (bme280_set_sensor_mode(BME280_POWERMODE_FORCED, &dev) != BME280_OK) {
        printf("Sensor Mode setzen fehlgeschlagen\n");
        return 1;
    }

    dev.delay_us(40000, dev.intf_ptr);

    struct bme280_data comp_data;
    if (bme280_get_sensor_data(BME280_ALL, &comp_data, &dev) != BME280_OK) {
        printf("Sensor Data lesen fehlgeschlagen\n");
        return 1;
    }


    float_data.push_back(comp_data.temperature);
    float_data.push_back(comp_data.pressure / 100.0f);
    float_data.push_back(comp_data.humidity);
    
    
    close(i2c_fd);
    
    return 0;
}


BME280_INTF_RET_TYPE BME_Sensor::user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr) {
    int fd = *(int *)intf_ptr;
    if (write(fd, &reg_addr, 1) != 1){
        return -1;
    }
    if (read(fd, data, len) != (int)len){
        return -1;
    }
    return 0;
}

BME280_INTF_RET_TYPE BME_Sensor::user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
    int fd = *(int *)intf_ptr;
    uint8_t buf[1 + len];
    buf[0] = reg_addr;
    for (uint32_t i = 0; i < len; i++) {
        buf[i + 1] = data[i];
    }
    if (write(fd, buf, len + 1) != (int)(len + 1)){
        return -1;
    }
    return 0;
}

void BME_Sensor::user_delay_us(uint32_t period_us, void *intf_ptr) {
    usleep(period_us);
}

#endif // __linux__


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

struct Automation_Config{
	std::string connection;
	std::string entity;
	std::string alias;
	int interval;
	std::string einheit;
	bool logfile;
};

class Time_Account{
public:
	Time_Account(const std::string& ent, const std::string& al) : entity(ent), alias(al){};

	void set_tag(const std::string& new_tag);
	
	void set_hours(const float& amount);
	
	std::string get_entity() const;
	std::string get_alias() const;
	std::vector<Entry> get_entry() const;

	std::string get_tag() const;
	float get_hours() const;
	
	void add_json_read_entry(const Entry& read_entry);
	
	void add_entry(const Entry& new_entry);

private:
	std::string entity{};
	std::string alias{};
	std::string tag{};
	
	float hours{0.f};
	std::vector<Entry> entry;
};

#endif // TIME_ACCOUNT_H


// -------- /home/eichi/Dev/Projekte/std/src/code/time_account.cpp ---------

void Time_Account::set_tag(const std::string& new_tag){
	tag = new_tag;
}
std::string Time_Account::get_tag() const {
	return tag;
}

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

	void read_all_accounts(std::vector<Time_Account>& all_accounts);
	
	std::string get_config_filepath() const;

	std::string get_entity_filepath() const;

	std::string get_accounts_filepath() const;

	Language get_config_language() const;

	std::string get_automatic_config_filepath() const { return automation_config_filepath; }
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

	void save_automation_config_file(const std::vector<std::string>& automation_config);
	std::vector<Automation_Config> read_automation_config_file();
	
private:
	std::string config_filepath{"../config.json"};
	
	const std::vector<std::string> allowed_keys = {
		"config_filepath"
		, "entity_filepath"
		, "accounts_filepath"
		, "language"
		, "automation_filepath"
	};
	
	std::string entity_filepath{"../files/"};
	std::string accounts_filepath{"../files/accounts.json"};
	std::string automation_config_filepath{"../automation_config.json"};
	
	Language config_language = Language::english;
};

#endif // JSON_HANDLER_H


// -------- /home/eichi/Dev/Projekte/std/src/code/json_handler.cpp ---------

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

	std::string connection = config_json.value("connection", "undefined");
	
	//config durchlaufen
	for(const auto& config : config_json["config"]){
		std::string interval = config.value("interval", "0");
		std::string logFile = config.value("log_file", "false");

		all_automations.push_back(Automation_Config{
			connection
			, config.value("entity", "")
			, config.value("alias", "")
			, stoi(interval)
			, config.value("einheit", "")
			, (logFile=="true" ? false : true )
		});
	}

	return all_automations;
}

void JSON_Handler::save_automation_config_file(const std::vector<std::string>& automation_config){

	if(automation_config.size() < 6){
		throw std::runtime_error{"##Automation_Config Error"};
	}
	
	json config;
	config["connection"] = automation_config[0];
	
	//configs
	config["config"] = {
		{
			{"entity", automation_config[1]},
			{"alias", automation_config[2]},
			{"interval", automation_config[3]},
			{"einheit", automation_config[4]},
			{"log_file", automation_config[5]}
		}
	};
	
    std::ofstream config_file(automation_config_filepath);
    if(config_file.is_open()){
        config_file << config.dump(4);
        config_file.close();
        std::cout << "##Automation Config File saved" << std::endl;
    }else{
        throw std::runtime_error{"##Cant open Config_File!"};
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
	, sensor
	, tag_not_found
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
	, tag
	, touch_sensor
	, messure_sensor
	, activate
	, i2c
	, automatic
	, environment
};


enum class Language{
	  english = 0
	, german 
};

enum class Tag{
	  none = 0
	, plant
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
    ,{"Temperature", "Temperature"}
    ,{"Pressure", "Pressure"}
    ,{"Humidity", "Humidity"}
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
    ,{"Temperature", "Temperatur"}
    ,{"Pressure", "Druck"}
    ,{"Humidity", "Feuchte"}
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
#include <cstdlib>
#include <fstream>

#ifdef __linux__
#endif

class Cmd_Ctrl{
public:
	
	//Check for valid Arguments
	bool is_argument_valid(const std::vector<std::string>& str_argv){
		
		for(const auto& pattern : regex_pattern){
			for(size_t i{1}; i < str_argv.size(); ++i){
				if(std::regex_match(str_argv[i], pattern.second)){
				
					return  true;
				}	
			}
		}
		
		return false;
	}

	std::map<error, std::string> get_str_error() const { return str_error; }
	std::map<command, std::regex> get_regex_pattern() const { return regex_pattern; }
private:

	std::map<command, std::regex> regex_pattern = {
		  { command::help,      		std::regex{R"(^--?help$)", std::regex_constants::icase } }
		, { command::add,       		std::regex{R"(^--?add$)", std::regex_constants::icase } }
		, { command::show,      		std::regex{R"(^(--?sh(ow)?|sh|show)$)", std::regex_constants::icase } }
		, { command::delete_,   		std::regex{R"(^(--?del(ete)?)$)", std::regex_constants::icase } }
		, { command::hours, 			std::regex{R"(^(--?h(ours)?|h|hours)$)", std::regex_constants::icase } }
		, { command::minutes, 			std::regex{R"(^(--?m(inutes)?|m|minutes)$)", std::regex_constants::icase} }
		, { command::config_filepath, 	std::regex{R"(^--?cf$)", std::regex_constants::icase } }
		, { command::user_filepath,  	std::regex{R"(^(--?f(ilepath)?|filepath)$)", std::regex_constants::icase } }
		, { command::language,  		std::regex{R"(^(--?l(anguage)?|language)$)", std::regex_constants::icase } }
		, { command::tag,				std::regex{R"(^--?tag$)", std::regex_constants::icase } }
		, { command::touch_sensor, 		std::regex{R"(^--?touch$)", std::regex_constants::icase } }
		, { command::messure_sensor,	std::regex{R"(^(--?mes(sure)?)$)", std::regex_constants::icase } }
	    , { command::activate,			std::regex{R"(^(--?a(ctivate)?)$)", std::regex_constants::icase } }
	    , { command::i2c, 				std::regex{R"(^--?i2c$)", std::regex_constants::icase } }
		, { command::automatic, 		std::regex{R"(^--?auto$)", std::regex_constants::icase } }
		, { command::environment, 		std::regex{R"(^--?env$)", std::regex_constants::icase } }
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
		, {error::sensor, "Sensor Error. Make sure you installed i2c.\nExecute on Command Line: 'sudo apt-get install i2c-tools'\nand try 'sudo i2cdetect -y 1'\nPort: 76 should be active. Succesfully installed."}
		, {error::tag_not_found, "Unknown Tag"}
	};
	
};

#ifdef __linux__
class Device_Ctrl{
public:
	Device_Ctrl(const std::string& error_prompt) : error_prompt(error_prompt){};
	
	void process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& alias){
	
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
		ss << "Temp: " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
			<< "Druck: " << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
			<< "Feuchte: " << std::fixed << std::setprecision(2) << output_sensor[2] << " %";
			
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
	
	std::vector<float> check_device(){
		
		std::vector<float> output_sensor;
		
		if(sensor.scan_sensor(output_sensor) == 1){
			throw std::runtime_error{error_prompt};
		}

		return output_sensor;
	}

	
	void write_Crontab(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& alias, bool logfile){
		
		std::string exe_filepath = jsonH->getExecutableDir() + "/";
		std::cout << "Crontab Exe Filepath " << exe_filepath << std::endl;
		
		std::string cronLine = "*/15 * * * * " + exe_filepath + "std -auto " + alias + " -mes";
		std::string logLine = " >> " + exe_filepath + "std.log 2>&1";

		//alte Crontab sichern
		system("crontab -l > /tmp/mycron");

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
	
private:
	Clock clock{};
	BME_Sensor sensor{};
	std::string error_prompt;
	std::vector<Automation_Config> all_automations;
	std::vector<Time_Account> all_accounts;

	
};
#endif // __linux__

class Arg_Manager{
public:
	Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::shared_ptr<Cmd_Ctrl>& ctrl);
	
	void proceed_inputs(const int& argc, const std::vector<std::string>& argv);

	bool run_environment() const { return run_env; }
	
private:
	bool run_env = false;
	
	std::shared_ptr<JSON_Handler> jsonH;
	std::vector<Time_Account> all_accounts;
	Translator translator{};
	Clock clock{};	
	std::vector<std::string> str_argv;
	int argc;
	//show Tabellen setw(max_length[]) 
	std::vector<int> max_length;
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	std::map<Tag, std::string> all_tags;
	
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
	
	void add_account(std::vector<Time_Account>& all_accounts, const std::string& tag);

	void add_tag_to_account(std::vector<Time_Account>& all_accounts, const std::string& tag);

#ifdef __linux__
	void add_sensor_data(std::vector<Time_Account>& all_accounts);
#endif // __linux__
	
	void add_hours(std::vector<Time_Account>& all_accounts, const std::string& amount);
	
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

Arg_Manager::Arg_Manager(const std::shared_ptr<JSON_Handler>& jH, const std::shared_ptr<Cmd_Ctrl>& ctrl): jsonH(jH)
{

	jsonH->read_all_accounts(all_accounts);
	
    regex_pattern = ctrl->get_regex_pattern();
    str_error = ctrl->get_str_error();
    
    translator.set_language(jsonH->get_config_language());
    
    max_length = {
        10 //Index Standard
        , 10 //Alias Standard
        , 15 //Entity Standard
        , static_cast<int>(translator.language_pack.at("total_hours").size())
        , static_cast<int>(translator.language_pack.at("hours").size() + 3)
        , 25
        , static_cast<int>(translator.language_pack.at("comment").size() + 10)
    };

    all_tags = {
	   	  {Tag::none, "none"}
		, {Tag::plant, "plant"}
	};

};

void Arg_Manager::proceed_inputs(const int& argc, const std::vector<std::string>& argv){

	this->str_argv = argv;
	this->argc = argc;
	
    /*if(start_environment){
    	str_argv.push_back("std");
    	argc = 1;
    	
    	//grab new argument INPUTS
    	if(input == "exit"){
    		start_environment = false;
    		return;
    	}	
    }*/
    
    switch(argc){
    
        case 2:
            {
                //Zeige Hilfe an
                //help
                if(std::regex_match(str_argv[1], regex_pattern.at(command::help))){
                
                    std::cout << help << std::endl;
                    break;
                }
                
                //Zeige alle <entity> und <alias> an
                //show
                if(std::regex_match(str_argv[1], regex_pattern.at(command::show))){
                
                    show_all(all_accounts);
                    break;
                }

                if(std::regex_match(str_argv[1], regex_pattern.at(command::environment))){
                	run_env = true;
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
                //del <alias>
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
                

                //i2c Sensor Messwert abfrage
				//-touch i2c
				if(std::regex_match(str_argv[1], regex_pattern.at(command::touch_sensor))){
				
		#ifdef __linux__
		
				    Device_Ctrl device{str_error.at(error::sensor)};
				    std::vector<float> output_sensor = device.check_device();

				    std::stringstream output_str;
				    output_str << translator.language_pack.at("Temperature") << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
				   		<< translator.language_pack.at("Pressure")<< ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
				    	<< translator.language_pack.at("Humidity") << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
				        
				    std::cout << output_str.str() << std::endl;
		#else
			std::cout << "Not available in this Version" << std::endl;
			
		#endif // __linux__
		
				    break;
				}
                                
                //i2c Sensor Messwerte für <alias> speichern
                // <alias> -mes
                if(std::regex_match(str_argv[2], regex_pattern.at(command::messure_sensor))){
                
        #ifdef __linux__
                
                    Device_Ctrl device{str_error.at(error::sensor)};
                    add_sensor_data(all_accounts);
                    
		#else
			std::cout << "Not available in this Version" << std::endl;
		#endif // __linux__
		
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
                
                    add_account(all_accounts, {});
                    break;
                }
        
                //Für Alias Stunden h oder Minuten m hinzufügen	OHNE Kommentar	
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                   || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
                    add_hours(all_accounts, str_argv[2]);
                    break;
                }

                //tag nachträglich hinzufügen
                //<alias> -tag plant
				if(std::regex_match(str_argv[2], regex_pattern.at(command::tag))) 
               	{
               		if(str_argv[3] == all_tags.at(Tag::none)){
                		add_tag_to_account(all_accounts, {});
                	}else
               		                	
                	if(str_argv[3] == all_tags.at(Tag::plant)){
                		add_tag_to_account(all_accounts, all_tags.at(Tag::plant));
                		
                	}else{
                		std::stringstream ss;
	                    ss << "\nPossible Tag:\n";
	                    for(const auto& str : all_tags){
	                        ss << " > " << str.second << '\n';
	                    }
	                   	throw std::runtime_error{str_error.at(error::tag_not_found) + ss.str()};
                	}
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
                    add_hours(all_accounts, str_argv[2]);
                    break;
                }

				//Automation konfigurieren
				//<alias> -a -mes <time_config>
       			if( std::regex_match(str_argv[2], regex_pattern.at(command::activate))
					&& std::regex_match(str_argv[3], regex_pattern.at(command::messure_sensor))
					//&& std::regex_match(str_argv[4], regex_pattern.at(command::))
       			){
       			
       	#ifdef __linux__		
       	
       				std::vector<std::string> automation_config = {
       					{"i2c", "ChocoHaze", str_argv[1], "15", "minutes", "true"}
       				};
       				jsonH->save_automation_config_file(automation_config);
       				
       				Device_Ctrl device{str_error.at(error::sensor)};
       				device.write_Crontab(jsonH, str_argv[1], true);
		#else
			std::cout << "Not available in this Version" << std::endl;
		#endif // __linux__
			
       				break;
       			}
	
                throw std::runtime_error{str_error.at(error::synthax)};
            };

        case 6:
        	{	
       		 	//Neuen Account mit tag hinzufügen
                //std add <entity> <alias> -tag plant
                if(std::regex_match(str_argv[1], regex_pattern.at(command::add))
                	&& std::regex_match(str_argv[4], regex_pattern.at(command::tag)) 
                	&& str_argv[5] == all_tags.at(Tag::plant))
               	{
                
                    add_account(all_accounts, all_tags.at(Tag::plant));
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
        , {"automation_filepath", jsonH->get_automatic_config_filepath()}
    };
    jsonH->save_config_file(new_data);
    
}

void Arg_Manager::change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath){
	
    std::map<std::string, std::string> new_data = {
          {"config_filepath", conf_filepath}
        , {"entity_filepath", ent_filepath}
        , {"accounts_filepath", acc_filepath}
        , {"language", translator.get_str_language()}
        , {"automation_filepath", std::string{ent_filepath + "automation_config.json"}}
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


void Arg_Manager::add_account(std::vector<Time_Account>& all_accounts, const std::string& tag){
    std::string entity = str_argv[2];
    std::string alias = str_argv[3];
	std::stringstream ss;
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

    ss << entity << "-> " << alias;
    if(!tag.empty()){
    	new_account.set_tag(tag);
    	ss <<  " -tag= " << tag;
    }
    ss << translator.language_pack.at("saved");
    
    all_accounts.push_back(new_account);
            
    jsonH->save_json_accounts(all_accounts);
    jsonH->save_json_entity(all_accounts, entity, {});
            
    std::cout << ss.str() << std::endl;
}

void Arg_Manager::add_hours(std::vector<Time_Account>& all_accounts, const std::string& amount){
	
    bool found_alias = false;
    auto localTime = clock.get_time();
    
    for(auto& account : all_accounts){
                        
        if(str_argv[1] == account.get_alias() ){
        
            float time_float = stof(amount);
            
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

#ifdef __linux__
void Arg_Manager::add_sensor_data(std::vector<Time_Account>& all_accounts){
    bool found_alias = false;
    auto localTime = clock.get_time();
	std::stringstream output_str;
	
    for(auto& account : all_accounts){
        if(str_argv[1] == account.get_alias() ){

            Device_Ctrl device{str_error.at(error::sensor)};
		    std::vector<float> output_sensor = device.check_device();
            
            output_str << translator.language_pack.at("Temperature") << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
                << translator.language_pack.at("Pressure")<< ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
                << translator.language_pack.at("Humidity") << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
                
            Entry entry{0.f, output_str.str(), localTime};
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
            << translator.language_pack.at("time_saved") << '\n'
            << output_str.str()
            << "BME Sensor Connection closed"
            << '\n' << std::setfill('=') << std::setw(10) << "=" << std::setfill(' ') << '\n'
            << std::endl;
    }else{
    
        throw std::runtime_error{str_error.at(error::not_found)};
    }
}
#endif // __linux__


void Arg_Manager::add_tag_to_account(std::vector<Time_Account>& all_accounts, const std::string& tag){

	bool found_alias = false;
	std::string entity;
	for(auto& account : all_accounts){
		if(account.get_alias() == str_argv[1]){
			
			account.set_tag(tag);
			found_alias = true;
			entity = account.get_entity();
			
			std::cout << "Tag: " << tag << " to " << account.get_alias() << " added" << std::endl;
			break;
		}
	}
	if(!found_alias){
       throw std::runtime_error{str_error.at(error::not_found)};
	}
	
	jsonH->save_json_accounts(all_accounts);
    jsonH->save_json_entity(all_accounts, entity, {});
    	
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

#include <vector>
#include <string>
#include <sstream>
#include <regex>

// Gibt eine Liste von Tokens zurück, getrennt durch Leerzeichen oder Komma
std::vector<std::string> split_input(const std::string& input) {
    std::vector<std::string> result;
    std::regex re(R"([\s]+)"); // regex: trennt an Leerzeichen ODER Kommas
    std::sregex_token_iterator it(input.begin(), input.end(), re, -1);
    std::sregex_token_iterator end;

    for (; it != end; ++it) {
        if (!it->str().empty()) {
            result.push_back(it->str());
        }
    }

    return result;
}


int main(int argc, char* argv[]){

	//Argumente entgegen nehmen und in vector<string> speichern
	std::vector<std::string> str_argv;
	for(int i{0}; i < argc; ++i){
		std::string arg = argv[i];
		str_argv.push_back(arg);
	}
	argv = {};
	if(static_cast<size_t>(argc) != str_argv.size()){
		std::cout << "!!Argument Error" << std::endl;
		return 1;
	}
	
	if(argc > 1){
		try{
			Cmd_Ctrl ctrl{};
			
			if(ctrl.is_argument_valid(str_argv)){
			
				std::shared_ptr<JSON_Handler> jsonH = std::make_shared<JSON_Handler>();

		#ifdef __linux__
				//Vorher Automation ausführen
				auto regex_pattern = ctrl.get_regex_pattern();
				if(std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
					&& std::regex_match(str_argv[3], regex_pattern.at(command::messure_sensor))
				){
					Device_Ctrl device{ctrl.get_str_error().at(error::unknown)};
					device.process_automation(jsonH, str_argv[2]);
					
					return 0;
				}
		#endif //__linux__

			
				//Init Argument Manager
				Arg_Manager arg_man{jsonH, std::make_shared<Cmd_Ctrl>(ctrl)};
				
				arg_man.proceed_inputs(argc, str_argv);
				
				if(arg_man.run_environment()){
					std::cout << "std - Environment started... |-- Close with 'exit'" << std::endl;
					do{
						bool arg_valid = false;
						std::vector<std::string> new_argv{"std"};
						
						do{
							//New Inputs parsen
							std::string input;
							std::cout << "@std > ";
							std::getline(std::cin, input);
												
							if(input == "exit"){
								return 0;
							}
							
							new_argv = split_input(input);
							new_argv.insert(new_argv.begin(), "std");

							arg_valid = ctrl.is_argument_valid(new_argv);
							
						}while(!arg_valid);
						
						str_argv = new_argv;
						argc = static_cast<int>(str_argv.size());

						try{
				
							arg_man.proceed_inputs(argc, str_argv);
							
						}catch(const std::runtime_error& rt){
							std::cout << "**" << rt.what() << std::endl;
						}
					}while(arg_man.run_environment());
				}
				
			}else{
			
				throw std::runtime_error{ctrl.get_str_error().at(error::unknown)};
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


