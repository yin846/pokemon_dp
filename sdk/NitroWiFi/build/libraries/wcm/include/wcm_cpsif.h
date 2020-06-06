/*---------------------------------------------------------------------------*
Project: NitroWiFi - WCM - include
File: wcm_cpsif.h

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: wcm_cpsif.h,v $
Revision 1.3  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.2  08/08/2005 11:15:49  terui
added WCM_GetApEssid function

Revision 1.1  07/07/2005 10:45:37  terui
new additions

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_WCM_CPSIF_H_
#define NITROWIFI_WCM_CPSIF_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/
#include <nitro/types.h>

/*---------------------------------------------------------------------------*
constant definitions
 *---------------------------------------------------------------------------*/

// WCM_SendDCFData function's returned value (when abnormal)
#define WCM_CPSIF_RESULT_BEFORE_INIT    (-1)    // WCM library is not initialized
#define WCM_CPSIF_RESULT_ILLEGAL_PARAM  (-2)    // specified parameters are incorrect
#define WCM_CPSIF_RESULT_IN_IRQ_MODE    (-3)    // calls in IRQ mode
#define WCM_CPSIF_RESULT_NO_CONNECTION  (-4)    // AP connection not confirmed or terminated midway through
#define WCM_CPSIF_RESULT_SEND_FAILURE   (-5)    // DCF frame send failed for some reason (needs to be resent)

/*---------------------------------------------------------------------------*
structure definitions
 *---------------------------------------------------------------------------*/

// DCF frame receipt callback function type
typedef void (*WCMRecvInd) (const u8*srcAddr, const u8*dstAddr, const u8*buf, s32 len);

/*---------------------------------------------------------------------------*
function definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
Name: WCM_SetRecvDCFCallback

Description: Registers the CPS receipt callback function.
The receipt callback function registered here is called every time
the wireless driver receives a DCF frame.
NOTICE: When the callback function registered here is within the overlay,
be aware that you need to overwrite the registered callback function
with NULL and clear it before the jump target disappears from memory.

Arguments: callback    -   Specifies a DCF receipt callback function.
Specify NULL to stop receipt notifications.

Returns: None.
 *---------------------------------------------------------------------------*/
void    WCM_SetRecvDCFCallback(WCMRecvInd callback);

/*---------------------------------------------------------------------------*
Name: WCM_GetApMacAddress

Description: Gets the MAC address for the AP to which the wireless driver is connected.
NOTICE: The contents of the buffer indicated by the pointer gotten in this function
can change as a result of interrupts, etc., and the buffer itself may
be released; be aware of these possibilities. We recommend that you call the function while interrupts are disabled
and that you back up the content to another buffer.

Arguments: None.

Returns: u8*     -   Returns a pointer to the MAC address for the connected AP.
                             Returns NULL if there is no connection.
 *---------------------------------------------------------------------------*/
u8*     WCM_GetApMacAddress(void);

/*---------------------------------------------------------------------------*
Name: WCM_GetApEssid

Description: Gets the ESS-ID for the AP to which the wireless driver is connected.
NOTICE: The contents of the buffer indicated by the pointer gotten in this function
can change as a result of interrupts, etc., and the buffer itself may
be released; be aware of these possibilities. We recommend that you call the function while interrupts are disabled
and that you back up the content to another buffer.

Arguments: length  -   Specifies a pointer to the u16-type variable that gets the length
of the ESS-ID. 0 is stored is the connection has not been established.
If it's not necessary to get the length, this argument may
be specified as NULL.

Returns: u8*     -   Returns a pointer to the ESS-ID data row for the connected
AP. Returns NULL if there is no connection.
 *---------------------------------------------------------------------------*/
u8*     WCM_GetApEssid(u16* length);

/*---------------------------------------------------------------------------*
Name: WCM_SendDCFData

Description: Indicates the sending of a DCF frame to the wireless driver. Internally blocked and
waited on until the DCF frame is sent or fails. In addition, fails
if the wireless connection state cannot send DCF frames.
NOTICE: It is assumed that this can be called asynchronously from multiple threads, but
be aware that it is assumed it won't be called from within the interrupt service.

Arguments: dstAddr -   Specifies the MAC address the DCF frame will go to.
buf     -   Specifies a pointer to the data to be sent. The address
needs to even-number. and the cache including the area
in which the data to be sent exists is stored (written to memory).
In addition, be aware that the content of the data itself must be
stored until there is a return from this function.
len     -   Specifies the length of the data to be sent in bytes.

Returns: s32     -   Returns the data length if the send was successful.
Returns a negative number if failed.
 *---------------------------------------------------------------------------*/
s32     WCM_SendDCFData(const u8* dstAddr, const u8* buf, s32 len);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* NITROWIFI_WCM_CPSIF_H_ */

/*---------------------------------------------------------------------------*
End of file
 *---------------------------------------------------------------------------*/
