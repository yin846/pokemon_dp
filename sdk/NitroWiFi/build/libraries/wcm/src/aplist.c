/*---------------------------------------------------------------------------*
Project: NitroWiFi - WCM - libraries
File: aplist.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: aplist.c,v $
Revision 1.4  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.3  07/19/2005 13:05:09  terui
deleted process to remove extra elements from the AP data

Revision 1.2  07/11/2005 12:03:29  terui
Revised to correct overwriting of the WMBssDesc structure's length member when the AP data list is uploaded.

Revision 1.1  07/07/2005 10:45:52  terui
new additions

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"

/*---------------------------------------------------------------------------*
constant definitions
 *---------------------------------------------------------------------------*/
#if WCM_DEBUG

// warning text template
static const char   aplistWarningText_NotInit[] = { "WCM library is not initialized yet.\n" };
static const char   aplistWarningText_NoIndexData[] = { "Could not found AP information that have index %d." };
#endif

/*---------------------------------------------------------------------------*
internal function prototype
 *---------------------------------------------------------------------------*/
static WCMApList*   WcmAllocApList(void);
static void         WcmFreeApList(WCMApList* aplist);
static WCMApList*   WcmGetOldestApList(void);
static WCMApList*   WcmGetLastApList(void);
static WCMApList*   WcmSearchApList(u8* bssid);
static WCMApList*   WcmSearchIndexedApList(u32 index);
static void         WcmAppendApList(WCMApList* aplist);

/*---------------------------------------------------------------------------*
Name: WCM_ClearApList

Description: Deletes the content of the AP data storage list, returning it to its initial state.
Clears the data storage area regardless of the lock status.

Arguments: None.

Returns: None.
 *---------------------------------------------------------------------------*/
void WCM_ClearApList(void)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();

