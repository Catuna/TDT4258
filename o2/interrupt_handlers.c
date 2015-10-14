#include <stdint.h>
#include <stdbool.h>

#include "blip.h"
#include "bang.h"
#include "lose.h"
#include "ofi.h"
#include "efm32gg.h"

#define SW1 0x01
#define SW2 0x02
#define SW3 0x04
#define SW4 0x08
#define SW5 0x10
#define SW6 0x20
#define SW7 0x40
#define SW8 0x80


/* TIMER1 interrupt handler */
int iCount=0;
int countDir=1;
int volumeLimiter=20;

int dataCounter=0;
int volume=0;
int playType=0;

void stopSound()
{
	playType=0;
	dataCounter=0;
	*GPIO_PA_DOUT = 0xFF00;
	*TIMER1_CMD &= (~1);
	*DAC0_CH0CTRL &= (~1);	//disable channel 0
  	*DAC0_CH1CTRL &= (~1);	//disable channel 1
}

void enableSound()
{
	stopSound();
	*TIMER1_CMD = 1;
	*DAC0_CH0CTRL |= 1;	//enable channel 0
  	*DAC0_CH1CTRL |= 1;	//enable channel 1
}

void __attribute__ ((interrupt)) DAC0_IRQHandler()
{
	switch(playType)
	{
	case 1:
	    *DAC0_CH0DATA=blipData[dataCounter];
	    *DAC0_CH1DATA=blipData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=blipLength)
	    	stopSound();
		break;

	case 2:
	    *DAC0_CH0DATA=bangData[dataCounter];
	    *DAC0_CH1DATA=bangData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=bangLength)
	    	stopSound();

		break;
	case 3:
	    *DAC0_CH0DATA=ofiData[dataCounter];
	    *DAC0_CH1DATA=ofiData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=ofiLength)
	    	stopSound();

		break;
		
	case 4:
	    *DAC0_CH0DATA=loseData[dataCounter];
	    *DAC0_CH1DATA=loseData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=loseLength)
	    	stopSound();

		break;
		
	}
	*DAC0_IFC = *DAC0_IF;
}


/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	*GPIO_IFC = *GPIO_IF;		//clear interrupt flag
	if((~*GPIO_PC_DIN) & SW1)
	{
		enableSound();
		playType=1;
		*GPIO_PA_DOUT=0xFE00;
		*TIMER1_TOP = (int)(48000000L/(3.5f*(44100L/4L)));
	}
	if((~*GPIO_PC_DIN) & SW3)
	{
		enableSound();
		playType=3;
		*GPIO_PA_DOUT=0xFB00;
		*TIMER1_TOP = (int)(48000000L/(3.5f*(44100L)/4L));
	}

  /*int * a;
  int b;
  a = &b;
  b=*a;*/
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
  	*GPIO_IFC = *GPIO_IF;		//Clear interrupt flag
	if((~*GPIO_PC_DIN) & SW2)
	{
		enableSound();
		playType=2;
		*GPIO_PA_DOUT=0xFD00;
		*TIMER1_TOP = (int)(48000000L/(3.5f*(44100L)/4L));
	}
	if((~*GPIO_PC_DIN) & SW4)
	{
		enableSound();
		playType=4;
		*GPIO_PA_DOUT=0xF700;
		*TIMER1_TOP = (int)(48000000L/(3.5f*(44100L)/4L));
	}

}
