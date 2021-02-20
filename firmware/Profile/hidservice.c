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
#include <ti/sysbios/knl/Task.h>

#include "hidservice.h"
#include "devinfoservice.h"
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

static Clock_Struct battPerClock;			//Clock used for Battery Service (For conformance)
static Clock_Struct idleTimeoutClock;		//Clock used for timeout checks

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

static gapRolesCBs_t hidDev_PeripheralCBs = {HidDev_stateChangeCB  // Profile State Change Callbacks
		};

// Bond Manager Callbacks
static const gapBondCBs_t hidDevBondCB = {(pfnPasscodeCB_t) HidDev_passcodeCB, HidDev_pairStateCB};
Task_Struct hidDeviceTask;
Char hidDeviceTaskStack[HIDDEVICE_TASK_STACK_SIZE];

// GAP State
static gaprole_States_t hidDevGapState = GAPROLE_INIT;

// TRUE if connection is secure
static uint8_t hidDevConnSecure = FALSE;

// GAP connection handle
static uint16_t gapConnHandle;

// TRUE if pairing in progress
static uint8_t hidDevPairingStarted = FALSE;

// Status of last pairing
static uint8_t pairingStatus = SUCCESS;

// Pairing state
static uint8_t hidDevGapBondPairingState = HID_GAPBOND_PAIRING_STATE_NONE;

static hidRptMap_t *pHidDevRptTbl;

static uint8_t hidDevRptTblLen;

static hidDevCB_t *pHidDevCB;

static hidDevCfg_t *pHidDevCfg;

// Whether to change to the preferred connection parameters
static uint8_t updateConnParams = TRUE;

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

// Task events and processing functions.
static void HidDev_init(void);
static void HidDev_taskFxn(UArg a0, UArg a1);
static void HidDev_processStackMsg(eventHdr_t *pMsg);
static void HidDev_processAppMsg(hidDevEvt_t *pMsg);
static void HidDev_processGattMsg(gattMsgEvent_t *pMsg);
static void HidDev_disconnected(void);
static void HidDev_highAdvertising(void);
static void HidDev_lowAdvertising(void);
static void HidDev_initialAdvertising(void);
static uint8_t HidDev_bondCount(void);
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

// Peripheral GAP role.
static void HidDev_stateChangeCB(gaprole_States_t newState);
static void HidDev_processStateChangeEvt(gaprole_States_t newState);

// Pair state.
static void HidDev_pairStateCB(uint16_t connHandle, uint8_t state, uint8_t status);
static void HidDev_processPairStateEvt(uint8_t state, uint8_t status);

// Passcode.
static void HidDev_passcodeCB(uint8_t *deviceAddr, uint16_t connectionHandle, uint8_t uiInputs, uint8_t uiOutputs);
static void HidDev_processPasscodeEvt(uint8_t *deviceAddr, uint16_t connectionHandle, uint8_t uiInputs, uint8_t uiOutputs);

// Battery events.
static void HidDev_batteryCB(uint8_t event);
static void HidDev_processBatteryEvt(uint8_t event);
static void HidDev_battPeriodicTask(void);

// Scan parameter events.
static void HidDev_scanParamCB(uint8_t event);

// Process reconnection delay
static void HidDev_reportReadyClockCB(UArg a0);

// Voice
static uint8_t HidDev_sendVoiceReport(uint8_t id, uint8_t type, uint8_t len, uint8_t *pData);


/* SAP Parameters for opening serial port to SNP */
static SAP_Params sapParams;
static uint8_t snpDeviceName[] = {'S', 'c', 'a', 'v', 'l', 'a', 'n', 'd', 'e', 'r', 's', ' ', 'G', 'a', 'm', 'e', 'p', 'a', 'd'};

/* GAP - SCAN RSP data (max size = 31 bytes) */
static uint8_t scanRspData[] = {
/* Complete Name */
0x13,/* length of this data */
SAP_GAP_ADTYPE_LOCAL_NAME_COMPLETE, 'S', 'c', 'a', 'v', 'l', 'a', 'n', 'd', 'e', 'r', 's', ' ', 'G', 'a', 'm', 'e', 'p', 'a', 'd',

/* Connection interval range */
0x05, /* length of this data */
0x12, /* GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE, */
LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL), HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL), LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL), HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL), 0x02, /* length of this data */
0x0A, /* GAP_ADTYPE_POWER_LEVEL, */
0 /* 0dBm */
};

