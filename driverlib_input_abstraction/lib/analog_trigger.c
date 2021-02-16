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
    GPIO_setAsinputPin(fource_input_1_port,fource_input_1_pin);
    GPIO_setAsinputPin(fource_input_2_port,fource_input_2_pin);
}


void read_FR (){
    fource1 = GPIO_getInputPinValue(fource_input_1_port, fource_input_1_pin);
    fource2 = GPIO_getInputPinValue(fource_input_2_port, fource_input_2_pin);
}

void print_FR(){
    //todo print to the console multiple ACD access
    printf();   //fource button1 and button1

}


