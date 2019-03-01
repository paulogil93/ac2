#include <detpic32.h>

void main(void) {
  TRISB = (TRISB & 0x00FF);   //sets pins B8-B15 as output
  TRISD = (TRISD & 0xFF9F);   //sets pins D5-D6 as output
  LATB = (LATB & 0x0000);     //sets the output of B8-B15 as '0'
  LATDbits.LATD5 = 0;         //display low control
  LATDbits.LATD6 = 1;         //display high control

  //code = {'a','b','c','d','e','f','g','.'};
  static const unsigned int code[8] = {0x0100, 0x0200, 0x0400, 0x0800,
                                       0x1000, 0x2000, 0x4000, 0x8000};
  char key = 0;
  static const int seg = 0x61;
  int index;

  while(1) {
    while(key != '.' && (key < 0x61 || key > 0x67)) {
      key = getChar();
    };

    //LATB = LATB & 0x0000;                 --> clears the segments
    //LATB = LATB | code[index];            --> lights up the correct segment
    //LATB = (LATB & 0x0000) | code[index]; --> last two lines combined

    index = (int)key - seg;

    if(index >= 0 && index < 7) {
      LATB = (LATB & 0x0000) | code[index];
    } else if(key == '.') {
      LATB = (LATB & 0x0000) | code[7];
    }

    key = 0;
  }
}
