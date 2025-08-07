#ifndef TIME_ACCOUNT_H
#define TIME_ACCOUNT_H

#include <string>
#include <vector>
#include <ctime>

struct Entry{
	float hours;
	std::string description;
	std::tm time_point;
};

struct Automation_Config{
	std::string connection;
	std::string entity;
	std::string alias;
	std::string executioner;
	std::string crontab_command;
	std::string crontab_command_speeking;
	std::string logfile;
	std::string device_name;
	
	bool operator==(const Automation_Config& other) const {
		return {
			connection == other.connection
			&& entity == other.entity
			&& alias == other.alias
			&& crontab_command == other.crontab_command
		};
	}
};

//Different Account Packages

class Time_Account{
public:
	Time_Account(const std::string& ent, const std::string& al) : entity(ent), alias(al){};

	void set_tag(const std::string& new_tag);
	
	void set_hours(const float& amount);
	
	std::string get_entity() const;
	std::string get_alias() const;
	std::vector<Entry> get_entry() const;

	std::string get_tag() const;
	float get_hours() const;
	
	void add_json_read_entry(const Entry& read_entry);
	
	void add_entry(const Entry& new_entry);

private:
	std::string entity{};
	std::string alias{};
	std::string tag{};
	
	float hours{0.f};
	std::vector<Entry> entry;
};

#endif // TIME_ACCOUNT_H
