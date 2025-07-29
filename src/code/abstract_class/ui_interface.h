#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

class UI_Interface{
public:
	virtual ~UI_Interface() = default;

	virtual void update() = 0;
};

#endif // UI_INTERFACE_H
