/**********************************************************************
 * \headerfile: led_matrix.h
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Prototype header for lcd.c
 * \version: Feb 11, 2021
 ***********************************************************************/
#ifndef LIB_LED_MATRIX_H_
#define LIB_LED_MATRIX_H_
/***********************************************************************
 *                          INCLUDE DELCARATIONS                       *
***********************************************************************/
#define LED_Matrix_Port      GPIO_PORT_P9
#define LED_Matrix_SDA       GPIO_PIN7
#define LED_Matrix_SCL       GPIO_PIN5
static DMA_ControlTable MSP_EXP432P401RLP_DMAControlTable[32];
#define MAP_SPI_MSG_LENGTH    26
/***********************************************************************
 *                      PREPROCESSOR MACROS/DEFINES                    *
***********************************************************************/







#endif /* LIB_LED_MATRIX_H_ */
