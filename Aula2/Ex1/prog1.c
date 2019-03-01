void main(void) 
{ 
   int counter = 0; 
   while(1) 
   { 
      while(readCoreTimer() < 20000000); 
      resetCoreTimer(); 
      printInt(++counter, 10); 
      putChar(' ');  // space 
   } 
} 