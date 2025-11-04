
VMQTT: MQTT Code FINAL

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ====================================================================
// ===== 1. ZUGANGSDATEN ANPASSEN (Muss angepasst werden!) =====
// ====================================================================

// WLAN
const char* ssid = "NVS-Europa";
const char* password = "nvsrocks";

// MQTT Broker (Dein Raspberry Pi)
const char* mqtt_server = "172.16.93.132"; // IP-Adresse des Mosquitto Brokers
const char* mqtt_user = "mqttuser";      // MQTT-Benutzername
const char* mqtt_password = "mqtt_"; // MQTT-Passwort
const char* mqtt_topic_out = "pflanzenbox/daten"; // Topic zum Senden

// ====================================================================
// ===== 2. HARDWARE & OBJEKTE (Dein vorhandener Code + MQTT) =====
// ====================================================================

#include <DHT.h>

// Pins definieren
#define DHTPIN 27            // DHT11 Signal
#define DHTTYPE DHT11
#define MOISTURE_PIN 34      // Bodenfeuchte (analog)
#define WATER_SENSOR_PIN 13  // 2-Kabel Wasserstand (digital)
//#define PUMP_PIN 26        // Pumpe noch nicht angeschlossen

// DHT Objekt
DHT dht(DHTPIN, DHTTYPE);

// MQTT-Objekte
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0; // Zeitstempel für das Sendeintervall

// Globale Funktionsdeklarationen
void setup_wifi();
void reconnect();

// ====================================================================
// ===== 3. SETUP Funktion =====
// ====================================================================

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
  
  // --- WLAN-Verbindung aufbauen ---
  setup_wifi();

  // --- MQTT-Konfiguration ---
  client.setServer(mqtt_server, 1883); // Port 1883
}

// ====================================================================
// ===== 4. LOOP Funktion (Hauptlogik) =====
// ====================================================================

void loop() {
  // WLAN-Verbindung prüfen und bei Bedarf wiederherstellen
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  
  // MQTT-Verbindung prüfen und bei Bedarf wiederherstellen
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Muss regelmäßig aufgerufen werden, um MQTT-Verbindung zu erhalten

  // --- Sende-Logik (Senden nur alle 30 Sekunden) ---
  long now = millis();
  if (now - lastMsg > 5000) { 
    lastMsg = now;

    // ------------------------------------
    // 1. Sensordaten auslesen
    // ------------------------------------
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int soilValue = analogRead(MOISTURE_PIN);
    float soilPercent = soilValue / 4095.0 * 100.0;
    int waterState = digitalRead(WATER_SENSOR_PIN);
    
    // Fehlerbehandlung
    if (isnan(temp) || isnan(hum)) {
      Serial.println("Fehler beim Lesen vom DHT11. Sende keine Daten.");
      return; 
    }

    // ------------------------------------
    // 2. JSON-Payload erstellen
    // ------------------------------------
    // Reserviere Speicherplatz (200 ist ausreichend für 4 Werte)
    StaticJsonDocument<200> doc;
    
    // Daten in das JSON-Dokument füllen
    doc["temperature"] = temp;
    doc["humidity"] = hum;
    doc["soil_percent"] = soilPercent;
    doc["water_ok"] = (waterState == HIGH); // true/false senden
    
    // JSON serialisieren und senden
    char jsonBuffer[200];
    serializeJson(doc, jsonBuffer);
    
    // --- MQTT Senden ---
    if (client.publish(mqtt_topic_out, jsonBuffer)) {
      Serial.print("MQTT gesendet an Topic '");
      Serial.print(mqtt_topic_out);
      Serial.print("': ");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("Fehler beim Senden der MQTT-Nachricht!");
    }
  }

  // --- Pumpensteuerung (Dein ursprünglicher Block) ---
  // HIER solltest du deine Pumpen-Logik einfügen, wenn die Zeit des
  // Pumpens den MQTT-Loop nicht zu lange blockiert (max. 1-2 Sekunden).
  // ...
  
  delay(100); 
}

// ====================================================================
// ===== 5. HILFSFUNKTIONEN =====
// ====================================================================

void setup_wifi() {
  delay(10);
  Serial.print("Verbinde mit ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi verbunden!");
  Serial.print("IP-Adresse ESP32: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Versuche, erneut zu verbinden
  while (!client.connected()) {
    Serial.print("Versuche MQTT-Verbindung...");
    
    // Erstelle eine eindeutige Client-ID
    String clientId = "ESP32-Pflanzenbox-";
    clientId += String(random(0xffff), HEX);
    
    // Versuche zu verbinden (mit Benutzername und Passwort)
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("verbunden!");
    } else {
      Serial.print("fehlgeschlagen, rc=");
      Serial.print(client.state());
      Serial.println(" versuche in 5 Sekunden erneut");
      delay(5000); // 5 Sekunden warten, bevor es erneut versucht wird
    }
  }
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




