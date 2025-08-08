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
#include <set>

#include "ctrl.h"
#include "clock.h"
#include "../exception/exception.h"
#include "../time_account.h"
#include "../json_handler.h"

class Device_Ctrl : public CTRL::Ctrl{
public:

	
	std::string process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& _command);
	
	std::vector<float> check_device(const std::string& name);

	std::string get_name() const { return name; }
	
private:
	Clock clock{};
	
	std::string name;
	std::string error_prompt{"Sensor Error. Make sure you installed i2c.\nExecute on Command Line: 'sudo apt-get install i2c-tools'\nand try 'sudo i2cdetect -y 1'\nPort: 76 should be active. Succesfully installed."};

};

#endif // DEVICE_CTRL_H
#endif // __linux__
