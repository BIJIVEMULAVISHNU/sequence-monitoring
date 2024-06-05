#include <Wire.h>
#include <MPU6050.h>
#include <WebServer.h>
#include <WiFi.h>

const char* ssid = "narzo 50A";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

WebServer server(80);

MPU6050 mpu;

int16_t AccX, AccY, AccZ;
int16_t GyroX, GyroY, GyroZ;

void setup() {
  Serial.begin(115200);
  delay(100);

  Wire.begin();
  mpu.initialize();

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // Read MPU6050 data
  mpu.getAcceleration(&AccX, &AccY, &AccZ);
  mpu.getRotation(&GyroX, &GyroY, &GyroZ);

  // Send MPU6050 data to server
  sendDataToServer();

  // Adjust the delay as needed
  delay(1000);
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(AccX, AccY, AccZ, GyroX, GyroY, GyroZ)); 
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(int16_t accX, int16_t accY, int16_t accZ, int16_t gyroX, int16_t gyroY, int16_t gyroZ) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP32 MPU6050 Data</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>ESP32 MPU6050 Data</h1>\n";
  
  ptr += "<p>Acceleration (X): ";
  ptr += accX;
  ptr += "</p>";
  ptr += "<p>Acceleration (Y): ";
  ptr += accY;
  ptr += "</p>";
  ptr += "<p>Acceleration (Z): ";
  ptr += accZ;
  ptr += "</p>";
  ptr += "<p>Gyroscope (X): ";
  ptr += gyroX;
  ptr += "</p>";
  ptr += "<p>Gyroscope (Y): ";
  ptr += gyroY;
  ptr += "</p>";
  ptr += "<p>Gyroscope (Z): ";
  ptr += gyroZ;
  ptr += "</p>";
  
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void sendDataToServer() {
  WiFiClient client;
  const char* serverUrl = "optimistic-lake-74691.pktriot.net";

  // Prepare JSON payload
  String payload = "{";
  payload += "\"accel_x\":" + String(AccX) + ",";
  payload += "\"accel_y\":" + String(AccY) + ",";
  payload += "\"accel_z\":" + String(AccZ) + ",";
  payload += "\"gyro_x\":" + String(GyroX) + ",";
  payload += "\"gyro_y\":" + String(GyroY) + ",";
  payload += "\"gyro_z\":" + String(GyroZ) + "}";

  // Connect to server
  if (client.connect(serverUrl, 80)) {
    Serial.println("Connected to server");

    // Send HTTP POST request
    client.print("POST /wordpress/your-endpoint HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(serverUrl);
    client.print("\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(payload.length());
    client.print("\r\n\r\n");
    client.print(payload);

    Serial.println("Data sent to server");

    // Disconnect from server
    client.stop();
  } else {
    Serial.println("Unable to connect to server");
  }
}
