#include <detpic32.h>

void delay(int ms);
int tension(int value);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);

int main(void) {
  TRISB = TRISB & 0x00FF;
  TRISD = TRISD & 0xFF9F;
  LATB = LATB & 0x0000;
  LATD = LATD & 0x0000;
  TRISBbits.TRISB4 = 1;     // sets pin RB4 as input
  AD1PCFGbits.PCFG4 = 0;    // RB4 configured as analog input
  AD1CON1bits.SSRC = 7;
  AD1CON1bits.CLRASAM = 1;
  AD1CON3bits.SAMC = 16;
  AD1CON2bits.SMPI = 3;     // AD1CON2bits.SMPI = XX-1
  AD1CHSbits.CH0SA = 4;
  AD1CON1bits.ON = 1;

  int i = 0;
  int voltage = 0;

  while(1) {
    delay(10);        //100Hz display refresh rate
    if(++i == 25) {   //250ms => f=4Hz
      AD1CON1bits.ASAM = 1;
      while(IFS1bits.AD1IF == 0);
      int average = ADC1BUF0 + ADC1BUF1 + ADC1BUF2 + ADC1BUF3;
      average = (int) average / 4;
      voltage = tension(average);
      i = 0;
      IFS1bits.AD1IF = 0;
    }
    send2displays(toBcd(voltage));
  }
  return 0;
}

int tension(int value) {
  return (value*33+511)/1023;
}

void delay(int ms) {
   for(; ms > 0; ms--) {
      resetCoreTimer();
      while(readCoreTimer() < 20000);
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

  if(displayFlag == 0) {
    LATDbits.LATD5 = 1;
    LATDbits.LATD6 = 0;
    LATB = (LATB & 0x0000) | codes[digit_low] << 8;
  } else {
    LATDbits.LATD5 = 0;
    LATDbits.LATD6 = 1;
    LATB = (LATB & 0x0000) | codes[digit_high] << 8 | 0x8000;
  }

  displayFlag = !displayFlag;   // alternates between displays low/high
}
