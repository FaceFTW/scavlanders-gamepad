/***********************************************************************
 * \file 		simple_application_processor.c
 * \author 		Texas Instruments
 * \author 		Alex Westerman
 * \brief 		Main code defining Bluetooth States and Behavior
 * \copyright 	See LICENSE_TI and LICENSE
 ***********************************************************************/

/***********************************************************************
 * 								INCLUDES
 ***********************************************************************/
#include <string.h>								//Standard C Strings Library
#include <stdlib.h>								//Standard C Library
#include <stdbool.h>							//Standard C Boolean Library
#include <pthread.h>							//TI RTOS Threading Library
#include <mqueue.h>								//TI RTOS Message Queue Library
#include <semaphore.h>							//TI RTOS Semaphore Library
#include <time.h>								//TI RTOS Time Library
#include <unistd.h>								//Standard C Misc. Library

#include <ti/drivers/GPIO.h>					//TI RTOS GPIO Library
#include <ti/drivers/UART.h>					//TI RTOS UART Library
#include <ti/drivers/Timer.h>					//TI RTOS Timer Library
#include <ti/display/Display.h>					//TI RTOS Display Library

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sap/sap.h>							//TI Simple Application Proessor (SAP) Library


/* Local Includes */
#include "simple_application_processor.h"
//#include "Profile/simple_gatt_profile.h"
#include "Profile/profile_util.h"
#include "Board.h"
#include "platform.h"
#include "Profile/hidservice.h"
#include "Profile/devinfoservice.h"

/***********************************************************************
 * 						 VARIABLE DECLARATIONS
 ***********************************************************************/
//Message Queue Vars
static mqd_t apQueueRec;
static mqd_t apQueueSend;
static sem_t notifySem;
Semaphore_Handle serviceHandle;		//
Queue_Handle serviceQueue;			//Queue Object for service Messages
Queue_Struct serviceMsg;				//Queue Structure for Service Messages

Clock_Struct battPerClock;			//Clock used for Battery Service (For conformance)
Clock_Struct idleTimeoutClock;		//Clock used for timeout checks

//Debugging Log Stream
extern Display_Handle displayOut;

//Thread-safe Timer Object
static Timer_Handle timer0;

//Task Configurations
static pthread_t apTask;
static pthread_t notifyTask;

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

/* Connection Handle - only one device currently allowed to connect to SNP */
static uint16_t connHandle = AP_DEFAULT_CONN_HANDLE;

/* BD address of the NWP */
static char nwpstr[] = "NWP:  0xFFFFFFFFFFFF";
#define nwpstrIDX       8

/* BD address of peer device in connection */
static char peerstr[] = "Peer: 0xFFFFFFFFFFFF";
#define peerstrIDX       8

/* Characteristic 3 Update string */
static uint8_t char3 = 3;

/* Characteristic 4 */
static uint8_t char4 = 0;

/*******************************************************************************
 *                         LOCAL FUNCTIONS
 ******************************************************************************/
static void AP_init(void);
static void* AP_taskFxn(void *arg0);
static void* AP_notifyTask(void *arg0);
static void AP_initServices(void);
static void AP_keyHandler(void);
static void AP_timerHandler(Timer_Handle handle);
static void AP_asyncCB(uint8_t cmd1, void *pParams);
static void AP_processSNPEventCB(uint16_t event, snpEventParam_t *param);
static void AP_SPWriteCB(uint8_t charID);
static void AP_SPcccdCB(uint8_t charID, uint16_t value);
static BLEProfileCallbacks_t simpleCBs = {AP_SPWriteCB, AP_SPcccdCB};

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

/*******************************************************************************
 *                        PUBLIC FUNCTIONS
 ******************************************************************************/
/*******************************************************************************
 * @fn      AP_createTask
 *
 * @brief   Task creation function for the Simple BLE Peripheral.
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
void AP_createTask(void) {
	pthread_attr_t pAttrs;
	struct sched_param priParam;
	int retc;
	int detachState;

	pthread_attr_init(&pAttrs);
	priParam.sched_priority = AP_TASK_PRIORITY;

	detachState = PTHREAD_CREATE_DETACHED;
	retc = pthread_attr_setdetachstate(&pAttrs, detachState);

	if (retc != 0) {
		while (1);
	}

	pthread_attr_setschedparam(&pAttrs, &priParam);

	retc |= pthread_attr_setstacksize(&pAttrs, AP_TASK_STACK_SIZE);

	if (retc != 0) {
		while (1);
	}

	/* Creating the two tasks */
	retc = pthread_create(&apTask, &pAttrs, AP_taskFxn, NULL);
	retc = pthread_create(&notifyTask, &pAttrs, AP_notifyTask, NULL);

	if (retc != 0) {
		while (1);
	}
}

