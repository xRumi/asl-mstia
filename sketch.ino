void setup() {
  DDRB = 0b1111; // set output pins
  PORTB = 1 << 4; // pull input to HIGH  
}

void loop() {
  static byte counter = 0;
  static byte prevBtn = 0;

  // read push button
  // btn = 1, if pushed
  // btn = 0, if not
  byte btn = !(PINB & (1 << 4));

  if (btn && !prevBtn) {
    // push button pressed

    counter = (counter + 1) % 16;
    PORTB = (PORTB & 0xF0) | counter;
  }

  prevBtn = btn;
  delay(50); // debounce
}
