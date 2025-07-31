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


#ifdef __linux__
class BME_Sensor : public Sensor{
public:
    int scan_sensor(std::vector<float>& float_data) override;

private:
    int i2c_fd;
	
    // Muss static sein, da Funktionszeiger in C keinen Zugriff auf `this` haben
    static BME280_INTF_RET_TYPE user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);

    static BME280_INTF_RET_TYPE user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

    static void user_delay_us(uint32_t period_us, void *intf_ptr);
};


#else
class BME_Sensor : public Sensor{
public:
    int scan_sensor(std::vector<float>& float_data) override {
    	throw std::runtime_error{"Not implemented"};
    };


};
#endif // __linux__

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
    
    if(comp_data.temperature == -40.f || comp_data.humidity == 0.f){
        throw std::runtime_error{"Messfehler"};
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
	std::string crontab_command;
	std::string logfile;
	std::string device_name;
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
	
	void save_json_entity(const std::vector<Time_Account>& all_accounts, const std::string& entity_to_save);
	
	void save_json_accounts(const std::vector<Time_Account>& all_accounts);

	void read_json_entity(std::vector<Time_Account>& all_accounts);

	void read_json_accounts(std::vector<Time_Account>& all_accounts);

	void save_automation_config_file(const std::vector<Automation_Config>& automation_config);
	std::vector<Automation_Config> read_automation_config_file();
	
	std::string get_log() const {
		return jsonH_log.str();
	}
	
private:
	
	std::stringstream jsonH_log;
	
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
	std::string automation_config_filepath{"../files/automation_config.json"};
	
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
				cfg.value("log_file", ""),
				cfg.value("device_name", "")
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
			{"log_file", auto_config.logfile},
			{"device_name", auto_config.device_name}
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
		jsonH_log << "Automation Config File saved" << std::endl;
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
            jsonH_log << "Config_File saved" << std::endl;
        }else{
            jsonH_log << "##Cant open Config_File!" << std::endl;
        }
    }else{
        jsonH_log << "##No new valid config entries to save" << std::endl;
    }
}


