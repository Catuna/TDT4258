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

void setSampleFrequency(int frequency);

int dataCounter=0;		//Global variable to store position in data array
int playType=0;			//Global variable that stores what sound to play

void stopSound()
{
	playType=0;				//Set play type to default value
	dataCounter=0;			//Set position to start of data
	*GPIO_PA_DOUT = 0xFF00;	//Clear LEDs
	*SETUP_SCR |= 6;		//Enable deep sleep
	//The dac channels consumes the most energy while in EM2. Turn off the channels to not produce noice when not used.
	*DAC0_CH0CTRL &= (~1);	//Disable DAC0 channel 0
  	*DAC0_CH1CTRL &= (~1);	//Disable DAC0 channel 1
}

void enableSound()
{
	stopSound();				//First, reset what ever is required
	*SETUP_SCR &= (~6);			//Disable deep sleep. WFI should now make processor enter EM1
	*DAC0_CH0CTRL |= 1;			//enable channel 0
  	*DAC0_CH1CTRL |= 1;			//enable channel 1
}

void __attribute__ ((interrupt)) DAC0_IRQHandler()
{
	switch(playType)
	{
	//if we are playing the blip data
	case 1:
	    *DAC0_CH0DATA=blipData[dataCounter];	//Push a sample from blipData to DAC0 channel 0
	    *DAC0_CH1DATA=blipData[dataCounter];	//Push a sample from blipData to DAC1 channel 1
	    dataCounter+=1;							//Use next sample next time
	    if(dataCounter>=blipLength)				//Stop sound if we are at the end of the data
	    	stopSound();
		break;

	//Same principle as case 1:
	case 2:
	    *DAC0_CH0DATA=bangData[dataCounter];	
	    *DAC0_CH1DATA=bangData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=bangLength)
	    	stopSound();

		break;

	//Same principle as case 1:
	case 3:
	    *DAC0_CH0DATA=ofiData[dataCounter];
	    *DAC0_CH1DATA=ofiData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=ofiLength)
	    	stopSound();

		break;
		
	//Same principle as case 1:
	case 4:
	    *DAC0_CH0DATA=loseData[dataCounter];
	    *DAC0_CH1DATA=loseData[dataCounter];
	    dataCounter+=1;
	    if(dataCounter>=loseLength)
	    	stopSound();

		break;
		
	}
	*DAC0_IFC = *DAC0_IF;	//clear the interrupt flag.
}


/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	*GPIO_IFC = *GPIO_IF;		//clear interrupt flag
	if((~*GPIO_PC_DIN) & SW1)	//if button 1 is pressde
	{
		enableSound();						//enable sound
		playType=1;							//play sound "blip"
		*GPIO_PA_DOUT=0xFE00;				//display LED D1
		setSampleFrequency(blipSampleRate);	//Set sample rate
	}
	if((~*GPIO_PC_DIN) & SW3)
	{
		enableSound();						//enable sound
		playType=3;							//play sound "ofi"
		*GPIO_PA_DOUT=0xFB00;				//set LED D3
		setSampleFrequency(ofiSampleRate);	//set sample rate
	}
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
  	*GPIO_IFC = *GPIO_IF;		//Clear interrupt flag
	if((~*GPIO_PC_DIN) & SW2)
	{
		enableSound();						//enable sound
		playType=2;							//play sound "bang"
		*GPIO_PA_DOUT=0xFD00;				//Display LED D2
		setSampleFrequency(bangSampleRate);	//set sample rate
	}
	if((~*GPIO_PC_DIN) & SW4)
	{
		enableSound();						//Enable sound
		playType=4;							//play sound "lose"
		*GPIO_PA_DOUT=0xF700;				//Display LED D4
		setSampleFrequency(loseSampleRate);	//set sample rate
	}

}
