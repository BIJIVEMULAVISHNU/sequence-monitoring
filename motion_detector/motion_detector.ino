#include <WiFi.h>

const char *ssid = "narzo 50A";
const char *password = "8106312321";
const char *thingboardServer = "thingsboard.cloud"; // e.g., "demo.thingsboard.io"
const char *thingboardToken = "H02TEyvpZVz31rk2sAGe";

const int pirSensorPin = 2; // Connect PIR sensor to digital pin 2 on ESP32

int pirState = LOW;
int lastPirState = LOW;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  pirState = digitalRead(pirSensorPin); 
  Serial.println("Sensor Value:");
Serial.println(pirState);

  if (pirState != lastPirState) {
    if (pirState == HIGH) {
      Serial.println("Motion detected!");
      sendToThingBoard("motionDetected", "1");
    } else {
      Serial.println("Motion ended.");
      sendToThingBoard("motionDetected", "0");
    }
  }

  lastPirState = pirState;
  delay(500);
}

void sendToThingBoard(const char *attribute, const char *value) {
  String payload = "{ \"" + String(attribute) + "\": " + String(value) + " }";

  // Create a TCP/IP client
  WiFiClient client;
  if (!client.connect(thingboardServer, 80)) {
    Serial.println("Connection to ThingBoard failed.");
    return;
  }

  // HTTP POST request to ThingBoard
  client.print("POST /api/v1/");
  client.print(thingboardToken);
  client.print("/telemetry HTTP/1.1\r\n");
  client.print("Host: ");
  client.print(thingboardServer);
  client.print("\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print("Content-Length: ");
  client.print(payload.length());
  client.print("\r\n\r\n");
  client.print(payload);

  Serial.println("Data sent to ThingBoard: " + payload);

  // Close the connection
  client.stop();
}
