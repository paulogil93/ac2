#include <detpic32.h>

void delay(int ms);
void send2displays(unsigned char value);

void main (void) {
  TRISB = TRISB & 0x00FF;   // sets pins B8-B15 as output
  TRISD = TRISD & 0xFF9F;   // SETS PINS D5-D6 as output
  LATB = LATB & 0x0000;
  LATD = LATD & 0x0000;

  unsigned char counter = 0;

  while(1) {
    int i = 0;
    do {
      delay(10);                // refresh freq = 100Hz => 10ms
      send2displays(counter);
    } while(++i < 20);          // increment freq = 5Hz => 10*20=200ms
    counter = ++counter%256;
  }
}

unsigned char toBcd(unsigned char value) {
  return ((value / 10) << 4) + (value % 10);
}

void send2displays(unsigned char value) {
  static const char codes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                               0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

  static char displayFlag = 0;    // saves value between call to the function
  int digit_low = value & 0x0F;
  int digit_high = value >> 4;
  int point;

  if(digit_low % 2 == 0) {
    point = 0x8000;
  } else {
    point = 0;
  }

  if(displayFlag == 0) {
    LATDbits.LATD5 = 1;
    LATDbits.LATD6 = 0;
    LATB = (LATB & 0x0000) | codes[digit_low] << 8 | 0x8000;
  } else {
    LATDbits.LATD5 = 0;
    LATDbits.LATD6 = 1;
    LATB = (LATB & 0x0000) | codes[digit_high] << 8 | point;
  }

  displayFlag = !displayFlag;   // alternates between displays low/high
}

void delay(int ms) {
   for(; ms > 0; ms--) {
      resetCoreTimer();
      while(readCoreTimer() < 20000);
   }
}
