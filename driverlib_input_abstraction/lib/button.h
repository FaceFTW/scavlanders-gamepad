/**********************************************************************
 * \headerfile: button.h
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Prototype header for button.c
 * \version: Feb 11, 2021
 ***********************************************************************/


#ifndef LIB_BUTTON_H_
#define LIB_BUTTON_H_

/***********************************************************************
 * 						    INCLUDE DELCARATIONS					   *
***********************************************************************/
#define botton_port1            GPIO_PORT_P4                        // todo find the GPIO port for the IO port.
#define botton_A                GPIO_PIN0
#define botton_B                GPIO_PIN1                        // todo find the GPIO port for the IO pin
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
#define button_all              0x0007



/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/
extern void init_botton(void);

extern void get_button(void);



#endif /* LIB_BUTTON_H_ */
