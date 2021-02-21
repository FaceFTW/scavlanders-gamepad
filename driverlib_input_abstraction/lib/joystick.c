/**********************************************************************
 * \headerfile: joystick.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Joystick Input Parsing and Encoding Code
 * \version: Feb 11, 2021
 ***********************************************************************/

#include "joystick.h"
#include"ti/devices/msp432p4xx/driverlib/driverlib.h"
static volatile x_value1;
static volatile y_value1;
static volatile x_value2;
static volatile y_value2;

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
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                GPIO_PIN0 | GPIO_PIN2 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN7
                        | GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                GPIO_PIN1 , GPIO_TERTIARY_MODULE_FUNCTION);


        /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM7 (A0 - A7)  with no repeat)
         * with internal 2.5v reference */
        MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM3, false);
        MAP_ADC14_configureConversionMemory(ADC_MEM0,
                ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                ADC_INPUT_A6, false);
        MAP_ADC14_configureConversionMemory(ADC_MEM1,
                ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                ADC_INPUT_A8, false);
        MAP_ADC14_configureConversionMemory(ADC_MEM2,
                ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                ADC_INPUT_A9, false);
        MAP_ADC14_configureConversionMemory(ADC_MEM3,
                ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                ADC_INPUT_A11, false);



        /* Enabling the interrupt when a conversion on channel 7 (end of sequence)
         *  is complete and enabling conversions */
        MAP_ADC14_enableInterrupt(ADC_INT3);

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


void xy_value_detect(){
    x_value1 = ADC_getResult(ADC_MEM0);
    y_value1 = ADC_getResult(ADC_MEM1);
    x_value2 = ADC_getResult(ADC_MEM2);
    y_value2 = ADC_getResult(ADC_MEM3);

    analog_x1 = (((x_value1*3.3)/) -1.75); //4bit
    analog_y1 = (((x_value1*3.3)/) -1.75);
    analog_x2 = (((x_value1*3.3)/) -1.75);
    analog_y2 = (((x_value1*3.3)/) -1.75);



}




