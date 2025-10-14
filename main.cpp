
V4;





#include <DHT.h>

// ===== Pins definieren =====
#define DHTPIN 27            // DHT11 Signal
#define DHTTYPE DHT11
#define MOISTURE_PIN 34      // Bodenfeuchte (analog)
#define WATER_SENSOR_PIN 13  // 2-Kabel Wasserstand (digital)
//#define PUMP_PIN 26        // Pumpe noch nicht angeschlossen

// ===== DHT Objekt =====
DHT dht(DHTPIN, DHTTYPE);

// ===== Pumpensteuerung =====
#define FEUCHTE_OPTIMAL 60   // Bodenfeuchte % für perfekte Feuchtigkeit
#define PUMP_INTERVAL_MS 5000 // 5 Sekunden Pumpe simuliert

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
}

void loop() {
  // --- DHT11: Temperatur & Luftfeuchtigkeit ---
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    Serial.print("🌡️   Temperatur: ");
    Serial.print(temp);
    Serial.print(" °C | 💧 Luftfeuchtigkeit: ");
    Serial.print(hum);
    Serial.println(" %");
  } else {
    Serial.println("Fehler beim Lesen vom DHT11!");
  }

  // --- Bodenfeuchtigkeit ---
  int soilValue = analogRead(MOISTURE_PIN);
  float soilPercent = soilValue / 4095.0 * 100;
  Serial.print("🌱 Erdfeuchtigkeit: ");
  Serial.print(soilPercent);
  Serial.println(" %");

  // --- Pumpe simulieren ---
  if (soilPercent < FEUCHTE_OPTIMAL) {
    Serial.println("💦 Boden zu trocken -> Pumpe würde jetzt laufen für 5 Sekunden!");
    // digitalWrite(PUMP_PIN, HIGH); // noch nicht angeschlossen
    delay(PUMP_INTERVAL_MS);
    // digitalWrite(PUMP_PIN, LOW);
  } else {
    Serial.println("✅ Boden feucht genug -> Pumpe bleibt aus.");
  }

  // --- Wasserfüllstand digital + Test ---
  int waterState = digitalRead(WATER_SENSOR_PIN);
  if (waterState == true) {
    Serial.println("💧 Wasser vorhanden");
  } else {
    Serial.println("🚫 Kein Wasser vorhanden");
  }


  Serial.println("-------------------------------------------------------------------------------------------------------");
  delay(1000);
}



