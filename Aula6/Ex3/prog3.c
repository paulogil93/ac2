#include <detpic32.h>
#define VECTOR 27           // PIC32 AD1 vector number

int main(void) {
  TRISBbits.TRISB6 = 0;     // sets pin B6 as output
  TRISBbits.TRISB4 = 1;     // sets pin RB4 as input
  AD1PCFGbits.PCFG4 = 0;    // RB4 configured as analog input
  AD1CON1bits.SSRC = 7;
  AD1CON1bits.CLRASAM = 1;
  AD1CON3bits.SAMC = 16;
  AD1CON2bits.SMPI = 0;     // AD1CON2bits.SMPI = XX-1 => XX=1
  AD1CHSbits.CH0SA = 4;
  AD1CON1bits.ON = 1;

  //  Interrupt system
  IPC6bits.AD1IP = 2;       // configure priority of A/D interrupts
  IEC1bits.AD1IE = 1;       // enable A/D interrupts
  IFS1bits.AD1IF = 0;       // clear A/D interrupt flag
  EnableInterrupts();
  AD1CON1bits.ASAM = 1;
  
  while(1) {
    LATBbits.LATB6 = 0;
  }
}

// Interrupt handler
void _int_(VECTOR) isr_adc(void) {
  printStr("ADC Value: ");
  printInt(ADC1BUF0, 16 | 3 << 16);
  putChar('\n');
  AD1CON1bits.ASAM = 1;
  IFS1bits.AD1IF = 0;
  LATBbits.LATB6 = 1;
}
