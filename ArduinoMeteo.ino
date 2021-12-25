#include "sensors.h"

#define PIN_MQ135 A10 // MQ135 Analog Input Pin
#define DHTPIN 2      // DHT Digital Input Pin
#define DHTTYPE DHT11
#define RELAY_PIN 8

#define ALTITUDE 810.0

#define LOOP_DELAY 5 * 1000
#define HEAT_DELAY 30 * 1000

Sensors sensors(DHTTYPE, DHTPIN, PIN_MQ135);

void setup()
{
  Serial.begin(9600);

  Serial.println("Booting...");
  Serial.print("Heating for ");
  Serial.print(HEAT_DELAY / 1000);
  Serial.println(" seconds...");

  #ifdef RELAY_PIN
    pinMode(RELAY_PIN, OUTPUT); // Relê para iniciar o circuito na fonte externa (opcional)
    digitalWrite(RELAY_PIN, HIGH);
  #endif

  sensors.init(HEAT_DELAY);

  Serial.println("Booted!");
}

void loop()
{
  unsigned long start = millis();

  float humidity = sensors.getHumidity();
  double temperature = sensors.getTemperature();
  double pressure = sensors.getPressureInSeaLevel(ALTITUDE, temperature);
  float co2 = sensors.getCO2(temperature, humidity, LOOP_DELAY - (millis() - start));

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

  unsigned long stop = millis();

  int dly = LOOP_DELAY - (stop - start);
  if (dly > 0)
    delay(dly);
}