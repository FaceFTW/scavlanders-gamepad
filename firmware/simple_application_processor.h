/**********************************************************************
 * \headerfile: simple_application_processor.h
 * \author:Texas Instruments
 * \author: Alex Westerman
 * Description:Prototype code defining Bluetooth States and Behavior
 * \copyright: See LICENSE_TI and LICENSE
 ***********************************************************************/
#ifndef SimpleAP_H
#define SimpleAP_H

#ifdef __cplusplus
extern "C" {
#endif

	/* Advertising interval when device is discoverable
	 * (units of 625us, 160=100ms)
	 */
#define DEFAULT_ADVERTISING_INTERVAL        160

	/* Limited discoverable mode advertises for 30.72s, and then stops
	 General discoverable mode advertises indefinitely */
#define DEFAULT_DISCOVERABLE_MODE           SAP_GAP_ADTYPE_FLAGS_GENERAL

	/* Minimum connection interval (units of 1.25ms, 80=100ms) if automatic
	 parameter update request is enabled */
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL   80

	/* Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic
	 parameter update request is enabled */
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL   800

	/* AP States */
	typedef enum {
		AP_RESET, AP_IDLE, AP_START_ADV, AP_CONNECTED, AP_CANCEL_ADV, AP_OAD, AP_SBL
	} ap_States_t;

	/* Task configuration */
#define AP_TASK_PRIORITY        1
#define AP_TASK_STACK_SIZE      2048

	/* Application Events */
#define AP_NONE                 0x00000001     // No Event
#define AP_EVT_PUI              0x00000002     // Power-Up Indication
#define AP_EVT_ADV_ENB          0x00000004     // Advertisement Enable
#define AP_EVT_ADV_END          0x00000008     // Advertisement Ended
#define AP_EVT_CONN_EST         0x00000010     // Connection Established Event
#define AP_EVT_CONN_TERM        0x00000020     // Connection Terminated Event
#define AP_EVT_BUTTON_RESET     0x00000040     // RESET
#define AP_EVT_BUTTON_RIGHT     0x00000080     // RIGHT Button Press - LP S1
#define AP_ERROR                0x00000100     // Error

	/* How often to perform periodic event (in usec) */
#define AP_PERIODIC_EVT_PERIOD               5000000
#define AP_DEFAULT_CONN_HANDLE               0xFFFF

	/* Company Identifier: Texas Instruments Inc. (13) */
#define TI_COMPANY_ID                        0x000D
#define TI_ST_DEVICE_ID                      0x03
#define TI_ST_KEY_DATA_ID                    0x00

	/* Task creation function for the Application Processor. */
	extern void AP_createTask(void);


#ifdef __cplusplus
}
#endif

#endif /* APPPROCESSOR_H */
