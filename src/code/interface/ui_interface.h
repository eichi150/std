#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

#include <map>
#include "emoji.h"

namespace UI{
	
class UI_Interface{
public:
	virtual ~UI_Interface() = default;

	virtual void update() = 0;
	
	std::map<emojID, std::pair<std::string, std::string>> get_emojis() const{
		return emoji.get_emojis();
	};

	std::string get_emoji_str(const emojID& id) const {
		return emoji.get_emoji_str(id);
	}
	
private:
	Emoji emoji;
}; //UI_Interface

}; //namespace UI
#endif // UI_INTERFACE_H
