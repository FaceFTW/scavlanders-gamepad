/**********************************************************************
 * \headerfile: joystick.h
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Prototype header for joystick.c
 * \version: Feb 11, 2021
 ***********************************************************************/
#ifndef LIB_JOYSTICK_H_
#define LIB_JOYSTICK_H_

/***********************************************************************
 * 						    INCLUDE DELCARATIONS					   *
***********************************************************************/

#define J_xy_port                                                     GPIO_PORT_P8
#define J_x_pin                                                       GPIO_PIN7
#define J_y_pin                                                       GPIO_PIN6

/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/
void init_joy(void);

void xy_value_detect(void);





#endif /* LIB_JOYSTICK_H_ */
