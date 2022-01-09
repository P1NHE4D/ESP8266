#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// relay setup
const int relayPin = 5;
int relayState = LOW;

// wifi setup
const char* ssid = "";
const char* password = "";

// mqtt broker
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);

void handleRelaySwitch(byte* payload) {
  char msg[(sizeof payload) + 1];
  memcpy(msg, payload, sizeof payload);
  msg[sizeof payload] = 0;
  if (msg == "ON") {
    relayState = HIGH;
    digitalWrite(relayPin, relayState);
  } else {
    relayState = LOW;
    digitalWrite(relayPin, relayState);
  }
}

void handleRelayState() {
  if (relayState == HIGH) {
    client.publish("relay-01-state", "ON");
  } else {
    client.publish("relay-01-state", "OFF");
  }
}

void handleMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  if (topic == "relay-01-switch") {
    handleRelaySwitch(payload);
  } else {
    handleRelayState();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT broker");
      client.subscribe("relay-01-switch");
      client.subscribe("relay-01-request-state");
    } else {
      delay(5000);
    }
  }
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
}

void setupPins() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);
}

void setupMQTT() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(handleMessage);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  setupWiFi();
  setupPins();
  setupMQTT();
  
  Serial.println("Setup complete");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