/*******************************************************************************
 * @fn      AP_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   None.
 *
 * @return  None.
 *******************************************(**********************************/
static void AP_init(void) {

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

	// Set up services.
	GGS_AddService (GATT_ALL_SERVICES);        // GAP
	GATTServApp_AddService(GATT_ALL_SERVICES);        // GATT attributes

	// Register for Battery service callback.
	Batt_Register (HidDev_batteryCB);

	// Register for Scan Parameters service callback.
	ScanParam_Register (HidDev_scanParamCB);

	// Initialize report ready clock timer
	Clock_construct(&reportReadyClock, HidDev_reportReadyClockCB, reportRdyClockTicks, &reportRdyClockParams);
	Timer_Params params;
	struct mq_attr attr;

	/* Create RTOS Queue */
	attr.mq_flags = 0;
	attr.mq_maxmsg = 64;
	attr.mq_msgsize = sizeof(uint32_t);
	attr.mq_curmsgs = 0;

	apQueueRec = mq_open("SimpleAP", O_RDWR | O_CREAT, 0664, &attr);
	apQueueSend = mq_open("SimpleAP", O_RDWR | O_CREAT | O_NONBLOCK, 0664, &attr);

	/* Setting up the timer in continuous callback mode that calls the callback
	 * functions every 5s.*/
	Timer_Params_init(&params);
	params.period = AP_PERIODIC_EVT_PERIOD;
	params.periodUnits = Timer_PERIOD_US;
	params.timerMode = Timer_CONTINUOUS_CALLBACK;
	params.timerCallback = AP_timerHandler;

	timer0 = Timer_open(Board_Timer0, &params);
	if (timer0 == NULL) {
		Display_print0(displayOut, 0, 0, "Failed to initialized Timer!");
	}

	/* Register Key Handler */
	GPIO_setCallback(Board_BUTTON0, (GPIO_CallbackFxn) AP_keyHandler);
	GPIO_enableInt(Board_BUTTON0);

	GPIO_setCallback(Board_BUTTON1, (GPIO_CallbackFxn) AP_bslKeyHandler);
	GPIO_enableInt(Board_BUTTON1);

	/* Write to the UART. */
	Display_print0(displayOut, 0, 0, "--------- Simple Application Processor Example ---------");
	Display_print0(displayOut, 0, 0, "Application Processor Initializing... ");

	/* Register to receive notifications from Simple Profile if characteristics
	 have been written to */
	SimpleProfile_RegisterAppCB(&simpleCBs);
}

/*******************************************************************************
 * @fn      AP_taskFxn
 *
 * @brief   Application task entry point for the Simple BLE Peripheral.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 ******************************************************************************/
