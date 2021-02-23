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
uint16_t ProfileUtil_getCharIDFromHdl(uint16_t charHdl,
        SAP_CharHandle_t *serviceCharHandles, uint8_t numATTRSupported)
{
    uint16_t ii;

    /* Find Characteristic based on Handle */
    for (ii = 0; ii < numATTRSupported; ii++)
    {
        /* Check if it is the charstick value handle or cccd handle */
        if (serviceCharHandles[ii].valueHandle == charHdl)
        {
            return PROFILE_ID_CREATE(ii, PROFILE_VALUE);
        } else if (serviceCharHandles[ii].userDescHandle == charHdl)
        {
            return PROFILE_ID_CREATE(ii, PROFILE_USERDESC);
        } else if (serviceCharHandles[ii].cccdHandle == charHdl)
        {
            return PROFILE_ID_CREATE(ii, PROFILE_CCCD);
        } else if (serviceCharHandles[ii].formatHandle == charHdl)
        {
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
uint16_t ProfileUtil_getHdlFromCharID(uint16_t charID,
        SAP_CharHandle_t *serviceCharHandles, uint8_t numATTRSupported)
{
    SAP_CharHandle_t charstick;

    if (PROFILE_ID_CHAR(charID) < numATTRSupported)
    {
        charstick = serviceCharHandles[PROFILE_ID_CHAR(charID)];

        switch (PROFILE_ID_CHARTYPE(charID))
        {
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
void ProfileUtil_convertBdAddr2Str(char *str, uint8_t *pAddr)
{
    uint8_t charCnt;
    char hex[] = "0123456789ABCDEF";

    /* Start from end of addr */
    pAddr += PROFILE_UTIL_ADDR_LEN;

    for (charCnt = PROFILE_UTIL_ADDR_LEN; charCnt > 0; charCnt--)
    {
        *str++ = hex[*--pAddr >> 4];
        *str++ = hex[*pAddr & 0x0F];
    }

    return;
}
