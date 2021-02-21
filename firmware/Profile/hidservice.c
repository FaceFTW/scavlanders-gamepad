/***********************************************************************
 * \file 		hidservice.c
 * \author		Texas Instruments
 * \author 		Alex Westerman
 * \brief
 * \copyright 	See LICENSE
 ***********************************************************************/
#include <stdint.h>
//#include <ti/ble5stack/central/gatt.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include "hidservice.h"
#include "devinfoservice.h"
#include "profile_util.h"
#include "bt_uuid.h"
#include "constants.h"

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
SAP_CharHandle_t hidServiceCharHandles[4];

static uint8_t hidServiceUUID = {SNP_16BIT_UUID_SIZE, {LO_UINT16(HID_SERV_UUID), HI_UINT16(HID_SERV_UUID)}};

/*******************************************************************************
 *                              Profile Attributes - TABLE
 ******************************************************************************/
/**HID SERVICE ATTRIBUTE**/
static uint8_t hidProfile_hidService = 0;
static uint8_t hidProfile_hidService_desc[] = "HID Service";
SAP_UserDescAttr_t hidProfile_hidService_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_hidService_desc), sizeof(hidProfile_hidService_desc), hidProfile_hidService_desc)};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc)};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_infoChar = 0;
static uint8_t hidProfile_infoChar_desc[] = "HID Information Characteristic";
SAP_UserDescAttr_t hidProfile_infoChar_userdesc = {SNP_GATT_PERMIT_ENCRYPT_READ, sizeof(hidProfile_infoChar_desc), sizeof(hidProfile_infoChar_desc), hidProfile_infoChar_desc)};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc)};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc)};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc)};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc)};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc)};

/**INCLUDE BATTERY SERVICE ATTRIBUTE**/
static uint8_t hidProfile_includeBattServ = 0;
static uint8_t hidProfile_includeBattServ_desc[] = "Include Battery Service";
SAP_UserDescAttr_t hidProfile_includeBattServ_userdesc = {SNP_GATT_PERMIT_READ, sizeof(hidProfile_includeBattServ_desc), sizeof(hidProfile_includeBattServ_desc), hidProfile_includeBattServ_desc)};

SAP_UserCCCDAttr_t char4CCCD = {SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE};

#define SERVAPP_NUM_ATTR_SUPPORTED 4

