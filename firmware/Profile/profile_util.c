/***********************************************************************
 * \headerfile 	profile_util.c
 * \author		Texas Instruments
 * \author 		Alex Westerman
 * \brief 		Main code defining common macros and functions related
 * 				to bluetooth prototype code
 * \copyright 	See LICENSE_TI and LICENSE
 ***********************************************************************/

/*******************************************************************************
 *                                INCLUDES
 ******************************************************************************/
#include <ti/sap/snp.h>
#include "profile_util.h"

/*******************************************************************************
 *                                FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 * @fn      utilExtractUuid16
 *
 * @brief   Extracts a 16-bit UUID from a GATT attribute
 *
 * @param   pAttr - pointer to attribute
 *
 * @param   pUuid - pointer to UUID to be extracted
 *
 * @return  Success or Failure
 ******************************************************************************/
uint16_t ProfileUtil_getCharIDFromHdl(uint16_t charHdl, SAP_CharHandle_t *serviceCharHandles, uint8_t numATTRSupported) {
	uint16_t ii;

	/* Find Characteristic based on Handle */
	for (ii = 0; ii < numATTRSupported; ii++) {
		/* Check if it is the charstick value handle or cccd handle */
		if (serviceCharHandles[ii].valueHandle == charHdl) {
			return PROFILE_ID_CREATE(ii, PROFILE_VALUE);
		} else if (serviceCharHandles[ii].userDescHandle == charHdl) {
			return PROFILE_ID_CREATE(ii, PROFILE_USERDESC);
		} else if (serviceCharHandles[ii].cccdHandle == charHdl) {
			return PROFILE_ID_CREATE(ii, PROFILE_CCCD);
		} else if (serviceCharHandles[ii].formatHandle == charHdl) {
			return PROFILE_ID_CREATE(ii, PROFILE_FORMAT);
		}
	}

	return PROFILE_UNKNOWN_CHAR;
}

/*******************************************************************************
 * @fn      utilExtractUuid16
 *
 * @brief   Extracts a 16-bit UUID from a GATT attribute
 *
 * @param   pAttr - pointer to attribute
 *
 * @param   pUuid - pointer to UUID to be extracted
 *
 * @return  Success or Failure
 */
/******************************************************************************/
uint16_t ProfileUtil_getHdlFromCharID(uint16_t charID, SAP_CharHandle_t *serviceCharHandles, uint8_t numATTRSupported) {
	SAP_CharHandle_t charstick;

	if (PROFILE_ID_CHAR(charID) < numATTRSupported) {
		charstick = serviceCharHandles[PROFILE_ID_CHAR(charID)];

		switch (PROFILE_ID_CHARTYPE(charID)) {
			case PROFILE_VALUE:
				return charstick.valueHandle;
			case PROFILE_USERDESC:
				return charstick.userDescHandle;
			case PROFILE_CCCD:
				return charstick.cccdHandle;
			case PROFILE_FORMAT:
				return charstick.formatHandle;
			default:
				break;
		}
	}

	return SNP_INVALID_HANDLE;
}

/*******************************************************************************
 * @fn      ProfileUtil_convertBdAddr2Str
 *
 * @brief   Convert Bluetooth address to string. Only needed when
 *          LCD display is used.
 *
 * @param   pAddr - BD address
 *
 * @return  BD address as a string
 ******************************************************************************/
void ProfileUtil_convertBdAddr2Str(char *str, uint8_t *pAddr) {
	uint8_t charCnt;
	char hex[] = "0123456789ABCDEF";

	/* Start from end of addr */
	pAddr += PROFILE_UTIL_ADDR_LEN;

	for (charCnt = PROFILE_UTIL_ADDR_LEN; charCnt > 0; charCnt--) {
		*str++ = hex[*--pAddr >> 4];
		*str++ = hex[*pAddr & 0x0F];
	}

	return;
}
