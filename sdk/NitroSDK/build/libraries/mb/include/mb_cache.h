/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_cache.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_cache.h,v $
  Revision 1.6  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  12/21/2005 14:25:56  yosizaki
  Fixed page-cache issue.

  Revision 1.4  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.3  12/07/2004 10:20:30  yosizaki
  fix to reload cache from correct archive. (not only "rom")

  Revision 1.2  11/11/2004 11:53:32  yosizaki
  change argument of MBi_ReadFromCache. (const_cast)
  add some members in MBiCacheList.

  Revision 1.1  11/10/2004 13:13:42  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#if	!defined(NITRO_MB_CACHE_H_)
#define NITRO_MB_CACHE_H_


#if	defined(__cplusplus)
extern  "C"
{
#endif


/* constant ----------------------------------------------------------------*/


#define	MB_CACHE_INFO_MAX	4

#define MB_CACHE_STATE_EMPTY   0
#define MB_CACHE_STATE_BUSY    1
#define MB_CACHE_STATE_READY   2
#define MB_CACHE_STATE_LOCKED  3


/* struct ------------------------------------------------------------------*/


/*
 * Structure for virtual memory by paging.
 * With this structure array, cache manage large capacity address space (CARD, etc.).
 */
    typedef struct
    {
        u32     src;                   /* logical source address*/
        u32     len;                   /* cache length*/
        u8     *ptr;                   /* pointer to cache buffer*/
        u32     state;                 /* if 1, ready to use*/
    }
    MBiCacheInfo;

    typedef struct
    {
        u32     lifetime;              /* if hit or timeout, set 0.*/
        u32     recent;                /* unused*/
        MBiCacheInfo *p_list;          /* unused*/
        u32     size;                  /* unused*/
        char    arc_name[FS_ARCHIVE_NAME_LEN_MAX + 1];  /* target archive*/
        u32     arc_name_len;          /* archive name length*/
        u8      reserved[32 - FS_ARCHIVE_NAME_LEN_MAX - 1 - sizeof(u32)];
        MBiCacheInfo list[MB_CACHE_INFO_MAX];
    }
    MBiCacheList;


/* function ----------------------------------------------------------------*/



/*---------------------------------------------------------------------------*
  Name:         MBi_InitCache

  Description:  Initializes cache list.
                

  Arguments:    pl         cache list

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_InitCache(MBiCacheList * pl);

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
    void    MBi_AttachCacheBuffer(MBiCacheList * pl, u32 src, u32 len, void *ptr, u32 state);

/*---------------------------------------------------------------------------*
  Name:         MBi_ReadFromCache

  Description:  Reads contents of specified address from cache.

  Arguments:    pl         cache list
                src        read source address
                len        read size (BYTE)
                dst        read destination address

  Returns:      If it hits the cache, reads and returns TRUE. Otherwise, FALSE.    
 *---------------------------------------------------------------------------*/
    BOOL    MBi_ReadFromCache(MBiCacheList * pl, u32 src, void *dst, u32 len);

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
    BOOL    MBi_TryLoadCache(MBiCacheList * pl, u32 src, u32 len);



#if	defined(__cplusplus)
}
#endif


#endif                                 /* NITRO_MB_CACHE_H_*/
