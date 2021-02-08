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
/*******************************************************************************
 *                                     INCLUDES
 ******************************************************************************/
#include <string.h>
#include <stdint.h>

#include <ti/sap/snp.h>
#include <ti/sap/snp_rpc.h>
#include <ti/sap/sap.h>

/* Local Includes */
#include "simple_gatt_profile.h"
#include "profile_util.h"

/*******************************************************************************
 *                                  GLOBAL TYPEDEFS
 ******************************************************************************/
/* Simple GATT Profile Service UUID: 0xFFF0 */
uint8_t simpleProfileServUUID[SNP_16BIT_UUID_SIZE] =
{
    LO_UINT16(SIMPLEPROFILE_SERV_UUID),
    HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

/* Characteristic 1 UUID: 0xFFF1 */
uint8_t simpleProfilechar1UUID[SNP_16BIT_UUID_SIZE] =
{
    LO_UINT16(SIMPLEPROFILE_CHAR1_UUID),
    HI_UINT16(SIMPLEPROFILE_CHAR1_UUID)
};

/* Characteristic 2 UUID: 0xFFF2 */
uint8_t simpleProfilechar2UUID[SNP_16BIT_UUID_SIZE] =
{
    LO_UINT16(SIMPLEPROFILE_CHAR2_UUID),
    HI_UINT16(SIMPLEPROFILE_CHAR2_UUID)
};

/* Characteristic 3 UUID: 0xFFF3 */
uint8_t simpleProfilechar3UUID[SNP_16BIT_UUID_SIZE] =
{
    LO_UINT16(SIMPLEPROFILE_CHAR3_UUID),
    HI_UINT16(SIMPLEPROFILE_CHAR3_UUID)
};

/* Characteristic 4 UUID: 0xFFF4 */
uint8_t simpleProfilechar4UUID[SNP_16BIT_UUID_SIZE] =
{
    LO_UINT16(SIMPLEPROFILE_CHAR4_UUID),
    HI_UINT16(SIMPLEPROFILE_CHAR4_UUID)
};

/*******************************************************************************
 *                             LOCAL VARIABLES
 ******************************************************************************/
static CharacteristicChangeCB_t simpleProfile_AppWriteCB = NULL;
static CCCDUpdateCB_t simpleProfile_AppCccdCB = NULL;
static uint8_t cccdFlag = 0;
static uint16_t connHandle = 0;

/*******************************************************************************
 *                              Profile Attributes - TYPEDEFS
 ******************************************************************************/
SAP_Service_t simpleService;
SAP_CharHandle_t simpleServiceCharHandles[4];

static UUIDType_t simpleServiceUUID =
{ SNP_16BIT_UUID_SIZE, simpleProfileServUUID };

/* Characteristic 1 Value */
static uint8_t simpleProfileChar1 = 0;

/* Simple Profile Characteristic 1 User Description */
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
SAP_UserDescAttr_t char1UserDesc =
{
    SNP_GATT_PERMIT_READ,
    sizeof(simpleProfileChar1UserDesp),
    sizeof(simpleProfileChar1UserDesp),
    simpleProfileChar1UserDesp
};

SAP_UserDescAttr_t char2UserDesc =
{
    SNP_GATT_PERMIT_READ,
    sizeof(simpleProfileChar2UserDesp),
    sizeof(simpleProfileChar2UserDesp),
    simpleProfileChar2UserDesp
};

SAP_UserDescAttr_t char3UserDesc =
{
    SNP_GATT_PERMIT_READ,
    sizeof(simpleProfileChar3UserDesp),
    sizeof(simpleProfileChar3UserDesp),
    simpleProfileChar3UserDesp
};

SAP_UserDescAttr_t char4UserDesc =
{
    SNP_GATT_PERMIT_READ,
    sizeof(simpleProfileChar4UserDesp),
    sizeof(simpleProfileChar4UserDesp),
    simpleProfileChar4UserDesp
};

SAP_UserCCCDAttr_t char4CCCD =
{
    SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE
};

#define SERVAPP_NUM_ATTR_SUPPORTED 4

static SAP_Char_t simpleProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] =
{
    /* Characteristic 1 Value Declaration */
    {
        {SNP_16BIT_UUID_SIZE, simpleProfilechar1UUID }, /* UUID */
        SNP_GATT_PROP_READ | SNP_GATT_PROP_WRITE,       /* Properties */
        SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE,   /* Permissions */
        &char1UserDesc                                  /* User Description */
    },

    /* Characteristic 2 Value Declaration */
    {
        {SNP_16BIT_UUID_SIZE, simpleProfilechar2UUID }, /* UUID */
        SNP_GATT_PROP_READ,                             /* Properties */
        SNP_GATT_PERMIT_READ,                           /* Permissions */
        &char2UserDesc                                  /* User Description */
    },

    /* Characteristic 3 Value Declaration */
    {
        {SNP_16BIT_UUID_SIZE, simpleProfilechar3UUID }, /* UUID */
        SNP_GATT_PROP_WRITE,                            /* Properties */
        SNP_GATT_PERMIT_WRITE,                          /* Permissions */
        &char3UserDesc                                  /* User Description */
    },

    /* Characteristic 4 Value Declaration */
    {
        {SNP_16BIT_UUID_SIZE, simpleProfilechar4UUID }, /* UUID */
        SNP_GATT_PROP_NOTIFICATION,                     /* Properties */
        0,                                              /* Permissions */
        &char4UserDesc,                                 /* User Description */
        &char4CCCD                                      /* CCCD */
    },
};

