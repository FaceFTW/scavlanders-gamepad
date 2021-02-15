/***********************************************************************
 * \file 		main_tirtos.c
 * \author		Texas Instruments
 * \author 		Alex Westerman
 * \brief 		Entry point code for TI RTOS
 * \copyright 	See LICENSE_TI and LICENSE
 ***********************************************************************/

/***********************************************************************
 * 								INCLUDE
***********************************************************************/
#include <stdint.h>							//Standard C Integer Library
#include <ti/sysbios/BIOS.h>				//TI RTOS SYS/BIOS Headers
#include <ti/display/Display.h>				//TI RTOS Display Library
#include <ti/drivers/GPIO.h>				//TI RTOS GPIO Library
#include <ti/drivers/Power.h>				//TI RTOS Power Management Library
#include <ti/drivers/Timer.h>				//TI RTOS Timer Library
#include <ti/drivers/UART.h>				//TI RTOS UART Library

#include "Board.h"							//MSP432-specific Code
#include "simple_application_processor.h"	//Custom Bluetooth Library


/***********************************************************************
 * 						VARIABLE DECLARATIONS
***********************************************************************/
Display_Handle displayOut;	//Output display handle that will be used to print out all debug/log statements

int main(void) {
	/* Call board initialization functions */
	Power_init();
	GPIO_init();
	UART_init();
	Timer_init();

	/* Open the display for output */
	displayOut = Display_open(Display_Type_HOST | Display_Type_UART, NULL);
	if (displayOut == NULL) {
		/* Failed to open display driver */
		while (1)
			;
	}

	/* Create main application processor task */
	AP_createTask();

	/* enable interrupts and start SYS/BIOS */
	BIOS_start();

	return 0;
}
