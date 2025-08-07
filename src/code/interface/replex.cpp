#include "replex.h"

myReplxx::myReplxx(std::shared_ptr<Cmd_Ctrl> _ctrl) : ctrl(_ctrl){

        if(ctrl){//zulässige Eingabe RegexPattern
            regex_pattern = ctrl->get_regex_pattern();
        }
        tab_completions[Tab_Cmd::activate_measure] = {
            "measure []"
        };
        
        tab_completions[Tab_Cmd::command] = {
            "exit"
            , "help"
            , "add"
            , "tag"
            , "show"
            , "delete"
            , "all"
            , "detail"
            , "activate"
            , "deactivate"
            , "touch BME280"
            , "hours"
            , "minutes"
        };
        
        //show tab_completions by pressing tab once
        this->set_double_tab_completion(false);
        setup_highlighter(*this);
    }

void myReplxx::set_tab_completion(//called by CLI_UI
    const std::vector<std::string>& alias_strings
    , const std::vector<std::string>& entity_strings
){
    tab_completions[Tab_Cmd::alias] = alias_strings;
    tab_completions[Tab_Cmd::entity] = entity_strings;
}

bool myReplxx::run_replxx(std::pair<int, std::vector<std::string>>& argc_input_buffer){
    if(regex_pattern.empty()){
        throw std::runtime_error{"RegexPattern fehlen im Replxx"};
    }

    int argc = argc_input_buffer.first;
    std::vector<std::string> input_buffer = argc_input_buffer.second;

    setup_hint_callback();
    //update Autocompletion each iteration
    setup_autocompletion();

    bool still_running = user_input(argc, input_buffer);
    
    argc_input_buffer.first = argc;
    argc_input_buffer.second = input_buffer;
    return still_running;
}

bool myReplxx::user_input(int& argc, std::vector<std::string>& input_buffer){

    input_buffer.clear();
    
    while (true) {
        
        char const* _input = this->input(cmd_line);
        if (!_input) {
            break;  // z. B. Ctrl-D
        }

        std::string input_str(_input);

        if (std::regex_match(input_str, regex_pattern.at(command::exit))) {
            return false;
        }
        if(input_str.empty()){
            continue;
        }
        if (!input_str.empty()) {

            parse_input_to_tokens(input_str, input_buffer, argc);

            if(!is_command_complete(input_buffer)){
                std::cout << "⚠️  Befehl unvollständig\n";
                input_buffer.clear();
                input_str.clear();
                continue;
            }
           
            //add input to the history
            history_add(input_str);
            //add last command to the history
            for(const auto& cmd : tab_completions.at(Tab_Cmd::command)){
                if(std::find(input_buffer.begin(), input_buffer.end(), cmd) != input_buffer.end()){
                    history_add(cmd);
                }
            }

            return true;
        }
    }
    return false;
}

bool myReplxx::is_command_complete(const std::vector<std::string>& tokens) {
    if (tokens.empty()) {
        return false;
    }
    //check tokens with regex_pattern /ctrl.h
    bool matches = std::any_of(
        tokens.begin(), tokens.end(),
        [this](const std::string& token) {
            return std::any_of(
                regex_pattern.begin(), regex_pattern.end(),
                [&token](const auto& pair) {
                    return std::regex_match(token, pair.second);
                }
            );
        }
    );
    if (!matches) {
        return false;
    }

    return true;
}

void myReplxx::parse_input_to_tokens(const std::string& input_, std::vector<std::string>& tokens, int& argc){
    std::stringstream ss(input_);
    std::string _cmd;

    tokens.push_back("std");
    argc += 1;

    while(ss >> _cmd){
        std::string skip_str;
        if(_cmd.front() == '[' || _cmd.front() == '"'){
            skip_str.append(_cmd.substr(1, _cmd.size() -1));
            _cmd = skip_str;
            --argc;
        }
        if(_cmd.back() == ']' || _cmd.back() == '"'){
            skip_str = _cmd;
            skip_str.pop_back();
            _cmd = skip_str;
        }
        
        ++argc;
        tokens.push_back(_cmd); 
    }
}

//give user hints for inputs in grey color
void myReplxx::setup_hint_callback() {
    //without it hints would be shown
    this->set_max_hint_rows(5);
    set_hint_callback(
        [this](const std::string& input, int& contextLen, replxx::Replxx::Color& color) -> replxx::Replxx::hints_t {
            replxx::Replxx::hints_t result;

            //extract last word
            std::istringstream iss(input);
            std::string word, last;
            while(iss >> word){
                last = word;
            }
            for (const auto& cmd : this->tab_completions.at(Tab_Cmd::command)) {
                if (cmd.rfind(last, 0) == 0) {  // cmd beginnt mit input
                    result.push_back(cmd);
                    contextLen = static_cast<int>(last.size());
                    color = replxx::Replxx::Color::YELLOW;
                }
            }
            if(std::regex_match(last, regex_pattern.at(command::activate))){
                for (const auto& cmd : this->tab_completions.at(Tab_Cmd::activate_measure)) {
                    if (cmd.rfind(last, 0) == 0) {  
                        result.push_back(cmd);
                        contextLen = static_cast<int>(last.size());
                        color = replxx::Replxx::Color::YELLOW;
                    }
                }
            }
            
            return result;
        }
    );
}

