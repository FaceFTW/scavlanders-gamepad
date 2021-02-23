/**********************************************************************
 * \headerfile: constants.h
 * \author Alex Westerman
 * Date Created: Feb 18, 2021
 * Description: Common Constants relevant in the HID Profile Code
 * \version: Feb 18, 2021
 ***********************************************************************/
#ifndef PROFILE_CONSTANTS_H_
#define PROFILE_CONSTANTS_H_
#ifdef _cplusplus
extern "C"{
#endif

#include "bt_uuid.h"
#include "profile_util.h"
#include "hidservice.h"

#define HID_PROTOCOL_MODE_LEN 1 // HID Protocol Mode
#define HID_INFORMATION_LEN 4   // HID Information
#define HID_REPORT_REF_LEN 2    // HID Report Reference Descriptor
#define HID_EXT_REPORT_REF_LEN 2// External Report Reference Descriptor

#define HID_STATE_CHANGE_EVT 0x0001
#define HID_BATT_SERVICE_EVT 0x0002
#define HID_PASSCODE_EVT 0x0004
#define HID_PAIR_STATE_EVT 0x0008

#define HIDDEVICE_TASK_PRIORITY 2

#ifndef HIDDEVICE_TASK_STACK_SIZE
#define HIDDEVICE_TASK_STACK_SIZE 400
#endif
/**UUID Definitions**/
uint8_t hidServUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(HID_SERV_UUID), HI_UINT16(HID_SERV_UUID)};

// HID Boot Keyboard Input Report characteristic
uint8_t hidBootKeyInputUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(BOOT_KEY_INPUT_UUID), HI_UINT16(BOOT_KEY_INPUT_UUID)};

// HID Boot Keyboard Output Report characteristic
uint8_t hidBootKeyOutputUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(BOOT_KEY_OUTPUT_UUID), HI_UINT16(BOOT_KEY_OUTPUT_UUID)};

// HID Information characteristic
uint8_t hidInfoUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(HID_INFORMATION_UUID), HI_UINT16(HID_INFORMATION_UUID)};

// HID Report Map characteristic
uint8_t hidReportMapUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(REPORT_MAP_UUID), HI_UINT16(REPORT_MAP_UUID)};

// HID Control Point characteristic
uint8_t hidControlPointUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(HID_CTRL_PT_UUID), HI_UINT16(HID_CTRL_PT_UUID)};

// HID Report characteristic
uint8_t hidReportUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(REPORT_UUID), HI_UINT16(REPORT_UUID)};

// HID Protocol Mode characteristic
uint8_t hidProtocolModeUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(PROTOCOL_MODE_UUID), HI_UINT16(PROTOCOL_MODE_UUID)};

/**GATT Services UUIDs**/
// Generic Access Profile Service UUID
uint8_t gapServiceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GAP_SERVICE_UUID), HI_UINT16(GAP_SERVICE_UUID)};

// Generic Attribute Profile Service UUID
uint8_t gattServiceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_SERVICE_UUID), HI_UINT16(GATT_SERVICE_UUID)};

/**GATT Declarations UUIDs**/
// Primary Service UUID
uint8_t primaryServiceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_PRIMARY_SERVICE_UUID), HI_UINT16(GATT_PRIMARY_SERVICE_UUID)};

// Secondary Service UUID
uint8_t secondaryServiceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_SECONDARY_SERVICE_UUID), HI_UINT16(GATT_SECONDARY_SERVICE_UUID)};

// Include UUID
uint8_t includeUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_INCLUDE_UUID), HI_UINT16(GATT_INCLUDE_UUID)};

// Characteristic UUID
uint8_t characterUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_CHARACTER_UUID), HI_UINT16(GATT_CHARACTER_UUID)};

/**GATT Descriptors UUIDs**/
// Characteristic Extended Properties UUID
uint8_t charExtPropsUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_CHAR_EXT_PROPS_UUID), HI_UINT16(GATT_CHAR_EXT_PROPS_UUID)};

// Characteristic User Description UUID
uint8_t charUserDescUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_CHAR_USER_DESC_UUID), HI_UINT16(GATT_CHAR_USER_DESC_UUID)};

// Client Characteristic Configuration UUID
uint8_t clientCharCfgUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_CLIENT_CHAR_CFG_UUID), HI_UINT16(GATT_CLIENT_CHAR_CFG_UUID)};

// Server Characteristic Configuration UUID
uint8_t servCharCfgUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_SERV_CHAR_CFG_UUID), HI_UINT16(GATT_SERV_CHAR_CFG_UUID)};

// Characteristic Presentation Format UUID
uint8_t charFormatUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_CHAR_FORMAT_UUID), HI_UINT16(GATT_CHAR_FORMAT_UUID)};

// Characteristic Aggregate Format UUID
uint8_t charAggFormatUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_CHAR_AGG_FORMAT_UUID), HI_UINT16(GATT_CHAR_AGG_FORMAT_UUID)};

/**GATT Characteristics**/
// Device Name UUID
uint8_t deviceNameUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(DEVICE_NAME_UUID), HI_UINT16(DEVICE_NAME_UUID)};

// Appearance UUID
uint8_t appearanceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(APPEARANCE_UUID), HI_UINT16(APPEARANCE_UUID)};

// Peripheral Privacy Flag UUID
uint8_t periPrivacyFlagUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(PERI_PRIVACY_FLAG_UUID), HI_UINT16(PERI_PRIVACY_FLAG_UUID)};

// Reconnection Address UUID
uint8_t reconnectAddrUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(RECONNECT_ADDR_UUID), HI_UINT16(RECONNECT_ADDR_UUID)};

// Peripheral Preferred Connection Parameters UUID
uint8_t periConnParamUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(PERI_CONN_PARAM_UUID), HI_UINT16(PERI_CONN_PARAM_UUID)};