void JSON_Handler::read_config_file(){
    config_filepath = getConfigFilePath();
    std::ifstream config_file(config_filepath);
    
    if(!config_file.is_open()){
        jsonH_log << "##Cant open " << config_filepath  << std::endl;
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
void JSON_Handler::save_json_entity(const std::vector<Time_Account>& all_accounts, const std::string& entity_to_save){
	
    std::vector<Time_Account> matching_accounts;
    if(!all_accounts.empty()){
        matching_accounts = collect_accounts_with_entity(all_accounts, entity_to_save);

        // Keine passenden Accounts?
        if (matching_accounts.empty()) {
            jsonH_log << "##Keine Eintäge für Entity '" << entity_to_save << "' gefunden." << std::endl;
            return;
        }

        jsonH_log << "~~matching accounts: "<< matching_accounts.size() << std::endl;
    }
    // 2. Struktur aufbauen
    json eintraege;
    eintraege["entity"] = entity_to_save;

    json alias_list = json::array();

    for (const auto& acc : matching_accounts) {
        
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

    eintraege["alias"] = alias_list;

    // 3. Datei schreiben
    std::string entity_filepath_total = entity_filepath + entity_to_save + ".json";
    std::ofstream file_entry(entity_filepath_total);
    if (file_entry.is_open()) {
        file_entry << eintraege.dump(4);
        file_entry.close();
        jsonH_log << "Saved" << entity_filepath_total << std::endl;
    } else {
        jsonH_log << "##Fehler beim Öffnen der Datei: " << entity_filepath_total << std::endl;
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
    jsonH_log << "Accounts_Filepath: " << accounts_filepath << '\n';
    
    std::ofstream file(accounts_filepath);
    if (file.is_open()) {
        file << daten.dump(4);
        file.close();
        jsonH_log << "Accounts-Datei gespeichert." << std::endl;
    } else {
        jsonH_log << "##Fehler beim oeffnen der Datei." << std::endl;
    }
}


void JSON_Handler::read_json_entity(std::vector<Time_Account>& all_accounts) {
    for (auto& account : all_accounts) {
        std::string filename = entity_filepath + account.get_entity() + ".json";
        std::ifstream entry_file(filename);
        if (!entry_file.is_open()) {
            jsonH_log << "##Eintragsdatei konnte nicht geöffnet werden: " << filename << std::endl;
            continue;
        }

        json entry_data;
        try {
            entry_file >> entry_data;
        } catch (const json::parse_error& e) {
            jsonH_log << "##JSON-Parse-Fehler: " << e.what() << std::endl;
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
                            jsonH_log << "##Zeit konnte nicht gelesen werden für Alias: " << str_alias << std::endl;
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
        jsonH_log << "##Datei konnte nicht geöffnet werden. " << accounts_filepath << std::endl;
        return;
    }

    if (eingabe.peek() == std::ifstream::traits_type::eof()) {
        jsonH_log << "##Datei ist leer.\n";
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
        jsonH_log << "##JSON-Fehler: " << e.what() << std::endl;
    }
}


// -------- /home/eichi/Dev/Projekte/std/src/code/translator.h ---------
#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <map>
#include <string>

enum class Language{
    english = 0
    , german 
};

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

    Language get_current_language() const {
	return language;
    }
    
private:
    Language language;
	
};

#endif // TRANSLATOR_H


// -------- /home/eichi/Dev/Projekte/std/src/code/translator.cpp ---------

std::map<std::string, std::string> Translator::english_pack = {
    {"language", "english"}
    ,{"str_language", "Language"}
    ,{"timepoint", "%Y-%m-%d %H:%M"}
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
    ,{"timepoint", "%d-%m-%Y %H:%M"}
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


// -------- /home/eichi/Dev/Projekte/std/src/code/device_ctrl.h ---------
#ifdef __linux__ // LINUX ONLY
#ifndef DEVICE_CTRL_H
#define DEVICE_CTRL_H

#include <iostream>
#include <iomanip>
#include <memory>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <cstdio>


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


class Device_Ctrl : public CTRL::Ctrl{
public:
	Device_Ctrl(const std::string& error_prompt);
	
	void show_log(bool set_to) override {
		show_ctrl_log = set_to;
	}
	std::string get_log() const override{
		return ctrl_log;
	}
	
	std::string process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command);
	
	std::string set_user_automation_crontab(const std::vector<std::string>& str_argv
		, const std::shared_ptr<JSON_Handler>& jsonH
		, const std::map<command, std::regex>& regex_pattern
	);
	
	std::vector<float> check_device(const std::string& name) override;

	std::string get_name() const { return name; }
	
	
private:
	std::regex integer_pattern = regex_pattern.at(command::integer);
	Clock clock{};
	
	std::string name;
	std::string error_prompt;
	std::vector<Automation_Config> all_automations;
	std::vector<Time_Account> all_accounts;
	
	std::map<weekday, std::string> str_weekday;
	std::map<months, std::string> str_months;
	
	
	std::pair<std::string, bool> get_user_crontag_line(const std::vector<std::string>& str_argv, const std::map<command, std::regex>& regex_pattern);

	bool write_Crontab(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command, const std::string& alias, bool logfile);

	std::string convert_crontabLine_to_speeking_str(const std::string& crontab_line);
	
	std::string check_that_between_A_B(const std::string& str, int A, int B, const std::string& error_prompt);
		
	std::vector<std::string> get_current_Crontab();
	
	bool crontab_contains(const std::vector<std::string>& crontabLines, const std::vector<std::string>& searched_targets);
};

#endif // DEVICE_CTRL_H
#endif // __linux__


// -------- /home/eichi/Dev/Projekte/std/src/code/device_ctrl.cpp ---------
#ifdef __linux__

Device_Ctrl::Device_Ctrl(const std::string& error_prompt) : error_prompt(error_prompt)
{
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

//std <alias> -a -mes "time"
std::string Device_Ctrl::set_user_automation_crontab(const std::vector<std::string>& str_argv
	, const std::shared_ptr<JSON_Handler>& jsonH, const std::map<command, std::regex>& regex_pattern)
{

	//Verarbeite User Arguments
	std::pair<std::string, bool> crontab_line;
	crontab_line = get_user_crontag_line(str_argv, regex_pattern);
	
	//write Command into Crontab
	if( !write_Crontab(jsonH, crontab_line.first, str_argv[1], crontab_line.second) ){
		std::cout << "Crontab existiert bereits. Kein neuer Eintrag in Crontab erforderlich." << std::endl;
	}
	std::cout << "Crontab written" << std::endl;
	
	//in Automation_Config File speichern
	std::vector<Automation_Config> automation_config;
	
    try{
        automation_config = jsonH->read_automation_config_file();
        
    }catch(const std::runtime_error& re){
		std::cerr << re.what() << std::endl;
	}

	
	automation_config.push_back(
		Automation_Config{
    		  "I2C"
    		, str_argv[0], str_argv[1] //entity alias
    		, crontab_line.first
    		,(crontab_line.second ? "true" : "false")
    		, str_argv[2] //device_name
   		}
 	);
       				
    jsonH->save_automation_config_file(automation_config);

	std::stringstream output;
	output 
		<< '\n' 
		<< "Time to execute: " 
		<< convert_crontabLine_to_speeking_str(crontab_line.first) 
		<< (crontab_line.second ? "with Logfile\n" : "\n");
		
	//Print
	return output.str();
}

std::string Device_Ctrl::process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command){

	//auto Accounts lesen
	all_automations = jsonH->read_automation_config_file();
	
	//auto Accounts erstellen
	std::string device_name;
	for(const auto& auto_config : all_automations){
		if(auto_config.crontab_command == command){
			Time_Account loaded_account{auto_config.entity, auto_config.alias};
					
			all_accounts.push_back(loaded_account);

			device_name = auto_config.device_name;
		}
	}
	
	//auto Accounts <entity>.json lesen
	jsonH->read_json_entity(all_accounts);

	//Sensor Werte abfragen
	std::vector<float> output_sensor = check_device(device_name);
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
		Entry entry{0.f, ss.str(), localTime};
		account.add_entry(entry);
		jsonH->save_json_entity(all_accounts, account.get_entity());
	}
	
	std::stringstream output;
	output 
		<< std::put_time(&localTime, "%Y-%m-%d %H:%M") 
		<< '\n' 
		<<  ss.str() 
		<< '\n';
		 
	return output.str();
}

std::vector<float> Device_Ctrl::check_device(const std::string& name) {

	this->name = name;
	
	std::vector<float> output_sensor;

	auto it = all_devices.find(name);
	if (it == all_devices.end()) {
		throw std::runtime_error{"Sensor not registered: " + name};
	}

	auto sensor_ptr = it->second;

	if (!sensor_ptr) {
		throw std::runtime_error{"Sensor pointer is null"};
	}

	// optional: dynamisch casten
	auto bme_ptr = std::dynamic_pointer_cast<BME_Sensor>(sensor_ptr);
	if (!bme_ptr) {
		throw std::runtime_error{"Sensor is not a BME_Sensor"};
	}

	if (bme_ptr->scan_sensor(output_sensor) == 1) {
		throw std::runtime_error{error_prompt};
	}

	return output_sensor;
}



std::pair<std::string, bool> Device_Ctrl::get_user_crontag_line(const std::vector<std::string>& str_argv, const std::map<command, std::regex>& regex_pattern){

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
		throw std::runtime_error{"Syntax Error"};
	}
	
	std::string crontab_line;
	for(const auto& c : crontab){
		crontab_line.append(c + ' ');
	}
	crontab_line.pop_back();
	
	return {crontab_line, with_logfile};
}

std::string Device_Ctrl::convert_crontabLine_to_speeking_str(const std::string& crontab_line){
	std::string result;
	// 0 */1 * * *
	// Every 1 hour @0 minutes 
	
	std::vector<std::string> splitted_crontab = split_line(crontab_line, std::regex{R"([\s]+)"});
	
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

bool Device_Ctrl::write_Crontab(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command, const std::string& alias, bool logfile){
	
	std::string exe_filepath = jsonH->getExecutableDir() + "/";
	//std::cout << "Crontab Exe Filepath " << exe_filepath << std::endl;
	
	std::string cronLine = command + " " + exe_filepath + "std -auto " + "\"" + command + "\"" + " -mes";
	std::string logLine = " >> " + exe_filepath + alias + "_std.log 2>&1";
	
	//alte Crontab sichern
	system("crontab -l > /tmp/mycron");

	std::vector<std::string> current_Crontab = get_current_Crontab();

	if(crontab_contains(current_Crontab, {cronLine, command}) ){
		return false;
	}
	if(logfile){
		cronLine.append(logLine);
	}
	//neue Zeile anhängen
	std::ofstream out("/tmp/mycron", std::ios::app);
	out << cronLine;
	out << '\n';
	out.close();

	//Neue Crontab setzen
	system("crontab /tmp/mycron");

	//aufräumen
	system("rm /tmp/mycron");

	return true;
}

bool Device_Ctrl::crontab_contains(const std::vector<std::string>& crontabLines, const std::vector<std::string>& searched_targets){

	for(const auto& line : crontabLines){
		if(line.empty() || line.size() < 8){
			continue;
		}
		if(searched_targets.size() >= 1){
		
			//targetLine in crontab?
			if(line.find(searched_targets[0]) != std::string::npos){
			
				//TargetChar in line vorhanden??
				std::vector<std::string> splitted_line = split_line(line, std::regex{R"([\s]+)"});
				
				if(searched_targets.size() >= 2){
					for(const auto& splitted : splitted_line){

						//command vergleich
						if(splitted == searched_targets[1]){
							std::cout << splitted << " bereits vorhanden" << std::endl;
							return true;
						}
					}
				}
				return true;
			}
		}
	}
	return false;
}

#endif // __linux__


// -------- /home/eichi/Dev/Projekte/std/src/code/cmd_ctrl.h ---------
#ifndef CMD_CTRL_H
#define CMD_CTRL_H

#include <regex>
#include <vector>
#include <map>
#include <string>

class Cmd_Ctrl : public CTRL::Ctrl{
public:
	void show_log(bool set_to) override {
		show_ctrl_log = set_to;
	}
	
	std::string get_log() const override{
		if(show_ctrl_log){
			return ctrl_log;
		}
		return {};
	}
	
	// Gibt eine Liste von Tokens zurück, getrennt durch Leerzeichen
	std::vector<std::string> split_input(const std::string& input);
	//Check for valid Arguments
	bool is_argument_valid(const std::vector<std::string>& str_argv);
	
	
	template <typename T>
	std::vector<std::string> parse_argv(int argc, T& argv){
		std::vector<std::string> str_argv;
		for(int i{0}; i < argc; ++i){
		
				std::string arg = argv[i];

				//Crontab Command nicht parsen bei automatischer abfrage
				if( i + 1 <= argc
					&& std::regex_match(arg, regex_pattern.at(command::automatic)))
				{
					str_argv.push_back(arg);
					
					std::string crontab_command = argv[i + 1];
					str_argv.push_back(crontab_command);
					++i;
					continue;
				}
				
				auto it = split_input(arg);
				if(!it.empty()){
					for(const auto& split : it){
						str_argv.push_back(split);
					}
				}
			}
			
			argv = {};
			
			//Log Output
			ctrl_log.append(std::to_string(argc));
			ctrl_log.append("\n");
			for(const auto& str : str_argv){
				ctrl_log.append(str);
				ctrl_log.append("\n");
			}
		return str_argv;
	}
	
};

#endif // CMD_CTRL_H


// -------- /home/eichi/Dev/Projekte/std/src/code/cmd_ctrl.cpp ---------

// Gibt eine Liste von Tokens zurück, getrennt durch Leerzeichen
std::vector<std::string> Cmd_Ctrl::split_input(const std::string& input) {
	std::vector<std::string> result;
	std::regex re(R"([\s]+)"); // regex: trennt an Leerzeichen
	
	std::sregex_token_iterator it(input.begin(), input.end(), re, -1);
	std::sregex_token_iterator end;

	for (; it != end; ++it) {
		if (!it->str().empty()) {
			result.push_back(it->str());
		}
	}

	return result;
}

//Check for valid Arguments
bool Cmd_Ctrl::is_argument_valid(const std::vector<std::string>& str_argv){

	//commands regex_pattern
	for(const auto& pattern : regex_pattern){
		for(size_t i{0}; i < str_argv.size(); ++i){
			if(std::regex_match(str_argv[i], pattern.second)){
				
				return  true;
			}	
		}
	}
	
	//device_regex_pattern
	for(const auto& pattern : device_regex_pattern){
		for(size_t i{0}; i < str_argv.size(); ++i){
			if(std::regex_match(str_argv[i], pattern.second)){

				return  true;
			}	
		}
	}
	
	return false;
}


// -------- /home/eichi/Dev/Projekte/std/src/code/arg_manager.h ---------
#ifndef ARG_MANAGER_H
#define ARG_MANAGER_H

#include <vector>
#include <memory>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>


#ifdef __linux__
#endif

enum class OutputType{
	show_all_accounts = 0
	, show_help
	, show_filepaths
	, show_language
	, show_specific_table
	, show_alias_table
	, show_alias_automation
	, show_all_log
	, show_user_output_log
	, COUNT //maxSize Bitset
};


using OutputBitset = std::bitset<static_cast<size_t>(OutputType::COUNT)>;

class Command{
public:
	
	virtual ~Command() = default;
	
	virtual void execute() = 0;
	
	virtual std::string get_log() const = 0;
	virtual std::string get_user_log() const = 0;
	
protected:
	std::stringstream cmd_log;
	std::stringstream user_output_log;
};


class Alias_Command : public Command{
public:
	Alias_Command(
		const std::vector<std::string>& str_argv
		, std::vector<Time_Account>& all_accounts
	
	) : str_argv(str_argv)
		, all_accounts(all_accounts)
	
	{
		//get Account from Alias
		std::any_of( all_accounts.begin(), all_accounts.end(),
			[this](const Time_Account& acc){
				if(this->str_argv[1] == acc.get_alias()){
					account = std::make_shared<Time_Account>(acc);
					return true;
				}
			return false;
		});
		
	};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override = 0;
	
	std::shared_ptr<Time_Account> get_account(){
		return account;
	}
	
protected:
	//Alias Account
	std::shared_ptr<Time_Account> account;
	std::vector<Time_Account>& all_accounts;
	std::vector<std::string> str_argv;	
};

class Add_Alias_Command : public Alias_Command{
public:
	Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
		
	) : Alias_Command(std::move(str_argv), all_accounts)
		, jsonH(jsonH)
		, str_error(str_error)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override = 0;
	
	//Save to file
	void finalize() {
		if(account){
			std::any_of(
				all_accounts.begin(), all_accounts.end(),
				[this](auto& acc){
					if(acc.get_alias() == account->get_alias()){
						acc = *account;
						return true;
					}
					return false;
				}
			);
			//Save Account Entity File
			jsonH->save_json_entity(all_accounts, account->get_entity());
		}
	}
	
protected:
	std::map<error, std::string> str_error;
	std::shared_ptr<JSON_Handler> jsonH;
};

class Hours_Add_Alias_Command : public Add_Alias_Command{
public:
	
	Hours_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<Translator> translator
		
	) : Add_Alias_Command(all_accounts, str_argv, jsonH, str_error)
		, regex_pattern(regex_pattern)
		, translator(translator)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override {
		return user_output_log.str();
	}
	
	void execute() override {
		
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		if(!std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
			user_output_log << "insert number for time value\n";
			cmd_log << "insert number for time value\n";
			
			throw std::runtime_error{"insert number for time value"};
		}
		
		Clock clock{};
		auto localTime = clock.get_time();
		
		float time_float;
		try{
			time_float = stof(str_argv[2]);
			if (std::regex_match(str_argv[3], regex_pattern.at(command::minutes))){
				time_float /= 60.f;
			}
		}catch(const std::runtime_error& re){
			cmd_log << "error addhours string_to_float "<< re.what() << "\n";
		}
				
		std::stringstream description;
		if(str_argv.size() > 4){
			for(size_t i{4}; i < str_argv.size(); ++i){
				description << str_argv[i] << " ";
			}
		}
				
		Entry entry{time_float, description.str(), localTime};
		account->add_entry(entry);
		
		//Save to file
		finalize();
		
		//Save All_Accounts
		jsonH->save_json_accounts(all_accounts);
		
		cmd_log 
			<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
			<< translator->language_pack.at("time_saved")
			<< std::endl;
			
		user_output_log << cmd_log.str();
    }
    
private:
	std::map<command, std::regex> regex_pattern;
	std::shared_ptr<Translator> translator;
	
}; // AddHours_Alias_Command


class Tag_Add_Alias_Command : public Add_Alias_Command{
public:
	Tag_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
	
	) : Add_Alias_Command(all_accounts, str_argv, jsonH, str_error)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override {
		return user_output_log.str();
	}
	
	void execute() override{
	
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		account->set_tag(str_argv[3]);
		
		//Save to file
		finalize();
		
		user_output_log << "Tag: " << account->get_tag() << " to " << account->get_alias() << " added" << std::endl;
		cmd_log << "Tag: " << account->get_tag() << " to " << account->get_alias() << " added" << std::endl;
		
	};
	
}; // Tag_Add_Alias_Command


