/**********************************************************************
 * \headerfile: joystick.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Joystick Input Parsing and Encoding Code
 * \version: Feb 11, 2021
 ***********************************************************************/

#include "joystick.h"

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>      //TI MSP432 DriverLib SDK
#include <stdio.h>                                          //Standard C IO Library
#include <stdint.h>                                         //Standard C Integer Library
#include <stdbool.h>                                        //Standard C Boolean Library
#include <stdlib.h>                                         //Standard C Libary
static uint16_t resultsBuffer[8];




/***********************************************************************
 * 							FUNCTION DECLARATIONS					   *
***********************************************************************/

void init_joy(){
    MAP_Interrupt_enableSleepOnIsrExit();

        /* Zero-filling buffer */
        memset(resultsBuffer, 0x00, 8 * sizeof(uint16_t));

        //![Simple REF Example]
        /* Setting reference voltage to 2.5  and enabling reference */
        MAP_REF_A_setReferenceVoltage(REF_A_VREF2_5V);
        MAP_REF_A_enableReferenceVoltage();
        //![Simple REF Example]

        /* Initializing ADC (MCLK/1/1) */
        MAP_ADC14_enableModule();
        MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
                0);

        /* Configuring GPIOs for Analog In */
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(J_xy_port,
           J_x_pin  | J_y_pin , GPIO_TERTIARY_MODULE_FUNCTION);


        /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM7 (A0 - A7)  with no repeat)
         * with internal 2.5v reference */
        MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, false);
        MAP_ADC14_configureConversionMemory(ADC_MEM0,
                ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                ADC_INPUT_A6, false);
        MAP_ADC14_configureConversionMemory(ADC_MEM1,
                ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                ADC_INPUT_A8, false);



        /* Enabling the interrupt when a conversion on channel 7 (end of sequence)
         *  is complete and enabling conversions */
        MAP_ADC14_enableInterrupt(ADC_INT0);


        /* Enabling Interrupts */
        MAP_Interrupt_enableInterrupt(INT_ADC14);
        MAP_Interrupt_enableMaster();

        /* Setting up the sample timer to automatically step through the sequence
         * convert.
         */
        MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

        /* Triggering the start of the sample */
        MAP_ADC14_enableConversion();
        MAP_ADC14_toggleConversionTrigger();


}




