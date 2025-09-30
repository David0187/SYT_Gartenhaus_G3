#include <DHT.h>

#define DHTPIN 25       // IO25
#define DHTTYPE DHT11   // Sensor-Typ
#define MOISTURE_PIN 34  // AO an IO34

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


  int sensorValue = analogRead(MOISTURE_PIN); // der sensor muss noch besser eingestellt werden
  float moisturePercent = sensorValue / 4095.0 * 100; // in %

  Serial.print("Feuchtigkeit: ");
  Serial.print(moisturePercent);
  Serial.println(" %");

  delay(1000);
}