// confirms if initialized
    if (w == NULL)
    {
        WCMi_Warning(aplistWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return;
    }

// clears as needed for the buffer for the AP data storage list
    if ((w->config.pbdbuffer != NULL) && (w->config.nbdbuffer > 0))
    {
        MI_CpuClear8(w->config.pbdbuffer, (u32) (w->config.nbdbuffer));
    }

    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
Name: WCM_CountApList

Description: Gets the number of AP data management blocks stored in the
AP data storage list.
NOTICE: Be aware that the number of blocks can change if an interrupt occurs
after the function is called if the list is not locked with the WCM_LockApList function.

Arguments: None.

Returns: s32     -   Returns the number of AP data management blocks.
If the number of blocks is not normally available, 0 is returned.
 *---------------------------------------------------------------------------*/
s32 WCM_CountApList(void)
{
    OSIntrMode          e = OS_DisableInterrupts();
    WCMWork*        w = WCMi_GetSystemWork();
    s32         count = 0;
    WCMApListHeader*    pHeader;

// confirms if initialized
    if (w == NULL)
    {
        WCMi_Warning(aplistWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return 0;
    }

// gets the number of AP data management blocks stored from the AP data storage list
    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        count = (s32) (pHeader->count);
    }

// normal end
    (void)OS_RestoreInterrupts(e);
    return count;
}

/*---------------------------------------------------------------------------*
Name: WCM_LockApList

Description: Changes the lock status of the AP data storage list.
NOTICE: When locked, the contents of the AP data storage list are not changed,
even when an AP is found via an interrupt; be aware that the found AP data is abandoned.

Arguments: lock    -   WCM_APLIST_LOCK     : Locks the list.
WCM_APLIST_UNLOCK   : Unlocks the list.
other               : Recognized as WCM_APLIST_LOCK.

Returns: s32     -   Returns the lock status prior to the call.
WCM_APLIST_UNLOCK   : Locked.
WCM_APLIST_UNLOCK   : Unlocked.
 *---------------------------------------------------------------------------*/
s32 WCM_LockApList(s32 lock)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();

// confirms if initialized
    if (w == NULL)
    {
        WCMi_Warning(aplistWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return 0;
    }

// changes the lock state in conjunction with the argument
    if (lock != WCM_APLIST_UNLOCK)
    {
        lock = (w->apListLock ? WCM_APLIST_LOCK : WCM_APLIST_UNLOCK);
        w->apListLock = WCM_APLIST_LOCK;
    }
    else
    {
        lock = (w->apListLock ? WCM_APLIST_LOCK : WCM_APLIST_UNLOCK);
        w->apListLock = WCM_APLIST_UNLOCK;
    }

// normal end
    (void)OS_RestoreInterrupts(e);
    return lock;
}

/*---------------------------------------------------------------------------*
Name: WCM_PointApList

Description: Extracts the frequency strength registered for the AP data
within the AP data storage list.
NOTICE: Be aware that it is possible the buffer contents for the indicated
pointer can be overwritten if an interrupt occurs after the function is called
if the list is not locked with the WCM_LockApList function.

Arguments: index       -   Specifies the index. It is necessary to specify an index smaller
than the number to be obtained via the WCM_CountApList function.

Returns: WMBssDesc*  -   Returns the pointer to the extracted AP data.
Returns NULL if extraction fails.
 *---------------------------------------------------------------------------*/
WMBssDesc* WCM_PointApList(s32 index)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();
    WMBssDesc*  bd = NULL;
    WCMApList*  p;

// confirms if initialized
    if (w == NULL)
    {
        WCMi_Warning(aplistWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return NULL;
    }

// searches for the AP data management block specified by the index
    p = WcmSearchIndexedApList((u32) index);

// abnormal end if not found
    if (p == NULL)
    {
        WCMi_Warning(aplistWarningText_NoIndexData, index);
        (void)OS_RestoreInterrupts(e);
        return NULL;
    }

// normal end
    bd = (WMBssDesc *) (p->data);
    (void)OS_RestoreInterrupts(e);
    return bd;
}

/*---------------------------------------------------------------------------*
Name: WCM_PointApListLinkLevel

Description: Extracts the frequency strength registered for the AP data
assigned to the specified index from within the AP data storage list.

Arguments: index       -   Specifies the index. It is necessary to specify an index smaller
than the number to be obtained via the WCM_CountApList function.

Returns: WMLinkLevel -   Returns the frequency strength for the extracted AP data at registration.
Returns WM_LINK_LEVEL_0 if extraction fails.
 *---------------------------------------------------------------------------*/
WMLinkLevel WCM_PointApListLinkLevel(s32 index)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();
    WMLinkLevel ll = WM_LINK_LEVEL_0;
    WCMApList*  p;

// confirms if initialized
    if (w == NULL)
    {
        WCMi_Warning(aplistWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WM_LINK_LEVEL_0;
    }

// searches for the AP data management block specified by the index
    p = WcmSearchIndexedApList((u32) index);

// abnormal end if not found
    if (p == NULL)
    {
        WCMi_Warning(aplistWarningText_NoIndexData, index);
        (void)OS_RestoreInterrupts(e);
        return WM_LINK_LEVEL_0;
    }

// normal end
    ll = (WMLinkLevel) (p->linkLevel);
    (void)OS_RestoreInterrupts(e);
    return ll;
}

/*---------------------------------------------------------------------------*
Name: WCMi_EntryApList

Description: Internal function called when the scan result AP is discovered.
If unlocked, the AP data storage list
is edited in conjunction with the config settings.
NOTICE: As it assumed that the function is called while interrupts are blocked,
calls from optional places are blocked.

Arguments: bssDesc     -   Pointer to the discovered AP data.
linkLevel   -   Frequency strength when the AP is discovered.

Returns: None.
 *---------------------------------------------------------------------------*/
void WCMi_EntryApList(WMBssDesc* bssDesc, u16 linkLevel)
{
    WCMWork*    w = WCMi_GetSystemWork();
    WCMApList*  aplist;

// initialization confirmation
    if (w == NULL)
    {
        return;
    }

// confirms AP list edit permissions
    if (w->apListLock != WCM_APLIST_UNLOCK)
    {
        return;
    }

// skipped if a non-AP NITRO parent unit
    if (bssDesc->gameInfoLength != 0)
    {
        return;
    }

// gets a point to the AP data storage buffer that needs editing
    {
// confirms whether the AP already stores the data
        aplist = WcmSearchApList(bssDesc->bssid);

// if the AP does not store the data
        if (aplist == NULL)
        {
// confirms the memory area to store the data to the parent unit
            aplist = WcmAllocApList();
        }

// if the buffer storing the data is full or is set for 'exchange'
        if ((aplist == NULL) && (w->config.nbdmode == WCM_APLIST_MODE_EXCHANGE))
        {
// gets the memory area where the oldest AP data is stored
            aplist = WcmGetOldestApList();
        }
    }

// edits the AD data storage buffer
    if (aplist != NULL)
    {
        aplist->linkLevel = linkLevel;

        /* should be 4-byte aligned, together with src and dst */
        MI_CpuCopyFast(bssDesc, aplist->data, WCM_APLIST_SIZE);

// moves to the list's end (i.e., the newest AP data)
        WcmAppendApList(aplist);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmAllocApList

Description: Assign a new AP data management block from the AP data storage
list area. The index for the confirmed blocks starts from 0.

Arguments: None.

Returns: WCMApList*  -   Pointer to assigned AP data management block.
Returns NULL if assignment fails.
 *---------------------------------------------------------------------------*/
static WCMApList* WcmAllocApList(void)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p = NULL;

// confirms if the AP data storage list is usable
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
// calculate how many blocks can be confirmed
        u32 maxBlock = (w->config.nbdbuffer - sizeof(WCMApListHeader)) / sizeof(WCMApList);

// can at least one block can be confirmed
        if (maxBlock > 0)
        {
            s32 i;

// is there a redundant memory block
            if (maxBlock > pHeader->count)
            {
// searches for a redundant memory block
                for (i = 0; i < maxBlock; i++)
                {
                    p = (WCMApList *) ((u32) pHeader + sizeof(WCMApListHeader) + (i * sizeof(WCMApList)));
                    if (p->state == WCM_APLIST_BLOCK_EMPTY)
                    {
                        break;
                    }
                }

                if (i < maxBlock)
                {
// changes the memory block to "in use"
                    p->state = WCM_APLIST_BLOCK_OCCUPY;
                    p->index = pHeader->count;

// adds newly confirmed blocks to the end of the list
                    p->next = NULL;
                    p->previous = pHeader->tail;
                    pHeader->tail = p;
                    if (p->previous)
                    {
                        ((WCMApList *) (p->previous))->next = p;
                    }
                    else
                    {
                        pHeader->head = p;
                    }

// increments the list number
                    pHeader->count++;
                }
            }
        }
    }

    return p;
}

/*---------------------------------------------------------------------------*
Name: WcmFreeApList

Description: Releases the block managing the AP data assigned by the AP data
data storage list area and returns it for new use.
There is no notification if for some reason normal block release
processes are not performed, as when the specified block does not exist.
NOTICE: Exists to serve as an example of list management; not used internally.

Arguments: aplist  -   Pointer to the AP data management block to be released.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmFreeApList(WCMApList* aplist)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p;

// confirms parameters
    if (aplist == NULL)
    {
        return; // abnormal parameters
    }

// confirms if the AP data storage list is usable
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
// finds an AP data management block that matches the pointer from the list
        p = pHeader->head;
        while (p)
        {
            if (p == aplist)
            {
// deletes from the list
                if (aplist->previous)
                {
                    ((WCMApList *) (aplist->previous))->next = aplist->next;
                }
                else
                {
                    pHeader->head = aplist->next;
                }

                if (aplist->next)
                {
                    ((WCMApList *) (aplist->next))->previous = aplist->previous;
                }
                else
                {
                    pHeader->tail = aplist->previous;
                }

// continues the index for the deleted block (to prevent index gaps)
                if (pHeader->count > 0)
                {
                    u32 index = aplist->index;

                    aplist = WcmSearchIndexedApList((u32) (pHeader->count - 1));
                    if (aplist != NULL)
                    {
                        aplist->index = index;
                    }

                    pHeader->count--;
                }

// releases the used block
                MI_CpuClear8(aplist, sizeof(WCMApList));
            }

            p = p->next;
        }
    }
}

/*---------------------------------------------------------------------------*
Name: WcmGetOldestApList

Description: Searches for the oldest registered AP data management block from
the AP data managed within the AP data storage list area.

Arguments: None.

Returns: WCMApList*     -   Returns a pointer to the AP data management block.
If there is no AP data registered in the list,
returns NULL.
 *---------------------------------------------------------------------------*/
static WCMApList* WcmGetOldestApList(void)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);

