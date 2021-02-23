/***********************************************************************
 * \file 		hidservice.c
 * \author		Texas Instruments
 * \author 		Alex Westerman
 * \brief
 * \copyright 	See LICENSE
 ***********************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <ti/ble5stack/central/gatt.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sap/snp.h>
#include <ti/sap/snp_rpc.h>
#include <ti/sap/sap.h>

#include "hidservice.h"
//#include "devinfoservice.h"
//#include "battservice.h"
#include "profile_util.h"
#include "bt_uuid.h"
#include "constants.h"

static hidDevCB_t *pHidDevCB;

static hidDevCfg_t *pHidDevCfg;

// HID report map length
uint8_t hidReportMapLen = sizeof(hidReportMap);

// HID report mapping table
static hidRptMap_t hidRptMap[HID_NUM_REPORTS];

/*********************************************************************
 * Profile Attributes - variables
 */
// HID Information characteristic value
static const uint8_t hidInfo[HID_INFORMATION_LEN] = {LO_UINT16(0x0111), HI_UINT16(0x0111),           // bcdHID (USB HID version)
0x00,           // bCountryCode
		HID_SERVICE_FLAGS           // Flags
		};

SAP_Service_t hidService;
SAP_CharHandle_t hidServiceCharHandles[5];

static uint8_t hidServiceUUID = {SNP_16BIT_UUID_SIZE, {LO_UINT16(HID_SERV_UUID), HI_UINT16(HID_SERV_UUID)}};

/*******************************************************************************
 *                              Profile Attributes - TABLE
 ******************************************************************************/
/**HID SERVICE ATTRIBUTE**/
static uint8_t hidProfile_hidService = 0;
static uint8_t hidProfile_hidService_desc[] = "HID Service";
SAP_UserDescAttr_t hidProfile_hidService_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_hidService_desc), sizeof(hidProfile_hidService_desc), hidProfile_hidService_desc};

///**INCLUDE BATTERY SERVICE ATTRIBUTE**/
//static uint8_t hidProfile_includeBattServ = 0;
//static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
//SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_infoChar = 0;
static uint8_t hidProfile_infoChar_desc[] = "HID Information Characteristic";
SAP_UserDescAttr_t hidProfile_infoChar_userdesc = {SNP_GATT_PERMIT_ENCRYPT_READ, sizeof(hidProfile_infoChar_desc), sizeof(hidProfile_infoChar_desc), hidProfile_infoChar_desc};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc};

SAP_UserCCCDAttr_t char4CCCD = {SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE};

#define SERVAPP_NUM_ATTR_SUPPORTED 5

static SAP_Char_t hidProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = {
//HID Service Characteristic
		{{SNP_16BIT_UUID_SIZE, hidServiceUUID}, /* UUID */
		SNP_GATT_PROP_READ, /* Properties */
		SNP_GATT_PERMIT_READ, /* Permissions */
		&hidProfile_hidService_userdesc /* User Description */},

//		//Include Battery Service Characteristic
//		{{SNP_16BIT_UUID_SIZE, includeUUID}, /* UUID */
//		SNP_GATT_PROP_READ, /* Properties */
//		SNP_GATT_PERMIT_READ, /* Permissions */
//		&hidProfile_includeBattServ_userdesc /* User Description */},

		//HID Information Characteristic Declaration
		{{SNP_16BIT_UUID_SIZE, hidInfoUUID}, /* UUID */
		SNP_GATT_PROP_READ, /* Properties */
		SNP_GATT_PERMIT_READ, /* Permissions */
		&hidProfile_infoChar_userdesc /* User Description */},

		//HID Service Characteristic
		{{SNP_16BIT_UUID_SIZE, hidServiceUUID}, /* UUID */
		SNP_GATT_PROP_READ, /* Properties */
		SNP_GATT_PERMIT_READ, /* Permissions */
		&hidProfile_hidService_userdesc /* User Description */},
		//HID Service Characteristic
		{{SNP_16BIT_UUID_SIZE, hidServiceUUID}, /* UUID */
		SNP_GATT_PROP_READ, /* Properties */
		SNP_GATT_PERMIT_READ, /* Permissions */
		&hidProfile_hidService_userdesc /* User Description */}, };
