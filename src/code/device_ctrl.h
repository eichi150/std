#ifdef __linux__ // LINUX ONLY cause of Crontab and I2C for BME280 Sensor
#ifndef DEVICE_CTRL_H
#define DEVICE_CTRL_H

#include <iostream>
#include <iomanip>
#include <memory>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <cstdio>

#include "./abstract_class/ctrl.h"

class Device_Ctrl : public CTRL::Ctrl{
public:
	Device_Ctrl(const std::string& error_prompt);
	
	void show_log(bool set_to) override {
		show_ctrl_log = set_to;
	}
	std::string get_log() const override{
		return ctrl_log.str();
	}
	
	std::string process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command);
	
	std::vector<float> check_device(const std::string& name) override;

	std::string get_name() const { return name; }
	
private:
	Clock clock{};
	
	std::string name;
	std::string error_prompt;
	std::vector<Automation_Config> all_automations;
	std::vector<Time_Account> all_accounts;
};

#endif // DEVICE_CTRL_H
#endif // __linux__
