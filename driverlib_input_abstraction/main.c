/***********************************************************************
 * \file: main.c
 * \author: Alex Westerman
 * Date Created: Feb 7, 2021
 * Description: Main Code File for ECE230 Project 6
 * \version: Feb 7, 2021
 ***********************************************************************/

/***********************************************************************
 * 							INCLUDE DEFINITIONS						   *
 ***********************************************************************/
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>		//TI MSP432 DriverLib SDK
#include <stdio.h>											//Standard C IO Library
#include <stdint.h>											//Standard C Integer Library
#include <stdbool.h>										//Standard C Boolean Library
#include <stdlib.h>											//Standard C Libary
#include <string.h>											//Standard C Strings Library

#include "lib/analog_trigger.h"								//Analog Trigger Hardware Abstraction API
#include "lib/button.h"										//Button Hardware Abstraction API
#include "lib/joystick.h"									//2-axis Joystick Hardware Abstraction API
#include "lib/lcd.h"										//LCD via I2C API
#include "lib/led_matrix.h"									//LED Matrix Hardware Abstraction API



/***********************************************************************
 * 							MAIN FUNCTION  							   *
***********************************************************************/
int main(void) {

	/***********************************************************************
	 * 						INITIALIZATION CODE							   *
	***********************************************************************/

	MAP_WDT_A_holdTimer();
	CS_setExternalClockSourceFrequency(32000,6000000); // this is setting up the core frequency to 6mhz
	MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
	MAP_FlashCtl_setWaitState(FLASH_BANK0, 8);
	MAP_FlashCtl_setWaitState(FLASH_BANK1, 8);
	CS_startHFXT(false);
	MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
	SysTick_setPeriod(3000000); // set the delay time to 500 ms
	SysTick_enableModule();



	/***********************************************************************
	 * 								MAIN LOOP							   *
	***********************************************************************/
	while (1) {
	    initLCD();
	    testLCD();
	    init_joy();
	    init_button();
	    while((SysTick_CTRL_COUNTFLAG_Msk & SysTick-> CTRL) == 0){

	    }





	}
}

/***********************************************************************
 * 						INTERRUPT SERVICE ROUTINES		      		   *
***********************************************************************/
extern void ADC14_IRQHandler();
extern void PORT1_IRQHandler();
extern void DMA_INT1_IRQHandler();
