/*    
    deepSleepDemo.ino
    Copyright (c) 2017 ItKindaWorks All right reserved.
    github.com/ItKindaWorks
    This file is part of ESPHelper
    ESPHelper is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    ESPHelper is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with ESPHelper.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ESPHelper.h"
#include "Metro.h"
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <EspProwl.h>

//setup macros for time
#define SECOND  1000L
#define MINUTE  SECOND * 60L
#define HOUR  MINUTE * 60L

//timers for turn off
Metro minRun = Metro(1 * MINUTE);
Metro maxRun = Metro(5 * MINUTE);

//NTP setup vars
static const char ntpServerName[] = "us.pool.ntp.org";
const int timeZone = -5;
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

//ESPHelper
netInfo homeNet = {.name = "NETWORK NICKNAME", .mqtt = "YOUR MQTT-IP", .ssid = "YOUR SSID", .pass = "YOUR NETWORK PASS"};
ESPHelper myESP(&homeNet);

//hostname for ESPHelper/OTA
char* netHostName = "NEW-ESP8266";

//initDone keeps track of initialization finished or not
bool initDone = false;

//done keeps track of whether the device has finished the task
bool done = false;

void setup() {
  
  myESP.OTA_enable();
  myESP.OTA_setPassword("YOUR_OTA_PASSWORD");
  myESP.OTA_setHostnameWithVersion(netHostName);


  // myESP.enableHeartbeat(2);    //07 / 12E ledPin
  myESP.enableHeartbeat(1);   //01 ledPin

  myESP.begin();
  myESP.setCallback(callback);

  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  EspProwl.begin();
  EspProwl.setApiKey("YOUR-PROWL-API-KEY");
  EspProwl.setApplicationName("EspProwl");

  
}

void loop(){

  if(myESP.loop() == FULL_CONNECTION){

    //once we have a full connection trigger some extra setup
    if(!initDone){
      delay(500);
      //setup the NTP connection and get the current time
      setupNTP();
      delay(500);
      //only set initDone to true if the time is set
      if(timeStatus() != timeNotSet){
        initDone = true;
      }
    }


    //if were not done running the code that we need to finish executing and we are done initializing    
    if(!done && initDone){

      //generate a char array to store the data being published
      char timeStamp[30];
      //fill the array with the time data
      createTimeString(timeStamp, 30);

      //create a string that will be published
      String pubString = String("Mailbox opened at ");
      //add the timestamp to the string
      pubString += timeStamp;

      //convert the string to a char array that can be published/pushed to IOS
      char message[50];
      pubString.toCharArray(message, 50);

      //publish/push the message to IOS
      myESP.publish("/test/uptime", message, true); 
      EspProwl.push("Mailbox ESP8266", message, 0);

      done = true;
    }
          
  }
  
  //send the ESP to deep sleep (to be woken up by pulling rst low)
  if((minRun.check() && done) || maxRun.check()){
      ESP.deepSleep(0, WAKE_RF_DEFAULT);
  } 

  yield();
}

void callback(char* topic, uint8_t* payload, unsigned int length) {

}


//generate a str with the current date/time
void createTimeString(char* buf, int length){
    time_t t = now();
    String timeString = String(hour(t));
    timeString += ":";
    timeString += minute(t);
    timeString += ":";
    timeString += second(t);
    timeString += " ";
    timeString += month(t);
    timeString += "/";
    timeString += day(t);
    timeString += "/";
    timeString += year(t);
    timeString.toCharArray(buf, length);
}









/*-------- NTP code ----------*/
void setupNTP(){
  delay(200);
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
}

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  //Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  //Serial.print(ntpServerName);
  //Serial.print(": ");
  //Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      //Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  //Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
