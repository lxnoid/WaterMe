#include <Arduino.h>
#include <ArduinoJson.h>
#include "esp_system.h"
// Konfiguration
#include <Preferences.h>
// Wifi
#include <WifiClient.h>
#include <WiFiManager.h>
// Webserver
#include <WebServer.h>
// MQTT
#include <PubSubClient.h>
// local resource
#include <helper.h>

// Hardware Setup
int analogPin = 36;
int boardLEDPin = 2;

// ==================================== Globals ==========================================
Preferences preferences;
t_pflanze pflaenzchen;

// -> Webserver
WebServer server(80);

// -> Zeiten
unsigned long alte_millis = 0;
unsigned long current_millis = 0;
unsigned long prev_millis = 0;

// -> MQTT & Wifi
WiFiClient wifi_client;
t_mqtt_val mqtt_v;
PubSubClient mqtt_client(wifi_client);
int mqtt_status = 10 * 60 * 1000; // in Millisekunden

// ==================================== Setup ==========================================
void setup()
{
  Serial.begin(115200);
  pinMode(boardLEDPin, OUTPUT);

  // Load old preferences
  preferences.begin("pflanze");
  pflaenzchen.plant_name = preferences.getString("name", "Helga");
  pflaenzchen.wert_minimal = preferences.getInt("min", 0);
  pflaenzchen.wert_maximal = preferences.getInt("max", 4500);

  // Connect to your wi-fi modem
  WiFiManager wifiManager;
  bool result = wifiManager.autoConnect();
  if (!result)
  {
    Serial.println("Nicht verbunden.");
  }
  else
  {
    Serial.println("Wifi is da!");
  }

  snprintf(mqtt_v.mqttServer, sizeof(mqtt_v.mqttServer), "%s", "nail.i234.me");
  mqtt_v.mqttPort = 10883;
  snprintf(mqtt_v.mqttUser, sizeof(mqtt_v.mqttUser), "%s", "");
  snprintf(mqtt_v.mqttPassword, sizeof(mqtt_v.mqttPassword), "%s", "");
  snprintf(mqtt_v.mqttClientId, sizeof(mqtt_v.mqttClientId), "%s", "");

  char mac_address[13];
  getMacAddress().toCharArray(mac_address, 13);
  snprintf(mqtt_v.mqttClientId, 1024, "%s", mac_address);
  // MQTT ------------------------------------------------------------------------------
  mqtt_client.setServer(mqtt_v.mqttServer, mqtt_v.mqttPort);

  while (!mqtt_client.connected())
  {
    Serial.println("...Connecting to MQTT");
    if (mqtt_client.connect(mqtt_v.mqttClientId, mqtt_v.mqttUser, mqtt_v.mqttPassword))
    {
      Serial.println("Connected to MQTT");
    }
    else
    {
      Serial.print("Failed connecting MQTT with state: ");
      Serial.print(mqtt_client.state());
      delay(2000);
    }
  }

  mqtt_client.publish("waterme/pflanzen/pflanze", "Hello World.");
  Serial.println("-- End of Setup --");
}

// ==================================== Loop ==========================================
void loop()
{
  static bool startup = false;
  static bool isBoardLEDon = false;
  unsigned long aktuelle_millis = millis();
  current_millis = millis();

  if (!startup)
  {
    // Check wi-fi is connected to wi-fi network
    Serial.println("WiFi connected successfully");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP()); // Show ESP32 IP on serial

    startWebServer();

    Serial.println("HTTP server started");
    startup = true;
  }

  server.handleClient();
  pflaenzchen.wert = analogRead(analogPin); // read the input pin

  if (aktuelle_millis - alte_millis >= 1000)
  {
    alte_millis = aktuelle_millis;
    if (isBoardLEDon)
    {
      digitalWrite(boardLEDPin, LOW);
      isBoardLEDon = false;
    }
    else
    {
      digitalWrite(boardLEDPin, HIGH);
      isBoardLEDon = true;
    }
  }

  if ((int)(current_millis - prev_millis) >= mqtt_status)
  {
    char mqtt_message[400];
    char mqtt_topic[120];
    char c_plant_name[32];
    pflaenzchen.plant_name.toCharArray(c_plant_name, 32);
    snprintf(mqtt_topic, 120, "waterme/pflanzen/pflanze-%s", mqtt_v.mqttClientId);
    snprintf(mqtt_message, 400, "{ \"name\" : \"%s\", \"value\" : \"%d\", \"min\" : \"%d\", \"max\" : \"%d\"  }", c_plant_name, pflaenzchen.wert, pflaenzchen.wert_minimal, pflaenzchen.wert_maximal);
    boolean status = mqtt_client.publish(mqtt_topic, mqtt_message, 80);
    if (!status)
    {
      Serial.println("MQTT - sending failed: " + status);
    }
    prev_millis = current_millis;
  }

  // check if MQTT and Wifi connected (wifi is set to reconnect on auto)
  if (!mqtt_client.connected())
  {
    mqtt_reconnect();
    delay(200);
  }
}

// ==================================== Getters ==========================================
WebServer *getServer(void)
{
  return &server;
}

t_mqtt_val *getMQTTSetup(void)
{
  return &mqtt_v;
}

PubSubClient *getMQTTServer(void)
{
  return &mqtt_client;
}

t_pflanze *getPlant(void)
{
  return &pflaenzchen;
}

Preferences *getPreferences(void)
{
  return &preferences;
}
