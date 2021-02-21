/**********************************************************************
 * \headerfile: lcd.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: LCD Control API via I2C Communication
 * \version: Feb 11, 2021
 ***********************************************************************/
#include "lib/lcd.h"

/***********************************************************************
 * 							FUNCTION DECLARATIONS					   *
***********************************************************************/
// the codes are from the Alex's project5. He authorized me to use it in the capstone with some modification
int delayMicroSec(uint32_t micros) {
    uint64_t ticks = sysClkFreq * micros / USEC_DIVISOR;
    if (ticks < 2) {
        return UNDERFLOW;
    }
    if (ticks > SYSTICK_LIMIT) {
        return OVERFLOW;
    }

    // Set the period of the SysTick counter
    SysTick->LOAD = ticks - 1;
    // Write any value to reset timer counter
    SysTick->VAL = 1;
    SysTick_enableModule();
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
        ;
    SysTick_disableModule();
    return SUCCESS;
}

/**
 * Simple unit conversion from microseconds to milliseconds
 * @param millis Length of delay in Milliseconds
 * @return Status flag (SUCCESS, UNDERFLOW, or OVERFLOW)
 */
int delayMilliSec(uint32_t millis) {
    return delayMicroSec(1000 * millis);
}

/*==LCD DRIVER FUNCTIONS==*/
/*!
 * Delay method based on instruction execution time.
 *   Execution times from Table 6 of HD44780 data sheet, with buffer.
 *
 * \param mode RS mode selection
 * \param instruction Instruction/data to write to LCD
 *
 * \return None
 */
void instructionDelay(uint8_t mode, uint8_t instruction) {
    if ((mode == DATA_MODE) || (instruction & NONHOME_MASK)) {
        delayMicroSec(SHORT_INSTR_DELAY);
    } else {
        delayMicroSec(LONG_INSTR_DELAY);
    }
}

/*!
 * Function to write instruction/data to LCD.
 *
 * \param mode          Write mode: 0 - control, 1 - data
 * \param instruction   Instruction/data to write to LCD
 *
 * \return None
 */
void writeInstruction(uint8_t mode, uint8_t instruction) {
    GPIO_setOutputLowOnPin(LCD_DAT_PORT, LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7);
    if (mode == DATA_MODE) {
        GPIO_setOutputHighOnPin(LCD_CFG_PORT, LCD_RS);
    } else {
        GPIO_setOutputLowOnPin(LCD_CFG_PORT, LCD_RS);
    }
    //Transmit Upper half
    P2->OUT = (P2->OUT & 0x00) + (instruction & 0xF0);
    GPIO_setOutputHighOnPin(LCD_CFG_PORT, LCD_EN);
    delayMicroSec(5);
    GPIO_setOutputLowOnPin(LCD_CFG_PORT, LCD_EN);

    //Transmit Lower Half
    P2->OUT = (P2->OUT & 0x00) + ((instruction & 0x0F) << 4);
    GPIO_setOutputHighOnPin(LCD_CFG_PORT, LCD_EN);
    delayMicroSec(5);
    GPIO_setOutputLowOnPin(LCD_CFG_PORT, LCD_EN);
    instructionDelay(mode, instruction);
}

/**
 * LCD Initalization Sequence. Configures LCD into 4-bit communication
 * mode, with
 */
void initLCD(void) {
    //write 0x30 after 15 ms, 4.1 ms, and 100 us respectively
    //then write 0x20 immediately afterward
    GPIO_setOutputLowOnPin(LCD_CFG_PORT, LCD_EN);
    P2->OUT = 0x30;
    delayMilliSec(40);
    GPIO_setOutputHighOnPin(LCD_CFG_PORT, LCD_EN);
    delayMilliSec(5);
    GPIO_setOutputLowOnPin(LCD_CFG_PORT, LCD_EN);
    delayMilliSec(5);
    GPIO_setOutputHighOnPin(LCD_CFG_PORT, LCD_EN);
    delayMilliSec(5);
    GPIO_setOutputLowOnPin(LCD_CFG_PORT, LCD_EN);
    delayMicroSec(150);
    GPIO_setOutputHighOnPin(LCD_CFG_PORT, LCD_EN);
    delayMilliSec(5);
    GPIO_setOutputLowOnPin(LCD_CFG_PORT, LCD_EN);
    delayMicroSec(5);
    P2->OUT = 0x20;
    GPIO_setOutputHighOnPin(LCD_CFG_PORT, LCD_EN);
    delayMicroSec(5);
    GPIO_setOutputLowOnPin(LCD_CFG_PORT, LCD_EN);

    //Now we can say this is a 4-bit interface
    writeInstruction(CTRL_MODE, FUNCTION_SET_MASK | N_FLAG_MASK);   //4-bit interface, 2 lines, 5x8 charsize
    delayMicroSec(LONG_INSTR_DELAY);
    writeInstruction(CTRL_MODE, DISPLAY_CTRL_MASK);
    delayMicroSec(LONG_INSTR_DELAY);
    writeInstruction(CTRL_MODE, CLEAR_DISPLAY_MASK);
    delayMicroSec(LONG_INSTR_DELAY);
    writeInstruction(CTRL_MODE, ENTRY_MODE_MASK | ID_FLAG_MASK);
    delayMicroSec(LONG_INSTR_DELAY);
    //Turn the LCD on

    writeInstruction(CTRL_MODE, DISPLAY_CTRL_MASK | D_FLAG_MASK);
    delayMicroSec(LONG_INSTR_DELAY);
    writeInstruction(CTRL_MODE, CLEAR_DISPLAY_MASK);
    delayMicroSec(LONG_INSTR_DELAY);

    writeInstruction(CTRL_MODE, SET_CURSOR_MASK | LCD_LINE2_LBL_ADDR);

    writeInstruction(CTRL_MODE, SET_CURSOR_MASK | LCD_LINE2_UNT_ADDR);

}