/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t hidAttrTbl[] = {
// HID Service
		{{ATT_BT_UUID_SIZE, primaryServiceUUID}, /* type */
		GATT_PERMIT_READ, /* permissions */
		0, /* handle */
		(uint8*) &hidService /* pValue */
		},

		// Included service (battery)
		{{ATT_BT_UUID_SIZE, includeUUID}, GATT_PERMIT_READ, 0, (uint8*) &include},

		// HID Information characteristic
		{{ATT_BT_UUID_SIZE, hidInfoUUID}, GATT_PERMIT_ENCRYPT_READ, 0, (uint8*) hidInfo},

		// HID Report Map characteristic
		{{ATT_BT_UUID_SIZE, hidReportMapUUID}, GATT_PERMIT_ENCRYPT_READ, 0, (uint8*) hidReportMap},

		// HID External Report Reference Descriptor
		{{ATT_BT_UUID_SIZE, extReportRefUUID}, GATT_PERMIT_READ, 0, hidExtReportRefDesc},

		// HID Report characteristic, key input
		{{ATT_BT_UUID_SIZE, hidReportUUID}, GATT_PERMIT_ENCRYPT_READ, 0, &hidReportKeyIn},

		// HID Report characteristic client characteristic configuration, key input
		{{ATT_BT_UUID_SIZE, clientCharCfgUUID}, GATT_PERMIT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, (uint8*) &hidReportKeyInClientCharCfg},

		// HID Report Reference characteristic descriptor, key input
		{{ATT_BT_UUID_SIZE, reportRefUUID}, GATT_PERMIT_READ, 0, hidReportRefKeyIn},

};

// Attribute index enumeration-- these indexes match array elements above
enum {
	HID_SERVICE_IDX,                // HID Service
	HID_INCLUDED_SERVICE_IDX,       // Included Service (battery)
	HID_INFO_IDX,                   // HID Information characteristic
	HID_REPORT_MAP_IDX,             // HID Report Map characteristic
	HID_EXT_REPORT_REF_DESC_IDX,    // HID External Report Reference Descriptor
	HID_REPORT_KEY_IN_DECL_IDX,     // HID Report characteristic declaration, key input
	HID_REPORT_KEY_IN_IDX,          // HID Report characteristic, key input

};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/***********************************************************************
 * 					UTILITY FUNCTIONS
 ***********************************************************************/

/*********************************************************************
 * @fn      HidDev_Register
 *
 * @brief   Register a callback function with HID Dev.
 *
 * @param   pCfg         - Parameter configuration.
 * @param   pfnServiceCB - Callback function.
 *
 * @return  None.
 */
