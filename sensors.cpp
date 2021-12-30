#include "sensors.h"

Sensors::Sensors(byte dht_type, byte dht_pin, byte mq_135_pin) : 
                 bmp_180_sensor(), dht_sensor(dht_pin, dht_type), mq_135_sensor(mq_135_pin)
{
    this->dht_type = dht_type;
    this->dht_pin = dht_pin;
    this->mq_135_pin = mq_135_pin;
}

void Sensors::init(int heat_delay = 5000)
{
    dht_sensor.begin();
    bmp_180_sensor.begin();

    delay(heat_delay);
}

double Sensors::getTemperature()
{
    int status = 0;
    double temperature = 0.0;

    status = bmp_180_sensor.startTemperature();
    delay(status);
    status = bmp_180_sensor.getTemperature(temperature);
    if (status == 0 || temperature == 0.0) {
        temperature = dht_sensor.readTemperature();
    }

    return temperature == 0.0 ? NAN : temperature;
}

float Sensors::getHumidity() { return dht_sensor.readHumidity(); }

double Sensors::getPressureInSeaLevel(double altitude, double temperature)
{
    int status = 0;
    double pressure = 0.0;

    status = bmp_180_sensor.startPressure(3);
    delay(status);
    status = bmp_180_sensor.getPressure(pressure, temperature);

    if (pressure == 0.0) {
        return NAN;
    }

    return bmp_180_sensor.sealevel(pressure, altitude);
}

// retry_timeout é quanto tempo essa função pode gastar além do necessário até conseguir um valor mais estável do mq-135
// o padrão para esse valor é 0, o que desativa o sistema de tentativas
float Sensors::getCO2(double temperature, double humidity, int retry_timeout = 0)
{
    int retrys = 0;
    while (retry_timeout > 0)
    {
        retry_timeout -= MQ135_RETRY_DELAY_MS;
        if (retry_timeout >= 0)
            retrys++;
    }

    float correctedPPM = mq_135_sensor.getCorrectedPPM(temperature, humidity);

    if (retrys > 0)
    {
        // Tenta pegar a a proporção de co2 novamente para confirmar se é realmente menor que a quantidade de co2
        // na atmosfera ou caso tenha uma variação para baixo em relação ao valor anterior (amenizando os picos)
        if (correctedPPM < ATMOCO2 || (isnan(old_gas_ppm) || correctedPPM < old_gas_ppm))
        {
            for (int i = 0; i < retrys; i++)
            {
                delay(MQ135_RETRY_DELAY_MS);
                correctedPPM = mq_135_sensor.getCorrectedPPM(temperature, humidity);
                if (correctedPPM >= ATMOCO2 && correctedPPM >= old_gas_ppm)
                    break;
            }

            old_gas_ppm = correctedPPM;
        }
    }

    return correctedPPM == 0.0 ? NAN : correctedPPM;
}