#include <DHT.h>

#define DHTPIN 25       // GPIO25
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
    Serial.println("Fehler beim Lesen vom DHT11!");
  }

  delay(10000); // nur alle 2 Sekunden messen

}

asdkasndjnsadasdasijdbahsdbhasdbhasd