/**GAP Advertising Data Struct**/
static uint8_t advertData[] = {
/* Flags; this sets the device to use limited discoverable
 mode (advertises for 30 seconds at a time) instead of general
 discoverable mode (advertises indefinitely) */
0x02, /* length of this data */
SAP_GAP_ADTYPE_FLAGS, DEFAULT_DISCOVERABLE_MODE | SAP_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

/* Manufacturer specific advertising data */
0x06, 0xFF, LO_UINT16(TI_COMPANY_ID), HI_UINT16(TI_COMPANY_ID),
TI_ST_DEVICE_ID,
TI_ST_KEY_DATA_ID, 0x00 /* Key state */
};
/***********************************************************************
 * 					UTILITY FUNCTIONS
 ***********************************************************************/
/*********************************************************************
 * @fn      HidDev_createTask
 *
 * @brief   Task creation function for the HID service.
 *
 * @param   none
 *
 * @return  none
 */
void HidDev_createTask(void) {
	Task_Params taskParams;

	// Configure task.
	Task_Params_init(&taskParams);
	taskParams.stack = hidDeviceTaskStack;
	taskParams.stackSize = HIDDEVICE_TASK_STACK_SIZE;
	taskParams.priority = HIDDEVICE_TASK_PRIORITY;

	Task_construct(&hidDeviceTask, HidDev_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      HidDev_init
 *
 * @brief   Initialization function for the Hid Dev Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ...).
 *
 * @param   none
 *
 * @return  none
 */
static void HidDev_init(void) {
	//Initialize and Configure Parameter varaibles for system objects
	Clock_Params battClockParams;
	Clock_Params reportRdyClockParams;

	Clock_Params_init(&battClockParams);
	battClockParams.arg = HID_BATT_PERIODIC_EVT;
	battClockParams.period = 0;
	battClockParams.startFlag = false;
	uint32_t battClockTicks = DEFAULT_BATT_PERIOD * (1000 / Clock_tickPeriod);
	Clock_Params_init(&reportRdyClockParams);
	reportRdyClockParams.arg = NULL;
	reportRdyClockParams.period = 0;
	reportRdyClockParams.startFlag = false;
	uint32_t reportRdyClockTicks = HID_REPORT_READY_TIME * (1000 / Clock_tickPeriod);

	// Create an RTOS queue for message from profile to be sent to app.
	serviceQueue = Queue_construct(&serviceMsg, NULL);

	// Create one-shot clocks for internal periodic events.
	Clock_construct(&battPerClock, HidDev_clockHandler, battClockTicks, &battClockParams);

	// Setup the GAP Bond Manager.
	{
		uint8_t syncWL = HID_AUTO_SYNC_WL;

		// If a bond is created, the HID Device should write the address of the
		// HID Host in the HID Device controller's white list and set the HID
		// Device controller's advertising filter policy to 'process scan and
		// connection requests only from devices in the White List'.
		void GAPBondMgr_SetParameter(GAPBOND_AUTO_SYNC_WL, sizeof(uint8_t), &syncWL);
	}

	// Set up services.
	GGS_AddService (GATT_ALL_SERVICES);        // GAP
	GATTServApp_AddService(GATT_ALL_SERVICES);        // GATT attributes

	DevInfo_AddService();
	Batt_AddService();
	ScanParam_AddService();

	// Register for Battery service callback.
	Batt_Register(HidDev_batteryCB);

	// Register for Scan Parameters service callback.
	ScanParam_Register(HidDev_scanParamCB);

	// Initialize report ready clock timer
	Clock_construct(&reportReadyClock, HidDev_reportReadyClockCB, reportRdyClockTicks, &reportRdyClockParams);
}

/*********************************************************************
 * @fn      HidDev_taskFxn
 *
 * @brief   Hid Dev Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   a0, a1 - not used
 *
 * @return  not used
 */
static void HidDev_taskFxn(UArg a0, UArg a1) {
	// Initialize the application.
	HidDev_init();

	// Application main loop.
	for (;;) {
		uint32_t events;

		events = Event_pend(syncEvent, Event_Id_NONE, HID_ALL_EVENTS, ICALL_TIMEOUT_FOREVER);

		if (events) {
			ICall_EntityID dest;
			ICall_ServiceEnum src;
			ICall_HciExtEvt *pMsg = NULL;

			if (ICall_fetchServiceMsg(&src, &dest, (void**) &pMsg) == ICALL_ERRNO_SUCCESS) {
				if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity)) {
					// Process inter-task message.
					HidDev_processStackMsg((ICall_Hdr*) pMsg);
				}

				if (pMsg) {
					freeMsg(pMsg);
				}
			}

			// If RTOS queue is not empty, process app message.
			if (events & HID_QUEUE_EVT) {
				while (!Queue_empty(appMsgQueue)) {
					hidDevEvt_t *pMsg = (hidDevEvt_t*) Util_dequeueMsg(appMsgQueue);
					if (pMsg) {
						// Process message.
						HidDev_processAppMsg(pMsg);

						// Free the space from the message.
						free(pMsg);
					}
				}
			}

			// Idle timeout.
			if (events & HID_IDLE_EVT) {
				if (hidDevGapState == GAPROLE_CONNECTED) {
					// If pairing in progress then restart timer.
					if (hidDevPairingStarted) {
						HidDev_StartIdleTimer();
					}
					// Else disconnect and don't allow reports to be sent
					else {
						hidDevReportReadyState = FALSE;
						GAPRole_TerminateConnection();
					}
				}
			}

			// Battery periodic event.
			if (events & HID_BATT_PERIODIC_EVT) {
				HidDev_battPeriodicTask();
			}

			// Send HID report event.
			if (events & HID_SEND_REPORT_EVT) {
				// If connection is secure
				if (hidDevConnSecure && hidDevReportReadyState) {
					hidDevReport_t *pReport = HidDev_dequeueReport();

					if (pReport != NULL) {
						// Send report.
						HidDev_sendReport(pReport->id, pReport->type, pReport->len, pReport->data);
					}

					// If there is another report in the queue
					if (!reportQEmpty()) {
						// Set another event.
						Event_post(syncEvent, HID_SEND_REPORT_EVT);
					}
				}
			}
		}
	}
}

