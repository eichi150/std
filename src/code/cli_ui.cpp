#include "cli_ui.h"

CLI_UI::CLI_UI(std::shared_ptr<Arg_Manager> manager) : arg_man(std::move(manager)){};

void CLI_UI::update(){
	
	const auto flags = arg_man->get_output_flags();
	
	if(flags.test(static_cast<size_t>(OutputType::show_filepaths)) ){
		
		show_filepaths();
	}
	
	if(flags.test(static_cast<size_t>(OutputType::show_language)) ){
		
		show_language();
	}
	
	arg_man->clear_output_flags();
}


void CLI_UI::show_filepaths() {
	std::cout 
        << "Config: " << arg_man->jsonH->get_config_filepath() << '\n'
        << arg_man->translator.language_pack.at("entity") << ": " << arg_man->jsonH->get_entity_filepath() << '\n' 
        << "Accounts: " << arg_man->jsonH->get_accounts_filepath() << std::endl;
}


void CLI_UI::show_language() {
    std::cout 
		<< arg_man->translator.language_pack.at("str_language")
		<< ": " 
		<< arg_man->translator.language_pack.at("language") << std::endl;
}

std::string CLI_UI::create_line(int width){
	std::ostringstream output_stream;
	//Trennlinie 
    output_stream << '\n' << std::setfill('=') << std::setw(width) 
        << "=" << std::setfill(' ') << '\n';
       
     return output_stream.str();
}

std::string CLI_UI::create_table(){
	
	std::ostringstream table;
	
	table 
		<< create_line(40) << '\n'
		
		<< std::endl;
		
	
	return table.str();
}
