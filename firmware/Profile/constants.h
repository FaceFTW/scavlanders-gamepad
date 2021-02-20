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

#define HID_PROTOCOL_MODE_LEN 1 // HID Protocol Mode
#define HID_INFORMATION_LEN 4   // HID Information
#define HID_REPORT_REF_LEN 2    // HID Report Reference Descriptor
#define HID_EXT_REPORT_REF_LEN 2// External Report Reference Descriptor

/**UUID Definitions**/
const uint8_t hidServUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(HID_SERV_UUID), HI_UINT16(HID_SERV_UUID)};

// HID Boot Keyboard Input Report characteristic
const uint8_t hidBootKeyInputUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(BOOT_KEY_INPUT_UUID), HI_UINT16(BOOT_KEY_INPUT_UUID)};

// HID Boot Keyboard Output Report characteristic
const uint8_t hidBootKeyOutputUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(BOOT_KEY_OUTPUT_UUID), HI_UINT16(BOOT_KEY_OUTPUT_UUID)};

// HID Information characteristic
const uint8_t hidInfoUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(HID_INFORMATION_UUID), HI_UINT16(HID_INFORMATION_UUID)};

// HID Report Map characteristic
const uint8_t hidReportMapUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(REPORT_MAP_UUID), HI_UINT16(REPORT_MAP_UUID)};

// HID Control Point characteristic
const uint8_t hidControlPointUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(HID_CTRL_PT_UUID), HI_UINT16(HID_CTRL_PT_UUID)};

// HID Report characteristic
const uint8_t hidReportUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(REPORT_UUID), HI_UINT16(REPORT_UUID)};

// HID Protocol Mode characteristic
const uint8_t hidProtocolModeUUID[ATT_BT_UUID_SIZE] = {LO_UINT16(PROTOCOL_MODE_UUID), HI_UINT16(PROTOCOL_MODE_UUID)};

/**GATT Services UUIDs**/
// Generic Access Profile Service UUID
const uint8_t gapServiceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GAP_SERVICE_UUID), HI_UINT16( GAP_SERVICE_UUID)};

// Generic Attribute Profile Service UUID
const uint8_t gattServiceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_SERVICE_UUID), HI_UINT16( GATT_SERVICE_UUID)};

/**GATT Declarations UUIDs**/
// Primary Service UUID
const uint8_t primaryServiceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_PRIMARY_SERVICE_UUID), HI_UINT16( GATT_PRIMARY_SERVICE_UUID)};

// Secondary Service UUID
const uint8_t secondaryServiceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_SECONDARY_SERVICE_UUID), HI_UINT16( GATT_SECONDARY_SERVICE_UUID)};

// Include UUID
const uint8_t includeUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_INCLUDE_UUID), HI_UINT16( GATT_INCLUDE_UUID)};

// Characteristic UUID
const uint8_t characterUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_CHARACTER_UUID), HI_UINT16( GATT_CHARACTER_UUID)};

/**GATT Descriptors UUIDs**/
// Characteristic Extended Properties UUID
const uint8_t charExtPropsUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_CHAR_EXT_PROPS_UUID), HI_UINT16( GATT_CHAR_EXT_PROPS_UUID)};

// Characteristic User Description UUID
const uint8_t charUserDescUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_CHAR_USER_DESC_UUID), HI_UINT16( GATT_CHAR_USER_DESC_UUID)};

// Client Characteristic Configuration UUID
const uint8_t clientCharCfgUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_CLIENT_CHAR_CFG_UUID), HI_UINT16( GATT_CLIENT_CHAR_CFG_UUID)};

// Server Characteristic Configuration UUID
const uint8_t servCharCfgUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_SERV_CHAR_CFG_UUID), HI_UINT16( GATT_SERV_CHAR_CFG_UUID)};

// Characteristic Presentation Format UUID
const uint8_t charFormatUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_CHAR_FORMAT_UUID), HI_UINT16( GATT_CHAR_FORMAT_UUID)};

// Characteristic Aggregate Format UUID
const uint8_t charAggFormatUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_CHAR_AGG_FORMAT_UUID), HI_UINT16( GATT_CHAR_AGG_FORMAT_UUID)};

/**GATT Characteristics**/
// Device Name UUID
const uint8_t deviceNameUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( DEVICE_NAME_UUID), HI_UINT16( DEVICE_NAME_UUID)};

// Appearance UUID
const uint8_t appearanceUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( APPEARANCE_UUID), HI_UINT16( APPEARANCE_UUID)};

// Peripheral Privacy Flag UUID
const uint8_t periPrivacyFlagUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( PERI_PRIVACY_FLAG_UUID), HI_UINT16( PERI_PRIVACY_FLAG_UUID)};

// Reconnection Address UUID
const uint8_t reconnectAddrUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( RECONNECT_ADDR_UUID), HI_UINT16( RECONNECT_ADDR_UUID)};

// Peripheral Preferred Connection Parameters UUID
const uint8_t periConnParamUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( PERI_CONN_PARAM_UUID), HI_UINT16( PERI_CONN_PARAM_UUID)};

// Central Address Resolution UUID
const uint8_t centAddrResUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( CENTRAL_ADDRESS_RESOLUTION_UUID), HI_UINT16( CENTRAL_ADDRESS_RESOLUTION_UUID)};

// Service Changed UUID
const uint8_t serviceChangedUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( SERVICE_CHANGED_UUID), HI_UINT16( SERVICE_CHANGED_UUID)};

// Valid Range UUID
const uint8_t validRangeUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_VALID_RANGE_UUID), HI_UINT16( GATT_VALID_RANGE_UUID)};

// External Report Reference Descriptor
const uint8_t extReportRefUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_EXT_REPORT_REF_UUID), HI_UINT16( GATT_EXT_REPORT_REF_UUID)};

// Report Reference characteristic descriptor
const uint8_t reportRefUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( GATT_REPORT_REF_UUID), HI_UINT16( GATT_REPORT_REF_UUID)};

// HID Information characteristic value
static const uint8_t hidInfo[HID_INFORMATION_LEN] = {LO_UINT16(0x0111), HI_UINT16(0x0111),           // bcdHID (USB HID version)
0x00,           // bCountryCode
		HID_SERVICE_FLAGS           // Flags
		};

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
		0x29, 0x0c,				//Usage Maximum: Button 12
		0x15, 0x00,				//Logical Minimum: 0
		0x25, 0x01,				//Logical Maximum: 1
		0x75, 0x0c,				//Report Size: 12
		0x95, 0x01,				//Report Count: 1
		0x81, 0x02,				//Input (Data, Var, Abs)
		0x05, 0x01,				//Usage Page: Generic Desktop
		0x09, 0x30,				//Usage: X (axis)
		0x09, 0x31,				//Usage: Y (axis)
		0x09, 0x32,				//Usage: Z (axis)
		0x09, 0x33,				//Usage: Rx (axis)
		0x15, 0x81,				//Logical Minimum: -127
		0x25, 0x7f,				//Logical Maximum: 127
		0x75, 0x08,				//Report Size: 8
		0x95, 0x04,				//Report Count: 4
		0x81, 0x02,				//Input (Data, Var, Abs)
		0xc0,					//End Collection (Physical)
		0xc0					//End Collection (Application)
		};

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