static void* AP_taskFxn(void *arg0) {
	ap_States_t state = AP_RESET;
	struct timespec ts;
	uint8_t enableAdv = 1;
	uint8_t disableAdv = 0;
	uint32_t curEvent = 0;
	uint32_t prio = 0;


	/* Initialize application */
	AP_init();

	Display_print0(displayOut, 0, 0, "Done!");

	/* Application main loop */
	while (1) {
		//HID CODE
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
				if (hidDevConnSecure &&hidDevReportReadyState) {
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

		//OLDCODE
		switch (state) {
			case AP_RESET: {
				/* Make sure CC26xx is not in BSL */
				GPIO_write(Board_RESET, Board_LED_OFF);
				GPIO_write(Board_MRDY, Board_LED_ON);

				usleep(10000);

				GPIO_write(Board_RESET, Board_LED_ON);

				/* Initialize UART port parameters within SAP parameters */
				SAP_initParams(SAP_PORT_REMOTE_UART, &sapParams);

				sapParams.port.remote.mrdyPinID = Board_MRDY;
				sapParams.port.remote.srdyPinID = Board_SRDY;
				sapParams.port.remote.boardID = Board_UART1;

				/* Setup NP module */
				SAP_open(&sapParams);

				/* Register Application thread's callback to receive
				 asynchronous requests from the NP. */
				SAP_setAsyncCB(AP_asyncCB);

				/* Reset the NP, and await a power-up indication.
				 Clear any pending power indications received prior to this
				 reset call */
				clock_gettime(CLOCK_REALTIME, &ts);
				ts.tv_sec += 1;

				mq_timedreceive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio, &ts);

				SAP_reset();

				do {
					curEvent = 0;
					mq_receive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio);

					if ((curEvent != AP_EVT_BSL_BUTTON) && (curEvent != AP_EVT_PUI)) {
						Display_printf(displayOut, 0, 0, "[bleThread] Warning! Unexpected Event %lu", curEvent);
					}
				} while ((curEvent != AP_EVT_BSL_BUTTON) && (curEvent != AP_EVT_PUI));

				if (curEvent == AP_EVT_BSL_BUTTON) {
					state = AP_SBL;
				} else if (curEvent == AP_EVT_PUI) {
					/* Read BD ADDR */
					SAP_setParam(SAP_PARAM_HCI, SNP_HCI_OPCODE_READ_BDADDR, 0,
					NULL);

					/* Setup Services - Service creation is blocking so
					 * no need to pend */
					AP_initServices();

					state = AP_IDLE;
				}
			}
				break;
			case AP_START_ADV: {
				/* Turn on user LED to indicate advertising */
				GPIO_write(Board_LED0, Board_LED_ON);

				Display_print0(displayOut, 0, 0, "Starting advertisement... ");

				SAP_setParam(SAP_PARAM_ADV, SAP_LIM_DISC_ADV_INT_MIN, HID_INITIAL_ADV_INT_MIN);

				SAP_setServiceParam(SNP_GGS_SERV_ID, SNP_GGS_DEVICE_NAME_ATT, sizeof(snpDeviceName), snpDeviceName);	//Setting Advertising Name
				SAP_setParam(SAP_PARAM_ADV, SAP_ADV_DATA_NOTCONN, sizeof(advertData), advertData);  					//Set advertising data.
				SAP_setParam(SAP_PARAM_ADV, SAP_ADV_DATA_SCANRSP, sizeof(scanRspData), scanRspData);					//Set scan response data.
				SAP_setParam(SAP_PARAM_ADV, SAP_ADV_STATE, 1, &enableAdv);												//Enable Advertising and await NP response

				do {
					curEvent = 0;
					mq_receive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio);

					if (curEvent != AP_EVT_ADV_ENB) {
						Display_printf(displayOut, 0, 0, "[bleThread] Warning! Unexpected Event %lu", curEvent);
					}
				} while (curEvent != AP_EVT_ADV_ENB);

				Display_print0(displayOut, 0, 0, "Done!");
				Display_print0(displayOut, 0, 0, "Waiting for connection (or timeout)... ");

				/* Wait for connection or button press to cancel advertisement */
				clock_gettime(CLOCK_REALTIME, &ts);
				ts.tv_sec += 30;
				curEvent = 0;
				mq_timedreceive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio, &ts);

				if (curEvent == AP_EVT_CONN_EST) {
					state = AP_CONNECTED;
				} else {
					state = AP_CANCEL_ADV;
					Display_print0(displayOut, 0, 0, "Advertisement Timeout!");
				}
			}
				break;

			case AP_CONNECTED:
				/* Before connecting, NP will send the stop ADV message */
				do {
					curEvent = 0;
					mq_receive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio);

					if (curEvent != AP_EVT_ADV_END) {
						Display_printf(displayOut, 0, 0, "[bleThread] Warning! Unexpected Event %lu", curEvent);
					}
				} while (curEvent != AP_EVT_ADV_END);

				/* Update State and Characteristic values on LCD */
				Display_print1(displayOut, 0, 0, "Peer connected! (%s)", peerstr);

				/* Events that can happen during connection:
				 *   - Client Disconnect
				 */
				do {
					curEvent = 0;
					mq_receive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio);

					if (curEvent != AP_EVT_CONN_TERM) {
						Display_printf(displayOut, 0, 0, "[bleThread] Warning! Unexpected Event %lu", curEvent);
					}
				} while (curEvent != AP_EVT_CONN_TERM);

				/* Client has disconnected from server */
				SAP_setParam(SAP_PARAM_CONN, SAP_CONN_STATE, sizeof(connHandle), (uint8_t*) &connHandle);

				do {
					curEvent = 0;
					mq_receive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio);

					if ((curEvent != AP_EVT_CONN_TERM) && (curEvent != AP_EVT_ADV_ENB)) {
						Display_printf(displayOut, 0, 0, "[bleThread] Warning! Unexpected Event %lu", curEvent);
					}
				} while (curEvent != AP_EVT_CONN_TERM);

				state = AP_CANCEL_ADV;

				break;

			case AP_CANCEL_ADV:
				Display_print0(displayOut, 0, 0, "Advertisement has been canceled!");

				/* Cancel Advertisement */
				SAP_setParam(SAP_PARAM_ADV, SAP_ADV_STATE, 1, &disableAdv);

				do {
					curEvent = 0;
					mq_receive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio);

					if (curEvent != AP_EVT_ADV_END) {
						Display_printf(displayOut, 0, 0, "[bleThread] Warning! Unexpected Event %lu", curEvent);
					}
				} while (curEvent != AP_EVT_ADV_END);

				state = AP_IDLE;
				break;

			case AP_IDLE:
				/* Turn off user LED to indicate stop advertising */
				GPIO_write(Board_LED0, Board_LED_OFF);
				Display_print0(displayOut, 0, 0, "State set to idle.");

				/* Key Press triggers state change from idle */
				do {
					curEvent = 0;
					mq_receive(apQueueRec, (char*) &curEvent, sizeof(uint32_t), &prio);

					if ((curEvent != AP_EVT_BUTTON_RIGHT) && (curEvent != AP_EVT_BSL_BUTTON)) {
						Display_printf(displayOut, 0, 0, "[bleThread] Warning! Unexpected Event %lu", curEvent);
					}
				} while ((curEvent != AP_EVT_BUTTON_RIGHT) && (curEvent != AP_EVT_BSL_BUTTON));

				if (curEvent == AP_EVT_BUTTON_RIGHT) {
					state = AP_START_ADV;
				} else if (curEvent == AP_EVT_BSL_BUTTON) {
					state = AP_SBL;
				}

				break;

			default:
				break;
		}
	}
}