class Delete_Alias_Command : public Alias_Command{
public:
	Delete_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
	
	) : Alias_Command(std::move(str_argv), all_accounts)
		, jsonH(jsonH)
		, str_error(str_error)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	}
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		cmd_log << "execute Delete_Command\n";
		if(all_accounts.empty()){
			throw std::runtime_error{str_error.at(error::untitled_error)};
		}
		
		std::string entity;
		std::string alias;
		
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		
		std::vector<Time_Account> adjusted_accounts;
		
		cmd_log << "account_size: " << all_accounts.size() << '\n';
		//remove out of all_accounts
		std::copy_if(all_accounts.begin(), all_accounts.end(),
			std::back_inserter(adjusted_accounts),
			[this](const Time_Account& acc){
				if(acc.get_alias() == this->account->get_alias()){
					cmd_log << this->account->get_alias(); 
					return false;
				}
				
				return true;
			}
		);
		cmd_log << "after_copy account_size: " << adjusted_accounts.size() << '\n';
		
		//update Files
		jsonH->save_json_accounts(adjusted_accounts);
		jsonH->save_json_entity(adjusted_accounts, entity);
		
		all_accounts = adjusted_accounts;
		cmd_log << " deleted\n";//translator.language_pack.at("deleted_out_of_accounts.json") << std::endl;
	}
	
	
