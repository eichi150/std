#ifndef DEVICE_PROCESSOR_H
#define DEVICE_PROCESSOR_H

#ifdef __linux__

#include <regex>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <cstdlib>
#include "ctrl.h"
#include "processor.h"
#include "device_ctrl.h"
#include "../json_handler.h"

//Regulates the automated Device Measuring, without starting the whole Programm
class Device_Processor : public Processor{
public:
	Device_Processor(
		std::map<command, std::regex> _regex_pattern
		, const std::vector<std::string>& _str_argv
		, std::shared_ptr<JSON_Handler> _jsonH
	);
	
	void process() override;

private:
	std::map<command, std::regex> regex_pattern;
	std::vector<std::string> str_argv;
	std::shared_ptr<JSON_Handler> jsonH;
}; // Device_Processor
#endif // __linux__


#endif // DEVICE_PROCESSOR_H
