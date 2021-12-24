#include "sensors.h"

#define PIN_MQ135 A10 // MQ135 Analog Input Pin
#define DHTPIN 2      // DHT Digital Input Pin
#define DHTTYPE DHT11

#define ALTITUDE 810.0

#define LOOP_DELAY 5000

Sensors sensors(DHTTYPE, DHTPIN, PIN_MQ135);

void setup()
{
  Serial.begin(9600);

  Serial.println("Booting...");

  sensors.init();

  Serial.println("Booted");
}

void loop()
{
  float humidity = sensors.getHumidity();
  double temperature = sensors.getTemperature();
  double pressure = sensors.getPressureInSeaLevel(ALTITUDE, temperature);
  float co2 = sensors.getCO2(temperature, humidity);

  if (!isnan(humidity))
  {
    Serial.print("Humidity: ");
    Serial.println(humidity);
  }
  if (!isnan(temperature))
  {
    Serial.print("Temperature: ");
    Serial.println(temperature);
  }
  if (!isnan(pressure))
  {
    Serial.print("Pressure: ");
    Serial.println(pressure);
  }
  if (!isnan(co2))
  {
    Serial.print("AirQuality: ");
    Serial.println(co2);
  }

  int dly = LOOP_DELAY - sensors.getDelayed();
  if (dly > 0)
    delay(dly);
}