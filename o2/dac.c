#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupDAC()
{
	*CMU_HFPERCLKEN0 |= (1<<17);      //Enable DAC0 in the CMU
	*DAC0_CTRL = 0x50010;             //Set outmode to pin

	//We don't want to enable the channels yet because we are not going to play sounds for some time

	//setup prs and make timer trigger conversion start
	*CMU_HFPERCLKEN0|=(1<<15);		             //set clock to enable PRS

	*PRS_CH0_CTRL|=(0b011101<<16)|(0b001);     //Enable TIMER1 as input and set TIMER1 overflow as input signal
	*DAC0_CH0CTRL|=(0<<4);				             //Select prschannel 0
	*DAC0_CH0CTRL|=(1<<2);				             //Enable dacch0 to trigger on PRS
	*DAC0_CH1CTRL|=(0<<4);										//select PRSchannel 0
	*DAC0_CH1CTRL|=(1<<2);										//enabble DACCH1 to trigger on PRS
	*DAC0_IEN |= 1;				                     //enable interrupt from CH0
	*ISER0|=(1<<8);                            //Setup NVIC to enable interrupt from DAC0
}