static SAP_Char_t hidProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = {
/* Characteristic 1 Value Declaration */
{{SNP_16BIT_UUID_SIZE, simpleProfilechar1UUID}, /* UUID */
SNP_GATT_PROP_READ | SNP_GATT_PROP_WRITE, /* Properties */
SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE, /* Permissions */
&char1UserDesc /* User Description */
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

};

// Attribute index enumeration-- these indexes match array elements above
enum {
	HID_SERVICE_IDX,                // HID Service
	HID_INCLUDED_SERVICE_IDX,       // Included Service (battery)
	HID_INFO_DECL_IDX,              // HID Information characteristic declaration
	HID_INFO_IDX,                   // HID Information characteristic
	HID_REPORT_MAP_DECL_IDX,        // HID Report Map characteristic declaration
	HID_REPORT_MAP_IDX,             // HID Report Map characteristic
	HID_EXT_REPORT_REF_DESC_IDX,    // HID External Report Reference Descriptor
	HID_REPORT_KEY_IN_DECL_IDX,     // HID Report characteristic declaration, key input
	HID_REPORT_KEY_IN_IDX,          // HID Report characteristic, key input

};

static hidRptMap_t *pHidDevRptTbl;

static uint8_t hidDevRptTblLen;

static hidDevCB_t *pHidDevCB;

static hidDevCfg_t *pHidDevCfg;

// Pending reports
static uint8_t firstQIdx = 0;
static uint8_t lastQIdx = 0;
static hidDevReport_t hidDevReportQ[HID_DEV_REPORT_Q_SIZE];

// Last report sent out
static hidDevReport_t lastReport = {0};

// State when HID reports are ready to be sent out
static volatile uint8_t hidDevReportReadyState = TRUE;

// Report ready delay clock
static Clock_Struct reportReadyClock;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void HidDev_processStackMsg(eventHdr_t *pMsg);
static void HidDev_processAppMsg(hidDevEvt_t *pMsg);
static void HidDev_processGattMsg(gattMsgEvent_t *pMsg);
static void HidDev_clockHandler(UArg arg);
static uint8_t HidDev_enqueueMsg(uint16_t event, uint8_t state, uint8_t *pData);

// HID reports.
static hidRptMap_t* HidDev_reportByHandle(uint16_t handle);
static hidRptMap_t* HidDev_reportById(uint8_t id, uint8_t type);
static hidRptMap_t* HidDev_reportByCccdHandle(uint16_t handle);
static void HidDev_enqueueReport(uint8_t id, uint8_t type, uint8_t len, uint8_t *pData);
static hidDevReport_t* HidDev_dequeueReport(void);
static void HidDev_sendReport(uint8_t id, uint8_t type, uint8_t len, uint8_t *pData);
static uint8_t HidDev_sendNoti(uint16_t handle, uint8_t len, uint8_t *pData);
static uint8_t HidDev_isbufset(uint8_t *buf, uint8_t val, uint8_t len);

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
 * @fn      HidDev_Report
 *
 * @brief   Send a HID report.
 *
 * @param   id    - HID report ID.
 * @param   type  - HID report type.
 * @param   len   - Length of report.
 * @param   pData - Report data.
 *
 * @return  None.
 */
void HidDev_Report(uint8_t id, uint8_t type, uint8_t len, uint8_t *pData) {
// Validate length of report
	if (len > HID_DEV_DATA_LEN) {
		return;
	}
// Make sure there're no pending reports.
	if (reportQEmpty()) {
// Send report.
		HidDev_sendReport(id, type, len, pData);

		return;
	}

// Else if not already advertising
	else if (hidDevGapState != GAPROLE_ADVERTISING) {
		HidDev_StartAdvertising();
	}

// HidDev task will send report when secure connection is established.
	HidDev_enqueueReport(id, type, len, pData);
}

/*********************************************************************
 * @fn      HidDev_passcodeCB
 *
 * @brief   Passcode callback.
 *
 * @param   deviceAddr - address of device to pair with, and could be either
 *                       public or random.
 * @param   connHandle - connection handle
 * @param   uiInputs   - pairing User Interface Inputs - Ask user to input
 *                       passcode.
 * @param   uiOutputs  - pairing User Interface Outputs - Display passcode.
 *
 * @return  none
 */
static void HidDev_passcodeCB(uint8_t *deviceAddr, uint16_t connHandle, uint8_t uiInputs, uint8_t uiOutputs) {
	hidDevPasscodeEvt_t *pcEvt;

// Allocate message data.
	if ((pcEvt = malloc(sizeof(hidDevPasscodeEvt_t)))) {
// Store the arguments.
		memcpy(pcEvt->deviceAddr, deviceAddr, B_ADDR_LEN);

		pcEvt->connHandle = connHandle;
		pcEvt->uiInputs = uiInputs;
		pcEvt->uiOutputs = uiOutputs;

// Queue the event.
		HidDev_enqueueMsg(HID_PASSCODE_EVT, 0, (uint8_t*) pcEvt);
	}
}

/*********************************************************************
 * @fn      HidDev_processPasscodeEvt
 *
 * @brief   Process passcode callback.
 *
 * @param   deviceAddr - address of device to pair with, and could be either
 *                       public or random.
 * @param   connHandle - connection handle
 * @param   uiInputs   - pairing User Interface Inputs - Ask user to input
 *                       passcode.
 * @param   uiOutputs  - pairing User Interface Outputs - Display passcode.
 *
 * @return  none
 */
static void HidDev_processPasscodeEvt(uint8_t *deviceAddr, uint16_t connHandle, uint8_t uiInputs, uint8_t uiOutputs) {
	if (pHidDevCB && pHidDevCB->passcodeCB) {
// Execute HID app passcode callback.
		(*pHidDevCB->passcodeCB)(deviceAddr, connHandle, uiInputs, uiOutputs);
	} else {
// This profile uses a default passcode. A real-life scenario would handle
// all pairing scenarios and likely generate this randomly
		GAPBondMgr_PasscodeRsp(connHandle, SUCCESS, B_APP_DEFAULT_PASSCODE);
	}
}

/*********************************************************************
 * @fn      HidDev_batteryCB
 *
 * @brief   Callback function for battery service.
 *
 * @param   event - service event
 *
 * @return  none
 */
static void HidDev_batteryCB(uint8_t event) {
// Queue the event.
	HidDev_enqueueMsg(HID_BATT_SERVICE_EVT, event, NULL);
}

/*********************************************************************
 * @fn      HidDev_processBatteryEvt
 *
 * @brief   Processes callback from the battery service.
 *
 * @param   event - service event
 *
 * @return  none
 */
static void HidDev_processBatteryEvt(uint8_t event) {
	if (event == BATT_LEVEL_NOTI_ENABLED) {
// If connected start periodic measurement.
		if (hidDevGapState == GAPROLE_CONNECTED) {
			Util_startClock (&battPerClock);
		}
	} else if (event == BATT_LEVEL_NOTI_DISABLED) {
// Stop periodic measurement.
		Util_stopClock (&battPerClock);
	}
}

/*********************************************************************
 * @fn      HidDev_scanParamCB
 *
 * @brief   Callback function for scan parameter service.
 *
 * @param   event - service event
 *
 * @return  none
 */
static void HidDev_scanParamCB(uint8_t event) {
// Do nothing.
}

/*********************************************************************
 * @fn      HidDev_battPeriodicTask
 *
 * @brief   Perform a periodic task for battery measurement.
 *
 * @param   none
 *
 * @return  none
 */
static void HidDev_battPeriodicTask(void) {
	if (hidDevGapState == GAPROLE_CONNECTED) {
// Perform battery level check.
		Batt_MeasLevel();

// Restart clock.
		Util_startClock (&battPerClock);
	}
}

/*********************************************************************
 * @fn      HidDev_reportByHandle
 *
 * @brief   Find the HID report structure for the given handle.
 *
 * @param   handle - ATT handle
 *
 * @return  Pointer to HID report structure
 */
static hidRptMap_t* HidDev_reportByHandle(uint16_t handle) {
	uint8_t i;
	hidRptMap_t *p = pHidDevRptTbl;

	for (i = hidDevRptTblLen; i > 0; i--, p++) {
		if (p->handle == handle && p->mode == hidProtocolMode) {
			return p;
		}
	}

	return NULL;
}

/*********************************************************************
 * @fn      HidDev_reportByCccdHandle
 *
 * @brief   Find the HID report structure for the given CCC handle.
 *
 * @param   handle - ATT handle
 *
 * @return  Pointer to HID report structure
 */
static hidRptMap_t* HidDev_reportByCccdHandle(uint16_t handle) {
	uint8_t i;
	hidRptMap_t *p = pHidDevRptTbl;

	for (i = hidDevRptTblLen; i > 0; i--, p++) {
		if ((p->pCccdAttr != NULL) && (p->pCccdAttr->handle == handle)) {
			return p;
		}
	}

	return NULL;
}

/*********************************************************************
 * @fn      HidDev_reportById
 *
 * @brief   Find the HID report structure for the Report ID and type.
 *
 * @param   id   - HID report ID
 * @param   type - HID report type
 *
 * @return  Pointer to HID report structure
 */
static hidRptMap_t* HidDev_reportById(uint8_t id, uint8_t type) {
	uint8_t i;
	hidRptMap_t *p = pHidDevRptTbl;

	for (i = hidDevRptTblLen; i > 0; i--, p++) {
		if (p->id == id && p->type == type && p->mode == hidProtocolMode) {
			return p;
		}
	}

	return NULL;
}

/*********************************************************************
 * @fn      HidDev_sendReport
 *
 * @brief   Send a HID report.
 *
 * @param   id    - HID report ID.
 * @param   type  - HID report type.
 * @param   len   - Length of report.
 * @param   pData - Report data.
 *
 * @return  None.
 */
static void HidDev_sendReport(uint8_t id, uint8_t type, uint8_t len, uint8_t *pData) {
	hidRptMap_t *pRpt;

// Get ATT handle for report.
	if ((pRpt = HidDev_reportById(id, type)) != NULL) {
		uint8_t value = GATTServApp_ReadCharCfg(gapConnHandle, GATT_CCC_TBL(pRpt->pCccdAttr->pValue));

// If notifications are enabled
		if (value & GATT_CLIENT_CFG_NOTIFY) {
			// After service discovery and encryption, the HID Device should
			// request to change to the preferred connection parameters that best
			// suit its use case.
			if (updateConnParams) {
				GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_REQ, sizeof(uint8_t), &updateConnParams);

				updateConnParams = FALSE;
			}

			// Send report notification
			if (HidDev_sendNoti(pRpt->handle, len, pData) == SUCCESS) {
				// Save the report just sent out
				lastReport.id = id;
				lastReport.type = type;
				lastReport.len = len;
				memcpy(lastReport.data, pData, len);
			}

			// Start idle timer.
			HidDev_StartIdleTimer();
		}
	}
}

