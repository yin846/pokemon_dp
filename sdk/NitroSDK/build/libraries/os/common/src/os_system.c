/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_system.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_system.c,v $
  Revision 1.33  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.32  01/10/2006 06:07:34  okubata_ryoma
  Moved functions related to OS_Terminate to another file.

  Revision 1.31  12/08/2005 07:06:09  okubata_ryoma
  small fix

  Revision 1.30  11/28/2005 04:26:56  seiki_masashi
  small fix

  Revision 1.29  11/25/2005 08:30:20  terui
  Reformed so that SVC_WaitByLoop is called with OS_WaitVBlankIntr

  Revision 1.28  11/22/2005 09:11:28  yosizaki
  support SDK_ENABLE_ARM7_PRINT.

  Revision 1.27  10/07/2005 08:21:16  yada
  add OS_WaitVBlankIntr()

  Revision 1.26  09/09/2005 04:36:22  yada
  print "\n" before exit string

  Revision 1.25  09/01/2005 00:31:19  yada
  avoid warning at FINALROM build in OS_Exit()

  Revision 1.24  08/31/2005 12:02:32  yada
  add OS_Exit()

  Revision 1.23  2005/03/04 11:09:27  yasu
  Changed __declspec(weak) to SDK_WEAK_SYMBOL

  Revision 1.22  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.21  2005/02/18 08:51:35  yasu
  Changed year of creation.

  Revision 1.20  2005/02/18 06:22:20  yasu
  Signed/Unsigned conversion warning control

  Revision 1.19  11/24/2004 10:55:51  yada
  Let OS_Terminate() and OS_Halt() be weak symbols.

  Revision 1.18  11/24/2004 04:33:51  yada
  just fix typo

  Revision 1.17  09/02/2004 06:22:17  yada
  only change comment

  Revision 1.16  05/24/2004 04:48:29  yada
  OS_WaitIntr() -> OS_WaitInterrupt()

  Revision 1.15  2004/05/20 06:36:24  yasu
  ARM7: Use SVC_WaitByLoop in OS_SpinWait

  Revision 1.14  05/10/2004 11:22:19  yada
  add OS_WaitIntr()

  Revision 1.13  04/06/2004 11:22:42  yada
  add "bx lr" in OS_Halt() for ARM9

  Revision 1.12  04/06/2004 01:37:45  yada
  Added OS_Terminate()

  Revision 1.11  03/29/2004 02:56:32  yada
  Changed so that OS_Halt() calls SVC_Halt()

  Revision 1.10  2004/03/26 02:53:46  yasu
  apply systemcall 2 for OS_Halt at ARM7

  Revision 1.9  03/04/2004 09:47:04  yada
  IRQ prohibited OSDisableInterrupt()    
  Prepared different function for prohibiting IRQ/FIQ

  Revision 1.8  2004/02/27 01:55:00  yasu
  add OS_SpinWait, move OS_Halt into os_system.c

  Revision 1.7  2004/02/13 08:45:30  yasu
  uncomment all if ARM7

  Revision 1.6  02/10/2004 12:12:43  yada
  Move OS_GetDTCMAddress, OS_GetITCMAddress to os_tcm.c

  Revision 1.5  02/10/2004 11:21:47  yada
  (none)

  Revision 1.4  02/10/2004 11:19:56  yada
  Stop inline for interrupt function.

  Revision 1.3  02/10/2004 02:29:27  yada
  ifdef code that is only valid for ARM9

  Revision 1.2  2004/02/10 01:25:30  yasu
  include code32.h

  Revision 1.1  02/09/2004 11:36:55  yada
  Created OS_GetDTCMAddress and OS_GetITCMAddress


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/code32.h>

//============================================================================
//          CONTROL INTERRUPT
//============================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableInterrupts

  Description:  Set CPSR to enable irq interrupts

  Arguments:    None.

  Returns:      last state of HW_PSR_IRQ_DISABLE
 *---------------------------------------------------------------------------*/
