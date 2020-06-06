/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_cache.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_cache.c,v $
  Revision 1.22  10/14/2005 06:37:11 AM  yada
  let some arguments of functions be const

  Revision 1.21  06/14/2005 08:34:13 AM  yosizaki
  fix flush-operations. (change to better solution)

  Revision 1.20  04/19/2005 10:30:47 AM  yosizaki
  fix around DC_FlushAll().
  optimize DC_FlushRange().

  Revision 1.19  04/05/2005 03:12:51 AM  yosizaki
  enable SDK_AVOID_FLUSH.

  Revision 1.18  02/15/2005 09:42:55 AM  yosizaki
  disable SDK_AVOID_FLUSH.

  Revision 1.17  02/10/2005 09:39:47 AM  yosizaki
  avoid flush-cache command.

  Revision 1.16  2005/01/24 00:23:20  takano_makoto
  fix Copyright header.

  Revision 1.15  01/21/2005 10:39:50 AM  takano_makoto
  * Added _Unlockdown, and corrected problem where Lockdown would not lock things properly.
  Revision 1.14  04/07/2004 05:45:47 AM  yada
  add explanation for function

  Revision 1.13  02/24/2004 12:16:46 PM  yada
  Implemented rounding for the cache line.

  Revision 1.12  02/09/2004 11:17:05 AM  yasu
  include code32.h

  Revision 1.11  02/09/2004 06:27:44 AM  yada
  Corrected problem where status up to DC_Disable, DC_Restore was not being returned correctly.

  Revision 1.10  02/05/2004 07:09:02 AM  yasu
  change SDK prefix iris -> nitro

  Revision 1.9  01/18/2004 01:51:52 AM  yada
  Only took care of indentations, etc.

  Revision 1.8  12/22/2003 01:37:00 AM  yada
  Added pre-load

  Revision 1.7  12/18/2003 02:01:10 AM  yada
  Revised code so that functions which return BOOL values return 0 or 1.

  Revision 1.6  12/18/2003 01:57:07 AM  yada
  Made corrections.

  Revision 1.5  12/18/2003 01:54:09 AM  yada
  Added instruction cache operations and write buffer empty wait.

  Revision 1.4  12/17/2003 11:32:06 AM  yada
  Coded data cache functions.

  Revision 1.3  12/17/2003 09:41:19  nishida_kenji
  suppress warnings

  Revision 1.2  12/17/2003 09:39:41 AM  nishida_kenji
  small bug fix

  Revision 1.1  12/17/2003 09:33:33 AM  yada
  Initial release (temporary release)


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/code32.h>


/* ON when temporarily avoiding the Flush command*/
#define SDK_AVOID_FLUSH


//===========================================================================
//              DATA CACHE CONTROL
//===========================================================================
/*---------------------------------------------------------------------------*
  Name:         DC_Enable

  Description:  enable data cache

  Arguments:    None

  Returns:      previous state
 *---------------------------------------------------------------------------*/