private:

	std::shared_ptr<JSON_Handler> jsonH;
	const std::map<error, std::string>& str_error;
	
}; // Delete_Alias_Command


#ifdef __linux__
class SensorData_Add_Alias_Command : public Add_Alias_Command{
public:
	SensorData_Add_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, const std::vector<std::string>& str_argv
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH
		, std::shared_ptr<Translator> translator
		
	) : Add_Alias_Command(all_accounts, str_argv, jsonH, str_error)
		, translator(translator)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override {
		if(!account){
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}
		Clock clock{};
		Device_Ctrl device{str_error.at(error::sensor)};
		
		auto localTime = clock.get_time();
		std::stringstream output_str;

		std::vector<float> output_sensor = device.check_device("BME280");
				
		output_str 
			<< translator->language_pack.at("Temperature") << ": " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
			<< translator->language_pack.at("Pressure") << ": "  << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
			<< translator->language_pack.at("Humidity") << ": "  << std::fixed << std::setprecision(2) << output_sensor[2] << " %\n";
					
		Entry entry{0.f, output_str.str(), localTime};
		account->add_entry(entry);
		
		//Save to file
		finalize();
		
		std::stringstream output;
		output
			<< std::put_time(&localTime, translator->language_pack.at("timepoint").c_str()) << '\n'
			<< translator->language_pack.at("time_saved") << '\n'
			<< output_str.str()
			<< std::setfill('=') << std::setw(10) << "=" << std::setfill(' ') << '\n';

		cmd_log << output.str() << device.get_name() << " Connection closed\n";
		user_output_log << output.str();
	};
	
private:
	std::shared_ptr<Translator> translator;
	
}; // SensorData_Add_Alias_Command


class Activate_Alias_Command : public Alias_Command{
public:
	Activate_Alias_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, const std::map<command, std::regex>& regex_pattern
		, const std::map<error, std::string>& str_error
		, std::shared_ptr<JSON_Handler> jsonH

	) : Alias_Command(std::move(str_argv), all_accounts)
		, regex_pattern(regex_pattern)
		, str_error(str_error)
		, jsonH(jsonH)
		
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		
		if( std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor)) ){
		#ifdef __linux__   
			cmd_log << "measure_sensor\n";
			configure_BME280();
		    
		#else
		    cmd_log << "Only available for Linux" << std::endl;
		    user_output_log << "Only available for Linux" << std::endl;
		    
		#endif // __linux__
		}else{
			cmd_log << str_error.at(error::synthax) << "\n";
			throw std::runtime_error{str_error.at(error::synthax)};
		}	
	};
	
	void configure_BME280(){
		
		cmd_log << "start configuration BME280";
		
		str_argv[2] = "BME280";
		
		Device_Ctrl device{str_error.at(error::sensor)};
		
		if(account){
			//an den beginn von str_argv die entity speichern -> für automation_config file
			this->str_argv[0] = account->get_entity();
		}else{
			throw std::runtime_error{ str_error.at(error::unknown_alias) };
		}
		    
		std::vector<command> commands = {
			command::logfile
			, command::minutes
			, command::hours
			, command::clock
			, command::day
		};
		
		//SET Device Settings
		std::stringstream ss;
		ss << device.set_user_automation_crontab(
		    str_argv
		    , jsonH
		    , device.get_specific_regex_pattern(commands)
		);
		
		user_output_log << ss.str();
		cmd_log << ss.str() << "configuration end";
	}
