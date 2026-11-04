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

  // connect to wifi and register event handlers
  // event handlers run on core 0
  // setup and loop runs on core 1
  WiFi.begin("Wokwi-GUEST", "", 6); // non-blocking
  Serial.println("Attempting to connect to WiFi.");

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("WiFi Connected.");
  }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print("IP Address Acquired: ");
    Serial.println(WiFi.localIP());
  }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print("WiFi lost: ");
    Serial.println(info.wifi_sta_disconnected.reason);
    Serial.println("Trying to Reconnect.");
    WiFi.reconnect();
  }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation

  static unsigned long elapsedTime = 0;
  static int fanLed = 0;
  static int fanOverride = 0;

  // DHT22 sensor, read every 2s
  if (millis() - elapsedTime >= 2000) {
    // send the data over WiFi or to MQTT server
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    Serial.println("Temp=" + String(data.temperature) + "°C; Hum=" + String(data.humidity) + "%");
    
    // automatically turn on the fan LED if temperature equal or greater than 25, turn off otherwise
    // provided that override button hasn't been pressed
    if (data.temperature >= 25.0 && !fanOverride) fanLed = 1;
    else if (data.temperature < 25.0 && !fanOverride) fanLed = 0;

    digitalWrite(LED_PIN, fanLed);
    elapsedTime = millis();
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
  delay(30); // debounce
}