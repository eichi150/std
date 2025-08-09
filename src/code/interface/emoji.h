#ifndef EMOJI_H
#define EMOJI_H

#include <map>
#include <string>
enum class emojID{
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
    , open_ordner
    , file
    , sidetable
};

class Emoji{
public:
    std::string get_emoji_str(const emojID& id) const {
        auto it = emojis.find(id);
        if(it != emojis.end()){
            return emojis.at(id).first;
        }
        return std::string{"@"};
    }
    std::string get_emoji_unicode(const emojID& id) const {
        auto it = emojis.find(id);
        if(it != emojis.end()){
            return emojis.at(id).second;
        }
        return std::string{"@"};
    }

    std::map<emojID, std::pair<std::string, std::string>> get_emojis() const {
        return emojis;
    }

private:
    std::map<emojID, std::pair<std::string, std::string>> emojis = {
        {emojID::default_,           {"@", "@"}}
        , {emojID::hourglass ,       {"⏳", "\\u23F3"}}
        , {emojID::pencil,           {"✏️", "\\u270F"}}
        , {emojID::tools,            {"🛠️", "\\u1F6E0"}}
        , {emojID::configuration,    {"🔧", "\\u1F527"}}
        , {emojID::loading,          {"🔄", "\\u1F501"}}
        , {emojID::save,             {"💾", "\\u1F4BE"}}
        , {emojID::delete_,          {"🗑️", "\\u1F5D1"}}
        , {emojID::brain,            {"🧠", "\\u1F9E0"}}
        , {emojID::search,           {"🔍", "\\u1F50D"}}
        , {emojID::statistics,       {"📊", "\\u1F4CA"}}
        , {emojID::water,            {"💧", "\\u1F4A7"}}
        , {emojID::plant,            {"🌱", "\\u1F331"}}
        , {emojID::sun,              {"🌞", "\\u1F31E"}}
        , {emojID::lightbulb,        {"💡", "@"}}
        , {emojID::stop,             {"🛑", "\\u1F6D1"}}
        , {emojID::question_mark,    {"❓", "\\u2753"}}
        , {emojID::exclamation_mark, {"❗", "\\u2757"}}
        , {emojID::warning,          {"⚠️", "\\u26A0"}}
        , {emojID::info,             {"ℹ️", "\\u2139"}}
        , {emojID::error_,           {"❌", "\\u274C"}}
        , {emojID::leafes,           {"🌿", "\\u1F33F"}}
        , {emojID::open_ordner,      {"📂", "\\u1F4C2"}}
        , {emojID::file,             {"📄", "\\u1F4C4"}}
        , {emojID::sidetable,        {"📋", "\\u1F4CB"}}
    };
};

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

#endif // EMOJI_H