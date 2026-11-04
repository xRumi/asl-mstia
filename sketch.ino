const int SPEED_STEP = 50;
const int SPEED_INIT = 300;

void setup() {
  Serial.begin(9600); // setup serial
  Serial.println("Current Speed: " + String(SPEED_INIT));
  
  DDRD = 0xF0; // set output pins
  PORTD = 0;
}

void loop() {
  static int speed = SPEED_INIT;
  static bool toRight = true;
  static byte led = 1;

  // shift led
  if (toRight) led <<= 1;
  else led >>= 1;
  // change direction if reached the end
  if (led == (1 << 3) || led == 1) toRight ^= 1;

  PORTD = (PORTD & 0x0F) | (led << 4);

  // read input
  if (Serial.available() > 0) {
    char input = Serial.read();

    if (input == 'f' || input == 's') {
      // change speed on input
      if (input == 'f') speed -= SPEED_STEP;
      else speed += SPEED_STEP;

      // limit speed to positive value
      // as delay(-ve) is infinite delay
      if (speed >= 0) Serial.println("New Speed: " + String(speed) + " ms");
      else {
        Serial.println("Speed can not go any lower.");
        speed = 0;
      }
    }
  }
  
  delay(speed);
}