private:

	const std::map<command, std::regex>& regex_pattern;
	const std::map<error, std::string>& str_error;
	std::shared_ptr<JSON_Handler> jsonH;
	
}; // Activate_Alias_Command


class TouchDevice_Command : public Command{
public:
	TouchDevice_Command(
		const std::map<error, std::string>& str_error
		, const std::string& device_name
		
		
	) : str_error(str_error)
		, arg(device_name)
		
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	}
	
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override {
		
		Device_Ctrl device{str_error.at(error::sensor)};
		
		auto it = device.device_regex_pattern.find(arg);
		
		if(it == device.device_regex_pattern.end()){
		    std::stringstream ss;
		    ss << "\nPossible Connections:\n";
			for(const auto& name : device.device_regex_pattern){
				ss << " > " << name.first << '\n';
			}
			cmd_log << str_error.at(error::synthax) << ss.str() << '\n';
		    throw std::runtime_error{str_error.at(error::synthax) + ss.str()};
		}
				    
		if(std::regex_match(arg, device.device_regex_pattern.at(arg)) ){

		    
		    std::vector<float> output_sensor = device.check_device(arg);
			if(output_sensor.empty()){
				throw std::runtime_error{"TouchDevice_Command " + str_error.at(error::sensor)};
			}
			
		    std::stringstream output_str; 
			output_str 
				<< "Temperature"<< ": " << std::fixed 	<< std::setprecision(2) << output_sensor[0] << " °C || "
				<< "Pressure" 	<< ": "  << std::fixed 	<< std::setprecision(2) << output_sensor[1] << " hPa || "
				<< "Humidity" 	<< ": "  << std::fixed 	<< std::setprecision(2) << output_sensor[2] << " %\n";
					        
			cmd_log << "Device touched\n" << output_str.str() << std::endl;
			user_output_log 
			<< ( output_sensor.empty() ? "Device touched\n" : output_str.str() )
			<< std::endl;	    
		}
		
		cmd_log << "\n===== Command_Log: =====\n" << device.get_log();
	};
	
private:
	std::map<error, std::string> str_error;
	std::string arg;
	
}; // TouchDevice_Command
#endif // __linux__


class Show_Command : public Command{
public:
	
	Show_Command(
		std::vector<Time_Account>& all_accounts
		, std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		, int argc
		, const std::map<command, std::regex>& regex_pattern
		, OutputBitset& output_flags
		, const std::map<error, std::string>& str_error
		
	) : str_argv(str_argv)
		, all_accounts(all_accounts)
		, jsonH(jsonH)
		, argc(argc)
		, regex_pattern(regex_pattern)
		, output_flags(output_flags)
		, str_error(str_error)	
	{};
	
	std::string get_log() const override{
		return cmd_log.str();
	}
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		cmd_log << "execute Show_Command\n";
		
		switch(argc){
			case 2:
				{
					with_2_args();
					break;
                }
			case 3:
				{
					with_3_args();
					break;
				}
			case 4:
				{
					with_4_args();
					break;
				}
			default:
				{
					cmd_log << "nothing activated to be shown\n";
					break;
				}
		};
	};
	

private:
	
	std::vector<Time_Account>& all_accounts;
	std::vector<std::string> str_argv;
	std::shared_ptr<JSON_Handler> jsonH;
	int argc;
	std::map<command, std::regex> regex_pattern;
	OutputBitset& output_flags;
	std::map<error, std::string> str_error;
	
	void with_2_args(){
		
		//Zeige alle <entity> und <alias> an
        //show
        if (std::regex_match(str_argv[1], regex_pattern.at(command::show))) {
			output_flags.set(static_cast<size_t>(OutputType::show_all_accounts));
		    cmd_log << "show all accounts\n";
            return;
        }
        throw std::runtime_error{str_error.at(error::synthax)};
    }

	void with_3_args(){
		//Zeige Filepaths, Language Setting, Alias Tabelle an
        //show filepath
        if(std::regex_match(str_argv[2], regex_pattern.at(command::config_filepath))){
                    
            output_flags.set(static_cast<size_t>(OutputType::show_filepaths));
            cmd_log << str_argv[2] << " show config_filepath.json\n";
			return;
        }
        //language anzeigen
        if(std::regex_match(str_argv[2], regex_pattern.at(command::language))){
                                       
			output_flags.set(static_cast<size_t>(OutputType::show_language));
			cmd_log << str_argv[2] << " show language_setting\n";
            return;    
        }
        //Zeige spezifischen Account an
        //show <alias>
        if(find_alias()){
			output_flags.set(static_cast<size_t>(OutputType::show_alias_table));
			
			cmd_log << str_argv[2] << " show alias table\n";
			return;
		}else{
			throw std::runtime_error{str_error.at(error::unknown_alias)};
		}	
	}
	
	
	void with_4_args(){
		//show automation für alias
		//.std sh <alias> -activate
        if( std::regex_match(str_argv[3], regex_pattern.at(command::activate)) 
			&& find_alias() ) 
        {  
			
			output_flags.set(static_cast<size_t>(OutputType::show_alias_automation));
			cmd_log << "show automation from alias table\n";
			return;
			
		}
		throw std::runtime_error{str_error.at(error::synthax)};
	}
	
	bool find_alias(){
		std::string alias = str_argv[2];
        auto it = std::find_if(
			all_accounts.begin(), all_accounts.end(),
			[&alias](const Time_Account& acc){
				return acc.get_alias() == alias;
			}
        );
        if(it != all_accounts.end()){
			return true;
		}
		throw std::runtime_error{str_error.at(error::unknown_alias)};
	}
		
}; // Show_Command



struct Add_account{
	std::string entity;
	std::string alias;
	std::string tag;
	
};

class Add_Alias : public Command{
public:
	Add_Alias(
		std::vector<Time_Account>& all_accounts
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
		, const Add_account& add
		, const std::map<command, std::regex>& pattern
	
	) : all_accounts(all_accounts),
		jsonH(jsonH),
		str_error(str_error),
		add(add),
		regex_pattern(pattern) 
	{
		cmd_log << "Add_Command\n";
	};
	std::string get_log() const override{
		return cmd_log.str();
	}
	
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		cmd_log << "execute Add_Command\n";
		std::stringstream ss;
		//Entity or Alias cant be named as a command
		bool is_entity = std::any_of(
			regex_pattern.begin(), regex_pattern.end(),
			[this](const auto& pair){
				return std::regex_match(this->add.entity, pair.second);
			}
		);
		bool is_alias = std::any_of(
			regex_pattern.begin(), regex_pattern.end(),
			[this](const auto& pair){
				return std::regex_match(this->add.alias, pair.second);
			}
		);
		if(is_entity || is_alias){
			throw std::runtime_error{str_error.at(error::user_input_is_command)};
		}	
		
