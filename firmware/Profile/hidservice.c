/***********************************************************************
 * \file 		hidservice.c
 * \author		Texas Instruments
 * \author 		Alex Westerman
 * \brief
 * \copyright 	See LICENSE
 ***********************************************************************/
#include <stdint.h>
#include <ti/ble5stack/central/gatt.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Clock.h>

#include "hidservice.h"
#include "profile_util.h"
#include "bt_uuid.h"
#include "constants.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

//RTOS related Variables
static Semaphore_Handle serviceHandle;		//
static Queue_Handle serviceQueue;			//Queue Object for service Messages
static Queue_Struct serviceMsg;				//Queue Structure for Service Messages

// HID report map length
uint8_t hidReportMapLen = sizeof(hidReportMap);

// HID report mapping table
static hidRptMap_t hidRptMap[HID_NUM_REPORTS];



/*********************************************************************
 * Profile Attributes - variables
 */

SAP_Service_t simpleService;
SAP_CharHandle_t simpleServiceCharHandles[4];

static UUIDType_t simpleServiceUUID = {SNP_16BIT_UUID_SIZE, simpleProfileServUUID};

/* Characteristic 1 Value */
static uint8_t simpleProfileChar1 = 0;
static uint8_t simpleProfileChar1UserDesp[17] = "Characteristic 1";

/* Characteristic 2 Value */
static uint8_t simpleProfileChar2 = 0;

/* Simple Profile Characteristic 2 User Description */
static uint8_t simpleProfileChar2UserDesp[17] = "Characteristic 2";

/* Characteristic 3 Value */
static uint8_t simpleProfileChar3 = 0;

/* Simple Profile Characteristic 3 User Description */
static uint8_t simpleProfileChar3UserDesp[17] = "Characteristic 3";

/* Characteristic 4 Value */
static uint8_t simpleProfileChar4 = 0;

/* Simple Profile Characteristic 4 User Description */
static uint8_t simpleProfileChar4UserDesp[17] = "Characteristic 4";

/*******************************************************************************
 *                              Profile Attributes - TABLE
 ******************************************************************************/
SAP_UserDescAttr_t char1UserDesc = {SNP_GATT_PERMIT_READ, sizeof(simpleProfileChar1UserDesp), sizeof(simpleProfileChar1UserDesp), simpleProfileChar1UserDesp};

SAP_UserDescAttr_t char2UserDesc = {SNP_GATT_PERMIT_READ, sizeof(simpleProfileChar2UserDesp), sizeof(simpleProfileChar2UserDesp), simpleProfileChar2UserDesp};

SAP_UserDescAttr_t char3UserDesc = {SNP_GATT_PERMIT_READ, sizeof(simpleProfileChar3UserDesp), sizeof(simpleProfileChar3UserDesp), simpleProfileChar3UserDesp};

SAP_UserDescAttr_t char4UserDesc = {SNP_GATT_PERMIT_READ, sizeof(simpleProfileChar4UserDesp), sizeof(simpleProfileChar4UserDesp), simpleProfileChar4UserDesp};

SAP_UserCCCDAttr_t char4CCCD = {SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE};

#define SERVAPP_NUM_ATTR_SUPPORTED 4