/*******************************************************************************
 *                                  LOCAL FUNCTIONS
 ******************************************************************************/
uint8_t simpleProfile_ReadAttrCB(void *context,
        uint16_t connectionHandle, uint16_t charHdl, uint16_t offset,
        uint16_t size, uint16_t *len, uint8_t *pData);

uint8_t simpleProfile_WriteAttrCB(void *context, uint16_t connectionHandle,
        uint16_t charHdl, uint16_t len, uint8_t *pData);

uint8_t simpleProfile_CCCDIndCB(void *context, uint16_t connectionHandle,
        uint16_t cccdHdl, uint8_t type, uint16_t value);

/*******************************************************************************
 * @fn      SimpleProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   none
 *
 * @return  Success or Failure
 ******************************************************************************/
uint32_t SimpleProfile_AddService(void)
{
    /* Reads through table, adding attributes to the NP. */
    simpleService.serviceUUID = simpleServiceUUID;
    simpleService.serviceType = SNP_PRIMARY_SERVICE;
    simpleService.charTableLen = SERVAPP_NUM_ATTR_SUPPORTED;
    simpleService.charTable = simpleProfileAttrTbl;
    simpleService.context = NULL;
    simpleService.charReadCallback = simpleProfile_ReadAttrCB;
    simpleService.charWriteCallback = simpleProfile_WriteAttrCB;
    simpleService.cccdIndCallback = simpleProfile_CCCDIndCB;
    simpleService.charAttrHandles = simpleServiceCharHandles;

    /* Service is setup, register with GATT server on the SNP. */
    SAP_registerService(&simpleService);

    return BLE_PROFILE_SUCCESS;
}

/*******************************************************************************
 * @fn      SimpleProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  BLE_PROFILE_SUCCESS or BLE_PROFILE_ALREADY_IN_REQ_MODE
 ******************************************************************************/
uint32_t SimpleProfile_RegisterAppCB(BLEProfileCallbacks_t *callbacks)
{
    simpleProfile_AppWriteCB = callbacks->charChangeCB;
    simpleProfile_AppCccdCB = callbacks->cccdUpdateCB;

    return (BLE_PROFILE_SUCCESS);
}

/*******************************************************************************
 * @fn      SimpleProfile_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  uint_fast8_t
 ******************************************************************************/
