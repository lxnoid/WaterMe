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
#ifndef helper_h
#define helper_h

// Typen
typedef struct pflanzen_werte {
  String plant_name;
  int wert;
  int wert_minimal;
  int wert_maximal;
} t_pflanze;

typedef struct mqtt_werte {
  char mqttServer[1024]; 
  int  mqttPort; 
  char mqttUser[1024];
  char mqttPassword[1024];
  char mqttClientId[1024];
} t_mqtt_val;

// Externe Funktionen
extern WebServer* getServer(void);
extern t_mqtt_val* getMQTTSetup(void);
extern PubSubClient* getMQTTServer(void);
extern t_pflanze* getPlant(void);
extern Preferences* getPreferences(void);

// Interne Funktionen
String getMacAddress();

void handle_root();
void handleAktuellerSensor();
void handleKonfiguration();
void handleUpdateKonfiguration();
void startWebServer();

boolean mqtt_reconnect();
#endif