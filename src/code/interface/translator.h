#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <map>
#include <string>

enum class Language{
    english = 0
    , german 
};

class Translator{
public:
    /*const std::map<std::string, std::string>& get_language_pack() const {
        return language_pack;
    }*/
    std::map<std::string, std::string> language_pack{};

    static const std::map<Language, std::string> dict_language;
    static const std::map<std::string, std::string> english_pack;
    static const std::map<std::string, std::string> german_pack;
    static const std::map<Language, std::map<std::string, std::string>> all_packs;

    void set_language(const Language& language);
	
    std::map<std::string, std::string> which_language_pack();

    std::string get_str_language();

    Language get_current_language() const {
	return language;
    }
    
private:
    Language language;
	
};

#endif // TRANSLATOR_H
