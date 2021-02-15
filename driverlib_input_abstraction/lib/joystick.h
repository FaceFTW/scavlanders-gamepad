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

#define J_x                                                     // todo gpio analog input
#define J_y                                                     //



/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/
void init_joy(void);

void x_value_detect(void);

void y_value_detect(void);

void x_value_print(void);

void y_value_print(void);

#endif /* LIB_JOYSTICK_H_ */
