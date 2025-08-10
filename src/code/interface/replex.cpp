#include "replex.h"

myReplxx::myReplxx(std::map<command, std::regex> _regex_pattern
    , std::map<emojID, std::pair<std::string, std::string>> _emojis

) : regex_pattern(_regex_pattern)
    , emojis(_emojis)

{
        tab_completions[Tab_Cmd::activate_measure] = {
            "measure []"
        };
        
        tab_completions[Tab_Cmd::zero_cmd] = {
            "exit"
            , "help"
            , "add"
            , "show"
            , "delete"
            , "touch BME280"
        };
        tab_completions[Tab_Cmd::alias_one_cmd] = {
            "tag"
            , "measure"
            , "activate"
            , "deactivate"
        };
        tab_completions[Tab_Cmd::show] = {
            "activate"
           , "deactivate"
        };
        tab_completions[Tab_Cmd::command] = {
            "all"
            , "detail"
            , "hours"
            , "minutes"
        };
        tab_completions[Tab_Cmd::hint_callback] = {
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
            , "measure"
        };
        //show tab_completions by pressing tab once
        this->set_double_tab_completion(false);
        setup_highlighter(*this);
    }

void myReplxx::set_tab_completion(//called by Master
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

    setup_hint_callback();
    //update Autocompletion each iteration
    setup_autocompletion();

    bool still_running = user_input(argc_input_buffer.first, argc_input_buffer.second);
    
    return still_running;
}

