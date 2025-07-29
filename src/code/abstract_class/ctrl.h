#ifndef CTRL_H
#define CTRL_H

#include <vector>
#include <map>
#include <regex>

#include "../json_handler.h"
#include "../time_account.h"
#include "../clock.h"
#include "../bme280/bme280_sensor.h"

class Ctrl{
public:
	Ctrl(){
		//zulässige Eingabe RegexPattern
		regex_pattern = {
			  { command::help,      		std::regex{R"(^--?help$)", std::regex_constants::icase } }
			, { command::add,       		std::regex{R"(^--?add$)", std::regex_constants::icase } }
			, { command::show,      		std::regex{R"(^(--?sh(ow)?|sh|show)$)", std::regex_constants::icase } }
			, { command::delete_,   		std::regex{R"(^(--?del(ete)?)$)", std::regex_constants::icase } }
			, { command::hours, 			std::regex{R"(^(--?h(ours)?)$)", std::regex_constants::icase } }
			, { command::minutes, 			std::regex{R"(^(--?m(inutes)?)$)", std::regex_constants::icase } }
			, { command::day, 				std::regex{R"(^--?day$)", std::regex_constants::icase} }
			, { command::logfile, 			std::regex{R"(^--?log$)", std::regex_constants::icase} }
			, { command::clock, 			std::regex{R"(^--?clock$)", std::regex_constants::icase} }
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
			, { command::process_log, 		std::regex{R"(^--?prolog$)", std::regex_constants::icase} }
		};
		//Error Outputs
		str_error = {
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
		
		//Alle externen Geräte
		all_devices = {
			{"BME280", std::make_shared<BME_Sensor>()}
		};
		//regex_pattern für Device_name generieren
		for(const auto& [name, _] : all_devices){
			device_regex_pattern[name] = create_regex_outOf_string(name);	
		}
	};
	
	virtual ~Ctrl(){};
		
	virtual std::vector<float> check_device(const std::string& name){
		throw std::runtime_error{"Not implemented"};
	};
	
	std::regex create_regex_outOf_string(const std::string& str){

		std::string pat = "\\b(" + str + "$)\\b";
	
		return std::regex{pat, std::regex_constants::icase};
	}
	
	//split string an regex_pattern
	virtual std::vector<std::string> split_line(const std::string& line, const std::regex& re){
		std::vector<std::string> result;
		std::sregex_token_iterator it(line.begin(), line.end(), re, -1);
		std::sregex_token_iterator end;

		for(; it != end; ++it){
			if(!it->str().empty()){
				result.push_back(it->str());
			}
		}

		return result;
	}
	
	std::map<error, std::string> get_str_error() const { 
		return str_error; 
	}
	std::map<command, std::regex> get_regex_pattern() const {
		return regex_pattern;
	}

	std::map<command, std::regex> get_specific_regex_pattern(const std::vector<command>& command_list) const {
		std::map<command, std::regex> pattern;

		for(const auto& command : command_list){
			pattern[command] = regex_pattern.at(command);
		}
		return pattern;
	};
	
	std::map<std::string, std::regex> device_regex_pattern;
protected:
	std::map<std::string, std::shared_ptr<Sensor>> all_devices;
	
	Clock clock{};
	std::regex integer_pattern{R"(^\d+$)"};
	
	std::map<command, std::regex> regex_pattern;
	std::map<error, std::string> str_error;

	
	template <typename T>
	std::shared_ptr<T> get_sensor_with_name(const std::string& name) {
		auto it = all_devices.find(name);
		if (it != all_devices.end()) {
			// Dynamischer Cast auf den gewünschten Typ T
			auto casted = std::dynamic_pointer_cast<T>(it->second);
			if (!casted) {
				throw std::runtime_error{"Type mismatch for sensor: " + name};
			}
			return casted;
		}
		throw std::runtime_error{"Sensor with name '" + name + "' not found"};
	}
	
	bool is_in_all_devices(const std::string& name){
		auto it = all_devices.find(name);
		if (it != all_devices.end()) {
			return true;
		}
		return false;
	}

};
#endif // CTRL_H
