#ifndef MY_REPLXX_H
#define MY_REPLXX_H

#include "extern/replxx/include/replxx.hxx"
#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <set>
#include <map>
#include <regex>
#include <memory>
#include <stdexcept>
#include <bitset>
#include <algorithm>
#include "../control/cmd_ctrl.h"
#include "emoji.h"



class myReplxx : public replxx::Replxx{
public:
    enum class Parse{
        succesfully_parsed = 0
        , start_parsing
        , parse
        , ended_parsing
    };
    enum class Tab_Cmd{
        command = 0
        , alias
        , entity
        , activate_measure
        , deactivate_measure
        , zero_cmd
        , alias_one_cmd
        , hint_callback
        , show
        , open_brackets
        , default_close_brackets
        , close_brackets
    };
public:
    myReplxx(
        std::map<command, std::regex> _regex_pattern
        , std::map<emojID, std::pair<std::string, std::string>> _emojis
    );

    bool run_replxx(std::pair<int, std::vector<std::string>>& argc_input_buffer);

    void set_tab_completion(const std::vector<std::string>& alias_strings
        , const std::vector<std::string>& entity_strings);
    
private:
    bool user_input(int& argc, std::vector<std::string>& input_buffer);
    bool parse_input_to_tokens(const std::string& input_, std::vector<std::string>& tokens, int& argc);
    bool is_command_complete(const std::vector<std::string>& tokens);
private: //Setup

    void setup_autocompletion();
    //collect possible completions with Tab_Cmd
    void select_completion(
        const std::vector<Tab_Cmd>& _cmd_vec,
        const std::string& token,
        replxx::Replxx::completions_t& result
    );

    static void setup_color(
        const std::vector<std::string>& Words
        , const std::string& input
        , replxx::Replxx::colors_t& colors
        , const replxx::Replxx::Color& set_color);

    void setup_highlighter(replxx::Replxx& repl);
    
    void setup_hint_callback();
private:
    std::bitset<4> bit_parse;
    std::map<command, std::regex> regex_pattern;

    int size_tab_cmd;
    std::map<Tab_Cmd, std::vector<std::string>> tab_completions;
    
    std::map<emojID, std::pair<std::string, std::string>> emojis;
    
    std::string cmd_line = emojis.at(emojID::hourglass).first + "std >> ";
};

#endif // MY_REPLEX_H