bool myReplxx::user_input(int& argc, std::vector<std::string>& input_buffer){

    bool wrong_input = false;
    history_add("exit");
    while (true) {
        
        //replxx::Replxx handles input
        char const* _input = this->input(cmd_line);
        if (!_input) {
            break;  // z. B. Ctrl-D
        }

        std::string input_str(_input);
        
        if (std::regex_match(input_str, regex_pattern.at(command::exit))) {
            input_str.clear();
            _input = {};
            return false;
        }
        if(input_str.empty()){
            _input = {};
            continue;
        }
        if (!input_str.empty()) {
           
            //if input wrong erase it out the history. 
            //if user inputs a large comment or something, he could correct the mistakes without rewriting it
            if (wrong_input == true) {
                // 1. hold current history
                std::ostringstream history_stream_out;
                history_save(history_stream_out);

                // 2. clear history
                history_clear();

                // 3. reread history and delete last entry
                std::istringstream history_stream_in(history_stream_out.str());
                std::string entry;
                std::vector<std::string> entries;
                while (std::getline(history_stream_in, entry)) {
                    entries.push_back(entry);
                }
                if (!entries.empty()) {
                    entries.pop_back(); 
                }
                
                // 4. add to history
                for (auto const& e : entries) {
                    if (e.substr(0, 3) != "###") {
                        history_add(e);
                    }
                }
                wrong_input = false;
            }

            //parse into tokens
            bool valid_tokens = parse_input_to_tokens(input_str, input_buffer, argc);

            //add input to the history
            history_add(input_str);

            if(!valid_tokens){
                wrong_input = true;
                std::cout << emojis.at(emojID::warning).first << "  Befehl unvollständig\n";
                input_buffer.clear();
                input_str.clear();
                argc = 0;
                continue;
            }

            if(!is_command_complete(input_buffer)){
                wrong_input = true;
                std::cout << emojis.at(emojID::warning).first << "  Befehl unvollständig\n";
                input_buffer.clear();
                input_str.clear();
                argc = 0;
                continue;
            }
            
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

    if(!matches){
        //input isnt command, check for alias
        bool alias_matches = std::any_of(
            tokens.begin(), tokens.end(),
            [this](const auto& token){
                return std::any_of(
                    tab_completions[Tab_Cmd::alias].begin(), tab_completions[Tab_Cmd::alias].end(),
                    [&token](const auto& alias){
                        return token == alias;
                    } 
                );
            }
        );
        if(!alias_matches || tokens.size() < 3){
            return false;
        }
        if(alias_matches && !bit_parse.test(static_cast<size_t>(Parse::succesfully_parsed))){
            return false;
        }
    }

    return true;
}

bool myReplxx::parse_input_to_tokens(const std::string& input_, std::vector<std::string>& tokens, int& argc){
    std::stringstream ss(input_);
    std::string _cmd;

    tokens.push_back("std");
    argc += 1;

    std::array<char, 4> parse_char_start{'[', '"', '(', '{'};
    std::array<char, 4> parse_char_end{']', '"', ')', '}'};

    size_t start_index = 0;
    bit_parse.reset();
   
    while(ss >> _cmd){
        std::string skip_str;

        if( !bit_parse.test(static_cast<size_t>(Parse::start_parsing)) ){
            for(size_t i{0}; i < parse_char_start.size(); ++i){
                if(_cmd.front() == parse_char_start[i]){
                    start_index = i;
                    bit_parse.set(static_cast<size_t>(Parse::parse));
                    bit_parse.set(static_cast<size_t>(Parse::start_parsing));

                    //erase first character
                    std::string fresh_cmd;
                    for(size_t i{1}; i < _cmd.size(); ++i){
                        fresh_cmd += _cmd[i];
                    }
                    _cmd = fresh_cmd;
                    break;
                }
            }
        }

        if(bit_parse.test(static_cast<size_t>(Parse::parse))){
            //if parse then dont count them as argc. str_argv.size() > argc
            --argc;
        }

        if( !bit_parse.test(static_cast<size_t>(Parse::ended_parsing)) 
            && bit_parse.test(static_cast<size_t>(Parse::parse))
        ){
            
            if(_cmd.back() == parse_char_end[start_index]){
                bit_parse.reset(static_cast<size_t>(Parse::parse));
                bit_parse.set(static_cast<size_t>(Parse::ended_parsing));
            }
            
        }
        if( bit_parse.test(static_cast<size_t>(Parse::ended_parsing)) ){
            skip_str = _cmd;
            skip_str.pop_back();
            _cmd = skip_str;
            ++argc;
        }
        ++argc;
        tokens.push_back(_cmd); 
    }
    if(bit_parse.test(static_cast<size_t>(Parse::start_parsing)) && bit_parse.test(static_cast<size_t>(Parse::ended_parsing))){
        bit_parse.set(static_cast<size_t>(Parse::succesfully_parsed));
        return true;
    }

    if(bit_parse.test(static_cast<size_t>(Parse::start_parsing)) && !bit_parse.test(static_cast<size_t>(Parse::ended_parsing))){
        return false;
    }
    if( !bit_parse.test(static_cast<size_t>(Parse::start_parsing)) && bit_parse.test(static_cast<size_t>(Parse::ended_parsing)) ){
        return false;
    }

    //not every input has to be parsed. so stay true
    return true;
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
            for (const auto& cmd : this->tab_completions.at(Tab_Cmd::hint_callback)) {
                if (cmd.rfind(last, 0) == 0) {  // cmd beginnt mit input
                    result.push_back(cmd);
                    contextLen = static_cast<int>(last.size());
                    color = replxx::Replxx::Color::YELLOW;
                }
            }
            
            if(std::regex_match(last, this->regex_pattern.at(command::activate))){
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
    replxx::Replxx::completions_t& result )
{
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
    set_max_hint_rows(0);

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

            size_t s_tok = tokens.size();
            
            if(s_tok == 0){
                //Command
                for(const auto& cmd : tab_completions.at(Tab_Cmd::zero_cmd)){
                    if(cmd.find(last_word) == 0){
                        result.emplace_back(cmd);
                    }
                }
                for(const auto& alias : tab_completions.at(Tab_Cmd::alias)){
                    result.emplace_back(alias);
                }
                return result;
            }

            // 1. Erste Wort vervollständigen, wenn nur ein Token
            if(s_tok == 1){
                if(std::regex_match(tokens[0], regex_pattern.at(command::show_env))){
                    //Alias
                    for(const auto& alias : tab_completions.at(Tab_Cmd::alias)){
                        if(alias.find(last_word) == 0){
                            result.emplace_back(alias);
                        }
                    }
                    
                    //Entity
                    for(const auto& entity : tab_completions.at(Tab_Cmd::entity)){
                        if(entity.find(last_word) == 0){
                            result.emplace_back(entity);
                        }
                    }

                //if alias is on first position
                }else if (std::find_if(tokens.begin(), tokens.end(),
                    [this](const auto& token) {
                        return std::find(
                            tab_completions.at(Tab_Cmd::alias).begin(),
                            tab_completions.at(Tab_Cmd::alias).end(),
                            token
                        ) != tab_completions.at(Tab_Cmd::alias).end();
                    }) != tokens.end())
                {
                    select_completion({Tab_Cmd::alias_one_cmd}, last_word, result);

                }else{
                    //Command
                    for(const auto& cmd : tab_completions.at(Tab_Cmd::zero_cmd)){
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
            }
            
            if(s_tok == 2){   

                if(std::regex_match(tokens[1], regex_pattern.at(command::deactivate))){

                    select_completion({Tab_Cmd::alias_one_cmd}, last_word, result);
                }

                //show alias or entitys
                if(std::regex_match(tokens[0], regex_pattern.at(command::show_env))){
                    select_completion({Tab_Cmd::alias, Tab_Cmd::entity}, last_word, result);
                    
                }
                
                if(std::regex_match(tokens[0], regex_pattern.at(command::add))){

                    select_completion({Tab_Cmd::entity}, last_word, result);
                }
                
                if(std::regex_match(tokens[0], regex_pattern.at(command::delete_))){

                    select_completion({Tab_Cmd::alias, Tab_Cmd::entity}, last_word, result);
                    
                }else{
                    select_completion({Tab_Cmd::alias_one_cmd}, last_word, result);
                }
            }

            if(s_tok == 3){
                //show alias or entitys
                if(std::regex_match(tokens[0], regex_pattern.at(command::show_env))){
                    select_completion({Tab_Cmd::show}, last_word, result);
                    
                }
                
                if(std::regex_match(tokens[1], regex_pattern.at(command::activate))){
                
                    select_completion({Tab_Cmd::activate_measure}, last_word, result);
                }
            }
                
            if(s_tok == 4){
                select_completion({Tab_Cmd::command}, last_word, result);
            }
            
            return result;
        }
    );
}


void myReplxx::setup_color(
    const std::vector<std::string>& Words
    , const std::string& input
    , replxx::Replxx::colors_t& colors
    , const replxx::Replxx::Color& set_color )
{
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
                "show"
                , "del"
                , "a"
                , "dea"
                //, "h"
                //, "m"
            };
            for(const auto& cmd : tab_completions.at(Tab_Cmd::hint_callback)){
                goodWords.push_back(cmd);
            }
            for(const auto& cmd : tab_completions.at(Tab_Cmd::activate_measure)){
                goodWords.push_back(cmd);
            }
            
            for(const auto& alias : tab_completions.at(Tab_Cmd::alias)){
                goodWords.push_back(alias);
            }

            static std::regex numberRegex{R"(\b\d+\b)"};
            static std::vector<std::string> badWords;/* = {
                "oops", "fail", "error", "foo"
            };*/
            //Highlight Commands Green
            setup_color(goodWords, input, colors, replxx::Replxx::Color::GREEN);
            //Highlight BadWords Red
            setup_color(badWords, input, colors, replxx::Replxx::Color::RED);
            
        }
    );
}
