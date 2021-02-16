/**********************************************************************
 * \headerfile: analog_trigger.h
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Prototype header for analog_trigger.c
 * \version: Feb 11, 2021
 ***********************************************************************/
#ifndef LIB_ANALOG_TRIGGER_H_
#define LIB_ANALOG_TRIGGER_H_

/***********************************************************************
 * 						    INCLUDE DELCARATIONS					   *
***********************************************************************/
#define fource_input_1_port             GPIO_PORT_P6// todo
#define fource_input_1_pin              GPIO_PIN1
#define fource_input_2_port             GPIO_PORT_P4          // todo GPIO Analog input port
#define fource_input_2_pin              GPIO_PIN0

/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/

extern void init_FR(void);

extern void read_FR(void);

extern void print_FR(void);

#endif /* LIB_ANALOG_TRIGGER_H_ */
