
V3;







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
    Serial.print("Es hat: ");
    Serial.print(temp);
    Serial.println(" °C");
  } else {
    Serial.println("Fehler");
  }

 int sensorValue = analogRead(MOISTURE_PIN);
  float moisturePercent = sensorValue / 4095.0 * 100; // in %

  Serial.print("Die Erdfeuchtigkeit beträgt: ");
  Serial.print(moisturePercent);
  Serial.println(" %");


  delay(2000); // 2 Sekunden

}






