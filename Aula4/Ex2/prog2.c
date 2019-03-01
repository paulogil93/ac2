#include <detpic32.h>

void delay(int ms);

void main(void) {
  LATE = (LATE & 0x0000);
  TRISE = (TRISE & 0x0000);
  int count = 0;

  while(1) {
    LATE = count;
    count++;
    delay(250);
  }
}

void delay(int ms) {
   for(; ms > 0; ms--) {
      resetCoreTimer();
      while(readCoreTimer() < 20000);
   }
}
