#include <Arduino.h>
#include <WiFiManager.h>
#include <WebServer.h>
#include "webpage.h"

void handle_root();

int analogPin = 36;
int boardLEDPin = 2;

typedef struct sensor_werte {
  int wert;
  int wert_minimal;
  int wert_maximal;
} t_sensor;

t_sensor sensor;

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

unsigned long alte_millis = 0;

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

void loop() {
  static bool startup = false;
  static bool isBoardLEDon = false;
  unsigned long aktuelle_millis = millis();

  if (!startup) {
    // Check wi-fi is connected to wi-fi network
    Serial.println("WiFi connected successfully");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

    server.on("/", handle_root);
    server.on("/readPOT", handleAktuellerSensor);
    server.begin();
    Serial.println("HTTP server started");
    startup = true;
  } 

  server.handleClient();
  sensor.wert = analogRead(analogPin);    // read the input pin


  if (aktuelle_millis - alte_millis >= 1000) {
    alte_millis = aktuelle_millis;
    Serial.println(String(aktuelle_millis));
    Serial.println(String(isBoardLEDon));
    if (isBoardLEDon) {
      digitalWrite(boardLEDPin, LOW);
      isBoardLEDon = false;
    } else {
      digitalWrite(boardLEDPin, HIGH);
      isBoardLEDon = true;
    }
  }
}
