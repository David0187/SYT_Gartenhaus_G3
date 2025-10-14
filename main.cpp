
V5;





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












V6;





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

  // --- Bodenfeuchtigkeit ---
  int soilValue = analogRead(MOISTURE_PIN);
  float soilPercent = soilValue / 4095.0 * 100;

  // --- Wasserfüllstand ---
  int waterState = digitalRead(WATER_SENSOR_PIN);

  // --- Pumpe simulieren ---
  bool pumpeAn = false;
  if (soilPercent < FEUCHTE_OPTIMAL) {
    pumpeAn = true;
  }

  // --- Alle Werte in einer Zeile ausgeben ---
  Serial.print("🌡️ Temp: ");
  if (!isnan(temp)) Serial.print(temp); else Serial.print("Fehler");
  Serial.print(" °C | 💧 Luft: ");
  if (!isnan(hum)) Serial.print(hum); else Serial.print("Fehler");
  Serial.print(" % | 🌱 Boden: ");
  Serial.print(soilPercent);
  Serial.print(" % | 💧 Wasser: ");
  Serial.print(waterState ? "Ja" : "Nein");
  Serial.print(" | 💦 Pumpe: ");
  Serial.print(pumpeAn ? "An" : "Aus");

  Serial.print("      "); // Leerzeichen zum Überschreiben alter Zeichen
  Serial.print("\r");     // Cursor zurück an den Anfang

  delay(2000); // Werte alle 2 Sekunden aktualisieren
}




