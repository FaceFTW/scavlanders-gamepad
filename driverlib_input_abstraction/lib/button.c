/**********************************************************************
 * \headerfile: button.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Code related to parsing and encoding button inputs
 * \version: Feb 11, 2021
 ***********************************************************************/
#include "button.h"
#include"ti/devices/msp432p4xx/driverlib/driverlib.h"

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

       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);

       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
       MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);


       /* Configuring Timer_A to have a period of approximately 500ms and
        * an initial duty cycle of 10% of that (3200 ticks)  */

       //![Simple Timer_A Example]

       /* Enabling interrupts and starting the watchdog timer */
       MAP_Interrupt_enableInterrupt(INT_PORT1);


       MAP_Interrupt_enableSleepOnIsrExit();
       MAP_Interrupt_enableMaster();

}
void PORT1_IRQHandler(void){
    uint32_t status_A = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    uint32_t status_A = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    uint32_t status_A = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);

    // might need the configuration of the ports
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status_A & GPIO_PIN1)
    {
        printf("button A is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button B is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button X is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button Y is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button U is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button D is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button R is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button L is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button P is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button Sel is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button Start is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button trigger right is pressed! ");
    }
    else if (status_A & GPIO_PIN1)
    {
        printf("button trugger left is pressed! ");
    }

}
