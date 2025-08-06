#include "translator.h"

const std::map<std::string, std::string> Translator::english_pack = {
    {"language", "english"}
    ,{"str_language", "Language"}
    ,{"timepoint", "%Y-%m-%d %H:%M"}
    ,{"deleted_out_of_accounts.json", " got deleted. File is still available for Export."}
    ,{"total_hours", "Total Hours"}
    ,{"entity", "Entity"}
    ,{"hours", "Hours"}
    ,{"timestamp", "Timestamp"}
    ,{"comment", "Comment"}
    ,{"time_saved", "Time saved"}
    ,{"saved", " saved"}
    ,{"No_Entrys_available", "No Entrys available"}
    ,{"Temperature", "Temperature"}
    ,{"Pressure", "Pressure"}
    ,{"Humidity", "Humidity"}
};

const std::map<std::string, std::string> Translator::german_pack = {
    {"language", "deutsch"}
    ,{"str_language", "Sprache"}
    ,{"timepoint", "%d-%m-%Y %H:%M"}
    ,{"deleted_out_of_accounts.json", " wurde gelöscht. Die Datei zu exportieren ist weiterhin möglich."}
    ,{"total_hours", "Stunden gesamt"}
    ,{"entity", "Entität"}
    ,{"hours", "Stunden"}
    ,{"timestamp", "Zeitstempel"}
    ,{"comment", "Kommentar"}
    ,{"time_saved", "Zeitstempel gespeichert"}
    ,{"saved", " gespeichert"}
    ,{"No_Entrys_available", "Keine Einträge verfügbar"}
    ,{"Temperature", "Temperatur"}
    ,{"Pressure", "Druck"}
    ,{"Humidity", "Feuchte"}
};

const std::map<Language, std::map<std::string, std::string>> Translator::all_packs = {
    {Language::english, english_pack}
    , {Language::german, german_pack}
};

const std::map<Language, std::string> Translator::dict_language = {
    {Language::english, "english"}
    , {Language::german, "german"}
};

void Translator::set_language(const Language& language){
    this->language = language;
    language_pack = which_language_pack();
}

std::map<std::string, std::string> Translator::which_language_pack(){
    //Validate language Packs consistency
    if( english_pack.size() != german_pack.size()
        || !std::equal(english_pack.begin(), english_pack.end(), german_pack.begin(),
            [](const auto& a, const auto& b){
                return a.first == b.first;
            }))
    {
        return all_packs.at(Language::english);
    }
    //Return requested language pack or default English
    auto it = all_packs.find(language);
    return (it != all_packs.end()) ? it->second : all_packs.at(Language::english);
}

std::string Translator::get_str_language(){
    auto it = dict_language.find(language);
    return (it != dict_language.end()) ? it->second : "english";
}
