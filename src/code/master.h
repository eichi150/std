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

#endif // MASTER_H
