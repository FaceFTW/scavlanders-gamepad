/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#ifndef PROFILE_UTIL_H
#define PROFILE_UTIL_H

/*******************************************************************************
 *                                   INCLUDES
 ******************************************************************************/
#include <ti/sap/sap.h>

/*******************************************************************************
 *                                   MACROS
 ******************************************************************************/
#define PROFILE_ID_CREATE(C,H)               (( C << 2 ) | H )
#define PROFILE_ID_CHAR(ID)                  ( ID >> 2 )
#define PROFILE_ID_CHARTYPE(ID)              ( ID & 0x03 )

/* Characteristic Handle Types */
#define PROFILE_VALUE                        0x00
#define PROFILE_USERDESC                     0x01
#define PROFILE_CCCD                         0x02
#define PROFILE_FORMAT                       0x03
#define PROFILE_UNKNOWN_CHAR                 0xFF

/* TI Base 128-bit UUID: F000XXXX-0451-4000-B000-000000000000 */
#define TI_BASE_UUID_128( uuid )  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16( uuid ), HI_UINT16( uuid ), 0x00, 0xF0

#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define BUILD_UINT32(lowByte0, lowByte1, lowByte2, lowByte3) \
          ((uint32_t)(((lowByte0) & 0x00FF) + (((lowByte1) & 0x00FF) << 8) + \
                 (((lowByte2) & 0x00FF) << 16) + (((lowByte3) & 0x00FF) << 24)))

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define HH_UINT32(a) (((a) >> 24) & 0xFF)
#define HL_UINT32(a) (((a) >> 16) & 0xFF)
#define LH_UINT32(a) (((a) >> 8) & 0xFF)
#define LL_UINT32(a) ((a) & 0xFF)

/* Generic Status Return Values */
#define BLE_PROFILE_SUCCESS                   0x00
#define BLE_PROFILE_FAILURE                   0x01
#define BLE_PROFILE_INVALIDPARAMETER          0x02
#define BLE_PROFILE_INVALID_TASK              0x03
#define BLE_PROFILE_MSG_BUFFER_NOT_AVAIL      0x04
#define BLE_PROFILE_INVALID_MSG_POINTER       0x05
#define BLE_PROFILE_INVALID_EVENT_ID          0x06
#define BLE_PROFILE_INVALID_INTERRUPT_ID      0x07
#define BLE_PROFILE_NO_TIMER_AVAIL            0x08
#define BLE_PROFILE_NV_ITEM_UNINIT            0x09
#define BLE_PROFILE_NV_OPER_FAILED            0x0A
#define BLE_PROFILE_INVALID_MEM_SIZE          0x0B
#define BLE_PROFILE_NV_BAD_ITEM_LEN           0x0C
#define BLE_PROFILE_INVALID_TASK_ID           INVALID_TASK
#define BLE_PROFILE_NOT_READY                 0x10
#define BLE_PROFILE_ALREADY_IN_REQ_MODE       0x11
#define BLE_PROFILE_INCORRECT_MODE            0x12
#define BLE_PROFILE_MEM_ALLOC_ERROR           0x13
#define BLE_PROFILE_NOT_CONNECTED             0x14
#define BLE_PROFILE_NO_RESOURCES              0x15
#define BLE_PROFILE_PENDING                   0x16
#define BLE_PROFILE_TIMEOUT                   0x17
#define BLE_PROFILE_INVALID_RANGE             0x18
#define BLE_PROFILE_LINK_ENCRYPTED            0x19
#define BLE_PROFILE_PROCEDURE_COMPLETE        0x1A
#define BLE_PROFILE_INVALID_MTU_SIZE          0x1B
#define BLE_PROFILE_GAP_USER_CANCELED         0x30
#define BLE_PROFILE_GAP_CONN_NOT_ACCEPTABLE   0x31
#define BLE_PROFILE_GAP_BOND_REJECTED         0x32
#define BLE_PROFILE_INVALID_PDU               0x40
#define BLE_PROFILE_INSUFFICIENT_AUTH         0x41
#define BLE_PROFILE_INSUFFICIENT_ENCRYPT      0x42
#define BLE_PROFILE_INSUFFICIENT_KEYSIZE      0x43

/*******************************************************************************
 *                                 PROFILE CALLBACKS
 ******************************************************************************/
/* Callback when a characteristic value has changed */
typedef void (*CharacteristicChangeCB_t)(uint8_t paramID);

/* Callback when a cccd update request is received */
typedef void (*CCCDUpdateCB_t)(uint8_t paramID, uint16_t value);

typedef struct
{
    CharacteristicChangeCB_t charChangeCB;
    CCCDUpdateCB_t cccdUpdateCB;
} BLEProfileCallbacks_t;

/*******************************************************************************
 *                                 FUNCTIONS
 ******************************************************************************/
extern uint16_t ProfileUtil_getCharIDFromHdl(uint16_t charHdl,
        SAP_CharHandle_t *serviceCharHandles, uint8_t numATTRSupported);
extern uint16_t ProfileUtil_getHdlFromCharID(uint16_t charID,
        SAP_CharHandle_t *serviceCharHandles, uint8_t numATTRSupported);
extern void ProfileUtil_convertBdAddr2Str(char *str, uint8_t *pAddr);

#define PROFILE_UTIL_ADDR_LEN 6

#endif /*  PROFILE_UTIL_H */