static SAP_Char_t simpleProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = {
/* Characteristic 1 Value Declaration */
{{SNP_16BIT_UUID_SIZE, simpleProfilechar1UUID}, /* UUID */
SNP_GATT_PROP_READ | SNP_GATT_PROP_WRITE, /* Properties */
SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE, /* Permissions */
&char1UserDesc /* User Description */
},

/* Characteristic 2 Value Declaration */
{{SNP_16BIT_UUID_SIZE, simpleProfilechar2UUID}, /* UUID */
SNP_GATT_PROP_READ, /* Properties */
SNP_GATT_PERMIT_READ, /* Permissions */
&char2UserDesc /* User Description */
},

/* Characteristic 3 Value Declaration */
{{SNP_16BIT_UUID_SIZE, simpleProfilechar3UUID}, /* UUID */
SNP_GATT_PROP_WRITE, /* Properties */
SNP_GATT_PERMIT_WRITE, /* Permissions */
&char3UserDesc /* User Description */
},

/* Characteristic 4 Value Declaration */
{{SNP_16BIT_UUID_SIZE, simpleProfilechar4UUID}, /* UUID */
SNP_GATT_PROP_NOTIFICATION, /* Properties */
0, /* Permissions */
&char4UserDesc, /* User Description */
&char4CCCD /* CCCD */
}, };
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

		// HID Information characteristic declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidInfoProps},

		// HID Information characteristic
		{{ATT_BT_UUID_SIZE, hidInfoUUID}, GATT_PERMIT_ENCRYPT_READ, 0, (uint8*) hidInfo},

		// HID Control Point characteristic declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidControlPointProps},

		// HID Control Point characteristic
		{{ATT_BT_UUID_SIZE, hidControlPointUUID}, GATT_PERMIT_ENCRYPT_WRITE, 0, &hidControlPoint},

		// HID Protocol Mode characteristic declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidProtocolModeProps},

		// HID Protocol Mode characteristic
		{{ATT_BT_UUID_SIZE, hidProtocolModeUUID}, GATT_PERMIT_ENCRYPT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, &hidProtocolMode},

		// HID Report Map characteristic declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidReportMapProps},

		// HID Report Map characteristic
		{{ATT_BT_UUID_SIZE, hidReportMapUUID}, GATT_PERMIT_ENCRYPT_READ, 0, (uint8*) hidReportMap},

		// HID External Report Reference Descriptor
		{{ATT_BT_UUID_SIZE, extReportRefUUID}, GATT_PERMIT_READ, 0, hidExtReportRefDesc},

		// HID Report characteristic declaration, key input
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidReportKeyInProps},

		// HID Report characteristic, key input
		{{ATT_BT_UUID_SIZE, hidReportUUID}, GATT_PERMIT_ENCRYPT_READ, 0, &hidReportKeyIn},

		// HID Report characteristic client characteristic configuration, key input
		{{ATT_BT_UUID_SIZE, clientCharCfgUUID}, GATT_PERMIT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, (uint8*) &hidReportKeyInClientCharCfg},

		// HID Report Reference characteristic descriptor, key input
		{{ATT_BT_UUID_SIZE, reportRefUUID}, GATT_PERMIT_READ, 0, hidReportRefKeyIn},

		// HID Report characteristic, LED output declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidReportLedOutProps},

		// HID Report characteristic, LED output
		{{ATT_BT_UUID_SIZE, hidReportUUID}, GATT_PERMIT_ENCRYPT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, &hidReportLedOut},

		// HID Report Reference characteristic descriptor, LED output
		{{ATT_BT_UUID_SIZE, reportRefUUID}, GATT_PERMIT_READ, 0, hidReportRefLedOut},

		// HID Boot Keyboard Input Report declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidReportBootKeyInProps},

		// HID Boot Keyboard Input Report
		{{ATT_BT_UUID_SIZE, hidBootKeyInputUUID}, GATT_PERMIT_ENCRYPT_READ, 0, &hidReportBootKeyIn},

		// HID Boot Keyboard Input Report characteristic client characteristic configuration
		{{ATT_BT_UUID_SIZE, clientCharCfgUUID}, GATT_PERMIT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, (uint8*) &hidReportBootKeyInClientCharCfg},

		// HID Boot Keyboard Output Report declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidReportBootKeyOutProps},

		// HID Boot Keyboard Output Report
		{{ATT_BT_UUID_SIZE, hidBootKeyOutputUUID}, GATT_PERMIT_ENCRYPT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, &hidReportBootKeyOut},

		// HID Report characteristic declaration, consumer control
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidReportCCInProps},

		// HID Report characteristic, consumer control
		{{ATT_BT_UUID_SIZE, hidReportUUID}, GATT_PERMIT_ENCRYPT_READ, 0, &hidReportCCIn},

		// HID Report characteristic client characteristic configuration, consumer control
		{{ATT_BT_UUID_SIZE, clientCharCfgUUID}, GATT_PERMIT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, (uint8*) &hidReportCCInClientCharCfg},

		// HID Report Reference characteristic descriptor, consumer control
		{{ATT_BT_UUID_SIZE, reportRefUUID}, GATT_PERMIT_READ, 0, hidReportRefCCIn},

		// HID Voice Start Input Report declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidReportVoiceStartProps},

		// HID Voice Start Input Report
		{{ATT_BT_UUID_SIZE, hidReportUUID}, GATT_PERMIT_ENCRYPT_READ, 0, &hidReportVoiceStart},

		// HID Voice Start Input Report characteristic client characteristic configuration
		{{ATT_BT_UUID_SIZE, clientCharCfgUUID}, GATT_PERMIT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, (uint8*) &hidReportVoiceStartInClientCharCfg},

		// HID Report Reference characteristic descriptor, Voice Start
		{{ATT_BT_UUID_SIZE, reportRefUUID}, GATT_PERMIT_READ, 0, hidReportRefVoiceStart},

		// HID Voice Data Input Report declaration
		{{ATT_BT_UUID_SIZE, characterUUID}, GATT_PERMIT_READ, 0, &hidReportVoiceDataProps},

		// HID Voice Data Input Report
		{{ATT_BT_UUID_SIZE, hidReportUUID}, GATT_PERMIT_ENCRYPT_READ, 0, &hidReportVoiceData},

		// HID Voice Data Input Report characteristic client characteristic configuration
		{{ATT_BT_UUID_SIZE, clientCharCfgUUID}, GATT_PERMIT_READ | GATT_PERMIT_ENCRYPT_WRITE, 0, (uint8*) &hidReportVoiceDataInClientCharCfg},

		// HID Report Reference characteristic descriptor, Voice Data
		{{ATT_BT_UUID_SIZE, reportRefUUID}, GATT_PERMIT_READ, 0, hidReportRefVoiceData},

};