/*******************************************************************************
 * @fn      AP_notifyTask
 *
 * @brief   Task that we will use for the notification service
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 ******************************************************************************/
static void* AP_notifyTask(void *arg0) {
	sem_init(&notifySem, 1, 0);

	while (1) {
		sem_wait(&notifySem);

		/* Set parameter value of char4. If notifications or indications have
		 been enabled, the profile will send them. */
		SimpleProfile_SetParameter(SP_CHAR4_ID, sizeof(char4), &char4);

		/* Increment the value of characteristic 4 every 5 seconds. This way
		 notification can be more clearly seen on client */
		char4++;
	}
}

/*******************************************************************************
 * @fn      AP_initServices
 *
 * @brief   Configure SNP and register services.
 *
 * @param   None.
 *
 * @return  None.
 ******************************************************************************/
static void AP_initServices(void) {

	/* Add the SimpleProfile Service to the SNP. */
	hidService_AddService();

	DevInfo_AddService();
	Batt_AddService();
	ScanParam_AddService();

	SAP_registerEventCB(AP_processSNPEventCB, 0xFFFF);
}

/*
 * This is a callback operating in the NPI task.
 * These are events this application has registered for.
 */
static void AP_processSNPEventCB(uint16_t event, snpEventParam_t *param) {
	uint32_t eventPend;

	switch (event) {
		case SNP_CONN_EST_EVT: {
			snpConnEstEvt_t *connEstEvt = (snpConnEstEvt_t*) param;

			/* Update Peer Addr String */
			connHandle = connEstEvt->connHandle;
			ProfileUtil_convertBdAddr2Str(&peerstr[peerstrIDX], connEstEvt->pAddr);

			/* Notify state machine of established connection */
			eventPend = AP_EVT_CONN_EST;
			mq_send(apQueueSend, (char*) &eventPend, sizeof(uint32_t), 1);
		}
			break;

		case SNP_CONN_TERM_EVT: {
			connHandle = AP_DEFAULT_CONN_HANDLE;

			/* Notify state machine of disconnection event */
			eventPend = AP_EVT_CONN_TERM;
			mq_send(apQueueSend, (char*) &eventPend, sizeof(uint32_t), 1);
		}
			break;

		case SNP_ADV_STARTED_EVT: {
			snpAdvStatusEvt_t *advEvt = (snpAdvStatusEvt_t*) param;
			if (advEvt->status == SNP_SUCCESS) {
				/* Notify state machine of Advertisement Enabled */
				eventPend = AP_EVT_ADV_ENB;
				mq_send(apQueueSend, (char*) &eventPend, sizeof(uint32_t), 1);
			} else {
				eventPend = AP_ERROR;
				mq_send(apQueueSend, (char*) &eventPend, sizeof(uint32_t), 1);
			}
		}
			break;

		case SNP_ADV_ENDED_EVT: {
			snpAdvStatusEvt_t *advEvt = (snpAdvStatusEvt_t*) param;
			if (advEvt->status == SNP_SUCCESS) {
				/* Notify state machine of Advertisement Disabled */
				eventPend = AP_EVT_ADV_END;
				mq_send(apQueueSend, (char*) &eventPend, sizeof(uint32_t), 1);
			}
		}
			break;

		default:
			break;
	}
}

