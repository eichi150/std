#ifndef INTERACTION_H
#define INTERACTION_H

#include <sstream>
#include <memory>
#include "../exception/exception.h"

class Interaction{
public:
	virtual ~Interaction() = default;
	
	virtual void interact() = 0;
	
	void set_logger(std::shared_ptr<ErrorLogger> log){
		logger = std::move(log);
	}
	void log(const std::string& msg){
		if(logger){
			logger->log(msg);
		}
	}
	std::string get_logs() const {
		return (logger ? logger->get_logs() : std::string{});
	}
	
	
	void set_output_logger(std::shared_ptr<ErrorLogger> log){
		output_logger = std::move(log);
	}
	void add_output(const std::string& msg){
		if(output_logger){
			output_logger->log(msg);
		}
	}
	std::string get_output_logs() const {
		return (output_logger ? output_logger->get_logs() : std::string{});
	}
protected:
	std::shared_ptr<ErrorLogger> output_logger;
	std::shared_ptr<ErrorLogger> logger;
};

#endif // INTERACTION_H
