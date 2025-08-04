#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H
#include <sstream>

namespace UI{
	
class UI_Interface{
public:
	virtual ~UI_Interface() = default;

	virtual void update() = 0;
	
protected:
	std::stringstream ui_interface_log;
}; //UI_Interface

}; //namespace UI
#endif // UI_INTERFACE_H
