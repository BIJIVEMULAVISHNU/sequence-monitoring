#include <SPI.h>

#include <LoRa.h>

#include "DHT.h"

#define DHTPIN A0     // what pin we're connected to

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

int hum;

float temp; //Stores temperature value

int counter = 0;

void setup() {    

  Serial.begin(9600);

  dht.begin();

  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {

    Serial.println("Starting LoRa failed!");

    while (1);

  }

}

void loop() {

  temp = dht.readTemperature();

  hum = dht.readHumidity();

  Serial.print("Sending packet: ");

  Serial.println(counter);

  // send packet

  LoRa.beginPacket();

  LoRa.print("Humidity: ");

  LoRa.print(hum);

  LoRa.print("c");

  LoRa.print("Temperature:");

  LoRa.print(temp);

  LoRa.endPacket();

  counter++;

  delay(5000);

}
