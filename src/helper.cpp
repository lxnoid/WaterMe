#include <helper.h>
#include "webpage.h"

String getMacAddress()
{
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	char baseMacChr[18] = {0};
	sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	return String(baseMacChr);
}

boolean mqtt_reconnect() {
  t_mqtt_val* mqtt_v = getMQTTSetup();
  PubSubClient* mqtt_client = getMQTTServer();
  if (mqtt_client->connect(mqtt_v->mqttClientId, mqtt_v->mqttUser, mqtt_v->mqttPassword )) {
    mqtt_client->publish("waterme/pflanzen/pflanze", "Hello World, again", true);
  }
  return mqtt_client->connected();
}

// Handle root url (/)
void handle_root() {
  WebServer* server = getServer();
  server->send(200, "text/html", webpageCode);
}

void handleAktuellerSensor()
{
  WebServer* server = getServer();
  t_pflanze* pflaenzchen = getPlant();
  String sensorWert = String(pflaenzchen->wert);
  server->send(200, "text/plane", sensorWert);
}

void handleKonfiguration()
{
  WebServer* server = getServer();
  t_pflanze* pflaenzchen = getPlant();
  String config = "{ \"name\" : \"" + pflaenzchen->plant_name + "\", \"min\" : \"" + pflaenzchen->wert_minimal + "\", \"max\" : \"" + pflaenzchen->wert_maximal + "\" }";
  server->send(200, "text/plane", config);
}

void handleUpdateKonfiguration()
{
  WebServer* server = getServer();
  t_pflanze* pflaenzchen = getPlant();
  Preferences* preferences = getPreferences();
  boolean changed = false;
  StaticJsonDocument<200> config;
  String json = server->arg(0);
  deserializeJson(config, json);

  const char* name = config["name"];
  long v_min = config["min"];
  long v_max = config["max"];

  if ( pflaenzchen->plant_name != String(name) ||
       pflaenzchen->wert_minimal != (int)v_min ||
       pflaenzchen->wert_maximal != (int)v_max ) 
  {
    changed = true;
  }

  pflaenzchen->plant_name = String(name);
  pflaenzchen->wert_minimal = (int)v_min;
  pflaenzchen->wert_maximal = (int)v_max;
  server->send(200);

  if (changed) {
      preferences->putString("name", pflaenzchen->plant_name);
      preferences->putInt("min", pflaenzchen->wert_minimal);
      preferences->putInt("max", pflaenzchen->wert_maximal);
  }
}

void startWebServer()
{
    WebServer* server = getServer();
    server->on("/", handle_root);
    server->on("/readADC", handleAktuellerSensor);
    server->on("/readConfig", handleKonfiguration);
    server->on("/updateConfig", handleUpdateKonfiguration);
    server->begin();
}