void HidDev_Register(hidDevCfg_t *pCfg, hidDevCB_t *pCBs) {
	pHidDevCB = pCBs;
	pHidDevCfg = pCfg;

// If configured and not zero, create the idle timeout clock.
	if ((pHidDevCfg != NULL) && (pHidDevCfg->idleTimeout != 0)) {

	}
	Clock_construct(&idleTimeoutClock, HidDev_clockHandler, pHidDevCfg->idleTimeout, 0, false, HID_IDLE_EVT);
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn          HidService_ReadAttrCB
 *
 * @brief       HidService attribute read callback.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr      - pointer to attribute
 * @param       pValue     - pointer to data to be read
 * @param       pLen       - length of data to be read
 * @param       offset     - offset of the first octet to be read
 * @param       maxLen     - maximum length of data to be read
 * @param       method     - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
uint8_t HidService_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method) {
	uint8_t status = SUCCESS;
	hidRptMap_t *pRpt;

	uint16_t uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

// Only report map is long.
	if (offset > 0 && uuid != REPORT_MAP_UUID) {
		return (ATT_ERR_ATTR_NOT_LONG);
	}

	if (uuid == REPORT_UUID || uuid == BOOT_KEY_INPUT_UUID || uuid == BOOT_KEY_OUTPUT_UUID || uuid == BOOT_MOUSE_INPUT_UUID) {
// Find report ID in table.
		if ((pRpt = HidDev_reportByHandle(pAttr->handle)) != NULL) {
			// Execute report callback.
			status = (*pHidDevCB->reportCB)(pRpt->id, pRpt->type, uuid, HID_DEV_OPER_READ, pLen, pValue);
		} else {
			*pLen = 0;
		}
	} else if (uuid == REPORT_MAP_UUID) {
// If the value offset of the Read Blob Request is greater than the
// length of the attribute value, an Error Response shall be sent with
// the error code Invalid Offset.
		if (offset > hidReportMapLen) {
			status = ATT_ERR_INVALID_OFFSET;
		} else {
			// Determine read length.
			*pLen = MIN(maxLen, (hidReportMapLen - offset));

			// Copy data.
			memcpy(&pValue, pAttr->pValue + offset, (unsigned int) *pLen);
		}
	} else if (uuid == HID_INFORMATION_UUID) {
		*pLen = HID_INFORMATION_LEN;
		memcpy(pValue, pAttr->pValue, HID_INFORMATION_LEN);
	} else if (uuid == GATT_REPORT_REF_UUID) {
		*pLen = HID_REPORT_REF_LEN;
		memcpy(pValue, pAttr->pValue, HID_REPORT_REF_LEN);
	} else if (uuid == GATT_EXT_REPORT_REF_UUID) {
		*pLen = HID_EXT_REPORT_REF_LEN;
		memcpy(pValue, pAttr->pValue, HID_EXT_REPORT_REF_LEN);
	}

// Restart idle timer.
	if (status == SUCCESS) {
		HidDev_StartIdleTimer();
	}

	return (status);
}

/*********************************************************************
 * @fn      HidService_WriteAttrCB
 *
 * @brief   HidService attribute write callback.
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr      - pointer to attribute
 * @param   pValue     - pointer to data to be written
 * @param   len        - length of data
 * @param   offset     - offset of the first octet to be written
 * @param   method     - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
uint8_t HidService_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t len, uint16_t offset, uint8_t method, void *idleTimerFxn) {
	uint8_t status = BLE_PROFILE_SUCCESS;
	hidRptMap_t *pRpt;

// Make sure it's not a blob operation (no attributes in the profile are long).
	if (offset > 0) {
		return (BLE_PROFILE_INVALID_MEM_SIZE);
	}

	uint16_t uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

	if (uuid == REPORT_UUID) {
// Find report ID in table.
		if ((pRpt = HidDev_reportByHandle(pAttr->handle)) != NULL) {
			// Execute report callback.
			status = (*pHidDevCB->reportCB)(pRpt->id, pRpt->type, uuid, HID_DEV_OPER_WRITE, &len, pValue);
		} else if (uuid == GATT_CLIENT_CHAR_CFG_UUID) {
			status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset, GATT_CLIENT_CFG_NOTIFY);
			if (status == SUCCESS) {
				uint16_t charCfg = BUILD_UINT16(pValue[0], pValue[1]);

				// Find report ID in table.
				if ((pRpt = HidDev_reportByCccdHandle(pAttr->handle)) != NULL) {
					// Execute report callback.
					(*pHidDevCB->reportCB)(pRpt->id, pRpt->type, uuid, (charCfg == GATT_CLIENT_CFG_NOTIFY) ? HID_DEV_OPER_ENABLE : HID_DEV_OPER_DISABLE, &len, pValue);
				}
			}
		}
	}

// Restart idle timer.
	if (status == BLE_PROFILE_SUCCESS) {
		(idleTimerFxn);
	}

	return (status);
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HidService_AddService
 *
 * @brief   Initializes the HID Service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   none
 *
 * @return  Success or Failure
 */
