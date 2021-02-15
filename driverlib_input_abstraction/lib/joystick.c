/**********************************************************************
 * \headerfile: joystick.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Joystick Input Parsing and Encoding Code
 * \version: Feb 11, 2021
 ***********************************************************************/
#include "joystick.h"
#include"ti/devices/msp432p4xx/driverlib/driverlib.h"
static volatile x_value;
static volatile y_value;

/***********************************************************************
 * 							FUNCTION DECLARATIONS					   *
***********************************************************************/
void init_joy(){
    GPIO_setAsinputPin();
    GPIO_setAsinputPin();

}

void xy_value_detect(){
    x_value = GPIO_getInputPinValue();
    y_value = GPIO_getInputPinValue();

}



void xy_value_print(){
    //todo print to the console
    printf("x value is", x_value,"y value is" , y_value);

}

