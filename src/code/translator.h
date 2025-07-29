#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <map>
#include <string>
#include "enum_class.h"

class Translator{
public:
    std::map<std::string, std::string> language_pack{};
	
    static std::map<Language, std::string> dict_language;
    static std::map<std::string, std::string> english_pack;
    static std::map<std::string, std::string> german_pack;
    static std::map<Language, std::map<std::string, std::string>> all_packs;

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
