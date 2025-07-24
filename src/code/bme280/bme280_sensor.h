#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "bme280.h"
#include <iomanip>
#include <sstream>
#include <vector>

class BME_Sensor {
public:
    int scan_sensor(std::vector<float>& float_data);

private:
    int i2c_fd;
	
    // Muss static sein, da Funktionszeiger in C keinen Zugriff auf `this` haben
    static BME280_INTF_RET_TYPE user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);

    static BME280_INTF_RET_TYPE user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

    static void user_delay_us(uint32_t period_us, void *intf_ptr);
};

#endif // BME280_SENSOR_H
