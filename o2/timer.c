#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to setup the timer */
void setupTimer()
{
  *CMU_HFPERCLKEN0 |= (1<<6);            //Enable timer 1 in cmu unit
  setSampleFrequency((int)(44100L/4L));  //set the sample frequency
  *TIMER1_CMD = 1;                       //Enable timer1.
}

//set the sample frequency in hertz
void setSampleFrequency(int frequency)
{
  /*NOTE
   * If the prescaler is set, then F_CPU should be divided by this prescaler
  */
  *TIMER1_TOP = (int)(F_CPU/(3.5f*frequency));   //Divide the clock frequency of the CPU by the sample frequency of the sound
}
