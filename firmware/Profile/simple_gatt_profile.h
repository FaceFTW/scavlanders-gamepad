/***********************************************************************
 * \headerfile 	simple_gatt_profile.h
 * \author		Texas Instruments
 * \author 		Alex Westerman
 * \brief 		Prototype Code defining a Bluetooth GATT Profile with
 * 				different characteristics
 * \copyright 	See LICENSE_TI and LICENSE
 ***********************************************************************/
#ifndef SIMPLEGATTPROFILE_H
#define SIMPLEGATTPROFILE_H

#ifdef __cplusplus
extern "C" {
#endif
	/*******************************************************************************
	 *                                CONSTANTS
	 ******************************************************************************/
#include "profile_util.h"

	/*******************************************************************************
	 *                                CONSTANTS
	 ******************************************************************************/
	/* Characteristic Types - These must be listed in order that they appear
	 in service */
#define SP_CHAR1                     0x00
#define SP_CHAR2                     0x01
#define SP_CHAR3                     0x02
#define SP_CHAR4                     0x03

#define SP_CHAR1_ID                  PROFILE_ID_CREATE(SP_CHAR1,PROFILE_VALUE)
#define SP_CHAR2_ID                  PROFILE_ID_CREATE(SP_CHAR2,PROFILE_VALUE)
#define SP_CHAR3_ID                  PROFILE_ID_CREATE(SP_CHAR3,PROFILE_VALUE)
#define SP_CHAR4_ID                  PROFILE_ID_CREATE(SP_CHAR4,PROFILE_VALUE)

	/* Simple Profile Service UUID */
#define SIMPLEPROFILE_SERV_UUID             0xFFF0

	/* Simple Profile Characteristic UUIDs */
#define SIMPLEPROFILE_CHAR1_UUID            0xFFF1
#define SIMPLEPROFILE_CHAR2_UUID            0xFFF2
#define SIMPLEPROFILE_CHAR3_UUID            0xFFF3
#define SIMPLEPROFILE_CHAR4_UUID            0xFFF4

	/*******************************************************************************
	 *                                  FUNCTIONS
	 ******************************************************************************/
	/*
	 * SimpleProfile_AddService- Initializes the Simple GATT Profile service by
	 *          GATT attributes with the GATT server.
	 *
	 * @param   services - services to add. This is a bit map and can
	 *                     contain more than one service.
	 */
	extern uint32_t SimpleProfile_AddService(void);

	/*
	 * SimpleProfile_RegisterAppCBs - Registers the application callback function.
	 *                    Only call this function once.
	 *
	 *    appCallbacks - pointer to application callbacks.
	 */
	extern uint32_t SimpleProfile_RegisterAppCB(BLEProfileCallbacks_t *callbacks);

	/*
	 * SimpleProfile_SetParameter - Set a Simple GATT Profile parameter.
	 *
	 *    param - Profile parameter ID
	 *    len - length of data to right
	 *    value - pointer to data to write.  This is dependent on
	 *          the parameter ID and WILL be cast to the appropriate
	 *          data type (example: data type of uint16 will be cast to
	 *          uint16 pointer).
	 */
	extern uint32_t SimpleProfile_SetParameter(uint8 param, uint8 len, void *value);

	/*
	 * SimpleProfile_GetParameter - Get a Simple GATT Profile parameter.
	 *
	 *    param - Profile parameter ID
	 *    value - pointer to data to write.  This is dependent on
	 *          the parameter ID and WILL be cast to the appropriate
	 *          data type (example: data type of uint16 will be cast to
	 *          uint16 pointer).
	 */
	extern uint32_t SimpleProfile_GetParameter(uint8 param, void *value);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEGATTPROFILE_H */

