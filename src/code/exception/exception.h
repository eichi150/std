#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <ostream>
#include <string>
#include <vector>
#include <memory>

class ErrorLogger{
public:
	virtual ~ErrorLogger() = default;
	virtual void log(const std::string& _log) = 0;
	virtual std::string get_logs() const = 0;
}; //ErrorLogger

class User_Logger : public ErrorLogger{
public:
	void log(const std::string& _log) override {
		_output.push_back(_log);
	}
	
	std::string get_logs() const override {
		std::stringstream ss;
		for(const auto& out : _output){
			ss << out << "\n";
		}
		return ss.str();
	}
    
private:
	std::vector<std::string> _output;
};

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
			return "SyntaxError: Syntax wrong";
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
