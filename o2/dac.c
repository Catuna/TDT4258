#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupDAC()
{
  *CMU_HFPERCLKEN0 |= (1<<17);
  *DAC0_CTRL = 0x00010;
  *DAC0_CH0CTRL = 1;	//enable channel 0
  *DAC0_CH1CTRL = 1;	//enable channel 1

	//setup prs and make timer trigger conversion start
	*CMU_HFPERCLKEN0|=(1<<15)|(1<<6);		//set clock to enable PRS and DMA
	*CMU_HFCORECLKEN0|=1;				//Set CMU to ...
	*PRS_CH0_CTRL|=(0b011101<<16)|(0b001);		//Enable TIMER1 as input and set TIMER1 overflow as input signal
	*DAC0_CH0CTRL|=(0<<4);				//Select prschannel 0
	*DAC0_CH0CTRL|=(1<<2);				//Enable dacch0 to trigger on PRS 
	*DAC0_IEN |= 1;				//enable interrupt
	*ISER0|=(1<<8/*DAC*/);
  
  /*
    TODO enable and set up the Digital-Analog Converter
    
    1. Enable the DAC clock by setting bit 17 in CMU_HFPERCLKEN0
    2. Prescale DAC clock by writing 0x50010 to DAC0_CTRL
    3. Enable left and right audio channels by writing 1 to DAC0_CH0CTRL and DAC0_CH1CTRL
    4. Write a continuous stream of samples to the DAC data registers, DAC0_CH0DATA and DAC0_CH1DATA, for example from a timer interrupt
  */
}