/*********************************************************************
 * @fn      hidDevSendNoti
 *
 * @brief   Send a HID notification.
 *
 * @param   handle - Attribute handle.
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  Success or failure.
 */
static uint8_t HidDev_sendNoti(uint16_t handle, uint8_t len, uint8_t *pData) {
	uint8_t status;
	attHandleValueNoti_t noti;

	noti.pValue = GATT_bm_alloc(gapConnHandle, ATT_HANDLE_VALUE_NOTI, len, NULL);
	if (noti.pValue != NULL) {
		noti.handle = handle;
		noti.len = len;
		memcpy(noti.pValue, pData, len);

// Send notification
		status = GATT_Notification(gapConnHandle, &noti, FALSE);
		if (status != SUCCESS) {
			GATT_bm_free((gattMsg_t*) &noti, ATT_HANDLE_VALUE_NOTI);
		}
	} else {
		status = bleMemAllocError;
	}

	return status;
}

/*********************************************************************
 * @fn      HidDev_enqueueReport
 *
 * @brief   Enqueue a HID report to be sent later.
 *
 * @param   id    - HID report ID.
 * @param   type  - HID report type.
 * @param   len   - Length of report.
 * @param   pData - Report data.
 *
 * @return  None.
 */
static void HidDev_enqueueReport(uint8_t id, uint8_t type, uint8_t len, uint8_t *pData) {
// Enqueue only if bonded.
	if (HidDev_bondCount() > 0) {
// Update last index.
		lastQIdx = (lastQIdx + 1) % HID_DEV_REPORT_Q_SIZE;

		if (lastQIdx == firstQIdx) {
			// Queue overflow; discard oldest report.
			firstQIdx = (firstQIdx + 1) % HID_DEV_REPORT_Q_SIZE;
		}

// Save report.
		hidDevReportQ[lastQIdx].id = id;
		hidDevReportQ[lastQIdx].type = type;
		hidDevReportQ[lastQIdx].len = len;
		memcpy(hidDevReportQ[lastQIdx].data, pData, len);

		if (hidDevConnSecure) {
			// Notify our task to send out pending reports.
			Event_post(syncEvent, HID_SEND_REPORT_EVT);
		}
	}
}

