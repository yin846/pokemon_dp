/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_ks.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_ks.c,v $
  Revision 1.7  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.6  06/07/2005 05:45:26  seiki_masashi
  Revised to reduce special processing related to key sharing

  Revision 1.5  03/03/2005 14:46:19  seiki_masashi
  Added WM_DISABLE_KEYSHARING switch
  Fixed WM_EndKeySharing, which always returned an error code

  Revision 1.4  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.3  11/02/2004 07:26:51  terui
  Comments revised.

  Revision 1.2  10/22/2004 04:37:28  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.1  09/10/2004 11:03:30  terui
  New upload reflecting the breakup of wm.c.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
  Name:         WM_StartKeySharing

  Description:  Enables the key sharing feature.
                By performing MP communication after enabling the feature, key sharing
                communication occurs accompanying MP communication.

  Arguments:    buf         -   Pointer to the buffer that stores the key information
                                Entity is a pointer to a WMDataSharingInfo structure.
                port        -   port number to use

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartKeySharing(WMKeySetBuf *buf, u16 port)
{
    return WM_StartDataSharing(buf, port, 0xffff, WM_KEYDATA_SIZE, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndKeySharing

  Description:  Disables the key sharing feature.

  Arguments:    buf         -   Pointer to the buffer that stores the key information
                                Entity is a pointer to a WMDataSharingInfo structure.

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndKeySharing(WMKeySetBuf *buf)
{
    return WM_EndDataSharing(buf);
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetKeySet

  Description:  Load one key set data that is key shared.

  Arguments:    buf         -   Pointer to the buffer that stores the key information
                                Entity is a pointer to a WMDataSharingInfo structure.
                keySet      -   Pointer to the buffer that reads key set.
                                Specify a buffer other than the one provided by WM_StartKeySharing.
                                 

  Returns:      MWErrCode   -   Returns process results.
 *---------------------------------------------------------------------------*/
WMErrCode WM_GetKeySet(WMKeySetBuf *buf, WMKeySet *keySet)
{
    WMErrCode result;
    u16     sendData[WM_KEYDATA_SIZE / sizeof(u16)];
    WMDataSet ds;
    WMArm9Buf *p = WMi_GetSystemWork();

    {
        sendData[0] = (u16)PAD_Read();
        result = WM_StepDataSharing(buf, sendData, &ds);
        if (result == WM_ERRCODE_SUCCESS)
        {
            keySet->seqNum = buf->currentSeqNum;

            {
                u16     iAid;
                for (iAid = 0; iAid < 16; iAid++)
                {
                    u16    *keyData;
                    keyData = WM_GetSharedDataAddress(buf, &ds, iAid);
                    if (keyData != NULL)
                    {
                        keySet->key[iAid] = keyData[0];
                    }
                    else
                    {
                        // Other party that failed to receive is 0
                        keySet->key[iAid] = 0;
                    }
                }
            }
            return WM_ERRCODE_SUCCESS; // Successful completion
        }
        else
        {
            return result;
        }
    }
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
