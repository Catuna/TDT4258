#include <stdint.h>
#include <stdbool.h>

#include "test.h"
#include "bang.h"
//#include "stelmosfire.h"
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

void resetSound()
{
	playType=0;
	dataCounter=0;
	*GPIO_PA_DOUT = 0xFF00;
}

void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */  
    *TIMER1_IFC = 1;
	switch(playType)
	{
	case 1:
	    *DAC0_CH0DATA=testData[dataCounter];
	    *DAC0_CH1DATA=testData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=testLength)
	    	resetSound();
		break;

	case 2:
	    *DAC0_CH0DATA=bangData[dataCounter];
	    *DAC0_CH1DATA=bangData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=bangLength)
	    	resetSound();

		break;
	case 3:
	    /*DAC0_CH0DATA=stelmosfireDataCh1[dataCounter];
	    *DAC0_CH1DATA=stelmosfireDataCh1[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=stelmosfireLength)
	    	resetSound();*/

		break;
		
	}
    /*if(*GPIO_PC_DIN < (0x00FF))
    	 *GPIO_PA_DOUT = 0x0;
	
    else
	*GPIO_PA_DOUT = 0xFF00;*/
		//dataCounter++;
	//DAC0_CH0DATA and DAC0_CH1DAT
  /* if(iCount+countDir>80)
    countDir=1;
  if(iCount+countDir<=0)
    countDir=-1;
  iCount+=countDir;
  *DAC0_CH0DATA = iCount*2;
  *DAC0_CH1DATA = iCount*2;

  *GPIO_PA_DOUT += 1;*/ 
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
  //*GPIO_PA_DOUT = (*GPIO_PC_DIN<<8);
  *GPIO_IFC = *GPIO_IF;

	if((~*GPIO_PC_DIN) & SW1)
	{
		playType=1;
		dataCounter=0;
		*GPIO_PA_DOUT=0xFE00;
	}
	if((~*GPIO_PC_DIN) & SW3)
	{
		playType=3;
		dataCounter=0;
		*GPIO_PA_DOUT=0xFB00;
	}

  /*int * a;
  int b;
  a = &b;
  b=*a;*/
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
  //*GPIO_PA_DOUT = (*GPIO_PC_DIN<<8);
  *GPIO_IFC = *GPIO_IF;

	if((~*GPIO_PC_DIN) & SW2)
	{
		playType=2;
		dataCounter=0;
		*GPIO_PA_DOUT=0xFD00;
	}

}
