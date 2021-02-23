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
//#include "lib/led_matrix.h"


const char uartInit[] = "UART Initialized!";
const char uartInitA[] = "button A is pressed! ";
const char uartInitB[] = "button B is pressed! ";
const char uartInitX[] = "button X is pressed! ";
const char uartInitY[] = "button Y is pressed! ";
const char uartInitTL[] = "button TL is pressed! ";
const char uartInitTR[] = "button TR is pressed! ";
const char uartInitSEL[] = "button Sel is pressed! ";
const char uartInitSTART[] = "button START is pressed! ";
const char uartInitP[] = "button PAIR is pressed! ";


const eUSCI_UART_ConfigV1 uartCfg = {
EUSCI_A_UART_CLOCKSOURCE_SMCLK,                         // SMCLK Clock Source
        13,                                             // BRDIV = 78
        0,                                              // UCxBRF = 2
        0x49,                                           // UCxBRS = 0
        EUSCI_A_UART_ODD_PARITY,                        // Odd Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                          // 8 bit data length
        };


const eUSCI_SPI_MasterConfig spiMasterConfig =
{ EUSCI_B_SPI_CLOCKSOURCE_SMCLK, 12000000, 1000000,
        EUSCI_B_SPI_MSB_FIRST,
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, EUSCI_B_SPI_3PIN };

const eUSCI_SPI_SlaveConfig spiSlaveConfig =
{ EUSCI_B_SPI_MSB_FIRST,
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
        EUSCI_B_SPI_3PIN
        };
void init_UART(void){
        GPIO_setAsPeripheralModuleFunctionInputPin(UART_PORT, UART_TX | UART_RX, GPIO_PRIMARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(botton_port1,  botton_A |  botton_B | botton_X |  botton_Y  |botton_trigger_left |botton_trigger_right| botton_Start |botton_Sel , GPIO_PRIMARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(botton_port2,  botton_P , GPIO_PRIMARY_MODULE_FUNCTION);

        /**UART_CONFIG**/
        UART_initModule(EUSCI_A0_BASE, &uartCfg);
        UART_enableModule(EUSCI_A0_BASE);
        transmitString(uartInit, strlen(uartInit));

}
void transmitString(char *data, uint8_t len) {
    //Just iterate over data and transmit each char
    int index;
    for (index = 0; index < len - 1; index++) {
        UART_transmitData(EUSCI_A0_BASE, (uint8_t) data[index]);
    }
    UART_transmitData(EUSCI_A0_BASE, (uint8_t) '\n');   //Line Feed Escacpe
    UART_transmitData(EUSCI_A0_BASE, (uint8_t) '\r');   //Carriage Return Escape
}



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
	    init_UART();
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
void ADC14_IRQHandler(void){
    uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);
    if(ADC_INT0 & status){
    x_value = ADC14_getResult(ADC_MEM0);
    y_value = ADC14_getResult(ADC_MEM1);

    analog_x = (float) (((x_value * 3.3) -1.75)/16384);
    analog_y = (float) (((y_value * 3.3) -1.75)/16384);
    ADC14_toggleConversionTrigger();

    }

    MAP_ADC14_clearInterruptFlag(status);




}


void PORT1_IRQHandler(void){
    uint32_t status_pad = MAP_GPIO_getEnabledInterruptStatus(botton_port1);
    uint32_t status_pair = MAP_GPIO_getEnabledInterruptStatus(botton_port2);


    // might need the configuration of the ports
    MAP_GPIO_clearInterruptFlag(botton_port1, status_pad);
    MAP_GPIO_clearInterruptFlag(botton_port2, status_pair);
    if (status_pad & botton_A)
    {
        transmitString(uartInitA, strlen(uartInitA));
    }
    else if (status_pad & botton_B)
    {
        transmitString(uartInitB, strlen(uartInitB));
    }
    else if (status_pad & botton_X)
    {
        transmitString(uartInitX, strlen(uartInitX));
    }
    else if (status_pad & botton_Y)
    {
        transmitString(uartInitY, strlen(uartInitY));
    }

    else if (status_pad & botton_Start)
    {
        transmitString(uartInitSTART, strlen(uartInitSTART));
    }
    else if (status_pad & botton_Sel)
    {
        transmitString(uartInitSEL, strlen(uartInitSEL));
    }
    else if (status_pad & botton_trigger_left)
    {
        transmitString(uartInitTL, strlen(uartInitTL));
    }
    else if (status_pad & botton_trigger_right)
    {
        transmitString(uartInitTR, strlen(uartInitTR));
    }
    else if (status_pair & botton_P)
    {

        transmitString(uartInitP, strlen(uartInitP));
    }

}