uint32_t SimpleProfile_SetParameter(uint8_t param, uint8_t len, void *value)
{
    uint_fast8_t ret = BLE_PROFILE_SUCCESS;
    switch (PROFILE_ID_CHAR(param))
    {
    case SP_CHAR1:
        if (len == sizeof(uint8_t))
        {
            simpleProfileChar1 = *((uint8*) value);
        } else
        {
            ret = BLE_PROFILE_INVALID_RANGE;
        }
        break;

    case SP_CHAR2:
        if (len == sizeof(uint8_t))
        {
            simpleProfileChar2 = *((uint8*) value);
        } else
        {
            ret = BLE_PROFILE_INVALID_RANGE;
        }
        break;

    case SP_CHAR3:
        if (len == sizeof(uint8_t))
        {
            simpleProfileChar3 = *((uint8*) value);
        } else
        {
            ret = BLE_PROFILE_INVALID_RANGE;
        }
        break;

    case SP_CHAR4:
        if (len == sizeof(uint8_t))
        {
            snpNotifIndReq_t localReq;
            simpleProfileChar4 = *((uint8*) value);

            /* Initialize Request */
            localReq.connHandle = connHandle;
            localReq.attrHandle = ProfileUtil_getHdlFromCharID(
                    PROFILE_ID_CREATE(SP_CHAR4, PROFILE_VALUE),
                    simpleServiceCharHandles, SERVAPP_NUM_ATTR_SUPPORTED);
            localReq.pData = (uint8_t *) &simpleProfileChar4;
            localReq.authenticate = 0;

            /* Check for whether a notification or indication should be sent.
               Both flags should never be allowed to be set by NWP */
            if (cccdFlag & SNP_GATT_CLIENT_CFG_NOTIFY)
            {
                localReq.type = SNP_SEND_NOTIFICATION;
                SNP_RPC_sendNotifInd(&localReq, sizeof(simpleProfileChar4));
            }
            else if (cccdFlag & SNP_GATT_CLIENT_CFG_INDICATE)
            {
                localReq.type = SNP_SEND_INDICATION;
                SNP_RPC_sendNotifInd(&localReq, sizeof(simpleProfileChar4));
            }
        } else
        {
            ret = BLE_PROFILE_INVALID_RANGE;
        }
        break;

    default:
        ret = BLE_PROFILE_INVALIDPARAMETER;
        break;
    }

    return (ret);
}

/*******************************************************************************
 * @fn      SimpleProfile_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  uint_fast8_t
 ******************************************************************************/
uint32_t SimpleProfile_GetParameter(uint8_t param, void *value)
{
    uint_fast8_t ret = BLE_PROFILE_SUCCESS;
    switch (PROFILE_ID_CHAR(param))
    {
    case SP_CHAR1:
        *((uint8*) value) = simpleProfileChar1;
        break;

    case SP_CHAR2:
        *((uint8*) value) = simpleProfileChar2;
        break;

    case SP_CHAR3:
        *((uint8*) value) = simpleProfileChar3;
        break;

    case SP_CHAR4:
        *((uint8*) value) = simpleProfileChar4;
        break;

    default:
        ret = BLE_PROFILE_INVALIDPARAMETER;
        break;
    }

    return (ret);
}

/*******************************************************************************
 * @fn          simpleProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      BLE_PROFILE_SUCCESS, blePending or Failure
 ******************************************************************************/
uint8_t simpleProfile_ReadAttrCB(void *context, uint16_t connectionHandle,
        uint16_t charHdl, uint16_t offset, uint16_t size, uint16_t * pLen,
        uint8_t *pData)
{
    /* Get characteristic from handle */
    uint8_t charID = ProfileUtil_getCharIDFromHdl(charHdl, simpleServiceCharHandles,
            SERVAPP_NUM_ATTR_SUPPORTED);
    uint8_t isValid = 0;

    /* Update connection handle (assumes one connection) */
    connHandle = connectionHandle;

    switch (PROFILE_ID_CHAR(charID))
    {
    case SP_CHAR1:
        switch (PROFILE_ID_CHARTYPE(charID))
        {
        case PROFILE_VALUE:
            *pLen = sizeof(simpleProfileChar1);
            memcpy(pData, &simpleProfileChar1, sizeof(simpleProfileChar1));
            isValid = 1;
            break;

        default:
            /* Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD */
            break;
        }
        break;
    case SP_CHAR2:
        switch (PROFILE_ID_CHARTYPE(charID))
        {
        case PROFILE_VALUE:
            *pLen = sizeof(simpleProfileChar2);
            memcpy(pData, &simpleProfileChar2, sizeof(simpleProfileChar2));
            isValid = 1;
            break;

        default:
            /* Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD */
            break;
        }
        break;
    default:
        /* Should not receive SP_CHAR3 || SP_CHAR4 reads */
        break;
    }

    if (isValid)
    {
        return (SNP_SUCCESS);
    }

    /* Unable to find handle - set len to 0 and return error code */
    *pLen = 0;
    return (SNP_UNKNOWN_ATTRIBUTE);
}

