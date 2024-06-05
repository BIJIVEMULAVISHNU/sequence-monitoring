#include <SoftwareSerial.h>

#define LM35_PIN A0  // LM35 temperature sensor pin
#define GAS_SENSOR_PIN A1  // Gas sensor (MQ2) pin
#define SPRINKLER_PIN 2  // Sprinkler control pin
#define BREAK_PRESSURE_PIN A2  // Break pressure sensor pin
#define BUZZER_PIN 3  // Buzzer pin
#define SOS_BUTTON_PIN 4  // SOS button pin

SoftwareSerial gsmSerial(7, 8);  // RX, TX for GSM module

// Define threshold values
#define FIRE_THRESHOLD 30.0  // Adjust as needed
#define GAS_THRESHOLD 200  // Adjust as needed
#define BREAK_PRESSURE_THRESHOLD 500  // Adjust as needed

void setup() {
  pinMode(LM35_PIN, INPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(SPRINKLER_PIN, OUTPUT);
  pinMode(BREAK_PRESSURE_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SOS_BUTTON_PIN, INPUT);

  Serial.begin(9600);
  gsmSerial.begin(9600);
}

void loop() {
  // Read sensors
  float temperature = readTemperature();
  int gasValue = readGasSensor();
  int breakPressure = readBreakPressure();
  int sosButtonState = digitalRead(SOS_BUTTON_PIN);

  // Check for fire conditions
  if (temperature > FIRE_THRESHOLD || gasValue > GAS_THRESHOLD) {
    // Fire detected
    activateSprinkler();
    sendGSMMessage("Fire detected! Please evacuate.");
    soundBuzzer();
  }

  // Check for emergency brake
  if (breakPressure > BREAK_PRESSURE_THRESHOLD || sosButtonState == HIGH) {
    // Activate emergency procedures
    activateSprinkler();
    sendGSMMessage("Emergency brake activated! Evacuate immediately.");
    soundBuzzer();
  }

  delay(1000);  // Adjust the delay as needed
}

float readTemperature() {
  // Read LM35 temperature sensor and convert to Celsius
  int rawValue = analogRead(LM35_PIN);
  float voltage = (rawValue / 1024.0) * 5.0;
  float temperature = (voltage - 0.5) * 100.0;
  return temperature;
}

int readGasSensor() {
  // Read gas sensor value
  return analogRead(GAS_SENSOR_PIN);
}

int readBreakPressure() {
  // Read break pressure sensor value
  return analogRead(BREAK_PRESSURE_PIN);
}

void activateSprinkler() {
  // Activate sprinkler system
  digitalWrite(SPRINKLER_PIN, HIGH);
}

void sendGSMMessage(String message) {
  // Send SMS using GSM module
  gsmSerial.println("AT+CMGF=1");
  delay(100);
  gsmSerial.println("AT+CMGS=\"+918106312321\"");  // Replace with the destination number
  delay(100);
  gsmSerial.println(message);
  delay(100);
  gsmSerial.println((char)26);  // End SMS
  delay(1000);
}

void soundBuzzer() {
  // Activate the buzzer
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}
