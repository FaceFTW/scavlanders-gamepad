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
// #include "lib/led_matrix.h"									//LED Matrix Hardware Abstraction API




/***********************************************************************
 * 							MAIN FUNCTION  							   *
***********************************************************************/
int main(void) {

	/***********************************************************************
	 * 						INITIALIZATION CODE							   *
	***********************************************************************/

	MAP_WDT_A_holdTimer();



	/***********************************************************************
	 * 								MAIN LOOP							   *
	***********************************************************************/
	while (1) {

	}
}

/***********************************************************************
 * 						INTERRUPT SERVICE ROUTINES		      		   *
***********************************************************************/
