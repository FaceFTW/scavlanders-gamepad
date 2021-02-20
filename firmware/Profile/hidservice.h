/******************************************************************************

 @file       hidservice.h

 @brief This file contains the HID service for Keyboard and Consumer
 Control.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 Release Name: simplelink_cc2640r2_sdk_ble_example_pack_01_50_00_62
 Release Date: 2017-11-01 10:38:41
 *****************************************************************************/

#ifndef HIDSERVICE_H
#define HIDSERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

	/*********************************************************************
	 * INCLUDES
	 */

	/*********************************************************************
	 * CONSTANTS
	 */

// Number of HID reports defined in the service
#define HID_NUM_REPORTS                  7

// HID Report IDs for the service
#define HID_RPT_ID_LED_OUT               0  // LED output report ID
#define HID_RPT_ID_KEY_IN                1  // Keyboard input report ID
#define HID_RPT_ID_CC_IN                 2  // Consumer Control input report ID
#define HID_RPT_ID_VOICE_START_IN        10  // Voice Start input report ID
#define HID_RPT_ID_VOICE_DATA_IN         11  // Voice Data input report ID

// HID feature flags
#define HID_SERVICE_FLAGS                HID_FLAGS_REMOTE_WAKE

//Length Vals
#define HID_DEV_DATA_LEN                      9

#ifdef HID_DEV_RPT_QUEUE_LEN
  #define HID_DEV_REPORT_Q_SIZE               (HID_DEV_RPT_QUEUE_LEN+1)
#else
  #define HID_DEV_REPORT_Q_SIZE               (10+1)
#endif

	/*********************************************************************
	 * TYPEDEFS
	 */

	/*********************************************************************
	 * MACROS
	 */

	/*********************************************************************
	 * Profile Callbacks
	 */

	/*********************************************************************
	 * API FUNCTIONS
	 */

	/*********************************************************************
	 * @fn      HidService_AddService
	 *
	 * @brief   Initializes the HID service by registering
	 *          GATT attributes with the GATT server.
	 *
	 * @param   none
	 *
	 * @return  Success or Failure
	 */
	extern bStatus_t HidService_AddService(void);


#ifdef __cplusplus
}
#endif

#endif /* HIDSERVICE_H */
