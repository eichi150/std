#ifndef MY_REPLXX_H
#define MY_REPLXX_H

#include "extern/replxx/include/replxx.hxx"
#include <iostream>
#include <vector>
#include <sstream>
#include <set>

class myReplxx : public replxx::Replxx{
public:
    
    bool run_replxx(std::vector<std::string>& input_buffer){

        setup_autocompletion();

        return user_input(input_buffer);
    }

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

            if (!input_str.empty()) {
                
                history_add(input_str);  // History speichern
                parse_input_to_tokens(input_str, input_buffer);

                prepare_tab_autoInsertion(input_buffer);

                return true;
            }
        }
        return false;
    }

    //prepare Unique Completions for AutoTab Insertion 
    void prepare_tab_autoInsertion(const std::vector<std::string>& parsed_inputs){
        
        std::set<std::string> unique_completions(completions.begin(), completions.end());
        unique_completions.insert(parsed_inputs.begin(), parsed_inputs.end());
        completions.assign(unique_completions.begin(), unique_completions.end());
    }

    void parse_input_to_tokens(const std::string& input, std::vector<std::string>& tokens){
        std::stringstream ss(input);
        std::string _cmd;
        tokens.push_back("std");
        while(ss >> _cmd){
            tokens.push_back(_cmd); 
        }
    }
    void setup_autocompletion(){
        // Autovervollständigung vorbereiten
        set_completion_callback(
            [this](std::string const& input, int& contextLen) -> replxx::Replxx::completions_t {
                replxx::Replxx::completions_t result;
                contextLen = input.size();
                for (auto const& c : this->completions) {
                    if (c.find(input) == 0) {
                        result.emplace_back(c);
                    }
                }
                return result;
            }
        );
    }

private:
    std::vector<std::string> completions = {"exit", "-help", "-add", "show", "-delete"};
    std::string cmd_line = "@std >> ";
};

#endif // MY_REPLEX_H