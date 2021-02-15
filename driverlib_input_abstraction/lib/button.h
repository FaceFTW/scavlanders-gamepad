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
#define botton_port                                     // todo find the GPIO port for the IO port.
#define botton_A
#define botton_B                                        // todo find the GPIO port for the IO pin
#define botton_X
#define botton_Y
#define botton_U
#define botton_D
#define botton_R
#define botton_L
#define botton_P
#define botton_Sel
#define botton_Start
#define button_all              0x0007

typedef enum _States{
    A,              // 0x00 == A button
    B,              // 0x01 == B button
    X,              // 0x02 == X button
    Y,              // 0x03 == Y button
    U,              // 0x04 == U button
    D,              // 0x05 == D button
    R,              // 0x06 == R button
    L,              // 0x07 == L button
    P,              // 0x08 == P button
    Sel,            // 0x09 == select button
    Start,          // 0x0A == start button
}States;

/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/
extern void init_botton(void);

extern void get_button(void);

extern void print_button(States states);

#endif /* LIB_BUTTON_H_ */
