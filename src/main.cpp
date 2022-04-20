#include <Arduino.h>
#include <ArduinoJson.h>
#include "esp_system.h"
// Wifi
#include <WifiClient.h>
#include <WiFiManager.h>
// Webserver
#include <WebServer.h>
// MQTT
#include <PubSubClient.h>
// local resource
#include "webpage.h"
String getMacAddress();
void handle_root();

int analogPin = 36;
int boardLEDPin = 2;


typedef struct pflanzen_werte {
  String plant_name;
  int wert;
  int wert_minimal;
  int wert_maximal;
} t_pflanze;

t_pflanze sensor = {
  .plant_name = "Helga",
  .wert = 0,
  .wert_minimal = 0,
  .wert_maximal = 4500
};

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

unsigned long alte_millis = 0;

char mqttServer[1024]    = "nail.i234.me"; 
int  mqttPort            = 10883; 
char mqttUser[1024]      = "";
char mqttPassword[1024]  = "";
char mqttClientId[1024]  = "";

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

// ==================================== Globals ==========================================
unsigned long current_millis = 0;
unsigned long prev_millis = 0;

int mqtt_status = 10 * 1000; //10s - sending every 10s

void setup() {
  Serial.begin(115200);
  pinMode(boardLEDPin, OUTPUT);

  // Connect to your wi-fi modem
  WiFiManager wifiManager;
  bool result = wifiManager.autoConnect();
  if (!result) {
    Serial.println("Nicht verbunden.");
  }
  else {
    Serial.println("Wifi is da!");
  }

  char mac_address[13];
  getMacAddress().toCharArray(mac_address, 13);
  snprintf(mqttClientId, 1024, "%s", mac_address);
  // MQTT ------------------------------------------------------------------------------
  mqtt_client.setServer(mqttServer, mqttPort);
  
  while (!mqtt_client.connected()) {
    Serial.println("...Connecting to MQTT");
    if (mqtt_client.connect(mqttClientId, mqttUser, mqttPassword )) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed connecting MQTT with state: ");
      Serial.print(mqtt_client.state());
      delay(2000);
    }
  }

  mqtt_client.publish("waterme/pflanzen/pflanze", "Hello World.");
  Serial.println("-- End of Setup --");
}

boolean mqtt_reconnect() {
  if (mqtt_client.connect(mqttClientId, mqttUser, mqttPassword )) {
    mqtt_client.publish("waterme/pflanzen/pflanze", "Hello World, again", true);
  }
  return mqtt_client.connected();
}

// Handle root url (/)
void handle_root() {
  server.send(200, "text/html", webpageCode);
}

void handleAktuellerSensor()
{
  String sensorWert = String(sensor.wert);
  server.send(200, "text/plane", sensorWert);
}

void handleKonfiguration()
{
  String config = "{ \"name\" : \"" + sensor.plant_name + "\", \"min\" : \"" + sensor.wert_minimal + "\", \"max\" : \"" + sensor.wert_maximal + "\" }";
  server.send(200, "text/plane", config);
}

void handleUpdateKonfiguration()
{
  StaticJsonDocument<200> config;
  String json = server.arg(0);
  deserializeJson(config, json);

  const char* name = config["name"];
  long v_min = config["min"];
  long v_max = config["max"];

  sensor.plant_name = String(name);
  sensor.wert_minimal = (int)v_min;
  sensor.wert_maximal = (int)v_max;

  server.send(200);
}

void loop() {
  static bool startup = false;
  static bool isBoardLEDon = false;
  unsigned long aktuelle_millis = millis();
  current_millis = millis();

  if (!startup) {
    // Check wi-fi is connected to wi-fi network
    Serial.println("WiFi connected successfully");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

    server.on("/", handle_root);
    server.on("/readADC", handleAktuellerSensor);
    server.on("/readConfig", handleKonfiguration);
    server.on("/updateConfig", handleUpdateKonfiguration);
    server.begin();
    Serial.println("HTTP server started");
    startup = true;
  } 

  server.handleClient();
  sensor.wert = analogRead(analogPin);    // read the input pin


  if (aktuelle_millis - alte_millis >= 1000) {
    alte_millis = aktuelle_millis;
    // Serial.println(String(aktuelle_millis));
    // Serial.println(String(isBoardLEDon));
    if (isBoardLEDon) {
      digitalWrite(boardLEDPin, LOW);
      isBoardLEDon = false;
    } else {
      digitalWrite(boardLEDPin, HIGH);
      isBoardLEDon = true;
    }
  }

  if ((int)(current_millis - prev_millis) >= mqtt_status) {
    char mqtt_message[400];
    char mqtt_topic[120];
    char c_plant_name[32];
    sensor.plant_name.toCharArray(c_plant_name, 32);
    snprintf(mqtt_topic, 120, "waterme/pflanzen/pflanze-%s", mqttClientId);
    snprintf(mqtt_message, 400, "{ \"name\" : \"%s\", \"value\" : \"%d\", \"min\" : \"%d\", \"max\" : \"%d\"  }", c_plant_name, sensor.wert, sensor.wert_minimal, sensor.wert_maximal);
    // Serial.println(mqtt_topic);
    // Serial.println(mqtt_message);
    boolean status = mqtt_client.publish(mqtt_topic, mqtt_message, 80);
    if (!status) {
      Serial.println("MQTT - sending failed: " + status);
    }
    prev_millis = current_millis;
  }
  //check if MQTT and Wifi connected (wifi is set to reconnect on auto)
  if (!mqtt_client.connected()) {
    mqtt_reconnect();
    delay(200);
  }

}

String getMacAddress()
{
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	char baseMacChr[18] = {0};
	sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	return String(baseMacChr);
}