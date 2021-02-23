/******************************************************************************

 @file       devinfoservice.c

 @brief This file contains the Device Information service.

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

/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <ti/ble5stack/central/gatt.h>

#include "constants.h"
#include "profile_util.h"
#include "devinfoservice.h"


extern void* memcpy(void *dest, const void *src, size_t len);



/*********************************************************************
 * Profile Attributes - variables
 */

// Device Information Service attribute
static const gattAttrType_t devInfoService = {ATT_BT_UUID_SIZE, devInfoServUUID};

// System ID characteristic

static uint8_t devInfoSystemId[DEVINFO_SYSTEM_ID_LEN] = {0, 0, 0, 0, 0, 0, 0, 0};

// Model Number String characteristic

static uint8_t devInfoModelNumber[DEVINFO_STR_ATTR_LEN + 1] = "Model Number";

// Serial Number String characteristic

static uint8_t devInfoSerialNumber[DEVINFO_STR_ATTR_LEN + 1] = "Serial Number";


static uint8_t devInfoFirmwareRev[DEVINFO_STR_ATTR_LEN + 1] = "Firmware Revision";

// Hardware Revision String characteristic

static uint8_t devInfoHardwareRev[DEVINFO_STR_ATTR_LEN + 1] = "Hardware Revision";


static uint8_t devInfoSoftwareRev[DEVINFO_STR_ATTR_LEN + 1] = "Software Revision";

// Manufacturer Name String characteristic

static uint8_t devInfoMfrName[DEVINFO_STR_ATTR_LEN + 1] = "Manufacturer Name";

// IEEE 11073-20601 Regulatory Certification Data List characteristic

static uint8_t defaultDevInfo11073Cert[] = {
DEVINFO_11073_BODY_EXP,     // authoritative body type
		0x00,                       // authoritative body structure type
									// authoritative body data follows below:
		'e', 'x', 'p', 'e', 'r', 'i', 'm', 'e', 'n', 't', 'a', 'l'};

// The length of this characteristic is not fixed
static uint8_t *devInfo11073Cert = defaultDevInfo11073Cert;
static uint8_t devInfo11073CertLen = sizeof(defaultDevInfo11073Cert);

// PnP ID characteristic
static uint8_t devInfoPnpIdProps = GATT_PROP_READ;
static uint8_t devInfoPnpId[DEVINFO_PNP_ID_LEN] = {1,                                      // Vendor ID source (1=Bluetooth SIG)
		LO_UINT16(0x000D), HI_UINT16(0x000D),   // Vendor ID (Texas Instruments)
		LO_UINT16(0x0000), HI_UINT16(0x0000),   // Product ID (vendor-specific)
		LO_UINT16(0x0110), HI_UINT16(0x0110)    // Product version (JJ.M.N)
		};

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t devInfoAttrTbl[] = {
// Device Information Service
		{{ATT_BT_UUID_SIZE, primaryServiceUUID}, /* type */
		GATT_PERMIT_READ, /* permissions */
		0, /* handle */
		(uint8_t*) &devInfoService /* pValue */
		},


		{{ATT_BT_UUID_SIZE, devInfoSystemIdUUID},
		GATT_PERMIT_READ, 0, (uint8_t*) devInfoSystemId},



		// Model Number Value
		{{ATT_BT_UUID_SIZE, devInfoModelNumberUUID},
		GATT_PERMIT_READ, 0, (uint8_t*) devInfoModelNumber},

		// Serial Number Value
		{{ATT_BT_UUID_SIZE, devInfoSerialNumberUUID},
		GATT_PERMIT_READ, 0, (uint8_t*) devInfoSerialNumber},


		// Firmware Revision Value
		{{ATT_BT_UUID_SIZE, devInfoFirmwareRevUUID},
		GATT_PERMIT_READ, 0, (uint8_t*) devInfoFirmwareRev},

		// Hardware Revision Value
		{{ATT_BT_UUID_SIZE, devInfoHardwareRevUUID},
		GATT_PERMIT_READ, 0, (uint8_t*) devInfoHardwareRev},


		// Software Revision Value
		{{ATT_BT_UUID_SIZE, devInfoSoftwareRevUUID},
		GATT_PERMIT_READ, 0, (uint8_t*) devInfoSoftwareRev},



		// Manufacturer Name Value
		{{ATT_BT_UUID_SIZE, devInfoMfrNameUUID},
		GATT_PERMIT_READ, 0, (uint8_t*) devInfoMfrName},



		// IEEE 11073-20601 Regulatory Certification Data List Value
		{{ATT_BT_UUID_SIZE, devInfo11073CertUUID},
		GATT_PERMIT_READ, 0, defaultDevInfo11073Cert},


		// PnP ID Value
		{{ATT_BT_UUID_SIZE, devInfoPnpIdUUID},
		GATT_PERMIT_READ, 0, (uint8_t*) devInfoPnpId}};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8_t devInfo_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Device Info Service Callbacks