/*********************************************************************
 * @fn      HidDev_dequeueReport
 *
 * @brief   Dequeue a HID report to be sent out.
 *
 * @param   id    - HID report ID.
 * @param   type  - HID report type.
 * @param   len   - Length of report.
 * @param   pData - Report data.
 *
 * @return  None.
 */
static hidDevReport_t* HidDev_dequeueReport(void) {
	if (reportQEmpty()) {
		return NULL;
	}

// Update first index.
	firstQIdx = (firstQIdx + 1) % HID_DEV_REPORT_Q_SIZE;

	return (&(hidDevReportQ[firstQIdx]));
}

/*********************************************************************
 * @fn      HidDev_isbufset
 *
 * @brief   Is all of the array elements set to a value?
 *
 * @param   buf - buffer to check.
 * @param   val - value to check each array element for.
 * @param   len - length to check.
 *
 * @return  TRUE if all "val".
 *          FALSE otherwise.
 */
static uint8_t HidDev_isbufset(uint8_t *buf, uint8_t val, uint8_t len) {
	uint8_t x;

// Validate pointer and length of report
	if ((buf == NULL) || (len > HID_DEV_DATA_LEN)) {
		return (FALSE);
	}

	for (x = 0; x < len; x++) {
// Check for non-initialized value
		if (buf[x] != val) {
			return (FALSE);
		}
	}

	return (TRUE);
}