/*********************************************************************
 * @fn      HidDev_StartDevice
 *
 * @brief   Start the GAP Role and Register the Bond Manager.
 *          This function is intended to be called from the application
 *          task after setting up the GAP Role and Bond Manager.
 *
 * @param   None.
 *
 * @return  None.
 */
void HidDev_StartDevice(void) {
	// Start the Device.
	void GAPRole_StartDevice(&hidDev_PeripheralCBs);

	// Register with bond manager after starting device.
	GAPBondMgr_Register((gapBondCBs_t*) &hidDevBondCB);
}

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
		Util_constructClock(&idleTimeoutClock, HidDev_clockHandler, pHidDevCfg->idleTimeout, 0, false, HID_IDLE_EVT);
	}
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

	// If connected
	if (hidDevGapState == GAPROLE_CONNECTED) {
		// If connection is secure
		if (hidDevConnSecure) {
			// Make sure there're no pending reports.
			if (reportQEmpty()) {
				// Send report.
				HidDev_sendReport(id, type, len, pData);

				return;
			}
		}
	}
	// Else if not already advertising
	else if (hidDevGapState != GAPROLE_ADVERTISING) {
		HidDev_StartAdvertising();
	}

	// HidDev task will send report when secure connection is established.
	HidDev_enqueueReport(id, type, len, pData);
}

/*********************************************************************
 * @fn      HidDev_processStackMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HidDev_processStackMsg(eventHdr_t *pMsg) {
	switch (pMsg->event) {
		case GATT_MSG_EVENT:
			HidDev_processGattMsg((gattMsgEvent_t*) pMsg);
			break;

		default:
			// Do nothing.
			break;
	}
}

/*********************************************************************
 * @fn      HidDev_processGattMsg
 *
 * @brief   Process GATT messages
 *
 * @return  none
 */
static void HidDev_processGattMsg(gattMsgEvent_t *pMsg) {
	GATT_bm_free(&pMsg->msg, pMsg->method);
}

