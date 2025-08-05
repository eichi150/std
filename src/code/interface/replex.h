#ifndef MY_REPLXX_H
#define MY_REPLXX_H

#include "extern/replxx/include/replxx.hxx"
#include <iostream>
#include <vector>

class myReplxx : public replxx::Replxx{
public:
    
    bool user_input(std::vector<std::string>& str_vec){
       
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

        str_vec.clear();
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
                std::cout << "Du hast eingegeben: " << input_str << "\n";

                std::stringstream ss(input_str);
                std::string _cmd;
                str_vec.push_back("std");
                while(ss >> _cmd){
                   str_vec.push_back(_cmd); 
                }
                /*for(const auto& str : str_vec){
                    std::cout << "i[ " << str << " ] ";
                }*/
                return true;
            }
        }
        return false;
    }

private:
    //replxx::Replxx rx;
    //std::vector<std::string> str_vec{};
    std::vector<std::string> completions = {"help", "hello", "halt", "history"};
    std::string cmd_line = "@std >> ";
};

#endif // MY_REPLEX_H