/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_init.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_init.c,v $
  Revision 1.50  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.49  11/22/2005 09:11:28  yosizaki
  support SDK_ENABLE_ARM7_PRINT.

  Revision 1.48  07/19/2005 06:06:11  yada
  add irq stack check

  Revision 1.47  04/26/2005 06:54:22  terui
  Fix comment

  Revision 1.46  04/18/2005 12:26:59  terui
  Deleted OSi_CheckOwnerInfo function

  Revision 1.45  04/12/2005 10:49:15  terui
  Made alterations to support Chinese language

  Revision 1.44  02/28/2005 05:26:28  yosizaki
  do-indent.

  Revision 1.43  02/10/2005 07:46:57  yada
  let call MI_Init()

  Revision 1.42  02/09/2005 09:17:48  terui
  Added feature to initialize VRAM exclusive control at time of initialization in ARM9.

  Revision 1.41  10/12/2004 08:01:04  yada
  delete OS_InitPrintServer()

  Revision 1.40  10/08/2004 07:49:13  yada
  consider for the situation of not calling OS_InitPrintServer()

  Revision 1.39  09/29/2004 06:06:42  yada
  OS_InitReset() is not called if SDK_SMALL_BUILD defined

  Revision 1.38  09/29/2004 06:02:34  yada
  CARD_Init() is not called if SDK_SMALL_BUILD defined

  Revision 1.37  09/28/2004 00:03:36  yada
  add CARD_Init() in OS_Init() on ARM9

  Revision 1.36  09/15/2004 06:15:20  yada
  in SDK_TEG release, PM_Init() not called.

  Revision 1.35  09/15/2004 01:01:08  yada
  in case SDK_TEG, CTRDG_Init() not called

  Revision 1.34  09/13/2004 10:48:24  yada
  add CTRDG_Init()

  Revision 1.33  09/02/2004 06:18:22  yada
  only change comment

  Revision 1.32  09/01/2004 04:20:03  yada
  add OS_InitReset()

  Revision 1.31  08/26/2004 12:17:23  yada
  always call PM_Init()

  Revision 1.30  08/16/2004 04:22:04  yada
  in ensata, do not call PM_Init()

  Revision 1.29  08/09/2004 02:12:10  yada
  add PM_Init in case of ARM9

  Revision 1.28  07/27/2004 06:39:55  yada
  add checker for system shared validity

  Revision 1.27  2004/07/10 04:07:46  yasu
  Add short comment

  Revision 1.26  07/08/2004 02:30:35  yada
  add calling OS_InitArenaEx()

  Revision 1.25  05/24/2004 04:50:08  yada
  fix profile pragma

  Revision 1.24  05/20/2004 02:56:46  yada
  add profile off

  Revision 1.23  05/20/2004 02:51:26  yada
  add OS_InitThread()

  Revision 1.22  03/18/2004 12:59:15  yada
  Changed so that Exception initialization is not called for ARM7 (provisional)

  Revision 1.21  03/09/2004 07:26:18  yada
  Moved the location for calling PXI_Init() to above OS_InitLock().

  Revision 1.20  2004/03/08 01:55:49  yasu
  add PXI_Init()

  Revision 1.19  03/01/2004 11:57:31  yada
  Creation of function comments in English.

  Revision 1.18  02/25/2004 11:31:18  yada
  Changes related to switch from systemClock to Tick

  Revision 1.17  02/24/2004 11:50:34  yada
  Removed PAD_InitXYButton() from OS_Init()'s A7 side

  Revision 1.16  02/24/2004 04:50:59  yada
  Put OS_InitValarm in the A9 side

  Revision 1.15  02/20/2004 12:24:21  yada
  A7 OS_Init(): Remove comment out

  Revision 1.14  2004/02/20 08:29:18  yasu
  Temporary fix

  Revision 1.13  02/19/2004 08:42:11  yada
  Added arena initialization in A7 side

  Revision 1.12  02/18/2004 08:56:56  yada
  Made so cannot call OS_InitArea() from A7

  Revision 1.11  02/18/2004 01:20:04  yada
  Put systemClock,Alarm,XYButton init in ARM7

  Revision 1.10  02/13/2004 02:29:59  yada
  Thought of ways to divide ARM9 and ARM7 processing.

  Revision 1.9  02/05/2004 10:51:53  yada
  Made to call OS_InitException()

  Revision 1.8  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.7  01/30/2004 11:45:06  yada
  Temporarily removed OS_InitException()

  Revision 1.6  01/30/2004 08:06:14  yada
  Made so that OS_Init() calls exception display system initialization

  Revision 1.5  01/18/2004 02:25:37  yada
  Reformed indents, etc.

  Revision 1.4  01/07/2004 01:43:51  yada
  Change to call OS_InitArena()

  Revision 1.3  12/26/2003 05:52:45  yada
  Changes as per unified type rules

  Revision 1.2  12/24/2003 07:54:20  yada
  Made to initialize wram

  Revision 1.1  12/22/2003 01:35:52  yada
  made init.h in accordance with OS_Init() installation    


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/mi/wram.h>
#include <nitro/pxi.h>
#include <nitro/spi.h>

