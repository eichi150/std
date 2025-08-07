#ifndef SENSOR_H
#define SENSOR_H
#include <vector>
#include <stdexcept>

class Sensor{
public:
    Sensor(){};
    virtual ~Sensor(){};
    virtual int scan_sensor(std::vector<float>& float_data){
    	throw std::runtime_error{"Not implemented"};
    };
};


#endif // SENSOR_H