// confirms if the AP data storage list is usable
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
// the first AP data management block on the list is the oldest
        return pHeader->head;
    }

// abnormal end
    return NULL;
}

/*---------------------------------------------------------------------------*
Name: WcmGetLastApList

Description: Searches for the newest registered AP data management block from
the AP data managed within the AP data storage list area.
NOTICE: Exists to serve as an example of list management; not used internally.

Arguments: None.

Returns: WCMApList*     -   Returns a pointer to the AP data management block.
If there is no AP data registered in the list,
returns NULL.
 *---------------------------------------------------------------------------*/
static WCMApList* WcmGetLastApList(void)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);

// confirms if the AP data storage list is usable
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
// the last AP data management block on the list is the newest
        return pHeader->tail;
    }

// abnormal end
    return NULL;
}

/*---------------------------------------------------------------------------*
Name: WcmSearchApList

Description: Searches for the AP data management block with the specified index from
the AP data managed within the AP data storage list area.

Arguments: bssid       -   Specifies a pointer to the search condition BSSID.

Returns: WCMApList*     -   Returns a pointer to the AP data management block.
If no AP data matches the BSSID,
returns NULL.
 *---------------------------------------------------------------------------*/
static WCMApList* WcmSearchApList(u8* bssid)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p = NULL;

