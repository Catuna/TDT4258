#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"


void setupTimer();
void setupDAC();
void wfi();
void setupGPIO();
void setSampleFrequency(int frequency);

int main(void) 
{  
  setupGPIO();                //Setup general purpose input/output
  setupDAC();                 //Setup digital to analog converter
  setupTimer();               //Setup timer
  
  /*page 140 manual. Used to prescale the clock. 
   *Only used for testing purposes*/
  //*CMU_HFPERCLKDIV = (1<<8) | (4 << 0);

  //Setup ability to deep sleep
  *SETUP_SCR |= 6;

  while(1)
  {
	   wfi();	 //call wait for inerrupt
  }

  return 0;
}

void wfi()
{
  //Compiler will insert the instructions in the appropriate position
	__asm__(
	"wfi;"
	);
}
