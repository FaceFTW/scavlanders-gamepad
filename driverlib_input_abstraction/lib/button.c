/**********************************************************************
 * \headerfile: button.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Code related to parsing and encoding button inputs
 * \version: Feb 11, 2021
 ***********************************************************************/
#include "button.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>      //TI MSP432 DriverLib SDK
#include <stdio.h>                                          //Standard C IO Library
#include <stdint.h>                                         //Standard C Integer Library
#include <stdbool.h>                                        //Standard C Boolean Library
#include <stdlib.h>

/***********************************************************************
 * 							FUNCTION DECLARATIONS					   *
***********************************************************************/
void init_button(){


       MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
       MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
       MAP_CS_initClockSignal(CS_SMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_2);
       MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);

       /* Configuring GPIO2.4 as peripheral output for PWM  and P6.7 for button
        * interrupt */

       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port1, botton_A);// A
       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port1, botton_B);// B
       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port1, botton_X);// X
       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port1, botton_Y);// Y
       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port1, botton_Start);// Start
       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port1, botton_Sel);// Select
       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port1, botton_trigger_left);// Trigger left
       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port1, botton_trigger_right);// Trigger right
       MAP_GPIO_setAsInputPinWithPullUpResistor(botton_port2, botton_P);// Pairing


       MAP_GPIO_clearInterruptFlag(botton_port1, botton_A);
       MAP_GPIO_clearInterruptFlag(botton_port1, botton_B);
       MAP_GPIO_clearInterruptFlag(botton_port1, botton_X);
       MAP_GPIO_clearInterruptFlag(botton_port1, botton_Y);
       MAP_GPIO_clearInterruptFlag(botton_port1, botton_Start);
       MAP_GPIO_clearInterruptFlag(botton_port1, botton_Sel);
       MAP_GPIO_clearInterruptFlag(botton_port1, botton_trigger_left);
       MAP_GPIO_clearInterruptFlag(botton_port1, botton_trigger_right);
       MAP_GPIO_clearInterruptFlag(botton_port2, botton_P);

       MAP_GPIO_enableInterrupt(botton_port1, botton_A);
       MAP_GPIO_enableInterrupt(botton_port1, botton_B);
       MAP_GPIO_enableInterrupt(botton_port1, botton_X);
       MAP_GPIO_enableInterrupt(botton_port1, botton_Y);
       MAP_GPIO_enableInterrupt(botton_port1, botton_Start);
       MAP_GPIO_enableInterrupt(botton_port1, botton_Sel);
       MAP_GPIO_enableInterrupt(botton_port1, botton_trigger_left);
       MAP_GPIO_enableInterrupt(botton_port1, botton_trigger_right);
       MAP_GPIO_enableInterrupt(botton_port2, botton_P);


       /* Configuring Timer_A to have a period of approximately 500ms and
        * an initial duty cycle of 10% of that (3200 ticks)  */

       //![Simple Timer_A Example]

       /* Enabling interrupts and starting the watchdog timer */
       MAP_Interrupt_enableInterrupt(INT_PORT1);


       MAP_Interrupt_enableSleepOnIsrExit();
       MAP_Interrupt_enableMaster();

}
void PORT1_IRQHandler(void){
    uint32_t status_pad = MAP_GPIO_getEnabledInterruptStatus(botton_port1);
    uint32_t status_pair = MAP_GPIO_getEnabledInterruptStatus(botton_port2);


    // might need the configuration of the ports
    MAP_GPIO_clearInterruptFlag(botton_port1, status_pad);
    MAP_GPIO_clearInterruptFlag(botton_port2, status_pair);
    if (status_pad & botton_A)
    {
        printf("button A is pressed! ");
    }
    else if (status_pad & botton_B)
    {
        printf("button B is pressed! ");
    }
    else if (status_pad & botton_X)
    {
        printf("button X is pressed! ");
    }
    else if (status_pad & botton_Y)
    {
        printf("button Y is pressed! ");
    }

    else if (status_pad & botton_Start)
    {
        printf("button Start is pressed! ");
    }
    else if (status_pad & botton_Sel)
    {
        printf("button Sel is pressed! ");
    }
    else if (status_pad & botton_trigger_left)
    {
        printf("button trigger left is pressed! ");
    }
    else if (status_pad & botton_trigger_right)
    {
        printf("button trigger right is pressed! ");
    }
    else if (status_pair & botton_P)
    {

        printf("button P is pressed! ");
    }

}
