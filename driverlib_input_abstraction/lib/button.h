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
#define botton_port1            GPIO_PORT_P4            // for right now, the code for the button is working, which can detect the button pressed
#define botton_A                GPIO_PIN0
#define botton_B                GPIO_PIN1
#define botton_X                GPIO_PIN2
#define botton_Y                GPIO_PIN3
#define botton_trigger_left                 GPIO_PIN4
#define botton_trigger_right                GPIO_PIN5
#define botton_Start            GPIO_PIN6
#define botton_Sel              GPIO_PIN7
#define botton_port2            GPIO_PORT_P6
#define botton_P                GPIO_PIN2
#define button_all              0x0007

#define UART_PORT       GPIO_PORT_P1
#define UART_TX         GPIO_PIN3
#define UART_RX         GPIO_PIN2


/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/
extern void init_botton(void);

extern void get_button(void);



#endif /* LIB_BUTTON_H_ */
