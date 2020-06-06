/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_cache.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_cache.c,v $
  Revision 1.6  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.5  12/21/2005 14:25:56  yosizaki
  Fixed page-cache issue.

  Revision 1.4  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.3  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.2  11/11/2004 11:51:13  yosizaki
  change argument of MBi_ReadFromCache. (const_cast)
  Added member 'is_hit' to avoid endless cache-missing.

  Revision 1.1  11/10/2004 13:14:58  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include "mb_cache.h"


/*---------------------------------------------------------------------------*
  Name:         MBi_InitCache

  Description:  Initializes cache list.
                

  Arguments:    pl         cache list

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_InitCache(MBiCacheList * pl)
{
    MI_CpuClear8(pl, sizeof(*pl));
}

/*---------------------------------------------------------------------------*
  Name:         MBi_AttachCacheBuffer

  Description:  Allocates buffer to cache list.
                
  Arguments:    pl         cache list
                ptr        buffer to allocate
                src        ptr source address
                len        ptr byte size
                state      assigned initial state
                           (MB_CACHE_STATE_READY or MB_CACHE_STATE_LOCKED)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_AttachCacheBuffer(MBiCacheList * pl, u32 src, u32 len, void *ptr, u32 state)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        /* Search for unregistered page*/
        MBiCacheInfo *pi = pl->list;
        for (;; ++pi)
        {
            if (pi >= &pl->list[MB_CACHE_INFO_MAX])
            {
                OS_TPanic("MBi_AttachCacheBuffer() failed! (over maximum count)");
            }
            if (pi->state == MB_CACHE_STATE_EMPTY)
            {
                pi->src = src;
                pi->len = len;
                pi->ptr = (u8 *)ptr;
                pi->state = state;
                break;
            }
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_ReadFromCache

  Description:  Reads contents of specified address from cache.

  Arguments:    pl         cache list
                src        read source address
                len        read size (BYTE)
                dst        read destination address

  Returns:      If it hits the cache, reads and returns TRUE. Otherwise, FALSE.    
 *---------------------------------------------------------------------------*/
BOOL MBi_ReadFromCache(MBiCacheList * pl, u32 src, void *dst, u32 len)
{
    BOOL    ret = FALSE;
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        /* Search for usable pages*/
        const MBiCacheInfo *pi = pl->list;
        for (; pi < &pl->list[MB_CACHE_INFO_MAX]; ++pi)
        {
            if (pi->state >= MB_CACHE_STATE_READY)
            {
                /* Determine if target address is within range*/
                const int ofs = (int)(src - pi->src);
                if ((ofs >= 0) && (ofs + len <= pi->len))
                {
                    /* Cache hit, so read*/
                    MI_CpuCopy8(pi->ptr + ofs, dst, len);
                    pl->lifetime = 0;
                    ret = TRUE;
                    break;
                }
            }
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_TryLoadCache

  Description:  Loads contents of specified address to cache.
                READY page cache with smallest address is nullified.    

  Arguments:    pl         cache list
                src        reload source address
                len        reload size (in bytes)

  Returns:      Returns TRUE if reload started. Otherwise returns FALSE.
                (When there is a single reload processing engine in the system,
                 this function returns FALSE if previous process is unfinished.)
 *---------------------------------------------------------------------------*/
BOOL MBi_TryLoadCache(MBiCacheList * pl, u32 src, u32 len)
{
    BOOL    ret = FALSE;
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        /* Search for page that can be reloaded*/
        MBiCacheInfo *trg = NULL;
        MBiCacheInfo *pi = pl->list;
        for (; pi < &pl->list[MB_CACHE_INFO_MAX]; ++pi)
        {
            if (pi->state == MB_CACHE_STATE_READY)
            {
                /* If smallest address, leave as candidate*/
                if (!trg || (trg->src > pi->src))
                {
                    trg = pi;
                }
            }
        }
        /* Request process if page target for reload can be found*/
        if (trg)
        {
            /* The process request to the task thread is described here*/
            (void)src;
            (void)len;
        }
        OS_TPanic("reload-system is not yet!");
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
    return ret;
}
