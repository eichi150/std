#include "translator.h"

std::map<std::string, std::string> Translator::english_pack = {
    {"language", "english"}
    ,{"str_language", "Language"}
    ,{"timepoint", "%Y-%m-%d %H:%M:%S"}
    ,{"deleted_out_of_accounts.json", " got deleted. File is still available for Export."}
    ,{"total_hours", "Total Hours"}
    ,{"entity", "Entity"}
    ,{"hours", "Hours"}
    ,{"timestamp", "Timestamp"}
    ,{"comment", "Comment"}
    ,{"time_saved", "Time saved"}
    ,{"saved", " saved"}
    ,{"No_Entrys_available", "No Entrys available"}
};

std::map<std::string, std::string> Translator::german_pack = {
    {"language", "deutsch"}
    ,{"str_language", "Sprache"}
    ,{"timepoint", "%d-%m-%Y %H:%M:%S"}
    ,{"deleted_out_of_accounts.json", " wurde gelöscht. Die Datei zu exportieren ist weiterhin möglich."}
    ,{"total_hours", "Stunden gesamt"}
    ,{"entity", "Entität"}
    ,{"hours", "Stunden"}
    ,{"timestamp", "Zeitstempel"}
    ,{"comment", "Kommentar"}
    ,{"time_saved", "Zeitstempel gespeichert"}
    ,{"saved", " gespeichert"}
    ,{"No_Entrys_available", "Keine Einträge verfügbar"}
};

std::map<Language, std::map<std::string, std::string>> Translator::all_packs = {
    {Language::english, english_pack}
    , {Language::german, german_pack}
};

std::map<Language, std::string> Translator::dict_language = {
    {Language::english, "english"}
    , {Language::german, "german"}
};

void Translator::set_language(const Language& language){
    this->language = language;
    language_pack = which_language_pack();
}

std::map<std::string, std::string> Translator::which_language_pack(){

    auto pack = all_packs.at(Language::english);
    if(english_pack.size() != german_pack.size()){
        pack = all_packs.at(Language::english);
        all_packs.clear();
        return pack;
    }
    
    bool same_keys = std::equal(
        english_pack.begin(), english_pack.end(),
        german_pack.begin(),
        [](const auto& a, const auto& b){
            return a.first == b.first;
        }
    );
        
    if(!same_keys){
        pack = all_packs.at(Language::english);
        all_packs.clear();
        return pack;
    }
    pack = all_packs.at(language);
    all_packs.clear();
    return pack;
}

std::string Translator::get_str_language(){
    return dict_language.at(language);
}