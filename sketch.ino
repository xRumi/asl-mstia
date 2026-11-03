#include <WiFi.h>
#include <PubSubClient.h>

void callback(const char* topic, byte* payload, unsigned int length);

WiFiClient espClient;
PubSubClient client("broker.emqx.io", 1883, callback, espClient);

const int BTN_1_PIN = 34;
const int BTN_2_PIN = 27;
const int RELAY_1_PIN = 25;
const int RELAY_2_PIN = 26;
const char* MQTT_CLIENT_ID = "esp32_asl_17";
const char* MQTT_CMD_TOPIC = "esp32_asl_17/cmd";

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker... ");

    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("Connected!");
      client.subscribe(MQTT_CMD_TOPIC);
    } else {
      Serial.println("Err = " + String(client.state()));
      delay(1000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BTN_1_PIN, INPUT_PULLUP); // useless as it is input only pin
  pinMode(BTN_2_PIN, INPUT_PULLUP);
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);

  // connect to wifi
  Serial.print("Connecting to WiFi.");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }
  Serial.println(" Connected!");
}

int relay1, relay2;
int prevBtn1, prevBtn2;

void buttonHandler(int pin) {
  int& prevBtn = pin == BTN_1_PIN ? prevBtn1 : prevBtn2;
  int& relay = pin == BTN_1_PIN ? relay1 : relay2;
  int relayPin = pin == BTN_1_PIN ? RELAY_1_PIN : RELAY_2_PIN;
  int btn;

  if (pin < 32) btn = !(REG_READ(GPIO_IN_REG) & (1 << pin));
  else btn = !(REG_READ(GPIO_IN1_REG) & (1 << (pin - 32)));

  if (btn && !prevBtn) {
    // btn pressed

    relay ^= 1;
    digitalWrite(relayPin, relay);
  }

  prevBtn = btn;
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation

  if (!client.loop()) reconnectMQTT();

  buttonHandler(BTN_1_PIN);
  buttonHandler(BTN_2_PIN);
  delay(40); // debounce
}

void callback(const char* topic, byte* payload, unsigned int length) {

  String msg(reinterpret_cast<const char*>(payload), length);
  Serial.println("topic: " + String(topic) + "; msg = " + msg);

  if (msg == "ON") {
    relay1 = 1;
    relay2 = 1;
  } else if (msg == "OFF") {
    relay1 = 0;
    relay2 = 0;
  }
  digitalWrite(RELAY_1_PIN, relay1);
  digitalWrite(RELAY_2_PIN, relay2);
}