		//Entity or Alias already taken?
		for(const auto& account : all_accounts){
			if(account.get_alias() == add.alias){
				throw std::runtime_error{str_error.at(error::double_alias)};
			}
			if(account.get_alias() == add.entity){
				throw std::runtime_error{str_error.at(error::alias_equal_entity)};
			}
		}
		
		Time_Account new_account{add.entity, add.alias};
		
		all_accounts.push_back(new_account);

		jsonH->save_json_accounts(all_accounts);
		
		jsonH->save_json_entity(all_accounts, add.entity);	
		
		ss << add.entity << "-> " << add.alias;
		if(!add.tag.empty()){
			new_account.set_tag(add.tag);
			ss <<  " -tag= " << add.tag;
		}
		ss << " Saved";
		cmd_log << ss.str() << std::endl;	
	};
	
private:
	
	std::vector<Time_Account>& all_accounts;
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<error, std::string> str_error;
	Add_account add;
	const std::map<command, std::regex>& regex_pattern;
	
}; // Add_Alias


class ChangeLanguage_Command : public Command{
public:
	ChangeLanguage_Command(
		std::shared_ptr<Translator> translator
		, const std::string& change_to_language
		, std::shared_ptr<JSON_Handler> jsonH
		, const std::map<error, std::string>& str_error
	
	) : translator(translator)
		, change_to_language(change_to_language)
		, jsonH(jsonH)
		, str_error(str_error)
	{};
	
	std::string get_log() const override {
		return cmd_log.str();
	};
	std::string get_user_log() const override{
		return user_output_log.str();
	}
	
	void execute() override{
		
		bool is_possible_language = std::any_of(
			translator->dict_language.begin(), translator->dict_language.end(),
			[this](const auto& language){
				return language.second == change_to_language;
			}
		);
		
                    
        if(!is_possible_language){
           std::stringstream ss;
           ss << "\nPossible Languages:\n";
			for(const auto& str : translator->dict_language){
				 ss << " > " << str.second << '\n';
			}                             
			throw std::runtime_error{str_error.at(error::unknown_language) + ss.str()};
        }

        change_config_json_language();
	};
	

private:
	std::shared_ptr<Translator> translator;
	std::string change_to_language;
	std::shared_ptr<JSON_Handler> jsonH;
	std::map<error, std::string> str_error;
	
	void change_config_json_language(){
		std::map<std::string, std::string> new_data = {
			  {"config_filepath", jsonH->get_config_filepath()}
			, {"entity_filepath", jsonH->get_entity_filepath()}
			, {"accounts_filepath", jsonH->get_accounts_filepath()}
			, {"language", change_to_language} // <- Change Language
			, {"automation_filepath", jsonH->get_automatic_config_filepath()}
		};
		jsonH->save_config_file(new_data);
	}
	
}; // ChangeLanguage_Command


class Arg_Manager{
public:
	Arg_Manager(std::shared_ptr<JSON_Handler> jH, std::shared_ptr<Cmd_Ctrl> ctrl_ptr);
	
	//Variables
	std::shared_ptr<JSON_Handler> jsonH;
	Translator translator{};
	
	//Methods
	void proceed_inputs(const int& _argc, const std::vector<std::string>& argv);

	bool run_environment() const { return run_env; }
	void set_output_flag(OutputType flag, bool value = true); 
	
	OutputBitset get_output_flags() const;
	void clear_output_flags();
	std::vector<Time_Account> get_all_accounts() const;
	std::vector<std::string> get_str_argv() const;
	std::string get_log() const;
	std::string get_user_output_log() const;
	std::shared_ptr<Time_Account> get_account_with_alias(const std::string& alias);
	
private:
	//log
	std::stringstream arg_manager_log;
	std::stringstream user_output_log;

	//arguments
	int argc;
	std::vector<std::string> str_argv;
	//controle instance
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;
	std::shared_ptr<Cmd_Ctrl> ctrl;
	//All Accounts
	std::vector<Time_Account> all_accounts;
	//cmd to be executed
	std::unique_ptr<Command> cmd;
	//OutputFlags
	OutputBitset output_flags;
	
	
	void change_config_json_file(const std::string& conf_filepath, const std::string& ent_filepath, const std::string& acc_filepath);

	void user_change_filepaths(const std::string& ent_filepath, const std::string& acc_filepath);
	
	bool run_env = false;
	
};//Arg_Manager


#endif // ARG_MANAGER_H


// -------- /home/eichi/Dev/Projekte/std/src/code/arg_manager.cpp ---------

// ============= //
// == PUBLIC ==  //
// ============= //


Arg_Manager::Arg_Manager(std::shared_ptr<JSON_Handler> jH, std::shared_ptr<Cmd_Ctrl> ctrl_ptr): jsonH(jH), ctrl(ctrl_ptr)
{

    jsonH->read_all_accounts(all_accounts);
	
    str_error = ctrl->get_str_error();
    
    translator.set_language(jsonH->get_config_language());

};

