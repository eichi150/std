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
#include <stdexcept>
#include "../control/cmd_ctrl.h"

enum class Emoji{
    default_ = 0
    , hourglass
    , pencil
    , tools
    , configuration
    , loading
    , save
    , delete_
    , brain
    , search
    , statistics
    , water
    , plant
    , sun
    , lightbulb
    , stop
    , question_mark
    , exclamation_mark
    , warning
    , info
    , error_
    , leafes
};

class Emoji_Handler{
public:
    
    std::string get_emoji_str(const Emoji& id) const {
        auto it = emojis.find(id);
        if(it != emojis.end()){
            return emojis.at(id).first;
        }
        return std::string{};
    }
    std::string get_emoji_unicode(const Emoji& id) const {
        auto it = emojis.find(id);
        if(it != emojis.end()){
            return emojis.at(id).second;
        }
        return std::string{};
    }

private:
    std::map<Emoji, std::pair<std::string, std::string>> emojis = {
        {Emoji::default_,           {"@", "@"}}
        , {Emoji::hourglass ,       {"⏳", "\\u23F3"}}
        , {Emoji::pencil,           {"✏️", "\\u270F"}}
        , {Emoji::tools,            {"🛠️", "\\u1F6E0"}}
        , {Emoji::configuration,    {"🔧", "\\u1F527"}}
        , {Emoji::loading,          {"🔄", "\\u1F501"}}
        , {Emoji::save,             {"💾", "\\u1F4BE"}}
        , {Emoji::delete_,          {"🗑️", "\\u1F5D1"}}
        , {Emoji::brain,            {"🧠", "\\u1F9E0"}}
        , {Emoji::search,           {"🔍", "\\u1F50D"}}
        , {Emoji::statistics,       {"📊", "\\u1F4CA"}}
        , {Emoji::water,            {"💧", "\\u1F4A7"}}
        , {Emoji::plant,            {"🌱", "\\u1F331"}}
        , {Emoji::sun,              {"🌞", "\\u1F31E"}}
        , {Emoji::lightbulb,        {"💡", "@"}}
        , {Emoji::stop,              {"🛑", "\\u1F6D1"}}
        , {Emoji::question_mark,    {"❓", "\\u2753"}}
        , {Emoji::exclamation_mark, {"❗", "\\u2757"}}
        , {Emoji::warning,          {"⚠️", "\\u26A0"}}
        , {Emoji::info,             {"ℹ️", "\\u2139"}}
        , {Emoji::error_,           {"❌", "\\u274C"}}
        , {Emoji::leafes,           {"🌿", "\\u1F33F"}}
    };
};
class myReplxx : public replxx::Replxx{
public:
    enum class Tab_Cmd{
        command = 0
        , alias
        , entity
        , activate_measure
    };
public:
    myReplxx(std::shared_ptr<Cmd_Ctrl> _ctrl);

    bool run_replxx(std::pair<int, std::vector<std::string>>& argc_input_buffer);

    void set_tab_completion(const std::vector<std::string>& alias_strings
        , const std::vector<std::string>& entity_strings);
    
private:
    bool user_input(int& argc, std::vector<std::string>& input_buffer);
    void parse_input_to_tokens(const std::string& input_, std::vector<std::string>& tokens, int& argc);
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
    std::shared_ptr<Cmd_Ctrl> ctrl;
    std::map<command, std::regex> regex_pattern;

    int size_tab_cmd;
    std::map<Tab_Cmd, std::vector<std::string>> tab_completions;
    
    Emoji_Handler emoji;
    
    std::string cmd_line = (emoji.get_emoji_str(Emoji::hourglass).empty() ? "@" : emoji.get_emoji_str(Emoji::hourglass)) + "std >> ";
};

#endif // MY_REPLEX_H



/*
✅ Allgemeine Status-Emojis
Emoji	Bedeutung	Unicode / C++ Syntax
✅	Erfolgreich	\u2705
❌	Fehler	\u274C
⚠️	Warnung	\u26A0
ℹ️	Info	\u2139
❓	Fragezeichen	\u2753
❗	Ausrufezeichen	\u2757
🔧 System / Tools / Prozess
Emoji	Bedeutung	Unicode / C++ Syntax
🛠️	Werkzeuge	\u1F6E0
🔄	Lädt neu	\u1F501
⏳	Lädt / wartet	\u23F3
🔧	Konfiguration	\u1F527
💾	Speichern	\u1F4BE
🗑️	Löschen	\u1F5D1
🗂️ Daten / Dateien
Emoji	Bedeutung	Unicode / C++ Syntax
📁	Ordner	\u1F4C1
📂	Geöffneter Ordner	\u1F4C2
📄	Datei	\u1F4C4
📋	Zwischenablage	\u1F4CB
📊	Statistik	\u1F4CA
🌱 Dein Pflanzenprojekt-Style
Emoji	Bedeutung	Unicode / C++ Syntax
🌱	Pflanze (jung)	\u1F331
🌿	Kraut / Blätter	\u1F33F
🌞	Sonne	\u1F31E
💧	Wasser	\u1F4A7
🌡️	Temperatur	\u1F321
🪴	Blumentopf	\u1FAB4
💡 Sonstiges
Emoji	Bedeutung	Unicode / C++ Syntax
🧠	Intelligenz/Analyse	\u1F9E0
🔍	Suche	\u1F50D
🛑	Stopp	\u1F6D1
✏️	Eingabe	\u270F
🖨️	Drucker	\u1F5A8
*/