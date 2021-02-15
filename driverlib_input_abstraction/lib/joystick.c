/**********************************************************************
 * \headerfile: joystick.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Joystick Input Parsing and Encoding Code
 * \version: Feb 11, 2021
 ***********************************************************************/
#include "joystick.h"
#include"ti/devices/msp432p4xx/driverlib/driverlib.h"

/***********************************************************************
 * 							FUNCTION DECLARATIONS					   *
***********************************************************************/
void init_joy(){
    GPIO_setAsinputPin();
    GPIO_setAsinputPin();

}

void x_value_detect(){

}

void y_value_detect(){

}

void x_value_print(){
    //todo print to the console
}

void y_value_print(){
    // todo print to the console
}
