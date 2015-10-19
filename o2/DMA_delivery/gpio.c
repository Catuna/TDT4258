#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to set up GPIO mode and interrupts*/
void setupGPIO()
{
  /* TODO set input and output pins for the joystick */
  
  /* Example of HW access from C code: turn on joystick LEDs D4-D8
   check efm32gg.h for other useful register definitions
  */
 
  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO; /* enable GPIO clock*/
  *GPIO_PA_CTRL = 2;  /* set high drive strength */
  *GPIO_PA_MODEH = 0x55555555; /* set pins A8-15 as output */
  *GPIO_PA_DOUT = 0xFF00; /* Turn off all LEDs. To turn on all LEDs write 0x00FF*/

  *GPIO_PC_MODEL = 0x33333333;
  *GPIO_PC_DOUT = 0xFF;


  *GPIO_EXTIPSELL = 0x22222222;
  *GPIO_EXTIFALL = 0xFF;
  *GPIO_IEN = 0xFF;
  *ISER0 = 0x802 | (1/*DMA*/)|(1<<8/*DAC*/);//TODO should not go here!  (1<<12 TIMER1)
}



 
