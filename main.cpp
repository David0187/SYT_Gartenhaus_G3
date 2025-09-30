#include <DHT.h>

#define DHTPIN 25       // IO25
#define DHTTYPE DHT11   // Sensor-Typ

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float temp = dht.readTemperature();

  if (!isnan(temp)) {
    Serial.print("Temperatur: ");
    Serial.print(temp);
    Serial.println(" °C");
  } else {
    Serial.println("Fehler");
  }

  delay(2000); // 2 Sekunden



