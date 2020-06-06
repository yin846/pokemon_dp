/*---------------------------------------------------------------------------*
Project: NitroWiFi - WCM - libraries
File: util.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: util.c,v $
Revision 1.3  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.2  10/05/2005 08:51:45  terui
Added a function for storing signal strength in an internal buffer.
Made improvements so that the link level retrieved by WCM_GetLinkLevel is calculated based on the data in the internal buffer.

Revision 1.1  09/10/2005 03:08:43  terui
Initial upload.

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"

/*---------------------------------------------------------------------------*
constant definitions
 *---------------------------------------------------------------------------*/
#define WCM_NETWORK_CAPABILITY_KIND_MASK        0x0003
#define WCM_NETWORK_CAPABILITY_KIND_ESS         0x0001
#define WCM_NETWORK_CAPABILITY_KIND_IBSS        0x0002
#define WCM_NETWORK_CAPABILITY_PRIVACY          0x0010
#define WCM_NETWORK_CAPABILITY_PREAMBLE_MASK    0x0020
#define WCM_NETWORK_CAPABILITY_PREAMBLE_LONG    0x0000
#define WCM_NETWORK_CAPABILITY_PREAMBLE_SHORT   0x0020

#define WCM_RSSI_COUNT_MAX                      16
#define WCM_RSSI_BORDER_HIGH                    28
#define WCM_RSSI_BORDER_MIDDLE                  22
#define WCM_RSSI_BORDER_LOW                     16

/*---------------------------------------------------------------------------*
Local function prototypes
 *---------------------------------------------------------------------------*/
static WMLinkLevel  WcmGetLinkLevel(void);

/*---------------------------------------------------------------------------*
Variable definitions
 *---------------------------------------------------------------------------*/
static u8       wcmRssi[WCM_RSSI_COUNT_MAX];
static u8       wcmRssiIndex = 0;

/*---------------------------------------------------------------------------*
Name: WCM_CompareBssID

Description: Compares two BSSIDs. Synchronous function.

Arguments: a       -   Specifies a pointer to a BSSID for comparison.
b       -   Specifies a pointer to the other BSSID for comparison.

Returns: BOOL    -   Returns TRUE if the two BSSIDs are identical.
If the BSSIDs differ, returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL WCM_CompareBssID(u8* a, u8* b)
{
    s32 i;

    for (i = 0; i < WCM_BSSID_SIZE; i++)
    {
        if (a[i] != b[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
Name: WCM_GetLinkLevel

Description: Obtains the link strength at DCF communication. Synchronous function.

Arguments: None.

Returns: WMLinkLevel -   Returns at link strength at one of four evaluated levels.
 *---------------------------------------------------------------------------*/
WMLinkLevel WCM_GetLinkLevel(void)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();
    WMLinkLevel ret = WM_LINK_LEVEL_0;

// initialization confirmation
    if (w != NULL)
    {
        if (w->phase == WCM_PHASE_DCF)
        {
            ret = WcmGetLinkLevel();
        }
    }

    (void)OS_RestoreInterrupts(e);
    return ret;
}

/*---------------------------------------------------------------------------*
Name: WCM_IsInfrastructureNetwork

Description: Checks whether the data obtained as AP search results
are those for an infrastructure network.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for an infrastructure network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL WCM_IsInfrastructureNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_KIND_MASK) == WCM_NETWORK_CAPABILITY_KIND_ESS)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
Name: WCM_IsAdhocNetwork

Description: Checks whether the data obtained as AP search results
are those for an ad-hoc network.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for an ad-hoc network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL WCM_IsAdhocNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_KIND_MASK) == WCM_NETWORK_CAPABILITY_KIND_IBSS)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
Name: WCM_IsPrivacyNetwork

Description: Checks whether the data obtained as AP search results are from
a network with long preamble settings applied.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for a WEP security network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL WCM_IsPrivacyNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if (bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PRIVACY)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
Name: WCM_IsShortPreambleNetwork

Description: Checks whether the data obtained as AP search results are from
a network with long preamble settings applied.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for a short preamble network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL WCM_IsShortPreambleNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PREAMBLE_MASK) == WCM_NETWORK_CAPABILITY_PREAMBLE_SHORT)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
Name: WCM_IsLongPreambleNetwork

Description: Checks whether the data obtained as AP search results are from
a network with long preamble settings applied.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for a long preamble network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL WCM_IsLongPreambleNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PREAMBLE_MASK) == WCM_NETWORK_CAPABILITY_PREAMBLE_LONG)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
Name: WCM_IsEssidHidden

Description: Checks whether the ESS-ID for the data obtained as AP search results
is hidden.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the ESS-ID is hidden.
Returns FALSE if a valid ESS-ID is published.
 *---------------------------------------------------------------------------*/