// Central Address Resolution UUID
uint8_t centAddrResUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(CENTRAL_ADDRESS_RESOLUTION_UUID), HI_UINT16(CENTRAL_ADDRESS_RESOLUTION_UUID)};

// Service Changed UUID
uint8_t serviceChangedUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(SERVICE_CHANGED_UUID), HI_UINT16(SERVICE_CHANGED_UUID)};

// Valid Range UUID
uint8_t validRangeUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_VALID_RANGE_UUID), HI_UINT16(GATT_VALID_RANGE_UUID)};

// External Report Reference Descriptor
uint8_t extReportRefUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_EXT_REPORT_REF_UUID), HI_UINT16(GATT_EXT_REPORT_REF_UUID)};

// Report Reference characteristic descriptor
uint8_t reportRefUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(GATT_REPORT_REF_UUID), HI_UINT16(GATT_REPORT_REF_UUID)};

// Device information service
uint8_t devInfoServUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(DEVINFO_SERV_UUID), HI_UINT16(DEVINFO_SERV_UUID)};

// System ID
uint8_t devInfoSystemIdUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(SYSTEM_ID_UUID), HI_UINT16(SYSTEM_ID_UUID)};

// Model Number String
uint8_t devInfoModelNumberUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(MODEL_NUMBER_UUID), HI_UINT16(MODEL_NUMBER_UUID)};

// Serial Number String
uint8_t devInfoSerialNumberUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(SERIAL_NUMBER_UUID), HI_UINT16(SERIAL_NUMBER_UUID)};

// Firmware Revision String
uint8_t devInfoFirmwareRevUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(FIRMWARE_REV_UUID), HI_UINT16(FIRMWARE_REV_UUID)};

// Hardware Revision String
uint8_t devInfoHardwareRevUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(HARDWARE_REV_UUID), HI_UINT16(HARDWARE_REV_UUID)};

// Software Revision String
uint8_t devInfoSoftwareRevUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(SOFTWARE_REV_UUID), HI_UINT16(SOFTWARE_REV_UUID)};

// Manufacturer Name String
uint8_t devInfoMfrNameUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(MANUFACTURER_NAME_UUID), HI_UINT16(MANUFACTURER_NAME_UUID)};

// IEEE 11073-20601 Regulatory Certification Data List
uint8_t devInfo11073CertUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(IEEE_11073_CERT_DATA_UUID), HI_UINT16(IEEE_11073_CERT_DATA_UUID)};

// PnP ID
uint8_t devInfoPnpIdUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(PNP_ID_UUID), HI_UINT16(PNP_ID_UUID)};

/* HID information flags */
#define HID_FLAGS_REMOTE_WAKE 0x01         // RemoteWake
#define HID_FLAGS_NORMALLY_CONNECTABLE 0x02// NormallyConnectable

/**HID REPORT DESCRIPTOR DATA**/
static const uint8_t hidReportMap[] = {0x05, 0x01,				//Usage Page: Generic Desktop
		0x09, 0x05,				//Usage: Gamepad
		0xa1, 0x01,				//Collection: Application
		0xa1, 0x00,				//Collection: Physical
		0x05, 0x09,				//Usage Page: Button
		0x19, 0x01,				//Usage Minimum: Button 1
		0x29, 0x08,				//Usage Maximum: Button 8
		0x15, 0x00,				//Logical Minimum: 0
		0x25, 0x01,				//Logical Maximum: 1
		0x75, 0x08,				//Report Size: 8
		0x95, 0x01,				//Report Count: 1
		0x81, 0x02,				//Input (Data, Var, Abs)
		0x05, 0x01,				//Usage Page: Generic Desktop
		0x09, 0x30,				//Usage: X (axis)
		0x09, 0x31,				//Usage: Y (axis)
		0x15, 0x81,				//Logical Minimum: -127
		0x25, 0x7f,				//Logical Maximum: 127
		0x75, 0x08,				//Report Size: 8
		0x95, 0x02,				//Report Count: 2
		0x81, 0x02,				//Input (Data, Var, Abs)
		0xc0,					//End Collection (Physical)
		0xc0					//End Collection (Application)
		};

typedef struct {
		uint16_t event; // Event type.
		uint8_t state; // Event state;
} appEvtHdr_t;

// Event passed from other profiles.
typedef struct {
		appEvtHdr_t hdr;					// Event header
		uint8_t *pData; // Event data
} hidDevEvt_t;

typedef struct {
		uint8_t deviceAddr[B_ADDR_LEN];
		uint16_t connHandle;
		uint8_t uiInputs;
		uint8_t uiOutputs;
} hidDevPasscodeEvt_t;

typedef struct {
		uint8_t id;
		uint8_t type;
		uint8_t len;
		uint8_t data[HID_DEV_DATA_LEN];
} hidDevReport_t;

// HID report mapping table
typedef struct {
		uint16_t handle;           // Handle of report characteristic
		gattAttribute_t *pCccdAttr;           // Pointer to CCCD attribute for report characteristic
		uint8_t id;                // Report ID
		uint8_t type;              // Report type
		uint8_t mode;              // Protocol mode (report or boot)
} hidRptMap_t;

// HID dev configuration structure
typedef struct {
		uint32_t idleTimeout;              // Idle timeout in milliseconds
		uint8_t hidFlags;    // HID feature flags
} hidDevCfg_t;

//Generic Event Header struct. Used to fit ICall/OSAL headers in a generic context without removing as much functionality
typedef struct {
		uint8_t event;           //!<  event
		uint8_t status;          //!< event status
} eventHdr_t;

#ifdef _cplusplus
}
#endif

#endif /* PROFILE_CONSTANTS_H_ */
