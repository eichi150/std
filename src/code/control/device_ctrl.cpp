#ifdef __linux__
#include "device_ctrl.h"

std::string Device_Ctrl::process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& command){

	//auto Accounts lesen
	all_automations = jsonH->read_automation_config_file();
	
	//auto Accounts erstellen
	std::string device_name;
	for(const auto& auto_config : all_automations){
		if(auto_config.crontab_command == command){
			Time_Account loaded_account{auto_config.entity, auto_config.alias};
					
			all_accounts.push_back(loaded_account);

			device_name = auto_config.device_name;
		}
	}
	
	//auto Accounts <entity>.json lesen
	jsonH->read_json_entity(all_accounts);

	//Sensor Werte abfragen
	std::vector<float> output_sensor = check_device(device_name);
	if(output_sensor.empty()){
		throw DeviceConnectionError{"§§ No Sensor Output"};
	}
	std::stringstream ss;
	ss << "Temperature: " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
		<< "Pressure: " << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
		<< "Humidity: " << std::fixed << std::setprecision(2) << output_sensor[2] << " %";
		
	std::tm localTime = clock.get_time();
	
	//auto Accounts speichern
	for(auto& account : all_accounts){
		Entry entry{0.f, ss.str(), localTime};
		account.add_entry(entry);
		jsonH->save_json_entity(all_accounts, account.get_entity());
	}
	
	std::stringstream output;
	output 
		<< std::put_time(&localTime, "%Y-%m-%d %H:%M") 
		<< '\n' 
		<<  ss.str() 
		<< '\n';
		 
	return output.str();
}

std::vector<float> Device_Ctrl::check_device(const std::string& name) {

	this->name = name;
	
	std::vector<float> output_sensor;

	auto it = all_devices.find(name);
	if (it == all_devices.end()) {
		throw DeviceConnectionError{"Sensor not registered: " + name};
	}

	auto sensor_ptr = it->second;

	if (!sensor_ptr) {
		throw DeviceConnectionError{"Sensor pointer is null"};
	}

	// optional: dynamisch casten
	auto bme_ptr = std::dynamic_pointer_cast<BME_Sensor>(sensor_ptr);
	if (!bme_ptr) {
		throw DeviceConnectionError{"Sensor is not a BME_Sensor"};
	}

	if (bme_ptr->scan_sensor(output_sensor) == 1) {
		throw DeviceConnectionError{error_prompt};
	}

	return output_sensor;
}

#endif // __linux__
