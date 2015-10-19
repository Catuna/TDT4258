#include "efm32gg.h"

struct ch0_config {
	uint16_t *SRC_END_PTR;// = 
	long *DEST_END_PTR;// = DAC0_CH0DATA+0x4;
	long *controlAddress;
	long *user;	
}CONFIGS[24];