#ifndef SDK_TEG
#include <nitro/ctrdg.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_Init

  Description:  initialize sdk os

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OS_Init(void)
{
#ifdef SDK_ARM9
    //---- system shared area check
    SDK_ASSERT((u32)&(OS_GetSystemWork()->command_area) == HW_CMD_AREA);

    //----------------------------------------------------------------
    // for ARM9

#ifdef SDK_ENABLE_ARM7_PRINT
    // Init PrintServer for ARM7 (if specified)
    OS_InitPrintServer();
#endif

    //---- Init Arena (arenas except SUBPRIV-WRAM)
    OS_InitArena();

    //---- Init interProcessor I/F
    //  Sync with ARM7 to enable OS_GetConsoleType()
    //  PXI_Init() must be called before OS_InitArenaEx()
    PXI_Init();

    //---- Init Spinlock
    OS_InitLock();

    //---- Init Arena (extended main)
    OS_InitArenaEx();

    //---- Init IRQ Table
    OS_InitIrqTable();

    //---- Init IRQ Stack checker
    OS_SetIrqStackChecker();

    //---- Init Exception System
    OS_InitException();

    //---- Init MI (Wram bank and DMA0 arranged)
    MI_Init();

    //---- Init VCountAlarm
    OS_InitVAlarm();

    //---- Init VRAM exclusive System
    OSi_InitVramExclusive();

    //---- Init Thread System
#ifndef SDK_NO_THREAD
    OS_InitThread();
#endif
    //---- Init Reset System
#ifndef SDK_SMALL_BUILD
    OS_InitReset();
#endif

    //---- Init Cartridge
#ifndef SDK_TEG
    CTRDG_Init();
#endif

    //---- Init Card
#ifndef SDK_SMALL_BUILD
    CARD_Init();
#endif

    //---- Init Power Manager
#ifndef SDK_TEG
    PM_Init();
#endif

#else  // SDK_ARM9
    //----------------------------------------------------------------
    // for ARM7

    //---- Init Arena (SUBPRIV-WRAM arena)
    OS_InitArena();

    //---- Init interProcessor I/F
    PXI_Init();

    //---- Init Spinlock
    OS_InitLock();

    //---- Init IRQ Table
    OS_InitIrqTable();

#define SDK_EXCEPTION_BUG
#ifndef SDK_EXCEPTION_BUG
    //---- Init Exception System
    OS_InitException();
#endif

    //---- Init Tick
    OS_InitTick();

    //---- Init Alarm System
    OS_InitAlarm();

    //---- Init Thread System
    OS_InitThread();

    //---- Init Reset System
#ifndef SDK_SMALL_BUILD
    OS_InitReset();
#endif

    //---- Init Cartridge
#ifndef SDK_TEG
    CTRDG_Init();
#endif

#endif // SDK_ARM9
}

#pragma profile reset