asm OSIntrMode OS_EnableInterrupts( void )
{
    mrs     r0, cpsr
    bic     r1, r0, #HW_PSR_IRQ_DISABLE
    msr     cpsr_c, r1
    and     r0, r0, #HW_PSR_IRQ_DISABLE

    bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         OS_DisableInterrupts

  Description:  Set CPSR to disable irq interrupts

  Arguments:    None.

  Returns:      last state of HW_PSR_IRQ_DISABLE
 *---------------------------------------------------------------------------*/
asm OSIntrMode OS_DisableInterrupts( void )
{
    mrs     r0, cpsr
    orr     r1, r0, #HW_PSR_IRQ_DISABLE
    msr     cpsr_c, r1
    and     r0, r0, #HW_PSR_IRQ_DISABLE

    bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         OS_RestoreInterrupts

  Description:  Restore CPSR irq interrupt

  Arguments:    state of irq interrupt bit

  Returns:      last state of HW_PSR_IRQ_DISABLE
 *---------------------------------------------------------------------------*/
asm OSIntrMode OS_RestoreInterrupts( register OSIntrMode state )
{
    mrs     r1, cpsr
    bic     r2, r1, #HW_PSR_IRQ_DISABLE
    orr     r2, r2, r0
    msr     cpsr_c, r2
    and     r0, r1, #HW_PSR_IRQ_DISABLE

    bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         OS_EnableInterrupts_IrqAndFiq

  Description:  Set CPSR to enable irq and fiq interrupts

  Arguments:    None.

  Returns:      last state of HW_PSR_IRQ_DISABLE & HW_PSR_FIQ_DISABLE
 *---------------------------------------------------------------------------*/
asm OSIntrMode OS_EnableInterrupts_IrqAndFiq( void )
{
    mrs     r0, cpsr
    bic     r1, r0, #HW_PSR_IRQ_FIQ_DISABLE
    msr     cpsr_c, r1
    and     r0, r0, #HW_PSR_IRQ_FIQ_DISABLE

    bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         OS_DisableInterrupts_IrqAndFiq

  Description:  Set CPSR to disable irq and fiq interrupts

  Arguments:    None.

  Returns:      last state of HW_PSR_IRQ_DISABLE & HW_PSR_FIQ_DISABLE
 *---------------------------------------------------------------------------*/
asm OSIntrMode OS_DisableInterrupts_IrqAndFiq( void )
{
    mrs     r0, cpsr
    orr     r1, r0, #HW_PSR_IRQ_FIQ_DISABLE
    msr     cpsr_c, r1
    and     r0, r0, #HW_PSR_IRQ_FIQ_DISABLE

    bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         OS_RestoreInterrupts_IrqAndFiq

  Description:  Restore CPSR irq and fiq interrupt

  Arguments:    state of irq and fiq interrupt bit

  Returns:      last state of HW_PSR_IRQ_DISABLE & HW_PSR_FIQ_DISABLE
 *---------------------------------------------------------------------------*/
asm OSIntrMode OS_RestoreInterrupts_IrqAndFiq( register OSIntrMode state )
{
    mrs     r1, cpsr
    bic     r2, r1, #HW_PSR_IRQ_FIQ_DISABLE
    orr     r2, r2, r0
    msr     cpsr_c, r2
    and     r0, r1, #HW_PSR_IRQ_FIQ_DISABLE

    bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         OS_GetCpsrIrq

  Description:  Get current state of CPSR irq interrupt

  Arguments:    None

  Returns:      last state of HW_PSR_IRQ_DISABLE
 *---------------------------------------------------------------------------*/
asm OSIntrMode_Irq OS_GetCpsrIrq( void )
{
    mrs     r0, cpsr
    and     r0, r0, #HW_PSR_IRQ_DISABLE

    bx      lr
}


//============================================================================
//          PROCESSER MODE
//============================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_GetProcMode

  Description:  Get processor mode from CPSR

  Arguments:    None.

  Returns:      CPU processor mode (field 0x10-0x1f)
 *---------------------------------------------------------------------------*/
asm OSProcMode OS_GetProcMode( void )
{
    mrs     r0, cpsr
    and     r0, r0, #HW_PSR_CPU_MODE_MASK

    bx      lr
}

#include <nitro/codereset.h>


//============================================================================
//          WAIT
//============================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SpinWait

  Description:  Spin and Wait for specified CPU cycles at least

  Arguments:    cycles      waiting CPU cycle

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef  SDK_ARM9
#include <nitro/code32.h>
asm void  OS_SpinWait( u32 cycle )
{
_1: subs   r0, r0, #4   // 3 cycle
    bcs    _1           // 1 cycle
    bx     lr
}
#include <nitro/codereset.h>
#else
void OS_SpinWait(u32 cycle)
{
    SVC_WaitByLoop((s32)cycle / 4);
}
#endif


/*---------------------------------------------------------------------------*
  Name:         OS_WaitInterrupt

  Description:  wait specifiled interrupt.
                the difference between OS_WaitIrq and OS_WaitInterrupt is:
                in waiting interrupt
                OS_WaitIrq does switch thread,
                OS_WaitInterrupt doesn't switch thread.
                OS_WaitIrq wait by using OS_SleepThread() with threadQueue,
                OS_WaitInterrupt wait by using OS_Halt().
                if SDK_NO_THREAD defined, 2 functions become same.

  Arguments:    clear      TRUE if want to clear interrupt flag before wait.
                           FALSE if not.
                irqFlags    bit of interrupts to wait for

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_WaitInterrupt(BOOL clear, OSIrqMask irqFlags)
{
    OSIntrMode cpsrIrq = OS_DisableInterrupts();
    BOOL    ime = OS_EnableIrq();

    if (clear)
    {
        (void)OS_ClearIrqCheckFlag(irqFlags);
    }

    while (!(OS_GetIrqCheckFlag() & irqFlags))
    {
        OS_Halt();
        (void)OS_EnableInterrupts();
        (void)OS_DisableInterrupts();
    }

    (void)OS_ClearIrqCheckFlag(irqFlags);
    (void)OS_RestoreIrq(ime);
    (void)OS_RestoreInterrupts(cpsrIrq);
}

/*---------------------------------------------------------------------------*
  Name:         OS_WaitVBlankIntr

  Description:  wait till vblank interrupt occurred.
				the difference between SVC_WaitVBlankIntr and OS_WaitVBlankIntr is:
                OS_WaitIrq switches thread,
                OS_WaitInterrupt doesn't switch threads.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_WaitVBlankIntr(void)
{
    SVC_WaitByLoop(1);
#if defined(SDK_ENABLE_ARM7_PRINT) && defined(SDK_ARM9)
    // PrintServer for ARM7 (if specified)
    OS_PrintServer();
#endif
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
}
