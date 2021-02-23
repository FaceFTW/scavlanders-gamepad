/**********************************************************************
 * \headerfile: joystick.h
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Prototype header for joystick.c
 * \version: Feb 11, 2021
 ***********************************************************************/
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>      //TI MSP432 DriverLib SDK
#include <stdio.h>                                          //Standard C IO Library
#include <stdint.h>                                         //Standard C Integer Library
#include <stdbool.h>                                        //Standard C Boolean Library
#include <stdlib.h>
#ifndef LIB_JOYSTICK_H_
#define LIB_JOYSTICK_H_

/***********************************************************************
 * 						    INCLUDE DELCARATIONS					   *
***********************************************************************/

#define J_xy_port                                                     GPIO_PORT_P8
#define J_x_pin                                                       GPIO_PIN7
#define J_y_pin                                                       GPIO_PIN6

volatile float analog_x;
volatile float analog_y;


volatile uint8_t x_value;
volatile uint8_t y_value;

/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/
void init_joy(void);

void xy_value_detect(void);





#endif /* LIB_JOYSTICK_H_ */
