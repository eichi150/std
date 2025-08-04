#ifndef DEVICE_PROCESSOR_H
#define DEVICE_PROCESSOR_H

class Processor{
public:
	virtual ~Processor() = default;
	
	virtual void process() = 0;
	
}; // Processor

#ifdef __linux__
//Regulates the automated Device Measuring, without starting the whole Programm
class Device_Processor : public Processor{
public:
	Device_Processor(
		std::map<command, std::regex> pattern
		, const std::vector<std::string>& str_argv
		, std::shared_ptr<JSON_Handler> jsonH
		
	) : regex_pattern(pattern)
		, str_argv(str_argv)
		, jsonH(std::move(jsonH))
	{};
	
	void process() override {
		
		//Vorher Automation ausführen
		if(	str_argv.size() >= 4 
			&& std::regex_match(str_argv[1], regex_pattern.at(command::automatic))
			&& std::regex_match(str_argv[3], regex_pattern.at(command::measure_sensor))
		){
			Device_Ctrl device{};
			std::cout << device.process_automation(jsonH, str_argv[2]) << std::endl;
			throw std::runtime_error{"regular Programm interrupt\n" + std::string{__FILE__} + " - Device_Processor"};
		}
	};

private:
	std::map<command, std::regex> regex_pattern;
	std::vector<std::string> str_argv;
	std::shared_ptr<JSON_Handler> jsonH;
}; // Device_Processor
#endif // __linux__


#endif // DEVICE_PROCESSOR_H
