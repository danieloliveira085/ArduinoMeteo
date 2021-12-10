#include <MQ135.h>
#include <DHT.h>
#include <SFE_BMP180.h>
#include <Wire.h>

#define PIN_MQ135 A10 // MQ135 Analog Input Pin
#define DHTPIN 2 // DHT Digital Input Pin
#define DHTTYPE DHT11

#define ALTITUDE 810.0

MQ135 mq135_sensor(PIN_MQ135);
DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 bmp;

void setup() {
  Serial.begin(9600);

  dht.begin();
  bmp.begin();

  Serial.println("Boot");
}

void loop() {
  char status;
  double temperature, pressure, humidity;
  int tmpDelay;
  
  humidity = dht.readHumidity();
  // temperature = dht.readTemperature();
  status = bmp.startTemperature();
  delay(status);
  tmpDelay += status;
  status = bmp.getTemperature(temperature);
  if (status == 0) {
    temperature = dht.readTemperature();  
  }

  status = bmp.startPressure(3);
  delay(status);
  tmpDelay += status;
  status = bmp.getPressure(pressure,temperature);
  if(pressure < 400) { //Tenta pegar a pressão novamente para confirmar
    for (int i = 0; i < 3; i++) {
      status = bmp.startPressure(3);
      delay(status);
      tmpDelay += status;
      status = bmp.getPressure(pressure,temperature);
      if (pressure >= 400) 
        break;
    }
  }
  
  double p0 = bmp.sealevel(pressure, ALTITUDE);
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

  if(!isnan(humidity)) {
    Serial.print("Humidity: ");
    Serial.println(humidity);
  }
  if(!isnan(temperature)) {
    Serial.print("Temperature: ");
    Serial.println(temperature);
  }
  if(!isnan(pressure)) {
    Serial.print("Pressure: ");
    Serial.println(p0);
//    Serial.println(pressure);
  }
  if(!isnan(correctedPPM)) {
    Serial.print("AirQuality: ");
    Serial.println(correctedPPM);
  }

  int dly = 5000 - tmpDelay;
  if (dly > 0) {
    delay(dly); 
  }
}
