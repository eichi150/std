#ifndef INTERACTION_H
#define INTERACTION_H

#include <sstream>

class Interaction{
public:
	virtual ~Interaction() = default;
	
	virtual std::string get_log() const = 0;
	virtual std::string get_user_output_log() const = 0;
	virtual void interact() = 0;
	
protected:
	std::stringstream interaction_log;
	std::stringstream user_output_log;
};

#endif // INTERACTION_H
