#include <WiFi.h>
#include "DHTesp.h"

const int DHT_PIN = 14;
const int LED_PIN = 4;
const int BTN_PIN = 22;

DHTesp dhtSensor;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  // connect to wifi
  WiFi.begin("Wokwi-GUEST", "", 6);
  Serial.println("Attempting to connect to WiFi.");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation

  static unsigned long elapsedTime1, elapsedTime2;
  static int isConnected = 0;

  if (WiFi.status() != WL_CONNECTED) {
    if (isConnected) Serial.println("Wi-Fi lost.");

    if (millis() - elapsedTime1 >= 10000 || isConnected) {
      Serial.println("Attempting to connect to WiFi.");
      WiFi.reconnect();
      elapsedTime1 = millis();
    }
    isConnected = false;
  } else if (!isConnected) {
    isConnected = true;
    Serial.println("WiFi Connected.");
  }

  static int fanLed = 0;
  static int fanOverride = 0;

  // DHT22 sensor 
  if (millis() - elapsedTime2 >= 2000) {
    // send the data over WiFi or to MQTT server
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    Serial.println("Temp=" + String(data.temperature) + "°C; Hum=" + String(data.humidity) + "%");
    
    if (data.temperature >= 25.0 && !fanOverride) fanLed = 1;
    else if (data.temperature < 25.0 && !fanOverride) fanLed = 0;
    
    digitalWrite(LED_PIN, fanLed);

    elapsedTime2 = millis();
  }

  // override button
  static int prevBtn = 0;
  int btn = !digitalRead(BTN_PIN);
  
  if (btn && !prevBtn) {
    // btn pressed
    Serial.println("Override mode ON.");

    fanOverride = 1;
    fanLed ^= 1;
    digitalWrite(LED_PIN, fanLed);
  }
  prevBtn = btn;
  delay(40); // debounce
}