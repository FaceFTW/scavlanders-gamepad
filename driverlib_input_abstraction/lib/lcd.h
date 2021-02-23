/**********************************************************************
 * \headerfile: lcd.h
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Prototype header for lcd.c
 * \version: Feb 11, 2021
 ***********************************************************************/
#ifndef LIB_LCD_H_
#define LIB_LCD_H_
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>      //TI MSP432 DriverLib SDK
#include <stdio.h>                                          //Standard C IO Library
#include <stdint.h>                                         //Standard C Integer Library
#include <stdbool.h>                                        //Standard C Boolean Library
#include <stdlib.h>                                         //Standard C Libary

/***********************************************************************
 * 						    INCLUDE DELCARATIONS					   *
***********************************************************************/

/***********************************************************************
 * 						PREPROCESSOR MACROS/DEFINES					   *
***********************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
// Notation: this code is from Alex, which he authoriezed me to use it in the project.
// Since we are using 4 bit mode instead of 8 bit, which is Alex.s project 5 code.
#define LCD_CFG_PORT        GPIO_PORT_P7    //LCD Control Port
#define LCD_RS              GPIO_PIN0       //LCD Control Register Select Pin
#define LCD_EN              GPIO_PIN1       //LCD Control Enable Pin
#define LCD_DAT_PORT        GPIO_PORT_P10    //LCD Data Signal Port
#define LCD_D4              GPIO_PIN4       //LCD Data Pin 4
#define LCD_D5              GPIO_PIN5       //LCD Data Pin 5
#define LCD_D6              GPIO_PIN6       //LCD Data Pin 6
#define LCD_D7              GPIO_PIN7       //LCD Data Pin 7




/**TIMER MACROS**/
#define REFRESH_CYCLE   32768               //1 Hz
#define ASYNC_DELAY     50                  //Offset from ADC Triggers (ms)

/**DELAY MACROS**/
#define UNDERFLOW       2
#define OVERFLOW        1
#define SUCCESS         0

#define USEC_DIVISOR    1000000
#define MSEC_DIVISOR    1000
#define SYSTICK_LIMIT   0x00FFFFFF

#define LONG_INSTR_DELAY    5000
#define SHORT_INSTR_DELAY   50

/**LCD MACROS**/
#define CTRL_MODE       0
#define DATA_MODE       1
#define LINE1_OFFSET    0x0
#define LINE2_OFFSET    0x40

/* Instruction masks */
#define CLEAR_DISPLAY_MASK  0x01
#define RETURN_HOME_MASK    0x02
#define ENTRY_MODE_MASK     0x04
#define DISPLAY_CTRL_MASK   0x08
#define CURSOR_SHIFT_MASK   0x10
#define FUNCTION_SET_MASK   0x20
#define SET_CGRAM_MASK      0x40
#define SET_CURSOR_MASK     0x80

#define NONHOME_MASK        0xFC
/* Field masks for instructions:
 * BF   = 1: Internally operating
 * BF   = 0: Instructions acceptable
 */
#define DL_FLAG_MASK        0x10    //DL = 1: 8 bits, DL = 0: 4 bits
#define N_FLAG_MASK         0x08    //N = 1: 2 lines, N = 0: 1 line
#define SC_FLAG_MASK        0x08    //S/C = 1: Display shift, 0: Cursor move
#define F_FLAG_MASK         0x04    //F = 1: 5 x 10 dots, F = 0: 5 x 8 dots
#define RL_FLAG_MASK        0x04    //R/L = 1: Shift to the right, 0: Shift to the left
#define D_FLAG_MASK         0x04    //D = 1: Display On, D = 0: Display Off
#define C_FLAG_MASK         0x02    //C = 1: Cursor On, D = 0: Cursor Off
#define ID_FLAG_MASK        0x02    //I/D = 1: Increment, 0: Decrement
#define B_FLAG_MASK         0x01    //B = 1: Cursor blink On, D = 0: Cursor blink Off
#define S_FLAG_MASK         0x01    //S = 1: Accompanies display shift

/*DDRAM Offsets*/
#define LCD_LINE1_VAL_ADDR  0x09    //Line 1 (Value) Cursor Pos
#define LCD_LINE1_LBL_ADDR  0x00    //Line 1 (Label) Cursor Pos
#define LCD_LINE2_VAL_ADDR  0x49    //Line 2 (Value) Cursor Pos
#define LCD_LINE2_LBL_ADDR  0x40    //Line 2 (Label) Cursor Pos
#define LCD_LINE2_UNT_ADDR  0x4F    //Line 2 (Units) Cursor Pos

/***********************************************************************
 * 							FUNCTION PROTOTYPES						   *
***********************************************************************/
extern int delayMicroSec(uint32_t micros);

extern int delayMilliSec(uint32_t millis) ;

extern void instructionDelay(uint8_t mode, uint8_t instruction);

extern void writeInstruction(uint8_t mode, uint8_t instruction);

extern void initLCD(void);

extern void dataInstruction(uint8_t data);

extern void printChar(char character);

extern void testLCD(void);


#ifdef __cplusplus
}
#endif


#endif /* LIB_LCD_H_ */
