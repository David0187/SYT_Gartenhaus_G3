
VMQTT: MQTT Code Test

#include <Arduino.h>
#include <DHT.h>

// ===== Sensor-Pins =====
#define DHTPIN 27
#define DHTTYPE DHT11
#define MOISTURE_PIN 34

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("ESP32 gestartet. Sensorwerte werden über Serial gesendet.");
}

void loop() {
  // Sensorwerte lesen
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soil = analogRead(MOISTURE_PIN);

  // Nur senden, wenn gültige Werte
  if (!isnan(temp) && !isnan(hum)) {
    // Ausgabe als CSV: Temp,Hum,Soil
    Serial.printf("%.1f,%.1f,%d\n", temp, hum, soil);
  }

  delay(2000); // alle 2 Sekunden
}



V5, immer wieder erscheinende Aktualisierung

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



V6, 1 Zeilige Aktualisierung

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



V7, Konzept mit Displays

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ===== Pins definieren =====
#define DHTPIN 27
#define DHTTYPE DHT11
#define MOISTURE_PIN 34
#define WATER_SENSOR_PIN 13
#define FEUCHTE_OPTIMAL 60
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// ===== DHT Objekt =====
DHT dht(DHTPIN, DHTTYPE);

// ===== OLED-Displays (verschiedene I2C-Adressen nötig!) =====
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // z. B. 0x3C
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // z. B. 0x3D
Adafruit_SSD1306 display3(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // z. B. 0x3E

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
  Wire.begin(21, 22); // SDA, SCL

  // Displays initialisieren
  if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    Serial.println("Display1 (0x3C) nicht gefunden!");
  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3D))
    Serial.println("Display2 (0x3D) nicht gefunden!");
  if (!display3.begin(SSD1306_SWITCHCAPVCC, 0x3E))
    Serial.println("Display3 (0x3E) nicht gefunden!");

  for (auto d : {&display1, &display2, &display3}) {
    d->clearDisplay();
    d->setTextSize(1);
    d->setTextColor(SSD1306_WHITE);
    d->setCursor(0, 10);
    d->println("🌿 Gewaechshaus");
    d->display();
  }
  delay(1000);
}

void loop() {
  // Messungen
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soilValue = analogRead(MOISTURE_PIN);
  float soilPercent = soilValue / 4095.0 * 100;
  int waterState = digitalRead(WATER_SENSOR_PIN);
  bool pumpeAn = (soilPercent < FEUCHTE_OPTIMAL);

  // Serial Ausgabe
  Serial.printf("Temp: %.1f°C | Luft: %.1f%% | Boden: %.1f%% | Wasser: %s | Pumpe: %s\n",
                temp, hum, soilPercent,
                waterState ? "Ja" : "Nein",
                pumpeAn ? "An" : "Aus");

  // Display 1 → Temperatur
  display1.clearDisplay();
  display1.setTextSize(2);
  display1.setTextColor(SSD1306_WHITE);
  display1.setCursor(0, 0);
  display1.print("Temp:");
  display1.setCursor(70, 0);
  display1.print(isnan(temp) ? 0 : temp);
  display1.print("C");
  display1.display();

  // Display 2 → Luftfeuchtigkeit
  display2.clearDisplay();
  display2.setTextSize(2);
  display2.setTextColor(SSD1306_WHITE);
  display2.setCursor(0, 0);
  display2.print("Luft:");
  display2.setCursor(70, 0);
  display2.print(isnan(hum) ? 0 : hum);
  display2.print("%");
  display2.display();

  // Display 3 → Bodenfeuchte
  display3.clearDisplay();
  display3.setTextSize(2);
  display3.setTextColor(SSD1306_WHITE);
  display3.setCursor(0, 0);
  display3.print("Boden:");
  display3.setCursor(70, 0);
  display3.print((int)soilPercent);
  display3.print("%");
  display3.display();

  delay(2000);
}



