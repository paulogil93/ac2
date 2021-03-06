#include <detpic32.h>

void delay(int ms);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);

void main (void) {
  TRISB = TRISB & 0x00FF;
  TRISD = TRISD & 0xFF9F;
  LATB = LATB & 0x0000;
  LATD = LATD & 0x0000;

  unsigned char counter = 0;

  while(1) {
    int i = 0;
    do {
      delay(10);
      send2displays(toBcd(counter));
    } while(++i < 20);
    counter = ++counter%60;
  }
}

void send2displays(unsigned char value) {
  static const char codes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                               0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

  static char displayFlag = 0;
  char isOdd;
  int digit_low = value & 0x0F;
  int digit_high = value >> 4;

  if(value % 2 == 0) {
    isOdd = 1;
  } else {
    isOdd = 0;
  }

  if(displayFlag == 0) {
    LATDbits.LATD5 = 1;
    LATDbits.LATD6 = 0;
    LATB = (LATB & 0x0000) | codes[digit_low] << 8;
    if(isOdd == 1) {
      LATB = LATB | 0x8000;
    }
  } else {
    LATDbits.LATD5 = 0;
    LATDbits.LATD6 = 1;
    LATB = (LATB & 0x0000) | codes[digit_high] << 8;
  }

  displayFlag = !displayFlag;

}

unsigned char toBcd(unsigned char value) {
  return ((value / 10) << 4) + (value % 10);
}

void delay(int ms) {
   for(; ms > 0; ms--) {
      resetCoreTimer();
      while(readCoreTimer() < 20000);
   }
}
