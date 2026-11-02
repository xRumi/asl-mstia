void setup() {
  DDRB = 0b1111; // set output pins
  PORTB = 1 << 4; // pull input to HIGH  
}

void loop() {
  static byte counter = 0;
  static bool prevBtn = false;

  byte btn = !(PINB & (1 << 4));

  if (btn && !prevBtn) {
    // push button pressed

    counter = (counter + 1) % 16;
    PORTB = (PORTB & 0xF0) | counter;
  }

  prevBtn = btn;
  delay(50); // debounce
}
