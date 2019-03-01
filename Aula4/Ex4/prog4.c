#include <detpic32.h>

void delay(int ms);

void main(void) {
  TRISB = (TRISB & 0x80FF);   //sets pins B8-B14 as output
  TRISD = (TRISD & 0xFF9F);   //sets pins D5-D6 as output
  LATB = (LATB & 0x0000);     //sets the output of B8-B14 as '0'
  LATDbits.LATD5 = 1;         //display low control
  LATDbits.LATD6 = 0;         //display high control

  unsigned char segment;

  while(1) {
    LATDbits.LATD5 = !LATDbits.LATD5;
    LATDbits.LATD6 = !LATDbits.LATD6;
    segment = 1;
    int i;

    for(i = 0; i < 7; i++) {
      LATB = (LATB & 0x0000) | (segment << 8);
      delay(1000);   //T = 1000ms => f = 1Hz
      segment = segment << 1;
    }
  }
}

void delay(int ms) {
   for(; ms > 0; ms--) {
      resetCoreTimer();
      while(readCoreTimer() < 20000);
   }
}
