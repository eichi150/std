#ifdef __linux__
#include "device_processor.h"

Device_Processor::Device_Processor(
	std::map<command, std::regex> _regex_pattern
	, const std::vector<std::string>& _str_argv
	, std::shared_ptr<JSON_Handler> _jsonH
	
	) : regex_pattern(_regex_pattern)
	, str_argv(_str_argv)
	, jsonH(_jsonH)
{};

void Device_Processor::process() {
	
	//Vorher Automation ausführen
	if(	str_argv.size() >= 4 
		&& regex_pattern.count(command::automatic) > 0
		&& regex_pattern.count(command::measure_sensor) > 0
		&& std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
		&& std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor))
	){
		Device_Ctrl device{};
		std::string output = device.process_automation(jsonH, str_argv[2]);
		throw std::runtime_error{"regular Programm interrupt\n" + std::string{__FILE__} + " - Device_Processor\n" + output};
	}
};
#endif // __linux__