/*********************************************************************
 * @fn      HidDev_clockHandler
 *
 * @brief   Clock handle for all clock events.  This function stores an event
 *          flag and wakes up the application's event processor.
 *
 * @param   arg - event flag.
 *
 * @return  None
 */
static void HidDev_clockHandler(UArg arg) {
	Event_post(syncEvent, arg);
}

/*********************************************************************
 * @fn      HidDev_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event  - message event.
 * @param   state  - message state.
 * @param   pData  - message data pointer.
 *
 * @return  TRUE or FALSE
 */
static uint8_t HidDev_enqueueMsg(uint16_t event, uint8_t state, uint8_t *pData) {
	hidDevEvt_t *pMsg;

// Create dynamic pointer to message.
	if (pMsg = malloc(sizeof(hidDevEvt_t))) {
		pMsg->hdr.event = event;
		pMsg->hdr.state = state;
		pMsg->pData = pData;

// Enqueue the message.
		return Util_enqueueMsg(appMsgQueue, syncEvent, (uint8*) pMsg);
	}

	return FALSE;
}

/*********************************************************************
 * @fn      HidDev_reportReadyClockCB
 *
 * @brief   Handles HID reports when delay has expired
 *
 * @param   None.
 *
 * @return  None.
 */
static void HidDev_reportReadyClockCB(UArg a0) {
// Allow reports to be sent
	hidDevReportReadyState = TRUE;

// If there are reports in the queue
	if (!reportQEmpty()) {
		Event_post(syncEvent, HID_SEND_REPORT_EVT);
	}
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
HCI_StatusCodes_t HidService_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method) {
	HCI_StatusCodes_t status = SUCCESS;
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
	} else if (uuid == PROTOCOL_MODE_UUID) {
		*pLen = HID_PROTOCOL_MODE_LEN;
		pValue[0] = pAttr->pValue[0];
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
HCI_StatusCodes_t HidService_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t len, uint16_t offset, uint8_t method) {
	HCI_StatusCodes_t status = SUCCESS;
	hidRptMap_t *pRpt;

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
HCI_StatusCodes_t HidService_AddService(void) {
	uint8 status = SUCCESS;

// Allocate Client Charateristic Configuration tables.
	hidReportKeyInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);
	hidReportBootKeyInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);
	hidReportCCInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);

// Initialize Client Characteristic Configuration attributes
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, hidReportKeyInClientCharCfg);
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, hidReportBootKeyInClientCharCfg);
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, hidReportCCInClientCharCfg);

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

// Battery level input report
	void Batt_GetParameter(BATT_PARAM_BATT_LEVEL_IN_REPORT, &(hidRptMap[2]));

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
uint8 Hid_GetParameter(uint8 id, uint8 type, uint16 uuid, uint8 *pLen, void *pValue) {
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
