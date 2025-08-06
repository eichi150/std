#ifndef CLOCK_H
#define CLOCK_H

#include <ctime>

class Clock{
public:
	std::tm get_time() const noexcept{
		std::time_t now = std::time(nullptr);
	#if defined(_POSIX_VERSION)
		std::tm localTime {};
		localtime_r(&now, &localTime);
		return localTime;
	#else
		//Fallback: not thread-safe but keeps build working on non-POSIX systems
		return *std::localtime(&now);
	#endif
	}
};
#endif // CLOCK_H