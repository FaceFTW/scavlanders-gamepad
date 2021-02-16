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

#define J_x1_port                                                     GPIO_PORT_P4
#define J_x1_pin                                                      GPIO_PIN2
#define J_y1_port                                                     GPIO_PORT_P4
#define J_y1_pin                                                      GPIO_PIN4
#define J_x2_port                                                     GPIO_PORT_P4
#define J_x2_pin                                                      GPIO_PIN5
#define J_y2_port                                                     GPIO_PORT_P4
#define J_y2_pin                                                      GPIO_PIN7


/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/
void init_joy(void);

void xy_value_detect(void);


void xy_value_print(void);



#endif /* LIB_JOYSTICK_H_ */
