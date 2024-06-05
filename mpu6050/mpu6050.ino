#include "Wire.h"
#include <MPU6050_light.h>
#include <WiFiClient.h>
#include <WiFi.h>

MPU6050 mpu(Wire);

const char* ssid = "Galaxy";
const char* password = "addy2452";
const char* thingsboardServer = "thingsboard.cloud"; // or use "thingsboard.cloud" for ThingsBoard Cloud

const int httpPort = 80;

WiFiClient client;

long timer = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) { } // stop everything if could not connect to MPU6050

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true, true); // gyro and accelero
  Serial.println("Done!\n");
}

void loop() {
  mpu.update();

  if (millis() - timer > 1000) { // send data every second
    sendDataToThingsBoard();
    timer = millis();
  }
}

void sendDataToThingsBoard() {
  // Prepare JSON payload
  String payload = "{";
  payload += "\"temperature\":" + String(mpu.getTemp()) + ",";
  payload += "\"accel_x\":" + String(mpu.getAccX()) + ",";
  payload += "\"accel_y\":" + String(mpu.getAccY()) + ",";
  payload += "\"accel_z\":" + String(mpu.getAccZ()) + ",";
  payload += "\"gyro_x\":" + String(mpu.getGyroX()) + ",";
  payload += "\"gyro_y\":" + String(mpu.getGyroY()) + ",";
  payload += "\"gyro_z\":" + String(mpu.getGyroZ()) + "}";
  
  // Connect to ThingsBoard
  if (client.connect(thingsboardServer, httpPort)) {
    Serial.println("Connected to ThingsBoard");

    // Send HTTP POST request
    client.print("POST /api/v1/Z6Z4wTnicvAEu6NZAdYJ/telemetry HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(thingsboardServer);
    client.print("\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(payload.length());
    client.print("\r\n\r\n");
    client.print(payload);

    Serial.println("Data sent to ThingsBoard");

    // Disconnect from ThingsBoard
    client.stop();
  } else {
    Serial.println("Unable to connect to ThingsBoard");
  }
}
