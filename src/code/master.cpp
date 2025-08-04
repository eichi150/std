#include "master.h"

STD_Master::STD_Master(
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
		log(std::string{__FILE__} + " - STD_Master");
	}
};


void STD_Master::run_std(){
	if(argc == 1){
		std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
		return;
	}
	
	if(!logger || !ctrl || !jsonH){
		throw std::runtime_error{"Pointer initiation failed"};
	}
	
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
				}
			#endif // __linux__
			
			
			log("run CLI");
			output_logger = std::make_shared<User_Logger>();	
			//Init Argument Manager
			arg_man = std::make_shared<Arg_Manager>(
				logger
				, output_logger
				, jsonH
				, ctrl
				, argc
				, str_argv
			);
			arg_man->process();
				

			//CLI UserInterface & ConsoleOutput				
			cli = std::make_unique<CLI_UI>(
				output_logger
				, arg_man
				, jsonH
				, arg_man->get_translator_ptr()
			);
			cli->update();
			
				
			//Debug Output
			logger->dump_log_for_Mode(logger, ErrorLogger::Mode::debug);
			
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

void STD_Master::log(const std::string& new_log) {
	logger->log(new_log);
};
