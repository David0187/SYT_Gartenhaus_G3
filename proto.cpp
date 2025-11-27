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
const char* mqtt_server = "172.16.93.132";
const char* mqtt_user = "mqttuser";
const char* mqtt_password = "mqtt_";
const char* mqtt_topic_out   = "Gruppe3/daten";
const char* mqtt_topic_pump  = "Gruppe3/pumpe";

// Sensorpins
#define DHTPIN 27
#define DHTTYPE DHT11
#define MOISTURE_PIN 34
#define RELAY_PIN 23

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;

// Pumpensteuerung
bool manualPumpOverride = false;
bool manualPumpState = false;

// Globale Sensorwerte für OLED
float g_temp = 0;
float g_luft = 0;
float g_soil = 0;
bool  g_pumpe = false;

// -------------------------------------------------------------

void u8g2_prepare() {
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
}

// OLED-Darstellung
void drawStr() {
    u8g2.clearBuffer();
    u8g2_prepare();

    u8g2.setCursor(0, 0);
    u8g2.print("Temp: ");
    u8g2.print(g_temp, 1);
    u8g2.print(" C");

    u8g2.setCursor(0, 12);
    u8g2.print("Humidity: ");
    u8g2.print(g_luft, 1);
    u8g2.print(" %");

    u8g2.setCursor(0, 24);
    u8g2.print("Soil: ");
    u8g2.print(g_soil, 1);
    u8g2.print(" %");

    u8g2.setCursor(0, 36);
    u8g2.print("Pump: ");
    u8g2.print(g_pumpe ? "ON" : "OFF");

    u8g2.sendBuffer();
}

// -------------------------------------------------------------
// MQTT-Callback
// -------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
    char msg[64]; // make sure it's large enough
    if (length >= sizeof(msg)) length = sizeof(msg) - 1;
    for (unsigned int i = 0; i < length; i++)
        msg[i] = toupper((char)payload[i]);
    msg[length] = '\0';

    if (strcmp(topic, mqtt_topic_pump) == 0) {
        if (strcmp(msg, "AN") == 0)       { manualPumpOverride = manualPumpState = true;  digitalWrite(RELAY_PIN, LOW);  }
        else if (strcmp(msg, "AUS") == 0) { manualPumpOverride = manualPumpState = false; digitalWrite(RELAY_PIN, HIGH); }
        else if (strcmp(msg, "AUTO") == 0){ manualPumpOverride = false; }
    }
}


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
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

// -------------------------------------------------------------
void reconnect() {
    while (!client.connected()) {
        Serial.print("Verbindung zu MQTT... ");
        String clientId = "ESP32-Pflanzenbox-" + String(random(0xffff), HEX);

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
void setup() {
    Serial.begin(115200);

    dht.begin();
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Pumpe AUS

    setup_wifi();

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    u8g2.begin();
}

// -------------------------------------------------------------
void loop() {

    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    long now = millis();
    if (now - lastMsg > 5000) {
        lastMsg = now;

        // Sensorwerte lesen
        g_temp = dht.readTemperature();
        g_luft = dht.readHumidity();
        int soilRaw = analogRead(MOISTURE_PIN);
        g_soil = soilRaw / 4095.0 * 100.0;

        if (!isnan(g_temp) && !isnan(g_luft)) {
            StaticJsonDocument<200> doc;
            doc["Temperatur"] = g_temp;
            doc["Luftfeuchtigkeit"] = g_luft;
            doc["Bodenfeuchtigkeit"] = g_soil;

            char jsonBuffer[200];
            serializeJson(doc, jsonBuffer);
            client.publish(mqtt_topic_out, jsonBuffer);

            Serial.print("Sensorwerte gesendet: ");
            Serial.println(jsonBuffer);
        }

        // Pumpensteuerung
        if (!manualPumpOverride) {
            if (g_soil < 30.0) {
                digitalWrite(RELAY_PIN, LOW);
                g_pumpe = true;
                Serial.println("Automatik: Trocken → Pumpe EIN");
            } else {
                digitalWrite(RELAY_PIN, HIGH);
                g_pumpe = false;
                Serial.println("Automatik: OK → Pumpe AUS");
            }
        } else {
            // manueller Modus
            digitalWrite(RELAY_PIN, manualPumpState ? LOW : HIGH);
            g_pumpe = manualPumpState;
        }
    }

    // OLED aktualisieren
    drawStr();
}
