#ifndef MY_REPLXX_H
#define MY_REPLXX_H

#include "extern/replxx/include/replxx.hxx"
#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <map>
#include <regex>
#include <memory>
#include "../control/cmd_ctrl.h"

class myReplxx : public replxx::Replxx{
public:
    enum class Tab_Cmd{
        command = 0
        , alias
        , entity
    };
public:
    myReplxx(std::shared_ptr<Cmd_Ctrl> _ctrl) : ctrl(_ctrl){
        //show tab_completions by pressing tab once
        this->set_max_hint_rows(0);
        tab_completions[Tab_Cmd::command] = {
            "exit"
            , "-help"
            , "-add"
            , "-sh", "-show"
            , "-del", "-delete"
            , "-me", "-measure"
            , "-a", "-activate"
            , "-dea", "-deactivate"
            , "-touch BME280"
        };
        //zulässige Eingabe RegexPattern
        if(ctrl){
            regex_pattern = ctrl->get_regex_pattern();
        }
    }

    bool run_replxx(std::vector<std::string>& input_buffer){

        setup_autocompletion();

        return user_input(input_buffer);
    }

    void set_tab_completion(const std::vector<std::string>& alias_strings, const std::vector<std::string>& entity_strings){
        tab_completions[Tab_Cmd::alias] = alias_strings;
        tab_completions[Tab_Cmd::entity] = entity_strings;
    }
private:

    bool user_input(std::vector<std::string>& input_buffer){

        input_buffer.clear();
        while (true) {
            
            char const* _input = this->input(cmd_line);
            if (!_input) {
                break;  // z. B. Ctrl-D
            }

            std::string input_str(_input);

            if (input_str == "exit" || input_str == "quit") {
                return false;
            }
            if(input_str.empty()){
                continue;
            }
            if (!input_str.empty()) {
                
                parse_input_to_tokens(input_str, input_buffer);

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

    bool is_command_complete(const std::vector<std::string>& tokens) {
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

    void parse_input_to_tokens(const std::string& input_, std::vector<std::string>& tokens){
        std::stringstream ss(input_);
        std::string _cmd;
        tokens.push_back("std");
        while(ss >> _cmd){
            tokens.push_back(_cmd); 
        }
    }

    //collect possible completions with Tab_Cmd
    void select_completion(
            const std::vector<Tab_Cmd>& _cmd_vec
            , const std::string& input_
            , const std::string& token
            , replxx::Replxx::completions_t& result
    ){
        std::vector<std::string> _name_vec;
        for(const auto& _cmd : _cmd_vec){
            auto it = tab_completions.find(_cmd);
            if(it == tab_completions.end()){
                continue;
            }
            for(const auto& vec : tab_completions.at(_cmd)){
                 _name_vec.push_back(vec);
            }
        }
        for(const auto& _name : _name_vec ){
            if(_name.find(token) == 0){
                result.emplace_back(input_.substr(0, size_tab_cmd) + " " + _name);
                
            }
        }
    }

    void setup_autocompletion(){
        // Autovervollständigung vorbereiten
        set_completion_callback(
            [this](std::string const& input, int& contextLen) -> replxx::Replxx::completions_t {
                replxx::Replxx::completions_t result;
                contextLen = input.size();

                std::vector<std::string> last_token;
                {
                    std::istringstream iss(input);
                    std::string token;
                    while(iss >> token){
                        last_token.push_back(token);
                    }
                }
                //commands
                if(input.substr(0, 1) == "-"){
                    for (auto const& c : tab_completions.at(Tab_Cmd::command)) {
                        if (c.find(input) == 0) {
                            result.emplace_back(c);
                        }
                    }
                }else{
                    //alias
                    for (auto const& c : tab_completions.at(Tab_Cmd::alias)) {
                        if (c.find(input) == 0) {
                            result.emplace_back(c);
                        }
                    }
                }
                
                //second word support
                for(const auto& tab_cmd : tab_completions.at(Tab_Cmd::command)){
                    size_tab_cmd = static_cast<int>(tab_cmd.size());
                    //input = tab_cmd
                    if(input.substr(0, size_tab_cmd) == tab_cmd && last_token.size() > 1){
                        //map
                        if(std::regex_match(tab_cmd, regex_pattern.at(command::show))){
                            select_completion({Tab_Cmd::alias, Tab_Cmd::entity}, input, last_token[1], result);
                           
                        }else if(std::regex_match(tab_cmd, regex_pattern.at(command::add))){
                            select_completion({Tab_Cmd::entity}, input, last_token[1], result);
                            
                        }else if(std::regex_match(tab_cmd, regex_pattern.at(command::delete_))){
                            select_completion({Tab_Cmd::alias, Tab_Cmd::entity}, input, last_token[1], result);
                        }
                        break;
                    }
                }
                return result;
            }
        );
    }

private:
    std::shared_ptr<Cmd_Ctrl> ctrl;
    std::map<command, std::regex> regex_pattern;

    int size_tab_cmd;
    std::map<Tab_Cmd, std::vector<std::string>> tab_completions;

    std::string cmd_line = "@std >> ";
};

#endif // MY_REPLEX_H