// Attribute index enumeration-- these indexes match array elements above
enum {
	HID_SERVICE_IDX,                // HID Service
	HID_INCLUDED_SERVICE_IDX,       // Included Service (battery)
	HID_INFO_DECL_IDX,              // HID Information characteristic declaration
	HID_INFO_IDX,                   // HID Information characteristic
	HID_CONTROL_POINT_DECL_IDX,     // HID Control Point characteristic declaration
	HID_CONTROL_POINT_IDX,          // HID Control Point characteristic
	HID_PROTOCOL_MODE_DECL_IDX,     // HID Protocol Mode characteristic declaration
	HID_PROTOCOL_MODE_IDX,          // HID Protocol Mode characteristic
	HID_REPORT_MAP_DECL_IDX,        // HID Report Map characteristic declaration
	HID_REPORT_MAP_IDX,             // HID Report Map characteristic
	HID_EXT_REPORT_REF_DESC_IDX,    // HID External Report Reference Descriptor
	HID_REPORT_KEY_IN_DECL_IDX,     // HID Report characteristic declaration, key input
	HID_REPORT_KEY_IN_IDX,          // HID Report characteristic, key input
	HID_REPORT_KEY_IN_CCCD_IDX,     // HID Report characteristic client characteristic configuration, key input
	HID_REPORT_REF_KEY_IN_IDX,      // HID Report Reference characteristic descriptor, key input
	HID_REPORT_LED_OUT_DECL_IDX,    // HID Report characteristic, LED output declaration
	HID_REPORT_LED_OUT_IDX,         // HID Report characteristic, LED output
	HID_REPORT_REF_LED_OUT_IDX,     // HID Report Reference characteristic descriptor, LED output
	HID_BOOT_KEY_IN_DECL_IDX,       // HID Boot Keyboard Input Report declaration
	HID_BOOT_KEY_IN_IDX,            // HID Boot Keyboard Input Report
	HID_BOOT_KEY_IN_CCCD_IDX,       // HID Boot Keyboard Input Report characteristic client characteristic configuration
	HID_BOOT_KEY_OUT_DECL_IDX,      // HID Boot Keyboard Output Report declaration
	HID_BOOT_KEY_OUT_IDX,           // HID Boot Keyboard Output Report
	HID_REPORT_CC_IN_DECL_IDX,      // HID Report characteristic declaration, consumer control
	HID_REPORT_CC_IN_IDX,           // HID Report characteristic, consumer control
	HID_REPORT_CC_IN_CCCD_IDX,      // HID Report characteristic client characteristic configuration, consumer control
	HID_REPORT_REF_CC_IN_IDX,       // HID Report Reference characteristic descriptor, consumer control
	HID_VOICE_START_IN_DECL_IDX,    // HID Voice Start Input Report declaration
	HID_VOICE_START_IN_IDX,         // HID Voice Start Input Report
	HID_VOICE_START_IN_CCCD_IDX,    // HID Voice Start Input Report characteristic client characteristic configuration
	HID_REPORT_REF_VOICE_START_IDX, // HID Report Reference characteristic descriptor, Voice Start
	HID_VOICE_DATA_IN_DECL_IDX,     // HID Voice Start Input Report declaration
	HID_VOICE_DATA_IN_IDX,          // HID Voice Start Input Report
	HID_VOICE_DATA_IN_CCCD_IDX,     // HID Voice Start Input Report characteristic client characteristic configuration
	HID_REPORT_REF_VOICE_DATA_IDX,  // HID Report Reference characteristic descriptor, Voice Start
};

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
bStatus_t HidService_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method) {
	return HidDev_ReadAttrCB(connHandle, pAttr, pValue, pLen, offset, maxLen, method);
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
bStatus_t HidService_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t len, uint16_t offset, uint8_t method) {
	bStatus_t status = SUCCESS;
	hidRptMap_t* pRpt;


	// Make sure it's not a blob operation (no attributes in the profile are long).
	if (offset > 0) {
		return (ATT_ERR_ATTR_NOT_LONG);
	}

	uint16_t uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

	if (uuid == REPORT_UUID || uuid == BOOT_KEY_OUTPUT_UUID) {
		// Find report ID in table.
		if ((pRpt = HidDev_reportByHandle(pAttr->handle)) != NULL) {
			// Execute report callback.
			status = (*pHidDevCB->reportCB)(pRpt->id, pRpt->type, uuid, HID_DEV_OPER_WRITE, &len, pValue);
		}
	} else if (uuid == HID_CTRL_PT_UUID) {
		// Validate length and value range.
		if (len == 1) {
			if (pValue[0] == HID_CMD_SUSPEND || pValue[0] == HID_CMD_EXIT_SUSPEND) {
				// Execute HID app event callback.
				(*pHidDevCB->evtCB)((pValue[0] == HID_CMD_SUSPEND) ? HID_DEV_SUSPEND_EVT : HID_DEV_EXIT_SUSPEND_EVT);
			} else {
				status = ATT_ERR_INVALID_VALUE;
			}
		} else {
			status = ATT_ERR_INVALID_VALUE_SIZE;
		}
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
	} else if (uuid == PROTOCOL_MODE_UUID) {
		if (len == HID_PROTOCOL_MODE_LEN) {
			if (pValue[0] == HID_PROTOCOL_MODE_BOOT || pValue[0] == HID_PROTOCOL_MODE_REPORT) {
				pAttr->pValue[0] = pValue[0];

				// Execute HID app event callback.
				(*pHidDevCB->evtCB)((pValue[0] == HID_PROTOCOL_MODE_BOOT) ? HID_DEV_SET_BOOT_EVT : HID_DEV_SET_REPORT_EVT);
			} else {
				status = ATT_ERR_INVALID_VALUE;
			}
		} else {
			status = ATT_ERR_INVALID_VALUE_SIZE;
		}
	}

	// Restart idle timer.
	if (status == SUCCESS) {
		HidDev_StartIdleTimer();
	}

	return (status);
}

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Service Callbacks
// Note: When an operation on a characteristic requires authorization and
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an
// operation on a characteristic requires authorization the Stack will call
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be
// made within these functions.
const gattServiceCBs_t hidCBs = {HidService_ReadAttrCB,  // Read callback function pointer
		HidService_WriteAttrCB, // Write callback function pointer
		NULL                  // Authorization callback function pointer
		};

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
bStatus_t HidService_AddService(void) {
	uint8 status = SUCCESS;

// Allocate Client Charateristic Configuration tables.
	hidReportKeyInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);

	if (hidReportKeyInClientCharCfg == NULL) {
		return bleMemAllocError;
	}

	hidReportBootKeyInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);
	if (hidReportBootKeyInClientCharCfg == NULL) {
		ICall_free (hidReportKeyInClientCharCfg);

		return bleMemAllocError;
	}

	hidReportCCInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);
	if (hidReportCCInClientCharCfg == NULL) {
		ICall_free (hidReportKeyInClientCharCfg);
		ICall_free (hidReportBootKeyInClientCharCfg);

		return bleMemAllocError;
	}

	hidReportVoiceStartInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);
	if (hidReportVoiceStartInClientCharCfg == NULL) {
		ICall_free (hidReportKeyInClientCharCfg);
		ICall_free (hidReportBootKeyInClientCharCfg);
		ICall_free (hidReportCCInClientCharCfg);

		return bleMemAllocError;
	}

	hidReportVoiceDataInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);
	if (hidReportVoiceDataInClientCharCfg == NULL) {
		ICall_free (hidReportKeyInClientCharCfg);
		ICall_free (hidReportBootKeyInClientCharCfg);
		ICall_free (hidReportCCInClientCharCfg);
		ICall_free (hidReportVoiceStartInClientCharCfg);

		return bleMemAllocError;
	}

