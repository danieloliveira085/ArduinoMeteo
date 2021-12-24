#include "sensors.h"

Sensors::Sensors(byte dht_type, byte dht_pin, byte mq_135_pin) : 
                 bmp_180_sensor(), dht_sensor(dht_pin, dht_type), mq_135_sensor(mq_135_pin)
{
    this->dht_type = dht_type;
    this->dht_pin = dht_pin;
    this->mq_135_pin = mq_135_pin;
}

void Sensors::init(int delayed = 5000) {
    dht_sensor.begin();
    bmp_180_sensor.begin();

    delay(delayed);
}

double Sensors::getTemperature() {
    int status = 0;
    double temperature = NAN;

    status = bmp_180_sensor.startTemperature();
    delay(status);
    status = bmp_180_sensor.getTemperature(temperature);
    if (status == 0 || isnan(temperature)) {
        temperature = dht_sensor.readTemperature();
    }

    return temperature;
}

float Sensors::getHumidity() { return dht_sensor.readHumidity(); }

double Sensors::getPressureInSeaLevel(double altitude, double temperature) {
    int status = 0;
    double pressure = NAN;

    status = bmp_180_sensor.startPressure(3);
    delay(status);
    status = bmp_180_sensor.getPressure(pressure, temperature);
    
    return bmp_180_sensor.sealevel(pressure, altitude);
}

float Sensors::getCO2(double temperature, double humidity) {
    float correctedPPM = mq_135_sensor.getCorrectedPPM(temperature, humidity);
    
    if (correctedPPM < 400) // Tenta pegar a a proporção de co2 novamente para confirmar se é realmente menor que 400ppm
    {
        for (int i = 0; i < MQ135_RETRYS; i++)
        {
        delay(MQ135_RETRY_DELAY_MS); //400ms até uma nova tentativa
        correctedPPM = mq_135_sensor.getCorrectedPPM(temperature, humidity);
        if (correctedPPM >= 400)
            break;
        }
    }

    return correctedPPM;
}