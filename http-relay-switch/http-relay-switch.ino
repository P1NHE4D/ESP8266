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

void handleRelaySwitch() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    relayState = !relayState;
    digitalWrite(relayPin, relayState);
    server.send(200, "text/plain", "");
  }
}

void handleRelayState() {
  if (server.method() != HTTP_GET) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    char message[10];
    snprintf(message, sizeof(message), "State: %d", relayState);
    server.send(200, "text/plain", message);
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
  server.on("/relay-switch", handleRelaySwitch);
  server.on("/relay-state", handleRelayState);
  server.onNotFound(handleNotFound);

  // start server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