// Initialize Client Characteristic Configuration attributes
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, hidReportKeyInClientCharCfg);
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, hidReportBootKeyInClientCharCfg);
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, hidReportCCInClientCharCfg);
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, hidReportVoiceStartInClientCharCfg);
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, hidReportVoiceDataInClientCharCfg);

// Register GATT attribute list and CBs with GATT Server App
	status = GATTServApp_RegisterService(hidAttrTbl, GATT_NUM_ATTRS(hidAttrTbl), GATT_MAX_ENCRYPT_KEY_SIZE, &hidCBs);

// Set up included service
	Batt_GetParameter(BATT_PARAM_SERVICE_HANDLE, &GATT_INCLUDED_HANDLE(hidAttrTbl, HID_INCLUDED_SERVICE_IDX));

// Construct map of reports to characteristic handles
// Each report is uniquely identified via its ID and type

// Key input report
	hidRptMap[0].id = hidReportRefKeyIn[0];
	hidRptMap[0].type = hidReportRefKeyIn[1];
	hidRptMap[0].handle = hidAttrTbl[HID_REPORT_KEY_IN_IDX].handle;
	hidRptMap[0].pCccdAttr = &hidAttrTbl[HID_REPORT_KEY_IN_CCCD_IDX];
	hidRptMap[0].mode = HID_PROTOCOL_MODE_REPORT;

