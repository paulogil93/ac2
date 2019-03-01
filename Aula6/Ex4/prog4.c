#include <detpic32.h>
#define VECTOR 27           // PIC32 AD1 vector number

volatile unsigned char value2display = 0; // global variable

void delay(int ms);
int tension(int value);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);

int main(void) {
  TRISB = TRISB & 0x00FF;   // sets pins B8-B15 as output
  TRISD = TRISD & 0xFF9F;   // sets pins D5-D6 as output
  LATB = LATB & 0x0000;     // resets pins B8-B15
  LATD = LATD & 0x0000;     // resets pins D5-D6
  TRISBbits.TRISB6 = 0;     // sets pin B6 as output
  TRISBbits.TRISB4 = 1;     // sets pin RB4 as input
  AD1PCFGbits.PCFG4 = 0;    // RB4 configured as analog input
  AD1CON1bits.SSRC = 7;
  AD1CON1bits.CLRASAM = 1;
  AD1CON3bits.SAMC = 16;
  AD1CON2bits.SMPI = 7;     // AD1CON2bits.SMPI = XX-1 => XX=3
  AD1CHSbits.CH0SA = 4;
  AD1CON1bits.ON = 1;

  //  Interrupt system
  IPC6bits.AD1IP = 2;       // configure priority of A/D interrupts
  IEC1bits.AD1IE = 1;       // enable A/D interrupts
  IFS1bits.AD1IF = 0;       // clear A/D interrupt flag
  EnableInterrupts();
  int i = 0;

  while(1) {
    delay(10);              // Refresh rate = 100Hz
    if(i++ == 25) {
      AD1CON1bits.ASAM = 1;
      i = 0;
    }
    send2displays(toBcd(value2display));
  }

  return 0;
}

// Interrupt handler
void _int_(VECTOR) isr_adc(void) {
  int average = 0;
  int *p = (int *) (&ADC1BUF0);

  for(; p <= (int *) (&ADC1BUFF); p+=4) {
    average += *p;
  }

  average = (int) (average / 8);
  value2display = tension(average);
  IFS1bits.AD1IF = 0;
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

  static char displayFlag = 0;    // saves value between calls to the function
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
