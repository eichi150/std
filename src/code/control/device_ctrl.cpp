#ifdef __linux__
#include "device_ctrl.h"

std::string Device_Ctrl::process_automation(const std::shared_ptr<JSON_Handler>& jsonH, const std::string& _command){
	//auto Accounts lesen
	std::vector<Automation_Config> all_automations = jsonH->read_automation_config_file();
	
	//accounts to process
	std::vector<Time_Account> for_process;
	std::string device_name;
	for(const auto& _cfg : all_automations){
		device_name = _cfg.device_name;
		Time_Account proc_account{_cfg.entity, _cfg.alias};
		for_process.push_back(proc_account);
	}

	//Entitys to save
	std::vector<std::string> entity_strings;
	std::set<std::string> unique_entities;
	for (const auto& _auto : all_automations) {
		unique_entities.insert(_auto.entity);
	}
	entity_strings.assign(unique_entities.begin(), unique_entities.end());

	//read each entity data
	std::vector<Time_Account> all_accounts;
	for(const auto& entity : entity_strings){
		jsonH->read_one_entity(all_accounts, entity);
	}
	jsonH->read_json_entity(all_accounts);

	/*for(const auto& acc : all_accounts){
		for(auto& _proc : for_process){
			if(_proc.get_alias() == acc.get_alias()){
				for(const auto& _entry : acc.get_entry()){
					_proc.add_entry(_entry);
				}
				
				break;
			}
		}
	}*/
	//get Sensordata
	std::vector<float> output_sensor = check_device(device_name);
	if(output_sensor.empty()){
		throw DeviceConnectionError{"§§ No Sensor Output"};
	}
	std::stringstream ss;
	ss << "Temperature: " << std::fixed << std::setprecision(2) << output_sensor[0] << " °C || "
		<< "Pressure: " << std::fixed << std::setprecision(2) << output_sensor[1] << " hPa || "
		<< "Humidity: " << std::fixed << std::setprecision(2) << output_sensor[2] << " %";
		
	std::tm localTime = clock.get_time();
	//Data pass in Time_Account
	for(auto& acc : all_accounts){
		for(const auto& _acc : for_process){
			if(_acc.get_alias() == acc.get_alias()){
				Entry entry{0.f, ss.str(), localTime};
				acc.add_entry(entry);
				break;
			}
			
		}
	}
	
	//Pass Account into the rest of the entitys data
	/*for(const auto& _proc : for_process){
		for(auto& acc : all_accounts){
			if(acc.get_alias() == _proc.get_alias()){
				acc = _proc;
				break;
			}
		}
	}*/
	//save each entity
	for(const auto& entity : entity_strings){
		jsonH->save_json_entity(all_accounts, entity);
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
