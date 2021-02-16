/*
 * pin_map.h
 *
 *  Created on: Feb 15, 2021
 *      Author: Yifan Dai
 */

#ifndef LIB_PIN_MAP_H_
#define LIB_PIN_MAP_H_
/*
 * Start of the button pin
 */

// notes: the ports are incorrect during the current status.
// and actually I am not going to define all the
#define botton_port1            GPIO_PORT_P4            // for right now, the code for the button is working, which can detect the button pressed
#define botton_A                GPIO_PIN0
#define botton_B                GPIO_PIN1
#define botton_X                GPIO_PIN2
#define botton_Y                GPIO_PIN3
#define botton_U                GPIO_PIN4
#define botton_D                GPIO_PIN5
#define botton_R                GPIO_PIN6
#define botton_L                GPIO_PIN7
#define botton_port2            GPIO_PORT_P5
#define botton_P                GPIO_PIN0
#define botton_Sel              GPIO_PIN1
#define botton_Start            GPIO_PIN2

/*
 * Start of the Joystick
 */
// the port was set up correctly, but we need multiple ADC at the same time
#define J_x1_port                                                     GPIO_PORT_P4
#define J_x1_pin                                                      GPIO_PIN2
#define J_y1_port                                                     GPIO_PORT_P4
#define J_y1_pin                                                      GPIO_PIN4
#define J_x2_port                                                     GPIO_PORT_P4
#define J_x2_pin                                                      GPIO_PIN5
#define J_y2_port                                                     GPIO_PORT_P4
#define J_y2_pin                                                      GPIO_PIN7
                                         //
/*
 * start of the Force sensing resistor pin
 */
// the port was set up correctly, but we need multiple ADC at the same time
#define fource_input_1_port             GPIO_PORT_P6// todo
#define fource_input_1_pin              GPIO_PIN1
#define fource_input_2_port             GPIO_PORT_P4
#define fource_input_2_pin              GPIO_PIN0
/*
 * start of the LED matrix pin
 */


/*
 * start of the LCD matrix pin
 */

#endif /* LIB_PIN_MAP_H_ */