// Note: When an operation on a characteristic requires authorization and
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an
// operation on a characteristic requires authorization the Stack will call
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be
// made within these functions.
const gattServiceCBs_t devInfoCBs = {devInfo_ReadAttrCB, // Read callback function pointer
		NULL,               // Write callback function pointer
		NULL                // Authorization callback function pointer
		};

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      DevInfo_AddService
 *
 * @brief   Initializes the Device Information service by registering
 *          GATT attributes with the GATT server.
 *
 * @return  Success or Failure
 */
uint8_t DevInfo_AddService(void) {
	// Register GATT attribute list and CBs with GATT Server App

}

/*********************************************************************
 * @fn      DevInfo_SetParameter
 *
 * @brief   Set a Device Information parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return  uint8_t
 */
uint8_t DevInfo_SetParameter(uint8_t param, uint8_t len, void *value) {
	uint8_t ret = SUCCESS;

	switch (param) {
		case DEVINFO_SYSTEM_ID:
			// verify length
			if (len == sizeof(devInfoSystemId)) {
				memcpy(devInfoSystemId, value, len);
			} else {
				ret = BLE_PROFILE_INVALID_RANGE;
			}
			break;

		case DEVINFO_MODEL_NUMBER:
			// verify length, leave room for null-terminate char
			if (len <= DEVINFO_STR_ATTR_LEN) {
				memset(devInfoModelNumber, 0, DEVINFO_STR_ATTR_LEN + 1);
				memcpy(devInfoModelNumber, value, len);
			} else {
				ret = BLE_PROFILE_INVALID_RANGE;
			}
			break;
		case DEVINFO_SERIAL_NUMBER:
			// verify length, leave room for null-terminate char
			if (len <= DEVINFO_STR_ATTR_LEN) {
				memset(devInfoSerialNumber, 0, DEVINFO_STR_ATTR_LEN + 1);
				memcpy(devInfoSerialNumber, value, len);
			} else {
				ret = BLE_PROFILE_INVALID_RANGE;
			}
			break;

		case DEVINFO_FIRMWARE_REV:
			// verify length, leave room for null-terminate char
			if (len <= DEVINFO_STR_ATTR_LEN) {
				memset(devInfoFirmwareRev, 0, DEVINFO_STR_ATTR_LEN + 1);
				memcpy(devInfoFirmwareRev, value, len);
			} else {
				ret = BLE_PROFILE_INVALID_RANGE;
			}
			break;

		case DEVINFO_HARDWARE_REV:
			// verify length, leave room for null-terminate char
			if (len <= DEVINFO_STR_ATTR_LEN) {
				memset(devInfoHardwareRev, 0, DEVINFO_STR_ATTR_LEN + 1);
				memcpy(devInfoHardwareRev, value, len);
			} else {
				ret = BLE_PROFILE_INVALID_RANGE;
			}
			break;

		case DEVINFO_SOFTWARE_REV:
			// verify length, leave room for null-terminate char
			if (len <= DEVINFO_STR_ATTR_LEN) {
				memset(devInfoSoftwareRev, 0, DEVINFO_STR_ATTR_LEN + 1);
				memcpy(devInfoSoftwareRev, value, len);
			} else {
				ret = BLE_PROFILE_INVALID_RANGE;
			}
			break;

		case DEVINFO_MANUFACTURER_NAME:
			// verify length, leave room for null-terminate char
			if (len <= DEVINFO_STR_ATTR_LEN) {
				memset(devInfoMfrName, 0, DEVINFO_STR_ATTR_LEN + 1);
				memcpy(devInfoMfrName, value, len);
			} else {
				ret = BLE_PROFILE_INVALID_RANGE;
			}
			break;

		case DEVINFO_11073_CERT_DATA: {
			// Allocate buffer for new certification
			uint8_t *pCert = malloc(len);
			if (pCert != NULL) {
				if (devInfo11073Cert != defaultDevInfo11073Cert) {
					// Free existing certification buffer
					free(devInfo11073Cert);
				}

				// Copy over new certification
				memcpy(pCert, value, len);

				// Update our globals
				devInfo11073Cert = pCert;
				devInfo11073CertLen = len;
			} else {
				ret = BLE_PROFILE_MEM_ALLOC_ERROR;
			}
		}
			break;

		case DEVINFO_PNP_ID:
			// verify length
			if (len == sizeof(devInfoPnpId)) {
				memcpy(devInfoPnpId, value, len);
			} else {
				ret = BLE_PROFILE_INVALID_RANGE;
			}
			break;

		default:
			ret = BLE_PROFILE_INVALIDPARAMETER;
			break;
	}

	return (ret);
}

