#include <ESP8266WiFi.h>;
#include <WiFiClient.h>;
#include <ThingSpeak.h>;

const char* ssid = "Js Inter5";   // Your Network SSID
const char* password = "123@45678#";       // Your Network Password

int val;
int pin = A0; // LM35 Pin Connected at A0 Pin

WiFiClient client;

unsigned long myChannelNumber =  2398568 ; //Your Channel Number (Without Brackets)
const char * myWriteAPIKey = "0C2BUCDLVU9Z7IP5"; //Your Write API Key

const char *thingsboardServer = "thingsboard.cloud";
const char *accessToken = "Dc4yE4z3pKmvMK2ayyga";

void setup()
{
  Serial.begin(9600);
  delay(10);
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");
  Serial.print("Connected to ");
  Serial.println(ssid);
  ThingSpeak.begin(client);
}

void loop()
{
  val = analogRead(pin); // Read Analog values and Store in val variable
  Serial.print("Darkness: ");
  Serial.println(val);                 // Print on Serial Monitor
  delay(1000);
  ThingSpeak.writeField(myChannelNumber, 1,val, myWriteAPIKey); //Update in ThingSpeak
  delay(100);
}