asm BOOL DC_Enable()
{
    mrc         p15, 0, r1, c1, c0, 0
    and         r0, r1, #HW_C1_DCACHE_ENABLE
    mov         r0, r0, LSR #HW_C1_DCACHE_ENABLE_SHIFT
    orr         r1, r1, #HW_C1_DCACHE_ENABLE
    mcr         p15, 0, r1, c1, c0, 0
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         DC_Disable

  Description:  disable data cache

  Arguments:    None

  Returns:      previous stats
 *---------------------------------------------------------------------------*/
asm BOOL DC_Disable()
{
    mrc         p15, 0, r1, c1, c0, 0
    and         r0, r1, #HW_C1_DCACHE_ENABLE
    mov         r0, r0, LSR #HW_C1_DCACHE_ENABLE_SHIFT
    bic         r1, r1, #HW_C1_DCACHE_ENABLE
    mcr         p15, 0, r1, c1, c0, 0
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         DC_Restore

  Description:  set state of data cache

  Arguments:    data cache state to be set

  Returns:      previous state
 *---------------------------------------------------------------------------*/
asm BOOL DC_Restore( register BOOL enable )
{
    //---- Argument process
    cmp         r0, #0
    moveq       r2, #0
    movne       r2, #HW_C1_DCACHE_ENABLE

    mrc         p15, 0, r1, c1, c0, 0
    and         r0, r1, #HW_C1_DCACHE_ENABLE
    mov         r0, r0, LSR #HW_C1_DCACHE_ENABLE_SHIFT
    bic         r1, r1, #HW_C1_DCACHE_ENABLE
    orr         r1, r1, r2
    mcr         p15, 0, r1, c1, c0, 0
    bx          lr
}

//===========================================================================
//              DATA CACHE (for all range)
//===========================================================================
/*---------------------------------------------------------------------------*
  Name:         DC_InvalidateAll

  Description:  invalidate all data cache

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_InvalidateAll( void )
{
    mov         r0, #0
    mcr         p15, 0, r0, c7, c6, 0
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         DC_StoreAll

  Description:  clean all data cache
                (write cache data to memory)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_StoreAll( void )
{
    mov         r1, #0
@1:
    mov         r0, #0
@2:
    orr         r2, r1, r0
    mcr         p15, 0, r2, c7, c10, 2
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, #HW_DCACHE_SIZE/4
    blt         @2

    add         r1, r1, #1<<HW_C7_CACHE_SET_NO_SHIFT
    cmp         r1, #0
    bne         @1

    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         DC_FlushAll

  Description:  clean and invalidate all data cache
                (write cache data to memory, and invalidate cache)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_FlushAll( void )
{
    mov         r12, #0
    mov         r1, #0          // r1: Set No counter (0-3)

@1:
    mov         r0, #0          // r0: Line counter (0-DCACHE_SIZE/4)

@2:
    orr         r2, r1, r0
    mcr         p15, 0, r12, c7, c10, 4 /* wait write buffer empty*/
    mcr         p15, 0, r2, c7, c14, 2  /* flush*/
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, #HW_DCACHE_SIZE/4
    blt         @2

    add         r1, r1, #1<<HW_C7_CACHE_SET_NO_SHIFT
    cmp         r1, #0
    bne         @1

    bx          lr
}

//===========================================================================
//              DATA CACHE (for specified range)
//===========================================================================
/*---------------------------------------------------------------------------*
  Name:         DC_InvalidateRange

  Description:  invalidate data cache in specified range

  Arguments:    startAddr     start address
                nBytes        size (in byte)

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_InvalidateRange( register void* startAddr, register u32 nBytes )
{
    add         r1, r1, r0
    bic         r0, r0, #HW_CACHE_LINE_SIZE - 1

@1:
    mcr         p15, 0, r0, c7, c6, 1
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, r1
    blt         @1
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         DC_StoreRange

  Description:  clean data cache in specified range
                (write cache data to memory)

  Arguments:    startAddr     start address
                nBytes        size (in byte)

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_StoreRange( register const void* startAddr, register u32 nBytes )
{
    add         r1, r1, r0
    bic         r0, r0, #HW_CACHE_LINE_SIZE - 1

@1:
    mcr         p15, 0, r0, c7, c10, 1
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, r1
    blt         @1
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         DC_FlushRange

  Description:  clean and invalidate data cache in specified range
                (write cache data to memory, and invalidate cache)

  Arguments:    startAddr     start address
                nBytes        size (in byte)

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_FlushRange( register const void* startAddr, register u32 nBytes )
{
    mov         r12, #0
    add         r1, r1, r0
    bic         r0, r0, #HW_CACHE_LINE_SIZE - 1
@1:
    mcr         p15, 0, r12, c7, c10, 4 /* wait write buffer empty*/
    mcr         p15, 0, r0, c7, c14, 1 /* flush*/
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, r1
    blt         @1

    bx          lr
}

