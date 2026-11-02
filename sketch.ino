#include <WiFi.h>
#include <HTTPClient.h>

HTTPClient http;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Connecting to WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6); // connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  http.setReuse(true);
  http.setTimeout(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation

  static unsigned long elapsedSinceLastRequest = 0;
  static int potValue = -1;
  int newPotValue = analogRead(34);

  // only report if it changes
  if (newPotValue != potValue) {
    potValue = newPotValue;
    Serial.println("pot: " + String(newPotValue));
  }

  if (millis() - elapsedSinceLastRequest >= 3000) {
    elapsedSinceLastRequest = millis();
    http.begin("http://httpbin.org/get?sensor=" + String(potValue));
    int responseCode = http.GET();
    Serial.println("HTTP GET = " + String(responseCode));
    http.end();
  }
}
