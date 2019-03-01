#include <detpic32.h>

int main(void) {
  TRISBbits.TRISB6 = 0;
  LATBbits.LATB6 = 0;
  TRISBbits.TRISB4 = 1;     // sets pin RB4 as input
  AD1PCFGbits.PCFG4 = 0;    // RB4 configured as analog input
  AD1CON1bits.SSRC = 7;
  AD1CON1bits.CLRASAM = 1;
  AD1CON3bits.SAMC = 16;
  AD1CON2bits.SMPI = 0;     // AD1CON2bits.SMPI = XX-1 => XX=1
  AD1CHSbits.CH0SA = 4;
  AD1CON1bits.ON = 1;
  volatile int aux;

  while(1) {
    LATBbits.LATB6 = 1;
    AD1CON1bits.ASAM = 1;         // starts conversion
    while(IFS1bits.AD1IF == 0);   // wait until end of conversion
    LATBbits.LATB6 = 0;
    aux = ADC1BUF0;
    //printInt(aux, 16 | 3 << 16);
    IFS1bits.AD1IF = 0;           // reset bit AD1IF
  }

  return 0;
}
