#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>
#include <stdexcept>
#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

#define LOG_INFO(msg) std::clog << "[INFO] " << msg << std::endl
#define LOG_ERROR(msg) std::cerr << "[ERROR]\n" << msg << std::endl
#define LOG_DEBUG(msg) std::clog << "[DEBUG]\n" << msg << std::endl

class ErrorLogger{
public:
	virtual ~ErrorLogger() = default;
	virtual void log(const std::string& _log) = 0;
	virtual std::string get_logs() const = 0;
	
	void set_debug_to(bool to_){
		debug_enable = to_;
	}
	bool is_debug_enabled() const  {
		return debug_enable;
	}
	virtual void clear() = 0;

	enum class Mode{
		info = 0
		, error
		, debug
	};

	static void dump_log_for_Mode(
		const std::shared_ptr<ErrorLogger>& logger
		, Mode mode
	)
	{
		if(logger){
			bool should_log = (mode == Mode::error) || logger->is_debug_enabled();
			if(should_log){
				std::string msg = logger->get_logs();
				
				switch(mode){
					case Mode::info:
						LOG_INFO(msg);
						break;
					case Mode::debug:
						LOG_DEBUG(msg);
						break;
					case Mode::error:
						LOG_ERROR(msg);
						break;
					default:
						break;
				}
			}
		}
	} // void dump_log

private:
	bool debug_enable = false;
	std::vector<std::string> _logs;
}; //ErrorLogger

class Default_Logger : public ErrorLogger{
public:
	void log(const std::string& new_log) override {

		_logs.push_back(new_log);
	};
	
	std::string get_logs() const override {
		std::stringstream ss_log;
		for(const auto& log : _logs){
			ss_log << log << "\n";
		}
		return ss_log.str();
	};

	void clear() override {
		_logs.clear();
	}
private:
	std::vector<std::string> _logs;
	
}; // Default_Logger

//this Class throws the logger out
class Logged_Error : public std::runtime_error{
public:
	Logged_Error(
		const std::string& msg
		, std::shared_ptr<ErrorLogger> logger
	
	) : std::runtime_error(msg)
		, _logger(logger)
	{};
	
	std::shared_ptr<ErrorLogger> get_logger() const {
		return _logger;
	};	
		
private:
	std::shared_ptr<ErrorLogger> _logger;
}; // Logged_Error




class SyntaxError : public std::runtime_error{
public:
	explicit SyntaxError(
		const std::string& msg
		
	) : std::runtime_error(formatMessage(msg))
	{};

private:
	static std::string formatMessage(const std::string& msg){
		if(msg.empty()){
			return "SyntaxError (Default Output)";
		}
		return "SyntaxError:\n" + msg;
	}
}; // SyntaxError


class DeviceConnectionError : public std::runtime_error{
public:
	explicit DeviceConnectionError(
		const std::string& msg
	
	) : std::runtime_error("DeviceConnectionError:\n" + msg)
	{};
	
}; // DeviceConnectionError

#endif // EXCEPTION_H
