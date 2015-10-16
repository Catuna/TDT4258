#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to set up GPIO mode and interrupts*/
void setupGPIO()
{

  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;  //enable GPIO clock
  *GPIO_PA_CTRL = 2;                          //set high drive strength
  *GPIO_PA_MODEH = 0x55555555;                //set pins A8-15 as output
  *GPIO_PA_DOUT = 0xFF00;                     //Turn off all LEDs. To turn on all LEDs write 0x00FF

  *GPIO_PC_MODEL = 0x33333333;                //Set pins C0 - C7 as input
  *GPIO_PC_DOUT = 0xFF;                       //Set pins to active low?


  *GPIO_EXTIPSELL = 0x22222222;               //Set all pins to trigger interrupt
  *GPIO_EXTIFALL = 0xFF;                      //Set pins to trigger interrupt on falling edge
  *GPIO_IEN = 0xFF;                           //Enable interrupt
  *ISER0 |= 0x802;                            //enable interrupt generation for gpio even and odd.
}



 
