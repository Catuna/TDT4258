 .syntax unified
	
	      .include "efm32gg.s"
BTN_SW1 = 0x1					//Left keypad left button
BTN_SW2 = 0x2					//Left keypad up button
BTN_SW3 = 0x4					//Left keypad right button
BTN_SW4 = 0x8					//Left keypad down button

BTN_SW6 = 0x20  				//Right keypad up
BTN_SW8 = 0x80					//Right keypad down

GPIO_HIGH_DRIVESTRENGTH = 0x2			//The specified value for high drivestrenght on the GPIO (20 mA)
GPIO_STANDARD_DRIVESTRENGTH = 0x0		//Value for standard drivestrength on the GPIO (6 mA)
GPIO_LOW_DRIVESTRENGTH = 0x3			//Value for low drivestrength on the GPIO (2 mA)
GPIO_LOWEST_DRIVESTRENGTH = 0x1			//Value for the lowest drivestrength possible on the GPIO (0,5 mA)



  .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	      .section .text


	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
  // The CPU will start executing here after a reset
	//
	/////////////////////////////////////////////////////////////////////////////

	      .globl  _reset
	      .type   _reset, %function
        .thumb_func
_reset:
	ldr r1, cmu_base_addr					//we need this to address the CMU address
	ldr r2, [r1,#CMU_HFPERCLKEN0]				//Load the value currently stored at the enable register. We don't want to overwrite current enables
	mov r3, #1						//Load 1 to be leftshifted to enable GPIO
	lsl r3, r3, #CMU_HFPERCLKEN0_GPIO			//Left shift the 1-bit to the position of GPIO
	orr r2, r2, r3						//Or with the current enable
	str r2, [r1,#CMU_HFPERCLKEN0]				//Store back to memory

	//Setup GPIO_PA as output
	ldr r1, gpio_a_base					//The base of gpio
	ldr r2, [r1,#GPIO_CTRL]					//Load the content of GPIO_CONTROL
	orr r2, r2, #GPIO_HIGH_DRIVESTRENGTH			//Set PA to high drivestrenght
	str r2, [r1,#GPIO_CTRL]					//Store the new ctrl register
	ldr r2, =0x55555555					//Set 16-bit value
	mov r3, #16						//We will left-shift r2 by this mutch
	lsl r2, r2, r3						//left shift r2 16 steps
	ldr r3, =0x55555555					//now get the rest of the bits
	orr r2,r2,r3						//or them
	str r2,[r1,#GPIO_MODEH]					//Store in modeh

	//turn off lights as default
	ldr r1, gpio_a_base					//select PA as base (A is output)
	ldr r2, =0xFFFFFFFF					//Write all pins to high (since the keypad is active high, this will turn them off)
	str r2,[r1,#GPIO_DOUT]					//store the value
	

	//Setup GPIO_PC as input
	ldr r1, gpio_c_base					//Set c as base
	ldr r2,=0x33333333					//Set the PC_MODEL
	str r2,[r1,#GPIO_MODEL]					//enable pc as input
	mov r2, #0xff
	str r2,[r1,#GPIO_DOUT]					//write 0xff to GPIO_PC_DOUT to enable internal pull-up



	//Setup GPIO as interrupt
	ldr r1,gpio_base					//setup the GPIO as base (The gpio manager)
	ldr r2,=0x22222222					
	
	str r2,[r1,#GPIO_EXTIPSELL]				//Store 0x22222222 in GPIO_EXTIPSHELL. This will select wich gpio_units that will be used
	mov r2,#0xFF
	str r2,[r1,#GPIO_EXTIFALL]				//Set GPIO to trigger on falling edge (The button is pressed)
	str r2,[r1,#GPIO_IEN]					//Enable gpio
	
	ldr r1,iser0						//set iser as base
	ldr r2,iser0_setup_value				//Set ISER (the interrupt-controller) and enable odd gpio and even gpio
	str r2,[r1]						//Store this value


	//Setup ability to deep sleep
	ldr r1,scr_base						//switch to scr base
	mov r2,#6	
	str r2,[r1]						//store 6 in scr. This should enable deep sleep


	b loop							//jump to the loop

loop:
	
	wfi							//enter deep sleep
	
	b loop							//branch back

scr_base:
	.long SCR

cmu_base_addr:
	.long CMU_BASE 			

gpio_a_base:
	.long GPIO_PA_BASE

gpio_c_base:
	.long GPIO_PC_BASE

gpio_base:
	.long GPIO_BASE

iser0:
	.long ISER0

//All the button addresses here does the trick with the lighting.

SW1_handler:
	ldr r1,gpio_a_base
	ldr r2,[r1,#GPIO_DOUT]					//load the contents of output
	mov r3, #1
	lsr r2,r2,r3						//rightshift by 1
	ldr r3,=0x8000
	orr r2,r2,r3						//we want to set the incoming bit

	str r2,[r1,#GPIO_DOUT]
	
	b end_button_check

SW2_handler:
	ldr r1,gpio_a_base
	ldr r2,[r1,#GPIO_DOUT]
	mvn r3,#0x100						//we want to invert it in order to clear the bit
	and r2,r2,r3						//clear the bit
	
	str r2,[r1,#GPIO_DOUT]

	b end_button_check

SW3_handler:
	ldr r1,gpio_a_base
	ldr r2,[r1,#GPIO_DOUT]					//load the contents of output
	mov r3, #1
	lsl r2,r2,r3						//leftshift by 1
	mov r3, #1
	orr r2,r2,r3						//we want to set the incoming bit so there will be no lights
	
	str r2,[r1,#GPIO_DOUT]

	b end_button_check

SW4_handler:
	ldr r1,gpio_a_base
	ldr r2,[r1,#GPIO_DOUT]
	mvn r3,#0x8000						//we want to invert it in order to clear the bit
	and r2,r2,r3						//clear the bit
	
	str r2,[r1,#GPIO_DOUT]

	b end_button_check



SW6_handler:
	ldr r1, gpio_a_base
	ldr r2, [r1, #GPIO_CTRL]
	ldr r3, =0xFFFFFFFC
	and r2,r2,r3
	orr r2, r2, #GPIO_HIGH_DRIVESTRENGTH
	str r2, [r1,#GPIO_CTRL]
	
	b end_button_check

SW8_handler:
	ldr r1, gpio_a_base
	ldr r2, [r1, #GPIO_CTRL]
	ldr r3, =0xFFFFFFFC
	and r2,r2,r3
	orr r2, r2, #GPIO_LOWEST_DRIVESTRENGTH
	str r2, [r1,#GPIO_CTRL]

	b end_button_check
	
        .thumb_func 						//We must set this in order for the assembler to treat this as a function 
gpio_handler:
	//Do cool stuff with lighting
	ldr r1,gpio_c_base
	ldr r2,[r1,#GPIO_DIN]
	mvn r2,r2						//the input is inverted, so we want to invert it. Now 1 means a pushdown

	mov r3, #BTN_SW1						
	and r3, r2, r3
	cmp r3,#BTN_SW1					//check if the left button is pressed on the left side of the gamepad
	beq SW1_handler

	mov r3, #BTN_SW2
	and r3,r2,r3
	cmp r3,#BTN_SW2						//check if the up button is pressed on the left side of the gamepad
	beq SW2_handler

	mov r3, #BTN_SW3
	and r3,r2,r3
	cmp r3,#BTN_SW3					//check if the right button is pressed on the left side of the gamepad
	beq SW3_handler

	mov r3, #BTN_SW4
	and r3,r2,r3
	cmp r3,#BTN_SW4					//check if the down button is pressed on the left side of the gamepad
	beq SW4_handler


	mov r3, #BTN_SW6					//check if the up button is pressed on the right side of the gamepad
	and r3, r2, r3
	cmp r3,#BTN_SW6
	beq SW6_handler

	mov r3, #BTN_SW8					//check if the down button is pressed on the right side of the gamepad
	and r3, r2, r3
	cmp r3,#BTN_SW8
	beq SW8_handler


end_button_check:						//branching done. Return to this point
	
	ldr r1,gpio_base					//select the GPIO base.
	ldr r2,[r1,#GPIO_IF]					//Read flag
	str r2,[r1,#GPIO_IFC]					//Clear the flag in the register that clears flag. This will disable the interrupt to be triggered again

	bx lr							//This will write the code to PC that specifies that we are done handing the interrupt

iser0_setup_value:
	.long 0x802

        .thumb_func
dummy_handler:

	