void Arg_Manager::proceed_inputs(const int& _argc, const std::vector<std::string>& argv){

    this->str_argv = argv;
    this->argc = _argc;
    this->regex_pattern = ctrl->get_regex_pattern();
    
    //enable show_logfile, erase '-prolog' cmd out of str_argv, argc -=1
    std::vector<std::string> str_argv_without_LOG_cmd;
    std::copy_if(
        str_argv.begin(), str_argv.end(),
        std::back_inserter(str_argv_without_LOG_cmd),
        [this](const std::string& str) {
	    if(!std::regex_match(str, this->regex_pattern.at(command::show_all_log))){
	    
		return true;
	    }
	    this->output_flags.set(static_cast<size_t>(OutputType::show_all_log));
	    this->argc -= 1;
	    ctrl->show_log(true);
	    arg_manager_log << "show log\n";
	    return false;
        }
    );
    str_argv = str_argv_without_LOG_cmd;
    
    //Zeige Hilfe an
    //help
    if(std::regex_match(str_argv[1], regex_pattern.at(command::help))){
	output_flags.set(static_cast<size_t>(OutputType::show_help));
	arg_manager_log << "show help\n";
	return;
    }
    
    //show Möglichkeiten
    if(std::regex_match(str_argv[1], regex_pattern.at(command::show))){
	std::vector<command> commands = {
	    command::config_filepath
	    , command::language
	    , command::help
	    , command::show
	    , command::activate
	};
	    
	arg_manager_log << "show Command\n";
	    
	cmd = std::make_unique<Show_Command>(
	    all_accounts
	    , str_argv
	    , jsonH
	    , argc
	    , ctrl->get_specific_regex_pattern(commands)
	    , output_flags
	    , str_error
	);
	
	if(cmd){
	    arg_manager_log << "show flags set\n";
	}
    }
    
    
    if(!cmd){
    switch(argc){
    
        case 2:
            {
                if(std::regex_match(str_argv[1], regex_pattern.at(command::environment))){
		    run_env = true;
		    arg_manager_log << "start environment\n";
		    break;
                }
                arg_manager_log << "error: syntax wrong\n";
                throw std::runtime_error{str_error.at(error::synthax)};
            }
            
        case 3:
            {	
                
                //Account löschen
                //<alias> -del
                if(std::regex_match(str_argv[2], regex_pattern.at(command::delete_))){
		    
		    arg_manager_log << "Delete Alias";
		    cmd = std::make_unique<Delete_Alias_Command>(
			all_accounts
			, str_argv
			, jsonH
			, str_error
		    );
                    
		    user_output_log << str_argv[1] << " Alias deleted\n";
		    arg_manager_log << str_argv[1] << " Alias deleted\n";
                    break;
                }
                //Language changeTo
                //-l ger
                if(std::regex_match(str_argv[1], regex_pattern.at(command::language))){
		    
		    arg_manager_log << "Change Language\n";
		    cmd = std::make_unique<ChangeLanguage_Command>(
			std::make_shared<Translator>(translator)
			, str_argv[2]
			, jsonH
			, str_error
		    );
		    
		    user_output_log << "Language changed to " << str_argv[2] << '\n';
		    arg_manager_log << "Language changed to " << str_argv[2] << '\n';
		    break;
                }

                //Connection abfrage
		    //-touch BME280
		if(std::regex_match(str_argv[1], regex_pattern.at(command::touch_sensor))){
				
		#ifdef __linux__
		    arg_manager_log << "Touch Device\n";
		    cmd = std::make_unique<TouchDevice_Command>(
			str_error
			, str_argv[2]
		    );
		#else
		    arg_manager_log << "-touch <device> Only available for Linux" << std::endl;
		    throw std::runtime_error{"Only available for Linux"};
		#endif // __linux__
		
		    break;
		}
                                
                //i2c Sensor Messwerte für <alias> speichern
                // <alias> -mes
                if(std::regex_match(str_argv[2], regex_pattern.at(command::measure_sensor))){
                
		#ifdef __linux__
		    cmd = std::make_unique<SensorData_Add_Alias_Command>(
			all_accounts
			, str_argv
			, str_error
			, jsonH
			, std::make_shared<Translator>(translator)
		    );
		#else
			arg_manager_log << "Only available for Linux\n";
			user_output_log << "Only available for Linux\n";
		#endif // __linux__
		
		    break;
                }
		arg_manager_log << "error: Syntax wrong\n";
                throw std::runtime_error{str_error.at(error::synthax)};
            }

        case 4:
            {
    
                //-f <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::user_filepath))){
    /* BUILD UP */            
                    user_change_filepaths(str_argv[2], str_argv[3]);
                    
                    arg_manager_log << str_argv[2] << '\n' << str_argv[3] << std::endl;
                    break;
                }
                //Neuen Account hinzufügen
                //add <entity> <alias> -tag ' '
                if(std::regex_match(str_argv[1], regex_pattern.at(command::add))){
		    
		    arg_manager_log << "add_account_noTag\n";
		    cmd = std::make_unique<Add_Alias>(
			all_accounts
			, jsonH
			, str_error
			, Add_account{str_argv[2], str_argv[3], ""}
			, regex_pattern
		    );
                    
		    user_output_log << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << '\n';
		    arg_manager_log << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << '\n';
                    break;
                }
        
                //Für Alias Stunden h oder Minuten m hinzufügen	OHNE Kommentar	
                //-h -m
                if( std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                   || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)) )
                {
		    
		    if( std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
			
			std::vector<command> commands = {
			    command::hours
			    , command::minutes
			    , command::integer
			};
			
			arg_manager_log << "add hours with comment\n";
			cmd = std::make_unique<Hours_Add_Alias_Command>(
			    all_accounts
			    , str_argv
			    , jsonH
			    , ctrl->get_specific_regex_pattern(commands)
			    , str_error
			    , std::make_shared<Translator>(translator)
			);
			break;
		    }
		    user_output_log << "insert number for time value\n";
		    arg_manager_log << "insert number for time value\n";
		    throw std::runtime_error{"insert number for time value"};
                }

                //tag nachträglich hinzufügen
                //<alias> -tag plant
		if(std::regex_match(str_argv[2], regex_pattern.at(command::tag))) 
               	{

		    arg_manager_log << "add Tag to account\n"; 
		    cmd = std::make_unique<Tag_Add_Alias_Command>(
			all_accounts
			, str_argv
			, jsonH
			, str_error
		    );
		    
		    break;
                }
		
		arg_manager_log << "error: Syntax wrong\n";
                throw std::runtime_error{str_error.at(error::synthax)};
            }
            
        case 5:
            {
                //-cf <configFilepath> <entityFilepath> <accountsFilepath>
                if(std::regex_match(str_argv[1], regex_pattern.at(command::config_filepath))){
    /* BUILD UP */           
                    change_config_json_file(str_argv[2], str_argv[3], str_argv[4]);
                    
                    arg_manager_log << str_argv[2] << '\n' << str_argv[3] << '\n' << str_argv[4] << std::endl;
                    break;
                }
    
                //Für Alias Stunden h oder Minuten m hinzufügen	MIT Kommentar
                //-h -m
                if(std::regex_match(str_argv[3], regex_pattern.at(command::hours))
                    || std::regex_match(str_argv[3], regex_pattern.at(command::minutes)))
                {
		    if( std::regex_match(str_argv[2], regex_pattern.at(command::integer)) ){
			
			std::vector<command> commands = {
			    command::hours
			    , command::minutes
			    , command::integer
			};
			
			arg_manager_log << "add hours without comment\n";
			cmd = std::make_unique<Hours_Add_Alias_Command>(
			    all_accounts
			    , str_argv
			    , jsonH
			    , ctrl->get_specific_regex_pattern(commands)
			    , str_error
			    , std::make_shared<Translator>(translator)
			);
			break;
		    }
		    user_output_log << "insert number for time value\n";
		    arg_manager_log << "insert number for time value\n";
		    throw std::runtime_error{"insert number for time value"};
                }
		
		//Automation konfigurieren
		//<alias> -a -mes "time_config"
		//<alias> -activate -measure
		if( std::regex_match(str_argv[2], regex_pattern.at(command::activate)) ){
		    
		    std::vector<command> commands = {
			command::logfile
			, command::minutes
			, command::hours
			, command::clock
			, command::day
			, command::measure_sensor
		    };
			
		    arg_manager_log << "Activate_Command\n";
		    cmd = std::make_unique<Activate_Alias_Command>(
			all_accounts
			, str_argv
			, ctrl->get_specific_regex_pattern(commands)
			, str_error
			, jsonH
		    );
		    
		    break;
		}
			
		arg_manager_log << "error: Syntax wrong\n";		
                throw std::runtime_error{str_error.at(error::synthax)};
            }

        case 6:
	    {	
		//Neuen Account mit tag hinzufügen
                //std add <entity> <alias> -tag plant
                if(std::regex_match(str_argv[1], regex_pattern.at(command::add))
		    && std::regex_match(str_argv[4], regex_pattern.at(command::tag)) )
               	{
		    arg_manager_log << "add_account_withTag " << all_accounts.size() << '\n';
		    	    
		    cmd = std::make_unique<Add_Alias>(
			all_accounts
			, jsonH
			, str_error
			, Add_account{str_argv[2], str_argv[3], str_argv[5]}
			, regex_pattern
		    );
		    arg_manager_log  << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << " | Tag: " << str_argv[5] << '\n';
		    user_output_log << "Account added\n" << str_argv[2] << " -> " << str_argv[3] << " | Tag: " << str_argv[5] << '\n';
                    break;
                }	
		    arg_manager_log << "error: Syntax wrong\n";
		    throw std::runtime_error{str_error.at(error::synthax)};
	    }

        
        default:
            {	
		arg_manager_log << "error: switchCase default\n";
                throw std::runtime_error{str_error.at(error::untitled_error)};
		
	    }
    };
    }
    if(cmd){
	cmd->execute();
	arg_manager_log
	    << "\n===== Command_Log: =====\n" 
	    << cmd->get_log();
	
	user_output_log << cmd->get_user_log();
	output_flags.set(static_cast<size_t>(OutputType::show_user_output_log));
    }else{
	bool throw_error = true;
	for(int i{0}; i < static_cast<int>(OutputType::COUNT); ++i){
	    if(output_flags.test(i)){
		throw_error = false;
		break;
	    }
	}
	
	if(throw_error){
	    arg_manager_log << "error: unknown_flag\n";
	    throw std::runtime_error{str_error.at(error::unknown)};
	}
    }
}

