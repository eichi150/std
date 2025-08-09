#ifndef CTRL_H
#define CTRL_H

#include <vector>
#include <map>
#include <regex>
#include <sstream>

#include "../json_handler.h"
#include "../time_account.h"
#include "../control/clock.h"
#include "../bme280/bme280_sensor.h"

enum class command{
	  help = 0
	, add
	, delete_
	, show
	, hours
	, minutes
	, day
	, logfile
	, clock
	, config_filepath
	, user_filepath
	, language
	, tag
	, touch_sensor
	, measure_sensor
	, activate
	, deactivate
	, i2c
	, automatic
	, environment
	, debug
	, integer
	, all
	, detail
	, exit
};

namespace CTRL{
	
class Ctrl{
public:
	Ctrl(){
		//zulässige Eingabe RegexPattern
		regex_pattern = {
			  { command::help,      		std::regex{R"(^-?help$)", std::regex_constants::icase } }
			, { command::add,       		std::regex{R"(^-?add$)", std::regex_constants::icase } }
			, { command::show,      		std::regex{R"(^(-?show|--?sh?|sh)$)", std::regex_constants::icase }}
			, { command::delete_,   		std::regex{R"(^(-?del(ete)?|delete)$)", std::regex_constants::icase } }
			, { command::hours, 			std::regex{R"(^(-?h(ours)?|hours)$)", std::regex_constants::icase } }
			, { command::minutes, 			std::regex{R"(^(-?m(inutes)?|minutes)$)", std::regex_constants::icase } }
			, { command::day, 				std::regex{R"(^-?day?|day$)", std::regex_constants::icase} }
			, { command::logfile, 			std::regex{R"(^-?log?|log$)", std::regex_constants::icase} }
			, { command::clock, 			std::regex{R"(^-?clock?|clock$)", std::regex_constants::icase} }
			, { command::config_filepath, 	std::regex{R"(^-?cf$)", std::regex_constants::icase } }
			, { command::user_filepath,  	std::regex{R"(^(-?f(ilepath)?|filepath)$)", std::regex_constants::icase } }
			, { command::language,  		std::regex{R"(^(-?l(anguage)?|language)$)", std::regex_constants::icase } }
			, { command::tag,				std::regex{R"(^-?tag?|tag$)", std::regex_constants::icase } }
			, { command::touch_sensor, 		std::regex{R"(^-?touch?|touch$)", std::regex_constants::icase } }
			, { command::measure_sensor,	std::regex{R"(^(-?me(asure)?|measure)$)", std::regex_constants::icase } }
			, { command::activate,			std::regex{R"(^(-?a(ctivate)?|activate)$)", std::regex_constants::icase } }
			, { command::deactivate,		std::regex{R"(^(-?dea(ctivate)?|deactivate)$)", std::regex_constants::icase } }
			, { command::i2c, 				std::regex{R"(^-?i2c$)", std::regex_constants::icase } }
			, { command::automatic, 		std::regex{R"(^-?auto$)", std::regex_constants::icase } }
			, { command::environment, 		std::regex{R"(^-?env$)", std::regex_constants::icase } }
			, { command::debug, 			std::regex{R"(^-?debug$)", std::regex_constants::icase} }
			, { command::integer, 			std::regex{R"(^\d+$)"} }
			, { command::all, 				std::regex{R"(^-?all|all$)", std::regex_constants::icase } }
			, { command::detail, 			std::regex{R"(^-?detail?|detail$)", std::regex_constants::icase } }
			, { command::exit, 				std::regex{R"(^-?exit?|exit$)", std::regex_constants::icase } }
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
	
	std::regex create_regex_outOf_string(const std::string& str){
		// Escape special regex characters
		std::string escaped = std::regex_replace(str, std::regex("[.^$|()\\[\\]{}*+?\\\\]"), "\\$&");
		std::string pat = "\\b(" + escaped + "$)\\b";
		return std::regex{pat, std::regex_constants::icase};
	}
	
	//split string an regex_pattern
	std::vector<std::string> split_line(const std::string& line, const std::regex& re){
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
	
	
	std::map<std::string, std::shared_ptr<Sensor>> all_devices;
	std::map<std::string, std::regex> device_regex_pattern;
	
protected:
	std::map<command, std::regex> regex_pattern;
	
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

};//Ctrl
}; //namespace CTRL
#endif // CTRL_H
