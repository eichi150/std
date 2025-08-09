#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <string>
#endif // _Win32

#include <map>
#include "emoji.h"
#include "../exception/exception.h"

namespace UI{
	
class UI_Interface{
public:
	UI_Interface(){
		bool emoji_ok = init_utf8_console();
		if(!emoji_ok){
			LOG_ERROR("Warnung: Emojis evtl. nicht sichtbar.");
		}
	};
	virtual ~UI_Interface() = default;

	virtual void update() = 0;
	
	std::map<emojID, std::pair<std::string, std::string>> get_emojis() const{
		return emoji.get_emojis();
	};

	std::string get_emoji_str(const emojID& id) const {
		return emoji.get_emoji_str(id);
	}

private:
	//for windows console to print emojis correctly
	bool init_utf8_console() {
	#ifdef _WIN32
		// UTF-8 Encoding für Konsole aktivieren
		if (!SetConsoleOutputCP(CP_UTF8)) {
			std::cerr << "Fehler: Konnte UTF-8 OutputCP nicht setzen.\n";
			return false;
		}

		// Prüfen, ob Windows Terminal oder ähnliches läuft
		char* wtProfile = std::getenv("WT_PROFILE_ID");
		if (wtProfile != nullptr) {
			// Windows Terminal erkannt → Emoji-Support ziemlich sicher vorhanden
			return true;
		}

		// cmd / klassische Powershell: Font prüfen
		// Direkten API-Weg gibt's nicht, nur Heuristik über Registry oder Testausgabe
		std::wcout << L"🛈 Testausgabe: 😀\n";
		std::wcout.flush();
		std::cout << "(Falls hier kein Emoji erscheint, fehlt Font-Support)\n";
		return true;
	#else
		return true; // auf Linux/macOS meist kein Problem
	#endif
	}

private:
	Emoji emoji;
}; //UI_Interface

}; //namespace UI
#endif // UI_INTERFACE_H
