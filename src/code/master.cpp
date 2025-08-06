#include "master.h"

STD_Master::STD_Master(
	const int _argc
	, char* argv[]

	) : argc(_argc)
	{
		logger = std::make_shared<Default_Logger>();
		if(logger){
			log(std::string{__FILE__} + " - STD_Master");
		}

		ctrl = std::make_shared<Cmd_Ctrl>(logger);
		jsonH = std::make_shared<JSON_Handler>(logger);
		
		//Parse Input Args-char* -> str_argv-std::string
		str_argv = ctrl->parse_argv(argc, argv);
	};

void STD_Master::log(const std::string& new_log) {
	logger->log(new_log);
};

void STD_Master::run_std(){
	if(argc == 1){
		std::cout << "Simple Time Documentation - github.com/eichi150/std" << std::endl;
		return;
	}
	
	if (!logger) throw std::runtime_error{"Logger initialization failed"};
	if (!ctrl) throw std::runtime_error{"Cmd_Ctrl initialization failed"};
	if (!jsonH) throw std::runtime_error{"JSON_Handler initialization failed"};

	try{
		if(ctrl->is_argument_valid(argc, str_argv)){
#ifdef __linux__
			//measure automation linux only
			linux_only();
#endif // __linux__
		
			output_logger = std::make_shared<User_Logger>();

			auto environment_pattern = ctrl->get_specific_regex_pattern({command::environment});
			if( std::regex_match(str_argv[1], environment_pattern.at(command::environment)) ){

				run_env = true;
				manager = std::make_shared<Env_Manager>(
					logger
					, output_logger
					, jsonH
					, ctrl
					, argc
					, str_argv
				);
			}else{
				
				//Init Argument Manager
				manager = std::make_shared<Arg_Manager>(
					logger
					, output_logger
					, jsonH
					, ctrl
					, argc
					, str_argv
				);
			}
			//CLI UserInterface & ConsoleOutput				
			cli = std::make_unique<CLI_UI>(
				output_logger
				, manager
				, jsonH
				, run_env
				, ctrl
			);
			log("run CLI");
			do{	
				if(manager){
					manager->manage();
				}
				if(cli){
					cli->update();
				}
				//Debug Output
				logger->dump_log_for_Mode(logger, ErrorLogger::Mode::debug);
				
			}while(cli->is_env_running());

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

#ifdef __linux__
void STD_Master::linux_only(){

	auto automatic_pattern = ctrl->get_specific_regex_pattern({command::automatic});
	if( std::regex_match(str_argv[1], automatic_pattern.at(command::automatic)) ){
		//Automated Device measuring before mainApplikation
		proc = std::make_shared<Device_Processor>(
			ctrl->get_regex_pattern()
			, str_argv
			, jsonH
		);
		if(proc){
			proc->process();
		}
	}
}
#endif // __linux__