/*********************************************************************
 * @fn      HidDev_stateChangeCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void HidDev_stateChangeCB(gaprole_States_t newState) {
	// Enqueue the message.
	HidDev_enqueueMsg(HID_STATE_CHANGE_EVT, newState, NULL);
}

/*********************************************************************
 * @fn      HidDev_processStateChangeEvt
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void HidDev_processStateChangeEvt(gaprole_States_t newState) {
	// If connected
	if (newState == GAPROLE_CONNECTED) {
		uint8_t param = FALSE;

		// Get connection handle.
		GAPRole_GetParameter(GAPROLE_CONNHANDLE, &gapConnHandle);

		// Connection not secure yet.
		hidDevConnSecure = FALSE;

		// Don't start advertising when connection is closed.
		GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &param);

		// Start idle timer.
		HidDev_StartIdleTimer();

		// If there are reports in the queue
		if (!reportQEmpty()) {
			Event_post(syncEvent, HID_SEND_REPORT_EVT);
		}
	}
	// If disconnected
	else if (hidDevGapState == GAPROLE_CONNECTED && newState != GAPROLE_CONNECTED) {
		HidDev_disconnected();

		updateConnParams = TRUE;

		if (pairingStatus == SMP_PAIRING_FAILED_CONFIRM_VALUE) {
			// Bonding failed due to mismatched confirm values.
			HidDev_initialAdvertising();

			pairingStatus = SUCCESS;
		}
#if AUTO_ADV
		else {
			uint8_t advState = TRUE;

			GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advState);
		}
#endif//AUTO_ADV
	}
	// If started
	else if (newState == GAPROLE_STARTED) {
		// Nothing to do for now!
	}

	// Update GAP state
	hidDevGapState = newState;

	// Notify application
	(*pHidDevCB->evtCB)(HID_DEV_GAPROLE_STATE_CHANGE_EVT);
}

/*********************************************************************
 * @fn      HidDev_disconnected
 *
 * @brief   Handle disconnect.
 *
 * @return  none
 */
static void HidDev_disconnected(void) {
	// Stop idle timer.
	HidDev_StopIdleTimer();

	// Reset state variables.
	hidDevConnSecure = FALSE;
	hidProtocolMode = HID_PROTOCOL_MODE_REPORT;
	hidDevPairingStarted = FALSE;
	hidDevGapBondPairingState = HID_GAPBOND_PAIRING_STATE_NONE;

	// Reset last report sent out
	memset(&lastReport, 0, sizeof(hidDevReport_t));

	// If bonded and normally connectable start advertising.
	if ((HidDev_bondCount() > 0) && (pHidDevCfg->hidFlags & HID_FLAGS_NORMALLY_CONNECTABLE)) {
		HidDev_lowAdvertising();
	}

	// Notify application
	(*pHidDevCB->evtCB)(HID_DEV_GAPBOND_STATE_CHANGE_EVT);
}

/*********************************************************************
 * @fn      HidDev_pairStateCB
 *
 * @brief   Pairing state callback.
 *
 * @param   connHandle - connection handle.
 * @param   state      - pairing state
 * @param   status     - status upon entering this state.
 *
 * @return  none
 */
static void HidDev_pairStateCB(uint16_t connHandle, uint8_t state, uint8_t status) {
	uint8_t *pData;

	// Allocate message data
	if ((pData = malloc(sizeof(uint8_t)))) {
		*pData = status;

		// Queue the event.
		HidDev_enqueueMsg(HID_PAIR_STATE_EVT, state, pData);
	}
}

/*********************************************************************
 * @fn      HidDev_processPairStateEvt
 *
 * @brief   Process pairing state callback.
 *
 * @param   state  - pairing state
 * @param   status - status upon entering this state.
 *
 * @return  none
 */
static void HidDev_processPairStateEvt(uint8_t state, uint8_t status) {
	if (state == GAPBOND_PAIRING_STATE_STARTED) {
		hidDevPairingStarted = TRUE;
	} else if (state == GAPBOND_PAIRING_STATE_COMPLETE) {
		hidDevPairingStarted = FALSE;
		pairingStatus = status;

		if (status == SUCCESS) {
			hidDevConnSecure = TRUE;
			Util_restartClock(&reportReadyClock, HID_REPORT_READY_TIME);
		}
	} else if (state == GAPBOND_PAIRING_STATE_BONDED) {
		if (status == SUCCESS) {
			hidDevConnSecure = TRUE;
			Util_restartClock(&reportReadyClock, HID_REPORT_READY_TIME);

#if DEFAULT_SCAN_PARAM_NOTIFY_TEST == TRUE
			ScanParam_RefreshNotify(gapConnHandle);
#endif
		}
	}

	// Update GAP Bond pairing state
	hidDevGapBondPairingState = state;

	// Notify application
	(*pHidDevCB->evtCB)(HID_DEV_GAPBOND_STATE_CHANGE_EVT);

	// Process HID reports
	if (!reportQEmpty() && hidDevConnSecure) {
		// Notify our task to send out pending reports.
		Event_post(syncEvent, HID_SEND_REPORT_EVT);
	}
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
			Util_startClock(&battPerClock);
		}
	} else if (event == BATT_LEVEL_NOTI_DISABLED) {
		// Stop periodic measurement.
		Util_stopClock(&battPerClock);
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
		Util_startClock(&battPerClock);
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
 * @fn      HidDev_sendVoiceReport
 *
 * @brief   Send a HID report.
 *
 * @param   id    - HID report ID.
 * @param   type  - HID report type.
 * @param   len   - Length of report.
 * @param   pData - Report data.
 *
 * @return  status.
 */
static uint8_t HidDev_sendVoiceReport(uint8_t id, uint8_t type, uint8_t len, uint8_t *pData) {
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
			return HidDev_sendNoti(pRpt->handle, len, pData);
		}
	}
	return FAILURE;
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
 * @fn      HidDev_highAdvertising
 *
 * @brief   Start advertising at a high duty cycle.

 * @param   None.
 *
 * @return  None.
 */
