#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "CH0_CONF.h"
#include "bang.h"

struct Control
{
	int cycle_ctrl : 3;
	int next_useburst : 1;
	int n_minus_1 : 10;
	int r_power : 4;
	int scr_prot_ctrl : 3;
	int dst_prot_ctrl : 3;
	int src_size : 2;
	int src_inc : 2;
	int dst_size : 2;
	int dst_inc : 2;
};

struct ChannelDescriptor
{
	volatile void * volatile sourceEndPointer;     /**< DMA source address end */
  	volatile void * volatile destinationEndPointer;     /**< DMA destination address end */
 	volatile uint32_t    control;       /**< DMA control register */
 	volatile uint32_t    USER; 
	/*uint16_t* sourceEndPointer;
	uint32_t* destinationEndPointer;
	struct Control control;
	uint32_t user;*/
};//descriptors[24];

void setupDMA()
{
	struct Control ctrl;
	ctrl.cycle_ctrl = 0b001;			//set up in basic mode for testing purposes.
	ctrl.next_useburst = 0b0;
	ctrl.n_minus_1 = 50;//0b111111111;
	ctrl.r_power = 0b1010;
	ctrl.scr_prot_ctrl = 0;
	ctrl.dst_prot_ctrl = 0;
	ctrl.src_size = 0b01;
	ctrl.src_inc = 0b01;
	ctrl.dst_size = 0b10;
	ctrl.dst_inc = 0b11;	
	
	//Could use struct Conrol. This is safer and shows what happens
	
	uint32_t controlValue=
	(0b001<<0)| 		//Use Basic mode (This would be changed to ping-pong mode if we got it working)
	(0b0<<3)|			//Don't use nextburst
	(512<<4)|			//Transfer 512 elements
	(0b0<<14)|			//Set arbitration after each transfer
	(0b01<<24)|			//Size of source data is halfwords
	(0b01<<26)|			//Incremet of source data is halfwords
	(0b10<<28)|			//Destination data size is word.
	(0b11<<30);			//Don't increment destination

	//Setup DMA descriptors
	volatile struct ChannelDescriptor* descriptors  =  (volatile struct ChannelDescriptor* ) 0x20000000;	//setup the pointer
	descriptors->sourceEndPointer = (void*)&bangData[(bangLength/2)-1];	//Pointer to end of source. This should work fine since it is in the middle
	descriptors->destinationEndPointer = (void*)DAC0_CH0DATA; 			//pointer to end of destination. This is the DAC0_CH0DATA
	descriptors->control=controlValue;									//Add in the control data							

		//Pek på sluttaddressen til kilden
		//pek på sluttaddressen til destinasjonen
		//Sett konfigurasjon
			//Størrelse per dataenhet
			//Antallet overføringer
			//Hvor mye som skal inkrementeres
			//
	//Setup DMA registers
	
	*DMA_CONFIG = 1;							//Enable DMA

	*DMA_CH0_CTRL = (0b001010 <<16) ;			//Set DAC0 as source of request
	*DMA_CH0_CTRL |= (0b0000<<0);				//Set DAC0 CH0 as signal
	*DMA_CTRLBASE = (uint32_t)descriptors;		//set the address of the descriptors
	
	*DMA_CHUSEBURSTS = 1;						//Was used in some example
	*DMA_CHENS = 1;								//Enable channel 0
	*DMA_IEN = 1;				//enable interrupt on channel 0
	*DMA_IFC = 1;				//clear pending requests
	
		//sett mode (ping pong eller basic)
		//Sett trigger. Hvilken kanal på periferialen. Sjekk at periferialen er satt opp riktig til å trigge DMA request
		//sett opp interrupt når alle samples er overført
		//kanskje prioritet.
	//Setup registers in trigger peripheral

	//Sett PRSEN, PRSSEL
	*CMU_HFPERCLKEN0|=(1<<15)|(1<<6);		//set clock to enable PRS and DMA
	*CMU_HFCORECLKEN0|=1;				//Set CMU to ...
	*PRS_CH0_CTRL|=(0b011101<<16)|(0b001);		//Enable TIMER1 as input and set TIMER1 overflow as input signal
	*DAC0_CH0CTRL|=(0<<4);				//Select prschannel 0
	*DAC0_CH0CTRL|=(1<<2);				//Enable dacch0 to trigger on PRS 
	//*DAC0_IEN |= 1;					//enable interrupt
}




//DMA_INIT
/*
*Enable DMA in CMU
*Reset DMA
	*Disable interrupts
	* Set to constant known values
*Clear pending IRQ
*Enable IRQ
*Bus interrupt error DMA->IEN = DMA_IEN_ERR
*Sett kontrollblokk DMA_CTRLBASE = controlBlock (uint32_t)
*Sett opp config registeret (uint32_t). Her skal hprot settes på riktig sted. DMA_CONFIG_EN skal settes
*/




























/*
long control=0b1101010100010100111111110001L;
	CONFIGS[0].SRC_END_PTR=&bangData[bangLenght-1];
	CONFIGS[0].DEST_END_PTR = DAC0_CH0DATA;
	CONFIGS[0].controlAddress=&control;
	CONFIGS[0].user=0;
	
	*DMA_CH0_CTRL|=(0b001010<<16)|(0b0000<<0);	//Select DAC0 as SRC_SEL and SIG_SEL on DAC_CH0.
	//*DMA_
	*DMA_CONFIG |= 1;				//Enable DMA
	//*DMA_CHREQMASKC=1;
	*DMA_CHALTC=1;
	*DMA_CHENS=1;
	*DMA_CTRLBASE = &CONFIGS[0];
*/