// LED output report
	hidRptMap[1].id = hidReportRefLedOut[0];
	hidRptMap[1].type = hidReportRefLedOut[1];
	hidRptMap[1].handle = hidAttrTbl[HID_REPORT_LED_OUT_IDX].handle;
	hidRptMap[1].pCccdAttr = NULL;
	hidRptMap[1].mode = HID_PROTOCOL_MODE_REPORT;

// Boot keyboard input report
// Use same ID and type as key input report
	hidRptMap[2].id = hidReportRefKeyIn[0];
	hidRptMap[2].type = hidReportRefKeyIn[1];
	hidRptMap[2].handle = hidAttrTbl[HID_BOOT_KEY_IN_IDX].handle;
	hidRptMap[2].pCccdAttr = &hidAttrTbl[HID_BOOT_KEY_IN_CCCD_IDX];
	hidRptMap[2].mode = HID_PROTOCOL_MODE_BOOT;

// Boot keyboard output report
// Use same ID and type as LED output report
	hidRptMap[3].id = hidReportRefLedOut[0];
	hidRptMap[3].type = hidReportRefLedOut[1];
	hidRptMap[3].handle = hidAttrTbl[HID_BOOT_KEY_OUT_IDX].handle;
	hidRptMap[3].pCccdAttr = NULL;
	hidRptMap[3].mode = HID_PROTOCOL_MODE_BOOT;

