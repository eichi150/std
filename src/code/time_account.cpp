#include "time_account.h"

void Time_Account::set_hours(const float& amount){
    hours = amount;
}

std::string Time_Account::get_entity()const {
    return entity;
}
std::string Time_Account::get_alias()const {
    return alias;
}
std::vector<Entry> Time_Account::get_entry()const {
    return entry;
}

float Time_Account::get_hours()const{
    return hours;
}

void Time_Account::add_json_read_entry(const Entry& read_entry){
    entry.push_back(read_entry);
}

void Time_Account::add_entry(const Entry& new_entry){
    hours += new_entry.hours;
    entry.push_back(new_entry);
}
