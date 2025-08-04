#ifndef MASTER_H
#define MASTER_H

#ifdef _WIN32
	#include <windows.h>
#endif // _WIN32

#ifdef __linux__
	#include <limits.h>
	#include <unistd.h>
#endif // __linux

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
#include <cstdlib>

#include "time_account.h"
#include "json.hpp"
#include "json_handler.h"
#include "./control/cmd_ctrl.h"
#include "./control/processor.h"
#include "./control/clock.h"
#include "./interface/translator.h"
#include "./interface/cli_ui.h"
#include "./exception/exception.h"

#include "arg_manager.h"
#include "./control/device_processor.h"


class STD_Master{
public:
	STD_Master(
		const int _argc
		, char* argv[]
	);
	
	void run_std();
	
private:

	void log(const std::string& new_log);

private:
	//logs
	std::shared_ptr<ErrorLogger> logger;
	std::shared_ptr<ErrorLogger> output_logger;
	
	//pointer
	std::shared_ptr<Cmd_Ctrl> ctrl;
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<Arg_Manager> arg_man;
	std::unique_ptr<CLI_UI> cli;
	std::shared_ptr<Processor> proc;
	
	//arguments as strings
	std::vector<std::string> str_argv;
	int argc;
};

/*class STD_Auto{
public:
	STD_Auto(
		const int _argc
		, char* argv[]
	) : argc(_argc)
	{
		logger = std::make_shared<Default_Logger>();
		
		ctrl = std::make_shared<Cmd_Ctrl>(logger);
		jsonH = std::make_shared<JSON_Handler>(logger);
		
		if(argc > 1 && ctrl){
			str_argv = ctrl->parse_argv(argc, argv);
		}
		if(logger){
			log(std::string{__FILE__} + " - STD_Auto");
		}
	};
	
	
	bool close_std() const {
		return close_std;
	}
	
	void run_automation(){
		
		try{
			if(ctrl->is_argument_valid(argc, str_argv)){
				#ifdef __linux__
					//Automated Device measuring before mainApplikation
					proc = std::make_shared<Device_Processor>(
						ctrl->get_regex_pattern()
						, str_argv
						, jsonH
					);
					if(proc){
						proc->process();
						
						close_std = true;
					}
				#endif // __linux__
			}else{
				throw std::invalid_argument{"Invalid Arguments"};
			}	
		//Error Output
		}catch(const Logged_Error& le){
				
			LOG_INFO(le.what());

			ErrorLogger::dump_log_for_Mode(logger, ErrorLogger::Mode::error);
				
		}catch(const SyntaxError& se){
				
			LOG_INFO(se.what());
			
		}catch(const DeviceConnectionError& de){
				
			std::cerr << "**" << de.what() << std::endl;
				
		}catch(const std::runtime_error& re){
				
			std::cerr << "**" << re.what() << std::endl;
				
		}catch(const std::invalid_argument& ia){
				
			std::cerr << "**" << ia.what() << std::endl;
				
		}
	}

private:
	bool close_std = false;
	std::shared_ptr<Cmd_Ctrl> ctrl;
	std::shared_ptr<JSON_Handler> jsonH;
	std::shared_ptr<ErrorLogger> logger;
	std::shared_ptr<Processor> proc;
};*/

#endif // MASTER_H
