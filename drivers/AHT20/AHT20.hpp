#pragma once
#include <stdint.h>
//#include "driver/i2c_master.h"

struct SensorReading{
    float temperature = 0;
    float humidity = 0;
};

class AHT20{
    static constexpr uint8_t addr = 0x38;
    uint8_t data[6] = {0};

    uint32_t hum_raw = 0;
    uint32_t temp_raw = 0;
    
    SensorReading reading;

    bool triggerMeasurement();
    bool readMeasurement();
    bool processMeasurements();
    bool writeMeasurements();
    
public:
    bool init();
    bool measure(SensorReading& out);

    AHT20() = default;
    AHT20(const AHT20&) = delete;
    AHT20& operator=(const AHT20&) = delete;
    ~AHT20() = default;
};