/*******************************************************************************
 * @fn      simpleProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  BLE_PROFILE_SUCCESS, blePending or Failure
 ******************************************************************************/
uint8_t simpleProfile_WriteAttrCB(void *context, uint16_t connectionHandle,
        uint16_t charHdl, uint16_t len, uint8_t *pData)
{
    uint_fast8_t status = SNP_UNKNOWN_ATTRIBUTE;
    uint8_t notifyApp = PROFILE_UNKNOWN_CHAR;

    /* Update connection Handle (assumes one connection) */
    connHandle = connectionHandle;

    /* Get characteristic from handle */
    uint8_t charID = ProfileUtil_getCharIDFromHdl(charHdl, simpleServiceCharHandles,
                            SERVAPP_NUM_ATTR_SUPPORTED);

    switch (PROFILE_ID_CHAR(charID))
    {
    case SP_CHAR1:
        switch (PROFILE_ID_CHARTYPE(charID))
        {
        case PROFILE_VALUE:
            if (len == sizeof(simpleProfileChar1))
            {
                simpleProfileChar1 = pData[0];
                status = SNP_SUCCESS;
                notifyApp = SP_CHAR1_ID;
            }
            break;
        default:
            /* Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD */
            break;
        }
        break;
    case SP_CHAR3:
        switch (PROFILE_ID_CHARTYPE(charID))
        {
        case PROFILE_VALUE:
            if (len == sizeof(simpleProfileChar3))
            {
                simpleProfileChar3 = pData[0];
                status = SNP_SUCCESS;
                notifyApp = SP_CHAR3_ID;
            }
            break;
        default:
            /* Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD */
            break;
        }
        break;
    default:
        /* Should not receive SP_CHAR2 || SP_CHAR4 writes */
        break;
    }

    /* If a characteristic value changed then callback function to notify
     * application of change */
    if ((notifyApp != PROFILE_UNKNOWN_CHAR) && simpleProfile_AppWriteCB)
    {
        simpleProfile_AppWriteCB(notifyApp);
    }

    return (status);
}

/*******************************************************************************
 * @fn      simpleProfile_CCCDIndCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  BLE_PROFILE_SUCCESS, blePending or Failure
 ******************************************************************************/
uint8_t simpleProfile_CCCDIndCB(void *context, uint16_t connectionHandle,
        uint16_t cccdHdl, uint8_t type, uint16_t value)
{
    uint_fast8_t status = SNP_UNKNOWN_ATTRIBUTE;
    uint_fast8_t notifyApp = PROFILE_UNKNOWN_CHAR;

    /* Update connection handle (assumes one connection) */
    connHandle = connectionHandle;

    /* Get characteristic from handle */
    uint8_t charID = ProfileUtil_getCharIDFromHdl(cccdHdl, simpleServiceCharHandles,
            SERVAPP_NUM_ATTR_SUPPORTED);

    switch (PROFILE_ID_CHAR(charID))
    {
    case SP_CHAR4:
        switch (PROFILE_ID_CHARTYPE(charID))
        {
        case PROFILE_CCCD:
            /* Set Global cccd Flag which will be used to to gate Indications
               or notifications when SetParameter() is called */
            cccdFlag = value;
            notifyApp = charID;
            status = SNP_SUCCESS;
            break;
        default:
            /* Should not receive SP_VALUE || SP_USERDESC || SP_FORMAT */
            break;
        }
        break;
    default:
        /* No other Characteristics have CCCB attributes */
        break;
    }

    /* If a characteristic value changed then callback function to notify
     * application of change.
     */
    if ((notifyApp != PROFILE_UNKNOWN_CHAR) && simpleProfile_AppCccdCB)
    {
        simpleProfile_AppCccdCB(notifyApp, value);
    }

    return (status);
}
