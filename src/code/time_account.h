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
	

class Time_Account{
public:
	Time_Account(const std::string& ent, const std::string& al) : entity(ent), alias(al){};
	
	void set_hours(const float& amount);
	
	std::string get_entity() const;
	std::string get_alias() const;
	std::vector<Entry> get_entry() const;
	
	float get_hours() const;
	
	void add_json_read_entry(const Entry& read_entry);
	
	void add_entry(const Entry& new_entry);
private:
	std::string entity{};
	std::string alias{};
	
	float hours{0.f};
	std::vector<Entry> entry;
};

#endif // TIME_ACCOUNT_H