// confirms parameters
    if (bssid == NULL)
    {
        return NULL;    // abnormal parameters
    }

// confirms if the AP data storage list is usable
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
// searches for the AP data management block that matches the BSSID
        p = pHeader->head;
        while (p)
        {
            if (WCM_CompareBssID(((WMBssDesc *) (p->data))->bssid, bssid))
            {
                break;
            }

            p = p->next;
        }
    }

    return p;
}

/*---------------------------------------------------------------------------*
Name: WcmSearchIndexedApList

Description: Searches for the AP data management block with the specified index from
the AP data managed within the AP data storage list area.

Arguments: index       -   Specifies the index.

Returns: WCMApList*     -   Returns a pointer to the AP data management block.
If no AP data matches the index,
returns NULL.
 *---------------------------------------------------------------------------*/
static WCMApList* WcmSearchIndexedApList(u32 index)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p = NULL;

// confirms if the AP data storage list is usable
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        p = pHeader->head;
        while (p)
        {
// searches for the AP data management block that matches the index
            if (p->index == index)
            {
                break;
            }

            p = p->next;
        }
    }

    return p;
}

/*---------------------------------------------------------------------------*
Name: WcmAppendApList

Description: Re-registers the AP data management block managed within
the AP data storage list as the the newest. In this case, the block's index is
not updated. If it is not managed in the list, it is handled as a
newly added block.

Arguments: aplist  -   Specifies a pointer to the AP data management block to be re-registered.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmAppendApList(WCMApList* aplist)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p = NULL;

// confirms parameters
    if (aplist == NULL)
    {
        return; // abnormal parameters
    }

// confirms if the AP data storage list is usable
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
// confirms if already existing in the list
        p = pHeader->head;
        while (p)
        {
            if (p == aplist)
            {
// temporarily deletes from the list
                if (p->previous)
                {
                    ((WCMApList *) (p->previous))->next = p->next;
                }
                else
                {
                    pHeader->head = p->next;
                }

                if (p->next)
                {
                    ((WCMApList *) (p->next))->previous = p->previous;
                }
                else
                {
                    pHeader->tail = p->previous;
                }
                break;
            }

            p = p->next;
        }

// adds to the end of the list
        aplist->next = NULL;
        aplist->previous = pHeader->tail;
        pHeader->tail = aplist;
        if (aplist->previous)
        {
            ((WCMApList *) (aplist->previous))->next = aplist;
        }
        else
        {
            pHeader->head = aplist;
        }

// if added anew, increments the list number
        if (p == NULL)
        {
            aplist->index = pHeader->count;
            pHeader->count++;
        }
    }
}

/*---------------------------------------------------------------------------*
End of file
 *---------------------------------------------------------------------------*/