static void HidDev_highAdvertising(void) {
	uint8_t param;

	void GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, HID_HIGH_ADV_INT_MIN);
	void GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, HID_HIGH_ADV_INT_MAX);
	void GAP_SetParamValue(TGAP_LIM_ADV_TIMEOUT, HID_HIGH_ADV_TIMEOUT);

	// Setup advertising filter policy first.
	param = HID_AUTO_SYNC_WL ? GAP_FILTER_POLICY_WHITE : GAP_FILTER_POLICY_ALL;
	void GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, sizeof(uint8_t), &param);

	param = TRUE;
	GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &param);
}

/*********************************************************************
 * @fn      HidDev_lowAdvertising
 *
 * @brief   Start advertising at a low duty cycle.
 *
 * @param   None.
 *
 * @return  None.
 */
static void HidDev_lowAdvertising(void) {
	uint8_t param;

	void GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, HID_LOW_ADV_INT_MIN);
	void GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, HID_LOW_ADV_INT_MAX);
	void GAP_SetParamValue(TGAP_LIM_ADV_TIMEOUT, HID_LOW_ADV_TIMEOUT);

	// Setup advertising filter policy first.
	param = HID_AUTO_SYNC_WL ? GAP_FILTER_POLICY_WHITE : GAP_FILTER_POLICY_ALL;
	void GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, sizeof(uint8_t), &param);

	param = TRUE;
	void GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &param);
}

/*********************************************************************
 * @fn      HidDev_initialAdvertising
 *
 * @brief   Start advertising for initial connection.
 *
 * @return  None.
 */
static void HidDev_initialAdvertising(void) {
	uint8_t param;

	Display_print0(displayOut, 0, 0, "Starting advertisement... ");

	/* Setting Advertising Name */
	SAP_setServiceParam(SNP_GGS_SERV_ID, SNP_GGS_DEVICE_NAME_ATT, sizeof(snpDeviceName), snpDeviceName);

	/* Set advertising data. */
	SAP_setParam(SAP_PARAM_ADV, SAP_ADV_DATA_NOTCONN, sizeof(advertData), advertData);

	/* Set scan response data. */
	SAP_setParam(SAP_PARAM_ADV, SAP_ADV_DATA_SCANRSP, sizeof(scanRspData), scanRspData);

	/* Enable Advertising and await NP response */
	SAP_setParam(SAP_PARAM_ADV, SAP_ADV_STATE, 1, &enableAdv);


	void GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, HID_INITIAL_ADV_INT_MIN);
	void GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, HID_INITIAL_ADV_INT_MAX);
	void GAP_SetParamValue(TGAP_LIM_ADV_TIMEOUT, HID_INITIAL_ADV_TIMEOUT);

	// Setup advertising filter policy first.
	param = GAP_FILTER_POLICY_ALL;
	void GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, sizeof(uint8_t), &param);

	params = TRUE;
	void GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &param);
}

/*********************************************************************
 * @fn      HidDev_bondCount
 *
 * @brief   Gets the total number of bonded devices.
 *
 * @param   None.
 *
 * @return  number of bonded devices.
 */
static uint8_t HidDev_bondCount(void) {
	uint8_t bondCnt = 0;

	void GAPBondMgr_GetParameter(GAPBOND_BOND_COUNT, &bondCnt);

	return bondCnt;
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
	hidReportVoiceStartInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);
	hidReportVoiceDataInClientCharCfg = (gattCharCfg_t*) malloc(sizeof(gattCharCfg_t) * linkDBNumConns);


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
