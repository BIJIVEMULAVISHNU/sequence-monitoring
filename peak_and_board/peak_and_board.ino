#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

const char* ssid = "Js Inter5";   // Your Network SSID
const char* password = "123@45678#";       // Your Network Password

int val;
int pin = A0; // LM35 Pin Connected at A0 Pin

WiFiClient client;

unsigned long myChannelNumber = 2398568; //Your ThingSpeak Channel Number
const char * myWriteAPIKey = "0C2BUCDLVU9Z7IP5"; //Your ThingSpeak Write API Key

const char* tbServer = "demo.thingsboard.io"; // ThingBoard Server
const char *accessToken = "Dc4yE4z3pKmvMK2ayyga";
const int tbPort = 80; // ThingBoard port (HTTP)

void setup()
{
  Serial.begin(9600);
  delay(10);
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");
  Serial.print("Connected to ");
  Serial.println(ssid);
  
  ThingSpeak.begin(client); // Initialize ThingSpeak
}

void loop()
{
  val = analogRead(pin); // Read Analog values and Store in val variable
  
  // Send data to ThingSpeak
  Serial.print("Darkness: ");
  Serial.println(val); // Print on Serial Monitor
  ThingSpeak.writeField(myChannelNumber, 1, val, myWriteAPIKey); // Update ThingSpeak
  
  // Send data to ThingBoard
  sendToThingBoard(val);
  
  delay(1000);
}

void sendToThingBoard(int value)
{
  if (client.connect(tbServer, tbPort)) 
  {
    Serial.println("Sending data to ThingBoard...");
    String postStr = String("{\"LightSensor\":") + value + "}";
    
    client.println("POST /api/v1/YOUR_ACCESS_TOKEN/telemetry HTTP/1.1");
    client.println("Host: demo.thingsboard.io");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(postStr.length());
    client.println();
    client.println(postStr);
    
    Serial.println("Data sent to ThingBoard.");
  }
  else 
  {
    Serial.println("Failed to connect to ThingBoard server.");
  }
  
  client.stop();
}
