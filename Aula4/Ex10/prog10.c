#include <detpic32.h>

void delay(int ms);

void main (void) {
  TRISB = TRISB & 0x00FF;
  TRISD = TRISD & 0xFF9F;
  LATB = LATB & 0x0000;
  LATDbits.LATD5 = 1;
  LATDbits.LATD6 = 0;

  static const char codes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                               0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

  char value = 0x41;
  char digit_high;
  char digit_low;

  while(1) {
    digit_low = value & 0x0F;
    digit_high = (value & 0xF0) >> 4;
    LATB = (LATB & 0x0000) | codes[digit_low] << 8;
    delay(10);
    LATDbits.LATD5 = !LATDbits.LATD5;
    LATDbits.LATD6 = !LATDbits.LATD6;
    LATB = (LATB & 0x0000) | codes[digit_high] << 8;
    delay(10);
    LATDbits.LATD5 = !LATDbits.LATD5;
    LATDbits.LATD6 = !LATDbits.LATD6;
  }
}

void delay(int ms) {
   for(; ms > 0; ms--) {
      resetCoreTimer();
      while(readCoreTimer() < 20000);
   }
}