std::string Arg_Manager::get_log() const {
    std::ostringstream log;
    log 
    << "Arg_Manager:\n"
    << arg_manager_log.str()
    << "\nJSON_Handler:\n"
    << jsonH->get_log()
    << std::endl;
			
    return log.str();
}

std::string Arg_Manager::get_user_output_log() const {
    return user_output_log.str();
}

std::shared_ptr<Time_Account> Arg_Manager::get_account_with_alias(const std::string& alias){
    
    std::shared_ptr<Time_Account> acc;
    std::any_of(
	all_accounts.begin(), all_accounts.end(),
	[&alias, &acc](const Time_Account& account){
	    if(account.get_alias() != alias){
		return false;
	    }
	    acc = std::make_shared<Time_Account>(account);
	    return true;
	}
    );
    if(!acc){
	throw std::runtime_error{str_error.at(error::unknown_alias)};
    }
    return acc;
}

void Arg_Manager::set_output_flag(OutputType flag, bool value){
    output_flags.set(static_cast<size_t>(flag), value);
}

OutputBitset Arg_Manager::get_output_flags() const {
    return output_flags;
}

void Arg_Manager::clear_output_flags(){
    output_flags.reset();
}
	
std::vector<Time_Account> Arg_Manager::get_all_accounts() const {
    return all_accounts;
}
	
std::vector<std::string> Arg_Manager::get_str_argv() const {
    return str_argv;
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

int main(int argc, char* argv[]){
	
	if(argc > 1){
		std::shared_ptr<Cmd_Ctrl> ctrl = std::make_shared<Cmd_Ctrl>();
		try{
			
			//Argumente entgegen nehmen und Parsen
			
			std::vector<std::string> str_argv = ctrl->parse_argv(argc, argv);

			if(ctrl->is_argument_valid(str_argv)){
				
				std::shared_ptr<JSON_Handler> jsonH = std::make_shared<JSON_Handler>();

		#ifdef __linux__
				auto regex_pattern = ctrl->get_regex_pattern();
				
				//Vorher Automation ausführen
				if(	str_argv.size() >= 4 
					&& std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
					&& std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor))
				){
					Device_Ctrl device{ctrl->get_str_error().at(error::unknown)};
					
					std::cout << device.process_automation(jsonH, str_argv[2]) << std::endl;
					
					return 0;
				}

		#endif //__linux__
				
				//Init Argument Manager
				std::shared_ptr<Arg_Manager> arg_man = std::make_shared<Arg_Manager>(jsonH, ctrl);
				
				arg_man->proceed_inputs(argc, str_argv);
				
				std::unique_ptr<CLI_UI> cli = std::make_unique<CLI_UI>(arg_man, ctrl->get_log());

				//Environment
				if(arg_man->run_environment()){
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
							
							new_argv = ctrl->split_input(input);
							new_argv.insert(new_argv.begin(), "std");
							new_argv = ctrl->parse_argv(new_argv.size(), new_argv);
							arg_valid = ctrl->is_argument_valid(new_argv);
							
							for(const auto& arg : new_argv){
								std::cout << arg << '\n';
							}
							
						}while(!arg_valid);
						
						str_argv = new_argv;
						argc = static_cast<int>(str_argv.size());

						try{
				
							arg_man->proceed_inputs(argc, str_argv);
							
						}catch(const std::runtime_error& rt){
							std::cout << "**" << rt.what() << std::endl;
						}
					}while(arg_man->run_environment());
				}

				//CLI UserInterface & ConsoleOutput				
				cli->update();

				
			}else{
			
				throw std::runtime_error{ctrl->get_str_error().at(error::unknown)};
			}
		//Error Output	
		}catch(const std::runtime_error& re){
			std::cerr << "**" << re.what() << "\n" << ctrl->get_log() << std::endl;
		}
		
	}else{
	
		std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
	}
	
	return 0;
}