/*******************************************************************************
 * This is a callback operating in the NPI task.
 * These are Asynchronous indications.
 ******************************************************************************/
static void AP_asyncCB(uint8_t cmd1, void *pParams) {
	uint32_t eventPend;

	switch (SNP_GET_OPCODE_HDR_CMD1(cmd1)) {
		case SNP_DEVICE_GRP: {
			switch (cmd1) {
				case SNP_POWER_UP_IND:
					/* Notify state machine of Power Up Indication */
					eventPend = AP_EVT_PUI;
					mq_send(apQueueSend, (char*) &eventPend, sizeof(uint32_t), 1);
					break;

				case SNP_HCI_CMD_RSP: {
					snpHciCmdRsp_t *hciRsp = (snpHciCmdRsp_t*) pParams;
					switch (hciRsp->opcode) {
						case SNP_HCI_OPCODE_READ_BDADDR:
							/* Update NWP Addr String */
							ProfileUtil_convertBdAddr2Str(&nwpstr[nwpstrIDX], hciRsp->pData);
							break;
						default:
							break;
					}
				}
					break;

				case SNP_EVENT_IND:
					break;

				default:
					break;
			}
		}
			break;

		default:
			break;
	}
}

/*******************************************************************************
 * @fn      AP_keyHandler
 *
 * @brief   Key event handler function
 *
 * @param   none
 *
 * @return  none
 ******************************************************************************/
void AP_keyHandler(void) {
	uint32_t eventPend;
	uint32_t ii;

	GPIO_disableInt(Board_BUTTON0);

	/* Delay for switch debounce */
	for (ii = 0; ii < 20000; ii++);

	GPIO_clearInt(Board_BUTTON0);
	GPIO_enableInt(Board_BUTTON0);

	eventPend = AP_EVT_BUTTON_RIGHT;
	mq_send(apQueueSend, (char*) &eventPend, sizeof(uint32_t), 1);
}

static void AP_timerHandler(Timer_Handle handle) {
	sem_post(&notifySem);
}

static void AP_SPWriteCB(uint8_t charID) {
	switch (PROFILE_ID_CHAR(charID)) {
		case SP_CHAR1:
			switch (PROFILE_ID_CHARTYPE(charID)) {
				case PROFILE_VALUE:
					/* Toggle LED every time a new value is received */
					GPIO_toggle(Board_LED2);
					GPIO_toggle(Board_LED3);
					break;
				default:
					/* Should not receive other types */
					break;
			}
			break;
		case SP_CHAR3:
			switch (PROFILE_ID_CHARTYPE(charID)) {
				case PROFILE_VALUE:
					/* Update LCD with new value of Characteristic 3 */
					SimpleProfile_GetParameter(charID, &char3);
					Display_print1(displayOut, 0, 0, "Characteristic 3: %d", char3);
					break;
				default:
					/* Should not receive other types */
					break;
			}
			break;
		default:
			/* Other Characteristics not writable */
			break;
	}
}

static void AP_SPcccdCB(uint8_t charID, uint16_t value) {
	switch (PROFILE_ID_CHAR(charID)) {
		case SP_CHAR4:
			switch (PROFILE_ID_CHARTYPE(charID)) {
				case PROFILE_CCCD:
					/* If indication or notification flags are set start periodic
					 clock that will write to characteristic 4 */
					if (value & (SNP_GATT_CLIENT_CFG_NOTIFY | SNP_GATT_CLIENT_CFG_INDICATE)) {
						Timer_start(timer0);
						Display_print0(displayOut, 0, 0, "Update enabled!");

						/* Update LCD with current value of Characteristic 4 */
						Display_print1(displayOut, 0, 0, "Characteristic 4: %d", char4);
					} else {
						/* Flags are not set so make sure clock has stopped and clear
						 appropriate fields of LCD */
						Timer_stop(timer0);
					}
					break;

				default:
					/* Should not receive other types */
					break;
			}
			break;

		default:
			break;
	}
}
