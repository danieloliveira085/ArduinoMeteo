#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

#include <MQ135.h>
#include <DHT.h>
#include <SFE_BMP180.h>

#define MQ135_RETRY_DELAY_MS 400

class Sensors
{
private:
    byte dht_pin;
    byte mq_135_pin;

    byte dht_type;

    SFE_BMP180 bmp_180_sensor;
    DHT dht_sensor;
    MQ135 mq_135_sensor;

    float old_gas_ppm = NAN;

public:
    Sensors(byte dht_type, byte dht_pin, byte mq_135_pin);

    void init(int heat_delay = 5000);

    double getTemperature();
    float getHumidity();
    double getPressureInSeaLevel(double altitude, double temperature);
    float getCO2(double temperature, double humidity, int retry_timeout = 0);
};

#endif