//===========================================================================
//              DATA CACHE (for specified range)
//===========================================================================
/*---------------------------------------------------------------------------*
  Name:         DC_TouchRange

  Description:  include specified area to data cache in advance

  Arguments:    startAddr     start address
                nBytes        size (in byte)

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_TouchRange( register const void* startAddr, register u32 nBytes )
{
    add         r1, r1, r0
    bic         r0, r0, #HW_CACHE_LINE_SIZE - 1

@1:
    pld         [r0]
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, r1
    blt         @1
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         DC_LockdownRange

  Description:  lock specified area to prevent not to release data cache

  Arguments:    startAddr     start address
                nBytes        size (in byte)

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_LockdownRange( register const void* startAddr, register u32 nBytes )
{
    add         r1, r1, r0                              // r1: End address
    bic         r0, r0, #HW_CACHE_LINE_SIZE - 1
    mrc         p15, 0, r3, c9, c0, 0
    and         r3, r3, #HW_C9_LOCKDOWN_SET_NO_MASK     // r3: Current set No
    cmp         r3, #3                                  // Error if 3 sets are locked down
    mvneq       r0, #0
    bxeq        lr
    
    stmfd       sp!, { lr, r0, r1 }
    ldr         r0,  =OS_DisableInterrupts
    blx         r0
    mov         r2, r0
    ldmfd       sp!, { lr, r0, r1 }
    
    orr         r3, r3, #HW_C9_LOCKDOWN_LOAD_MODE       // Cache lockdown-Load Mode
    mcr         p15, 0, r3, c9, c0, 0
    
@111:
    mcr         p15, 0, r0, c7, c14, 1                  // Clean/disable data in cache once
    ldr         r12, [r0]                               // Read data and place in cache
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, r1
    blt         @111
    add         r3, r3, #1                              // Cache normal mode & Set No increment
    bic         r0, r3, #HW_C9_LOCKDOWN_LOAD_MODE
    mcr         p15, 0, r3, c9, c0, 0
    
    stmfd       sp!, { lr }
    mov         r0, r2
    ldr         r1, =OS_RestoreInterrupts
    blx         r1
    ldmfd       sp!, { lr }
    
    bx          lr
}


/*---------------------------------------------------------------------------*
  Name:         DC_UnlockdownAll

  Description:  unlock all data cache to enable to release

  Arguments:    none.

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_UnlockdownAll( void )
{
    mov     r3, #0
    mcr     p15, 0, r3, c9, c0, 0
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         DC_Unlockdown

  Description:  unlock any data cache to enable to release

  Arguments:    num  - specify number of datablock to unlock.

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_Unlockdown( register u32 num )
{
    mrc     p15, 0, r3, c9, c0, 0
    and     r3, r3, #HW_C9_LOCKDOWN_SET_NO_MASK
    subs    r3, r3, r0
    movmi   r3, #0
    mcr     p15, 0, r3, c9, c0, 0
    bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         DC_WaitWriteBufferEmpty

  Description:  wait till write buffer becomes to be empty

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void DC_WaitWriteBufferEmpty()
{
    mov         r0, #0
    mcr         p15, 0, r0, c7, c10, 4
    bx          lr
}


//===========================================================================
//              INSTRUCTION CACHE CONTROL
//===========================================================================
/*---------------------------------------------------------------------------*
  Name:         IC_Enable

  Description:  enable instruction cache

  Arguments:    None

  Returns:      previous state
 *---------------------------------------------------------------------------*/
