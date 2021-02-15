/**********************************************************************
 * \headerfile	platform.h
 * \author 		Texas Instruments
 * \author 		(Reformatted and Doxygenized by Alex Westerman)
 * \brief		TI-provided MSP432 Platform specific prototypes and macros
 * \copyright 	See LICENSE_TI and LICENSE
 ***********************************************************************/

#include <stdint.h>

#ifndef __PLATFORM_H
#define __PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif
	
	/* Function used to reboot the device */
	extern void MCU_rebootDevice(void);
	
	/* CRC Calculation Function */
	extern uint32_t MCU_calculateCRC32(uint8_t *imageAddress, uint32_t imageLengthBytes);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_H */
