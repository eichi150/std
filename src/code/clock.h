#ifndef CLOCK_H
#define CLOCK_H

#include <ctime>

class Clock{
public:
	std::tm get_time(){
		std::time_t now = std::time(nullptr);
		std::tm localTime = *std::localtime(&now);

		return localTime;
	}
};

#endif // CLOCK_H