asm BOOL IC_Enable()
{
    mrc         p15, 0, r1, c1, c0, 0
    and         r0, r1, #HW_C1_ICACHE_ENABLE
    mov         r0, r0, LSR #HW_C1_ICACHE_ENABLE_SHIFT
    orr         r1, r1, #HW_C1_ICACHE_ENABLE
    mcr         p15, 0, r1, c1, c0, 0
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         IC_Disable

  Description:  disable instruction cache

  Arguments:    None

  Returns:      previous stats
 *---------------------------------------------------------------------------*/
asm BOOL IC_Disable()
{
   mrc          p15, 0, r1, c1, c0, 0
   and          r0, r1, #HW_C1_ICACHE_ENABLE
   mov          r0, r0, LSR #HW_C1_ICACHE_ENABLE_SHIFT
   bic          r1, r1, #HW_C1_ICACHE_ENABLE
   mcr          p15, 0, r1, c1, c0, 0
   bx           lr
}

/*---------------------------------------------------------------------------*
  Name:         IC_Restore

  Description:  set state of instruction cache

  Arguments:    instruction cache state to be set

  Returns:      previous stats
 *---------------------------------------------------------------------------*/
asm BOOL IC_Restore( register BOOL enable )
{
    //---- Argument process
    cmp         r0, #0
    moveq       r2, #0
    movne       r2, #HW_C1_ICACHE_ENABLE
    
    mrc         p15, 0, r1, c1, c0, 0
    and         r0, r1, #HW_C1_ICACHE_ENABLE
    mov         r0, r0, LSR #HW_C1_ICACHE_ENABLE_SHIFT
    bic         r1, r1, #HW_C1_ICACHE_ENABLE
    orr         r1, r1, r2
    mcr         p15, 0, r1, c1, c0, 0
    bx          lr
}

//===========================================================================
//              INSTRUCTION CACHE
//===========================================================================
/*---------------------------------------------------------------------------*
  Name:         IC_InvalidateAll

  Description:  invalidate all instruction cache

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void IC_InvalidateAll( void )
{
    mov         r0, #0
    mcr         p15, 0, r0, c7, c5, 0
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         IC_InvalidateRange

  Description:  invalidate instruction cache in specified range

  Arguments:    startAddr     start address
                nBytes        size (in byte)

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void IC_InvalidateRange( register void* startAddr, register u32 nBytes )
{
    add         r1, r1, r0
    bic         r0, r0, #HW_CACHE_LINE_SIZE - 1

@1:
    mcr         p15, 0, r0, c7, c5, 1
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, r1
    blt         @1
    bx          lr
}

/*---------------------------------------------------------------------------*
  Name:         IC_PrefetchRange

  Description:  include specified area to instruction cache in advance

  Arguments:    startAddr     start address
                nBytes        size (in byte)

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void IC_PrefetchRange( register const void* startAddr, register u32 nBytes )
{
    add         r1, r1, r0
    bic         r0, r0, #HW_CACHE_LINE_SIZE - 1

@1:
    mcr         p15, 0, r0, c7, c13, 1
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, r1
    blt         @1
    bx          lr
}


#include    <nitro/itcm_begin.h>

/*---------------------------------------------------------------------------*
  Name:         IC_LockdownRange

  Description:  lock specified area to prevent not to release instruction cache

  Arguments:    startAddr     start address
                nBytes        size (in byte)

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void IC_LockdownRange( register const void* startAddr, register u32 nBytes )
{
    add         r1, r1, r0                              // r1: End address
    bic         r0, r0, #HW_CACHE_LINE_SIZE - 1
    mrc         p15, 0, r3, c9, c0, 1
    and         r3, r3, #HW_C9_LOCKDOWN_SET_NO_MASK     // r3: Current set No
    cmp         r3, #3                                  // Error if 3 sets are locked down
    mvneq       r0, #0
    bxeq        lr

    stmfd       sp!, { lr, r0, r1 }
    ldr         r0,  =OS_DisableInterrupts
    blx         r0
    mov         r2, r0
    ldmfd       sp!, { lr, r0, r1 }

    orr         r3, r3, #HW_C9_LOCKDOWN_LOAD_MODE       // Cache lockdown-Load Mode
    mcr         p15, 0, r3, c9, c0, 1

@1:
    mcr         p15, 0, r0, c7, c5, 1                   // Disable from cache once
    mcr         p15, 0, r0, c7, c13, 1                  // Prefetch
    add         r0, r0, #HW_CACHE_LINE_SIZE
    cmp         r0, r1
    blt         @1
    add         r3, r3, #1                              // Cache normal mode & Set No increment
    bic         r0, r3, #HW_C9_LOCKDOWN_LOAD_MODE
    mcr         p15, 0, r3, c9, c0, 1
    
    stmfd       sp!, { lr }
    mov         r0, r2
    ldr         r1, =OS_RestoreInterrupts
    blx         r1
    ldmfd       sp!, { lr }

    bx          lr
}

#include    <nitro/itcm_end.h>


/*---------------------------------------------------------------------------*
  Name:         IC_UnlockdownAll

  Description:  unlock all instruction cache to enable to release

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void IC_UnlockdownAll( void )
{
    mov     r3, #0
    mcr     p15, 0, r3, c9, c0, 1
    bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         IC_Unlockdown

  Description:  unlock any instruction cache to enable to release

  Arguments:    num  - specify number of datablock to unlock.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void IC_Unlockdown( register u32 num )
{
    mrc     p15, 0, r3, c9, c0, 1
    and     r3, r3, #HW_C9_LOCKDOWN_SET_NO_MASK
    subs    r3, r3, r0
    movmi   r3, #0
    mcr     p15, 0, r3, c9, c0, 1
    bx      lr
}


#include <nitro/codereset.h>
