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
    /*
    GPIO_setAsinputPin();
    GPIO_setAsinputPin();
    GPIO_setAsinputPin();
    GPIO_setAsinputPin();
    */
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 1);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 1);

       /* Setting DCO to 48MHz  */
    MAP_PCM_setPowerState(PCM_AM_LDO_VCORE1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

       //![Single Sample Mode Configure]
       /* Initializing ADC (MCLK/1/4) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4,
               0);

       /* Configuring GPIOs (5.5 A0) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN5,
                                                   GPIO_TERTIARY_MODULE_FUNCTION);

       /* Configuring ADC Memory */
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                                    ADC_INPUT_A0, false);

       /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

   /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();
       //![Single Sample Mode Configure]

       /* Enabling interrupts */
    MAP_ADC14_enableInterrupt(ADC_INT0);
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();

}


void xy_value_detect(){
    x_value1 = GPIO_getInputPinValue(J_x1_port,J_x1_port);
    y_value1 = GPIO_getInputPinValue(J_y1_port,J_y1_port);
    x_value2 = GPIO_getInputPinValue(J_x2_port,J_x2_port);
    y_value2 = GPIO_getInputPinValue(J_y2_port,J_y2_port);

}



void xy_value_print(){
    //todo print to the console multiple ADC access

    printf("x value1 is", x_value1,"y value1 is" , y_value1);
    printf("x value2 is", x_value2,"y value2 is" , y_value2);

}

