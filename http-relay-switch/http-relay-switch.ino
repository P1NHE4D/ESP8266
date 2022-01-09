#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// relay setup
const int relayPin = 5;
int relayState = LOW;

// wifi setup
const char* ssid = "";
const char* password = "";

// http server setup
ESP8266WebServer server(80);

void handleSwitchOn() {
  if (server.method() != HTTP_GET) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    relayState = HIGH;
    digitalWrite(relayPin, relayState);
    server.send(200, "text/plain", "");
  }
}

void handleSwitchOff() {
  if (server.method() != HTTP_GET) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    server.send(200, "text/plain", "");
  }
}

void handleSwitchStatus() {
  if (server.method() != HTTP_GET) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    char msg[10];
    sprintf(msg, "%d", relayState);
    Serial.println(msg);
    server.send(200, "text/plain", msg);
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);

  // server setup
  server.on("/switchOn", handleSwitchOn);
  server.on("/switchOff", handleSwitchOff);
  server.on("/switchStatus", handleSwitchStatus);
  server.onNotFound(handleNotFound);

  // start server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
