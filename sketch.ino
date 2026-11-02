void setup() {
  Serial.begin(9600); // setup serial
  DDRD = 0xF0; // set output pins
}

const int fastSpeed = 100, slowSpeed = 300;

void loop() {
  static bool goFast = false, toRight = true;
  static byte led = 1;

  // shift led
  if (toRight) led <<= 1;
  else led >>= 1;

  if (led == (1 << 3) || led == 1) toRight ^= 1;

  PORTD = (PORTD & 0x0F) | (led << 4);

  // read input
  if (Serial.available() > 0) {
    char input = Serial.read();

    if (input == 'f' || input == 's') {
      goFast = (input == 'f');

      Serial.print("New Speed: ");
      Serial.print(goFast ? fastSpeed : slowSpeed);
      Serial.println(" ms");
    }
  }
  
  delay(goFast ? fastSpeed : slowSpeed);
}