/*********************************************************************
 * @fn      DevInfo_GetParameter
 *
 * @brief   Get a Device Information parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to get.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return  uint8_t
 */
uint8_t DevInfo_GetParameter(uint8_t param, void *value) {
	uint8_t ret = SUCCESS;

	switch (param) {
		case DEVINFO_SYSTEM_ID:
			memcpy(value, devInfoSystemId, sizeof(devInfoSystemId));
			break;

		case DEVINFO_MODEL_NUMBER:
			memcpy(value, devInfoModelNumber, DEVINFO_STR_ATTR_LEN);
			break;
		case DEVINFO_SERIAL_NUMBER:
			memcpy(value, devInfoSerialNumber, DEVINFO_STR_ATTR_LEN);
			break;

		case DEVINFO_FIRMWARE_REV:
			memcpy(value, devInfoFirmwareRev, DEVINFO_STR_ATTR_LEN);
			break;

		case DEVINFO_HARDWARE_REV:
			memcpy(value, devInfoHardwareRev, DEVINFO_STR_ATTR_LEN);
			break;

		case DEVINFO_SOFTWARE_REV:
			memcpy(value, devInfoSoftwareRev, DEVINFO_STR_ATTR_LEN);
			break;

		case DEVINFO_MANUFACTURER_NAME:
			memcpy(value, devInfoMfrName, DEVINFO_STR_ATTR_LEN);
			break;

		case DEVINFO_11073_CERT_DATA:
			memcpy(value, devInfo11073Cert, devInfo11073CertLen);
			break;

		case DEVINFO_PNP_ID:
			memcpy(value, devInfoPnpId, sizeof(devInfoPnpId));
			break;

		default:
			ret = BLE_PROFILE_INVALIDPARAMETER;
			break;
	}

	return (ret);
}

/*********************************************************************
 * @fn          devInfo_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static uint8_t devInfo_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method) {
	uint8_t status = SUCCESS;
	uint16_t uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

	// If the value offset of the Read Blob Request is greater than the
	// length of the attribute value, an Error Response shall be sent with
	// the error code Invalid Offset.
	switch (uuid) {
		case SYSTEM_ID_UUID:
			// verify offset
			if (offset > sizeof(devInfoSystemId)) {
				status = BLE_PROFILE_INVALID_MEM_SIZE;
			} else {
				// determine read length
				*pLen = MIN(maxLen, (sizeof(devInfoSystemId) - offset));

				// copy data
				memcpy(pValue, &devInfoSystemId[offset], *pLen);
			}
			break;

	case MODEL_NUMBER_UUID:
		case SERIAL_NUMBER_UUID:
		case FIRMWARE_REV_UUID:
		case HARDWARE_REV_UUID:
		case SOFTWARE_REV_UUID:
		case MANUFACTURER_NAME_UUID: {
			uint16_t len = strlen((char*) (pAttr->pValue));

			// verify offset
			if (offset > len) {
				status = BLE_PROFILE_INVALID_MEM_SIZE;
			} else {
				// determine read length (exclude null terminating character)
				*pLen = MIN(maxLen, (len - offset));

				// copy data
				memcpy(pValue, &(pAttr->pValue[offset]), *pLen);
			}
		}
			break;

		case IEEE_11073_CERT_DATA_UUID:
			// verify offset
			if (offset > devInfo11073CertLen) {
				status = BLE_PROFILE_INVALID_MEM_SIZE;
			} else {
				// determine read length
				*pLen = MIN(maxLen, (devInfo11073CertLen - offset));

				// copy data
				memcpy(pValue, &devInfo11073Cert[offset], *pLen);
			}
			break;

		case PNP_ID_UUID:
			// verify offset
			if (offset > sizeof(devInfoPnpId)) {
				status = BLE_PROFILE_INVALID_MEM_SIZE;
			} else {
				// determine read length
				*pLen = MIN(maxLen, (sizeof(devInfoPnpId) - offset));

				// copy data
				memcpy(pValue, &devInfoPnpId[offset], *pLen);
			}
			break;

		default:
			*pLen = 0;
			status = ATT_ERR_ATTR_NOT_FOUND;
			break;
	}

	return (status);
}

/*********************************************************************
 *********************************************************************/
