#include <detpic32.h>

void delay(int ms);

void main(void) {
  LATDbits.LATD0 = 0;   //initialize RD0
  TRISDbits.TRISD0 = 0; //RD0 configured as output

  while(1) {
    delay(500);
    LATDbits.LATD0 = !LATDbits.LATD0;
  }
}

void delay(int ms) {
   for(; ms > 0; ms--) {
      resetCoreTimer();
      while(readCoreTimer() < 20000);
   }
}
