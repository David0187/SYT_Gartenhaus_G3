#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <ctype.h>

// OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const char* ssid = "NVS-Europa";
const char* password = "nvsrocks";

// MQTT
const char* mqtt_server     = "172.16.93.132";
const char* mqtt_user       = "mqttuser";
const char* mqtt_password   = "mqtt_";
const char* mqtt_topic_out  = "Gruppe3/daten";
const char* mqtt_topic_pump = "Gruppe3/pumpe";

// Sensorpins
#define DHTPIN 27
#define DHTTYPE DHT11
#define MOISTURE_PIN 34
#define RELAY_PIN 23

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

// Pumpensteuerung
bool manualPumpOverride = false;
bool manualPumpState = false;

// Globale Werte für OLED
float g_temp = 0;
float g_luft = 0;
float g_soil = 0;
bool  g_pumpe = false;

// Bodenfeuchte-Kalibrierung ANPASSEN!
int SOIL_WET = 1200;
int SOIL_DRY = 3200;

// -------------------------------------------------------------
// OLED
// -------------------------------------------------------------
void u8g2_prepare() {
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontPosTop();
}

void drawStr() {
    u8g2.clearBuffer();
    u8g2_prepare();

    u8g2.setCursor(0, 0);
    u8g2.printf("Temp: %.1f C", g_temp);

    u8g2.setCursor(0, 12);
    u8g2.printf("Humidity: %.1f %%", g_luft);

    u8g2.setCursor(0, 24);
    u8g2.printf("Soil: %.1f %%", g_soil);

    u8g2.setCursor(0, 36);
    u8g2.print("Pump: ");
    u8g2.print(g_pumpe ? "ON" : "OFF");

    u8g2.sendBuffer();
}

// -------------------------------------------------------------
// MQTT Callback
// -------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
    char msg[64];
    if (length >= sizeof(msg)) length = sizeof(msg) - 1;

    for (unsigned int i = 0; i < length; i++)
        msg[i] = toupper(payload[i]);
    msg[length] = '\0';

    // Leerzeichen entfernen
    while (length > 0 && msg[length - 1] == ' ') {
        msg[length - 1] = '\0';
        length--;
    }

    if (strcmp(topic, mqtt_topic_pump) == 0) {
        if (strcmp(msg, "AN") == 0) {
            manualPumpOverride = true;
            manualPumpState = true;
            digitalWrite(RELAY_PIN, LOW);
            g_pumpe = true;
        }
        else if (strcmp(msg, "AUS") == 0) {
            manualPumpOverride = true;
            manualPumpState = false;
            digitalWrite(RELAY_PIN, HIGH);
            g_pumpe = false;
        }
        else if (strncmp(msg, "AUTO", 4) == 0) {
            manualPumpOverride = false;
        }
    }
}

// -------------------------------------------------------------
// WIFI
// -------------------------------------------------------------
void setup_wifi() {
    Serial.print("Verbinde mit WLAN ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi verbunden!");
}

// -------------------------------------------------------------
// MQTT reconnect
// -------------------------------------------------------------
void reconnect() {
    while (!client.connected()) {
        Serial.print("MQTT Verbinde... ");
        String clientId = "ESP32-" + String(random(0xffff), HEX);

        if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
            Serial.println("OK");
            client.subscribe(mqtt_topic_pump);
        } else {
            Serial.print("Fehler rc=");
            Serial.println(client.state());
            delay(5000);
        }
    }
}

// -------------------------------------------------------------
// SETUP
// -------------------------------------------------------------
void setup() {
    Serial.begin(115200);

    dht.begin();

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // AUS

    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    u8g2.begin();
}

// -------------------------------------------------------------
// LOOP
// -------------------------------------------------------------
void loop() {

    if (!client.connected()) reconnect();
    client.loop();

    static unsigned long lastSensor = 0;
    static unsigned long lastDisplay = 0;

    unsigned long now = millis();

    // ---------------------------------------------------------
    // Sensorwerte alle 5 Sekunden
    // ---------------------------------------------------------
    if (now - lastSensor > 5000) {
        lastSensor = now;

        float t = dht.readTemperature();
        float h = dht.readHumidity();

        if (isnan(t) || isnan(h)) {
            Serial.println("DHT Fehler");
        } else {
            g_temp = t;
            g_luft = h;
        }

        int raw = analogRead(MOISTURE_PIN);
        g_soil = map(raw, SOIL_WET, SOIL_DRY, 100, 0);
        g_soil = constrain(g_soil, 0, 100);

        // MQTT senden
        StaticJsonDocument<200> doc;
        doc["Temperatur"] = g_temp;
        doc["Luftfeuchtigkeit"] = g_luft;
        doc["Bodenfeuchtigkeit"] = g_soil;

        char json[200];
        serializeJson(doc, json);
        client.publish(mqtt_topic_out, json);

        // Automatik
        if (!manualPumpOverride) {
            if (g_soil < 30) {
                digitalWrite(RELAY_PIN, LOW);
                g_pumpe = true;
                Serial.println("Auto: Trocken → Pumpe EIN");
            } else {
                digitalWrite(RELAY_PIN, HIGH);
                g_pumpe = false;
                Serial.println("Auto: OK → Pumpe AUS");
            }
        } else {
            digitalWrite(RELAY_PIN, manualPumpState ? LOW : HIGH);
            g_pumpe = manualPumpState;
        }
    }

    // ---------------------------------------------------------
    // OLED alle 500 ms aktualisieren
    // ---------------------------------------------------------
    if (now - lastDisplay > 500) {
        lastDisplay = now;
        drawStr();
    }
}
