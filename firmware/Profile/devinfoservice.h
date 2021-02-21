/******************************************************************************

 @file       devinfoservice.h

 @brief This file contains the Device Information service definitions and
 prototypes.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2012-2017, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
 its contributors may be used to endorse or promote products derived
 from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_01_50_00_58
 Release Date: 2017-10-17 18:09:51
 *****************************************************************************/

#ifndef DEVINFOSERVICE_H
#define DEVINFOSERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//#include <ti/ble5stack/central/gatt.h>
#include <ti/sap/sap.h>

	#include <stdint.h>

	/*********************************************************************
	 * CONSTANTS
	 */

// Device Information Service Parameters
#define DEVINFO_SYSTEM_ID                 0
#define DEVINFO_MODEL_NUMBER              1
#define DEVINFO_SERIAL_NUMBER             2
#define DEVINFO_FIRMWARE_REV              3
#define DEVINFO_HARDWARE_REV              4
#define DEVINFO_SOFTWARE_REV              5
#define DEVINFO_MANUFACTURER_NAME         6
#define DEVINFO_11073_CERT_DATA           7
#define DEVINFO_PNP_ID                    8

// IEEE 11073 authoritative body values
#define DEVINFO_11073_BODY_EMPTY          0
#define DEVINFO_11073_BODY_IEEE           1
#define DEVINFO_11073_BODY_CONTINUA       2
#define DEVINFO_11073_BODY_EXP            254

// System ID length
#define DEVINFO_SYSTEM_ID_LEN             8

// PnP ID length
#define DEVINFO_PNP_ID_LEN                7

// String attribute length
#ifndef DEVINFO_STR_ATTR_LEN
#define DEVINFO_STR_ATTR_LEN            20
#endif

	/*
	 * DevInfo_AddService- Initializes the Device Information service by registering
	 *          GATT attributes with the GATT server.
	 *
	 */

	extern uint8_t DevInfo_AddService(void);

	/*********************************************************************
	 * @fn      DevInfo_SetParameter
	 *
	 * @brief   Set a Device Information parameter.
	 *
	 * @param   param - Profile parameter ID
	 * @param   len - length of data to right
	 * @param   value - pointer to data to write.  This is dependent on
	 *          the parameter ID and WILL be cast to the appropriate
	 *          data type (example: data type of uint16 will be cast to
	 *          uint16 pointer).
	 *
	 * @return  uint8_t
	 */
	uint8_t DevInfo_SetParameter(uint8_t param, uint8_t len, void *value);

	/*
	 * DevInfo_GetParameter - Get a Device Information parameter.
	 *
	 *    param - Profile parameter ID
	 *    value - pointer to data to write.  This is dependent on
	 *          the parameter ID and WILL be cast to the appropriate
	 *          data type (example: data type of uint16 will be cast to
	 *          uint16 pointer).
	 */
	extern uint8_t DevInfo_GetParameter(uint8_t param, void *value);

#ifdef __cplusplus
}
#endif

#endif /* DEVINFOSERVICE_H */
