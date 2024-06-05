#include <b64.h>
#include <HttpClient.h>

#include <WiFi.h>
#include <DHT.h>

#define DHTTYPE DHT22   // DHT 22

// Put your SSID & Password
const char* ssid = "narzo 50A";  // Enter SSID here
const char* password = "8106312321";  // Enter Password here

const char* server = "thingsboard.cloud";
const char* deviceToken = "8S5C9y9Acd2jFywmivf7";

#define DHT_Pin 23 // DHT Sensor 
DHT dht(DHT_Pin, DHTTYPE);    // Initialize DHT sensor.            

void setup() {
  Serial.begin(115200);
  pinMode(DHT_Pin, INPUT);
  dht.begin();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  
  // Connect to your local Wi-Fi network
  WiFi.begin(ssid, password);
  
  // Check if Wi-Fi is connected to the network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());
}

void loop() {
  float Temperature = dht.readTemperature(); // Gets the values of the temperature
  float Humidity = dht.readHumidity(); // Gets the values of the humidity 

  if (isnan(Temperature) || isnan(Humidity)) {
    Serial.println("Failed to Read");
    delay(1000);
  } else {
    // Sending data to ThingsBoard
    sendDataToThingsBoard(Temperature, Humidity);
  }
}

void sendDataToThingsBoard(float Temperature, float Humidity) {
  WiFiClient client;
  HTTPClient http;
  String url = String("http://") + server + "/api/v1/" + deviceToken + "/telemetry";
  String payload = String("{\"Temperature\":") + Temperature + ", \"Humidity\":" + Humidity + "}";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error sending data to ThingsBoard. HTTP Response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
