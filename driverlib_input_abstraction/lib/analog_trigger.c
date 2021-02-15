/**********************************************************************
 * \headerfile: analog_trigger.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Input Parsing of Analog Triggers (FSRs)
 * \version: Feb 11, 2021
 ***********************************************************************/
#include "analog_trigger.h"
#include"ti/devices/msp432p4xx/driverlib/driverlib.h"
static volatile fource1;
static volatile fource2;

/***********************************************************************
 * 							FUNCTION DECLARATIONS					   *
***********************************************************************/
void init_FR(){
    GPIO_setAsinputPin();
    GPIO_setAsinputPin();
}
void read_FR (){
    fource1 = GPIO_getInputPinValue();
    fource2 = GPIO_getInputPinValue();
}

void print_FR(){
    //todo print to the console
    printf();   //fource button1 and button1

}