void myReplxx::select_completion(
    const std::vector<Tab_Cmd>& _cmd_vec,
    const std::string& token,
    replxx::Replxx::completions_t& result
) {
    for(const auto& _cmd : _cmd_vec){
        auto it = tab_completions.find(_cmd);
        if(it == tab_completions.end()){
            continue;
        }
        for(const auto& candidate : it->second){
            if(candidate.find(token) == 0){
                result.emplace_back(candidate);
            }
        }
    }
}

void myReplxx::setup_autocompletion(){
    this->set_max_hint_rows(0);
    set_completion_callback(
        [this](const std::string& input, int& contextLen) -> replxx::Replxx::completions_t {
            replxx::Replxx::completions_t result;

            // Tokenize input
            std::istringstream iss(input);
            std::vector<std::string> tokens;
            std::string word;
            while(iss >> word){
                tokens.push_back(word);
            }

            // get last token and calculate contextLen
            std::string last_word;
            auto last_space_pos = input.find_last_of(' ');
            if (last_space_pos == std::string::npos) {
                last_word = input;
                contextLen = static_cast<int>(input.size());
            } else {
                last_word = input.substr(last_space_pos + 1);
                contextLen = static_cast<int>(last_word.size());
            }

            if(tokens.empty()){
                return result;
            }

            // 1. Erste Wort vervollständigen, wenn nur ein Token
            if(tokens.size() == 1){
                //Command
                for(const auto& cmd : tab_completions.at(Tab_Cmd::command)){
                    if(cmd.find(last_word) == 0){
                        result.emplace_back(cmd);
                    }
                }
                //Alias
                for(const auto& alias : tab_completions.at(Tab_Cmd::alias)){
                    if(alias.find(last_word) == 0){
                        result.emplace_back(alias);
                    }
                }
            }
            //with more than one word
            else if(tokens.size() > 1){
                for(size_t i{0}; i < tokens.size(); ++i){
                    const std::string& cmd = tokens[i];
                    //alias or entitys after command
                    if(std::regex_match(cmd, regex_pattern.at(command::show))){

                        select_completion({Tab_Cmd::alias, Tab_Cmd::entity}, last_word, result);
                    }else
                    
                    if(std::regex_match(cmd, regex_pattern.at(command::add))){

                        select_completion({Tab_Cmd::entity}, last_word, result);
                    }else
                    
                    if(std::regex_match(cmd, regex_pattern.at(command::delete_))){

                        select_completion({Tab_Cmd::alias, Tab_Cmd::entity}, last_word, result);
                    }else
                    
                    if(std::regex_match(cmd, regex_pattern.at(command::activate))){
                        
                        select_completion({Tab_Cmd::activate_measure}, last_word, result);
                    }else{
                        //commands after alias
                        select_completion({Tab_Cmd::command}, last_word, result);
                    }
                }
            }
            return result;
        }
    );
}


void myReplxx::setup_color(
    const std::vector<std::string>& Words
    , const std::string& input
    , replxx::Replxx::colors_t& colors
    , const replxx::Replxx::Color& set_color
){
    //Highlight Commands Green
    for(const auto& word : Words){
        size_t pos = input.find(word);
        if(pos != std::string::npos){
            for(size_t i{0}; i < word.size(); ++i){
                if(pos + i < colors.size()){
                    colors[pos + i] = set_color;
                }
            }
        }
    }
}

void myReplxx::setup_highlighter(replxx::Replxx& repl){
    repl.set_highlighter_callback(
        [this](const std::string& input, replxx::Replxx::colors_t& colors){
            static std::vector<std::string> goodWords = { //shortform Commands
                "sh"
                , "del"
                , "me"
                , "a"
                , "dea"
                //, "h"
                //, "m"
            };
            for(const auto& cmd : tab_completions.at(Tab_Cmd::command)){
                goodWords.push_back(cmd);
            }
            for(const auto& cmd : tab_completions.at(Tab_Cmd::activate_measure)){
                goodWords.push_back(cmd);
            }
            
            for(const auto& alias : tab_completions.at(Tab_Cmd::alias)){
                goodWords.push_back(alias);
            }

            static std::regex numberRegex{R"(\b\d+\b)"};
            static std::vector<std::string> badWords = {
                "oops", "fail", "error", "foo"
            };
            //Highlight Commands Green
            setup_color(goodWords, input, colors, replxx::Replxx::Color::GREEN);
            //Highlight BadWords Red
            setup_color(badWords, input, colors, replxx::Replxx::Color::RED);
            
        }
    );
}