uint8_t HidService_AddService(void) {
	/* Reads through table, adding attributes to the NP. */
	hidService.serviceUUID = hidServiceUUID;
	hidService.serviceType = SNP_PRIMARY_SERVICE;
	hidService.charTableLen = SERVAPP_NUM_ATTR_SUPPORTED;
	hidService.charTable = hidProfileAttrTbl;
	hidService.context = NULL;
	hidService.charReadCallback = HidService_ReadAttrCB;
	hidService.charWriteCallback = HidService_WriteAttrCB;
	hidService.cccdIndCallback = HidService_CCCDIndCB;
	hidService.charAttrHandles = hidServiceCharHandles;

	/* Service is setup, register with GATT server on the SNP. */
	SAP_registerService(&hidService);

	return BLE_PROFILE_SUCCESS;

// Set up included service
	//Batt_GetParameter(BATT_PARAM_SERVICE_HANDLE, &GATT_INCLUDED_HANDLE(hidAttrTbl, HID_INCLUDED_SERVICE_IDX));

// Construct map of reports to characteristic handles
// Each report is uniquely identified via its ID and type

// Key input report
	hidRptMap[0].id = hidReportRefKeyIn[0];
	hidRptMap[0].type = hidReportRefKeyIn[1];
	hidRptMap[0].handle = hidAttrTbl[HID_REPORT_KEY_IN_IDX].handle;
	hidRptMap[0].pCccdAttr = &hidAttrTbl[HID_REPORT_KEY_IN_CCCD_IDX];
	hidRptMap[0].mode = HID_PROTOCOL_MODE_REPORT;

// Battery level input report
	//void Batt_GetParameter(BATT_PARAM_BATT_LEVEL_IN_REPORT, &(hidRptMap[1]));

// Setup report ID map
	HidDev_RegisterReports(HID_NUM_REPORTS, hidRptMap);

	return status;
}

/*********************************************************************
 * @fn      HidKbd_SetParameter
 *
 * @brief   Set a HID Kbd parameter.
 *
 * @param   id     - HID report ID.
 * @param   type   - HID report type.
 * @param   uuid   - attribute uuid.
 * @param   len    - length of data to right.
 * @param   pValue - pointer to data to write.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
uint32_t Hid_SetParameter(uint8 id, uint8 type, uint16 uuid, uint8 len, void *pValue) {
	uint_fast8_t ret = BLE_PROFILE_SUCCESS;

	switch (uuid) {
		case REPORT_UUID:
			if (type == HID_REPORT_TYPE_OUTPUT) {
				if (len == 1) {
					hidReportLedOut = *((uint8*) pValue);
				} else {
					ret = BLE_PROFILE_INVALID_RANGE;
				}
			} else {
				ret = BLE_PROFILE_INVALID_MSG_POINTER
			}
			break;

		default:
			// Ignore the request
			break;
	}

	return (ret);
}

/*********************************************************************
 * @fn      HidKbd_GetParameter
 *
 * @brief   Get a HID Kbd parameter.
 *
 * @param   id     - HID report ID.
 * @param   type   - HID report type.
 * @param   uuid   - attribute uuid.
 * @param   pLen   - length of data to be read
 * @param   pValue - pointer to data to get.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
uint8_t Hid_GetParameter(uint8 id, uint8 type, uint16 uuid, uint8 *pLen, void *pValue) {
	switch (uuid) {
		case REPORT_UUID:
			if (type == HID_REPORT_TYPE_OUTPUT) {
				*((uint8_t*) pValue) = hidReportLedOut;
				*pLen = 1;
			} else {
				*pLen = 0;
			}
			break;

		default:
			*pLen = 0;
			break;
	}

	return (BLE_PROFILE_SUCCESS);
}

/*********************************************************************
 *********************************************************************/
