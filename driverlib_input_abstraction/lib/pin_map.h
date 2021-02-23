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
#define botton_port_pad         GPIO_PORT_P4
#define botton_A                GPIO_PIN0
#define botton_B                GPIO_PIN1
#define botton_X                GPIO_PIN2
#define botton_Y                GPIO_PIN3
#define botton_Sel              GPIO_PIN4
#define botton_Start            GPIO_PIN5
#define botton_trigger_left                 GPIO_PIN6
#define botton_trigger_right                GPIO_PIN7
#define botton_port2            GPIO_PORT_P6
#define botton_P                GPIO_PIN2

/*
 * Start of the Joystick
 */
// the port was set up correctly, but we need multiple ADC at the same time
#define J_x1_port                                                     GPIO_PORT_P8
#define J_x1_pin                                                      GPIO_PIN7
#define J_y1_pin                                                      GPIO_PIN6



/*
 * start of the LED matrix pin
 */

#define LED_Matrix_Port      GPIO_PORT_P9
#define LED_Matrix_SDA       GPIO_PIN7
#define LED_Matrix_SCL       GPIO_PIN5
/*
 * start of the LCD matrix pin
 */
#define LCD_CFG_PORT        GPIO_PORT_P7    //LCD Control Port
#define LCD_RS              GPIO_PIN0       //LCD Control Register Select Pin
#define LCD_EN              GPIO_PIN1       //LCD Control Enable Pin
#define LCD_DAT_PORT        GPIO_PORT_P10    //LCD Data Signal Port
#define LCD_D4              GPIO_PIN4       //LCD Data Pin 4
#define LCD_D5              GPIO_PIN5       //LCD Data Pin 5
#define LCD_D6              GPIO_PIN6       //LCD Data Pin 6
#define LCD_D7              GPIO_PIN7       //LCD Data Pin 7



#endif /* LIB_PIN_MAP_H_ */
