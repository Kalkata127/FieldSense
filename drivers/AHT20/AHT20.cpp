#include "AHT20.hpp"

bool AHT20::init(){
    vTaskDelay(pdMS_TO_TICKS(101));

    uint8_t cmd = 0x71;
    uint8_t status_byte = 0;

    esp_err_t err = i2c_master_transmit_receive(g_sensor_dev, &cmd, 1, &status_byte, 1, 1000); 
    
    if(err != ESP_OK){
        ESP_LOGE(TAG, "AHT20 i2c failed");
        return false;
    }

    if((status_byte & 0x18) != 0x18){
        return false;
    }

    return true; 
}

bool AHT20::triggerMeasurement()
{
    vTaskDelay(pdMS_TO_TICKS(11));
    const uint8_t trig_measurement_cmds[] = {0xAC, 0x33, 0x00};
    esp_err_t err = i2c_master_transmit(g_sensor_dev, trig_measurement_cmds, sizeof(trig_measurement_cmds), 1000);

    if(err != ESP_OK)
        return false;

    return true;
}

bool AHT20::readMeasurement(){
    vTaskDelay(pdMS_TO_TICKS(81));
    
    for(int attempt = 0; attempt < 10; attempt++){
        esp_err_t err = i2c_master_receive(g_sensor_dev, data, sizeof(data), 1000);
        if(err != ESP_OK){
            return false;
        }
        if((data[0] & 0x80)==0){
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if(data[0] & 0x80){
        return false;
    }

    return true;
}

bool AHT20::processMeasurements()
{
    hum_raw = 0;
    hum_raw = (uint32_t)data[1] << 16;
    hum_raw |= (uint32_t)data[2] << 8;
    hum_raw |= (uint32_t)data[3];
    hum_raw >>= 4; // hum_raw = hum_raw >> 4

    temp_raw = 0;
    temp_raw = (uint32_t)data[3] & 0x0F;
    temp_raw <<= 16;
    temp_raw |= (uint32_t)data[4] << 8;
    temp_raw |= (uint32_t)data[5];
    
    return true;
}

bool AHT20::writeMeasurements()
{
    reading.humidity = (hum_raw / 1048576.0f) * 100.0f;
    reading.temperature = ((temp_raw / 1048576.0f) * 200) - 50;
    return true;
}

bool AHT20::measure(SensorReading &out){
    if(!triggerMeasurement()) return false;
    if(!readMeasurement()) return false;
    processMeasurements();
    writeMeasurements();
    out = reading;
    return true;
}