BOOL WCM_IsEssidHidden(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        s32 i;

        if (bssDesc->ssidLength == 0)
        {
            return TRUE;    // stealth if length is 0
        }

        for (i = 0; (i < bssDesc->ssidLength) && (i < WM_SIZE_SSID); i++)
        {
            if (bssDesc->ssid[i] != 0x00)
            {
                return FALSE;
            }
        }

        return TRUE;        // stealth if total length portion is 0x00
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
Name: WCMi_GetRssiAverage

Description: Gets the average RSSI value from the received signal strength history
                stored in the internal buffer.

Arguments: None.

Returns: u8      -   Returns the average RSSI value. Since this is an historical average,
                            care must be taken to return a valid value based on past history
                            even when not connected.
 *---------------------------------------------------------------------------*/
u8 WCMi_GetRssiAverage(void)
{
    s32 sum = 0;
    s32 i;

    if (wcmRssiIndex > WCM_RSSI_COUNT_MAX)
    {
        for (i = 0; i < WCM_RSSI_COUNT_MAX; i++)
        {
            sum += wcmRssi[i];
        }

        sum /= WCM_RSSI_COUNT_MAX;
    }
    else if (wcmRssiIndex > 0)
    {
        for (i = 0; i < wcmRssiIndex; i++)
        {
            sum += wcmRssi[i];
        }

        sum /= wcmRssiIndex;
    }

    return(u8) sum;
}

/*---------------------------------------------------------------------------*
Name: WcmGetLinkLevel

Description: Retrieves the link level based on the received signal strength history
                stored in the internal buffer.

Arguments: None.

Returns: WMLinkLevel -   Returns the link level.
 *---------------------------------------------------------------------------*/
static WMLinkLevel WcmGetLinkLevel(void)
{
    u8          ave = WCMi_GetRssiAverage();
    WMLinkLevel level = WM_LINK_LEVEL_0;

// Calculate the level based on sum
    if (ave >= WCM_RSSI_BORDER_HIGH)
    {
        level = WM_LINK_LEVEL_3;
    }
    else if (ave >= WCM_RSSI_BORDER_MIDDLE)
    {
        level = WM_LINK_LEVEL_2;
    }
    else if (ave >= WCM_RSSI_BORDER_LOW)
    {
        level = WM_LINK_LEVEL_1;
    }

    return level;
}

/*---------------------------------------------------------------------------*
Name: WCMi_ShelterRssi

Description: Store the RSSI value of the received signal strength in the internal buffer whenever DCF data
                has been received.

Arguments: rssi    -   Represents the RSSI value for the received signal strength

Returns: None.
 *---------------------------------------------------------------------------*/
void WCMi_ShelterRssi(u8 rssi)
{
    u8  raw;

    if (rssi & 0x02)
    {
        raw = (u8) (rssi >> 2);
    }
    else
    {
        raw = (u8) (25 + (rssi >> 2));
    }

    wcmRssi[wcmRssiIndex % WCM_RSSI_COUNT_MAX] = raw;
    if (wcmRssiIndex >= WCM_RSSI_COUNT_MAX)
    {
        wcmRssiIndex = (u8) (((1 + wcmRssiIndex) % WCM_RSSI_COUNT_MAX) + WCM_RSSI_COUNT_MAX);
    }
    else
    {
        wcmRssiIndex++;
    }
}

/*---------------------------------------------------------------------------*
End of file
 *---------------------------------------------------------------------------*/