// Consumer Control input report
	hidRptMap[4].id = hidReportRefCCIn[0];
	hidRptMap[4].type = hidReportRefCCIn[1];
	hidRptMap[4].handle = hidAttrTbl[HID_REPORT_CC_IN_IDX].handle;
	hidRptMap[4].pCccdAttr = &hidAttrTbl[HID_REPORT_CC_IN_CCCD_IDX];
	hidRptMap[4].mode = HID_PROTOCOL_MODE_REPORT;

// Voice Start input report
	hidRptMap[5].id = hidReportRefVoiceStart[0];
	hidRptMap[5].type = hidReportRefVoiceStart[1];
	hidRptMap[5].handle = hidAttrTbl[HID_VOICE_START_IN_IDX].handle;
	hidRptMap[5].pCccdAttr = &hidAttrTbl[HID_VOICE_START_IN_CCCD_IDX];
	hidRptMap[5].mode = HID_PROTOCOL_MODE_REPORT;

// Voice Data input report
	hidRptMap[6].id = hidReportRefVoiceData[0];
	hidRptMap[6].type = hidReportRefVoiceData[1];
	hidRptMap[6].handle = hidAttrTbl[HID_VOICE_DATA_IN_IDX].handle;
	hidRptMap[6].pCccdAttr = &hidAttrTbl[HID_VOICE_DATA_IN_CCCD_IDX];
	hidRptMap[6].mode = HID_PROTOCOL_MODE_REPORT;

// Battery level input report
	VOID Batt_GetParameter(BATT_PARAM_BATT_LEVEL_IN_REPORT, &(hidRptMap[2]));

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
					ret = ATT_ERR_INVALID_VALUE_SIZE;
				}
			} else {
				ret = ATT_ERR_ATTR_NOT_FOUND;
			}
			break;

		case BOOT_KEY_OUTPUT_UUID:
			if (len == 1) {
				hidReportBootKeyOut = *((uint8*) pValue);
			} else {
				ret = ATT_ERR_INVALID_VALUE_SIZE;
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
uint8 HidKbd_GetParameter(uint8 id, uint8 type, uint16 uuid, uint8 *pLen, void *pValue) {
	switch (uuid) {
		case REPORT_UUID:
			if (type == HID_REPORT_TYPE_OUTPUT) {
				*((uint8*) pValue) = hidReportLedOut;
				*pLen = 1;
			} else {
				*pLen = 0;
			}
			break;

		case BOOT_KEY_OUTPUT_UUID:
			*((uint8*) pValue) = hidReportBootKeyOut;
			*pLen = 1;
			break;

		default:
			*pLen = 0;
			break;
	}

	return (SUCCESS);
}

/*********************************************************************
 *********************************************************************/
