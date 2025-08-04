/*#ifndef ARG_PROCESSOR_H
#define ARG_PROCESSOR_H
#include <map>
#include <vector>
#include <memory>
#include <regex>
#include <string>

#include "../json_handler.h"
#include "processor.h"
class Arg_Processor : public Processor{
public:
	Arg_Processor(
		std::map<command, std::regex> _regex_pattern
		, const std::vector<std::string>& _str_argv
		, std::shared_ptr<JSON_Handler> _jsonH
	): regex_pattern(_regex_pattern)
		, str_argv(_str_argv)
		, jsonH(std::move(_jsonH))
	{};
	 
	void process() override = 0;

protected:
	std::map<command, std::regex> regex_pattern;
	std::vector<std::string> str_argv;
	std::shared_ptr<JSON_Handler> jsonH;
};


#endif // ARG_PROCESSOR_H*/
