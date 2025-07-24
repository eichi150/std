#include "bme280_sensor.h"


    int BME_Sensor::scan_sensor() {
        const char *i2c_device = "/dev/i2c-1";
        static uint8_t dev_addr = BME280_I2C_ADDR_PRIM; // 0x76

        // I2C öffnen
        if ((i2c_fd = open(i2c_device, O_RDWR)) < 0) {
            perror("I2C open");
            return 1;
        }

        if (ioctl(i2c_fd, I2C_SLAVE, dev_addr) < 0) {
            perror("I2C ioctl");
            return 1;
        }

        struct bme280_dev dev;
        dev.intf = BME280_I2C_INTF;
        dev.intf_ptr = &i2c_fd;
        dev.read = user_i2c_read;
        dev.write = user_i2c_write;
        dev.delay_us = user_delay_us;

        if (bme280_init(&dev) != BME280_OK) {
            printf("Sensorinitialisierung fehlgeschlagen\n");
            return 1;
        }

        struct bme280_settings settings;
        settings.osr_h = BME280_OVERSAMPLING_1X;
        settings.osr_p = BME280_OVERSAMPLING_16X;
        settings.osr_t = BME280_OVERSAMPLING_2X;
        settings.filter = BME280_FILTER_COEFF_16;
        settings.standby_time = BME280_STANDBY_TIME_1000_MS;

        uint8_t settings_sel = BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP |
                               BME280_SEL_OSR_HUM | BME280_SEL_FILTER;

        if (bme280_set_sensor_settings(settings_sel, &settings, &dev) != BME280_OK) {
            printf("Sensor Settings fehlgeschlagen\n");
            return 1;
        }

        if (bme280_set_sensor_mode(BME280_POWERMODE_FORCED, &dev) != BME280_OK) {
            printf("Sensor Mode setzen fehlgeschlagen\n");
            return 1;
        }

        dev.delay_us(40000, dev.intf_ptr);

        struct bme280_data comp_data;
        if (bme280_get_sensor_data(BME280_ALL, &comp_data, &dev) != BME280_OK) {
            printf("Sensor Data lesen fehlgeschlagen\n");
            return 1;
        }

        printf("Temperatur: %.2f °C\n", comp_data.temperature);
        printf("Druck:      %.2f hPa\n", comp_data.pressure / 100.0);
        printf("Feuchte:    %.2f %%\n", comp_data.humidity);

        close(i2c_fd);
        return 0;
    }



    // Muss static sein, da Funktionszeiger in C keinen Zugriff auf `this` haben
    BME280_INTF_RET_TYPE BME_Sensor::user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr) {
        int fd = *(int *)intf_ptr;
        if (write(fd, &reg_addr, 1) != 1)
            return -1;
        if (read(fd, data, len) != (int)len)
            return -1;
        return 0;
    }

    BME280_INTF_RET_TYPE BME_Sensor::user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
        int fd = *(int *)intf_ptr;
        uint8_t buf[1 + len];
        buf[0] = reg_addr;
        for (uint32_t i = 0; i < len; i++) {
            buf[i + 1] = data[i];
        }
        if (write(fd, buf, len + 1) != (int)(len + 1))
            return -1;
        return 0;
    }

    void BME_Sensor::user_delay_us(uint32_t period_us, void *intf_ptr) {
        usleep(period_us);
    }



