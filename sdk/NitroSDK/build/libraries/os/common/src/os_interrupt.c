/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - OS
  File:     os_interrupt.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_interrupt.c,v $
  Revision 1.32  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.31  10/04/2005 01:39:31  yada
  modified functions for stack overflow check

  Revision 1.30  09/20/2005 00:49:41  yada
  suppress OS_Printf() in OS_GetIrqStackStatus(). sorry.

  Revision 1.29  07/19/2005 06:00:04  yada
  add irq stack check

  Revision 1.28  02/28/2005 05:26:28  yosizaki
  do-indent.

  Revision 1.27  01/12/2005 11:11:30  takano_makoto
  fix copyright header.

  Revision 1.26  01/12/2005 01:05:08  yada
  add about SDK_TCM_APPLY

  Revision 1.25  09/02/2004 06:24:25  yada
  only change comment

  Revision 1.24  08/16/2004 04:18:58  yada
  in ARM9, no hook in VBlank interrupt

  Revision 1.23  08/10/2004 12:14:22  yada
  delete vblank counter variable, it's moved to shared area

  Revision 1.22  08/10/2004 08:01:26  yada
  add OS_GetVBlankCount(). make vblank hook

  Revision 1.21  07/10/2004 07:53:51  yada
  delete unused array

  Revision 1.20  2004/07/10 04:07:08  yasu
  Delete unused code

  Revision 1.19  05/25/2004 10:44:04  yada
  disable IME before touch IE or IF

  Revision 1.18  04/30/2004 02:19:55  yada
  add OSi_IrqThreadQueue

  Revision 1.17  03/17/2004 06:35:04  yada
  After escaping Timer callback it is not certain whether IRQ has stopped,
  but made it so the state does not change.

  Revision 1.16  03/02/2004 11:05:05  yada
  Put void to function call that does not have return value.

  Revision 1.15  03/02/2004 09:56:03  yada
  Changed the handling of the enable flag when the TIMER interrupt's handler ends
  so it is not touched.

  Revision 1.14  03/01/2004 11:57:14  yada
  Creation of function comments in English.

  Revision 1.13  02/13/2004 01:28:58  yada
  Sorting include files accordingly for ARM9 and AMR7

  Revision 1.12  02/12/2004 11:08:30  yada
  Worked on distribution of processing for ARM9/ARM7.

  Revision 1.11  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.10  02/03/2004 11:20:36  yada
  Added OSi_EnterTimerCallback()

  Revision 1.9  02/02/2004 05:33:03  yada
  Take into consideration irq system callback

  Revision 1.8  01/18/2004 02:25:37  yada
  Reformed indents, etc.

  Revision 1.7  01/17/2004 11:44:20  yada
  Created Async mode for DMA transfers

  Revision 1.6  12/25/2003 07:31:26  yada
  Changes as per unified type rules

  Revision 1.5  12/12/2003 01:27:14  yada
  Corrected due to the name change of include (os/system.h and hw/armArch.h)

  Revision 1.4  12/08/2003 12:19:29  yada
  Changed the file to place in an include statement from MemoryMap.h to mmap_global.h

  Revision 1.3  2003/11/30 04:20:10  yasu
  Put CPU CPSR settings-related description in cpsr.h

  Revision 1.2  2003/11/29 03:53:25  yasu
  Code attempted to transfer IRQHandler code to TCM?  
  Commenting it out until overlay setup is arranged.

  Revision 1.1  11/29/2003 01:25:46  yada
  File name changed

  Revision 1.9  2003/11/28 12:18:27  yasu
  typo correction

  Revision 1.8  2003/11/28 12:17:23  yasu
  Added OS_IRQHandler size check ASSERT

  Revision 1.7  11/26/2003 00:45:07  yada
  Corrected mistaken return target for OS_RestoreInterrupt()

  Revision 1.6  11/25/2003 10:44:08  yada
  Name change: OS_xxxInterruptxxx > OS_xxxIRQxxx

  Revision 1.5  11/13/2003 06:51:15  yada
  Rewrite DMA function following dma.h revision     

  Revision 1.4  11/11/2003 12:56:52  yada
  (none)

  Revision 1.3  11/10/2003 08:06:21  yada
  (1.2 comment correction) Made so interrupt process routine uses OS_InterruptHandler

  Revision 1.2  11/10/2003 08:05:07  yada
  Inverted input from the pad.

  Revision 1.1  11/06/2003 09:30:40  yada
  Provisional version


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

/*---------------------------------------------------------------------------*
  Name:         OS_InitIrqTable

  Description:  initialize IRQ handler table

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
extern OSThreadQueue OSi_IrqThreadQueue;

void OS_InitIrqTable(void)
{
    //---- initialize thread queue for irq
    OS_InitThreadQueue(&OSi_IrqThreadQueue);

#ifdef SDK_ARM7
    //---- clear VBlank counter
    OSi_SetVBlankCount(0);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetIrqFunction

  Description:  set IRQ handler function

  Arguments:    intrBit         IRQ mask bit
                function        funtion to set

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetIrqFunction(OSIrqMask intrBit, OSIrqFunction function)
{
    int     i;
    OSIrqCallbackInfo *info;

    for (i = 0; i < OS_IRQ_TABLE_MAX; i++)
    {
        if (intrBit & 1)
        {
            info = NULL;

            // ---- if dma(0-3)
            if (REG_OS_IE_D0_SHIFT <= i && i <= REG_OS_IE_D3_SHIFT)
            {
                info = &OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT];
            }
            // ---- if timer(0-3)
            else if (REG_OS_IE_T0_SHIFT <= i && i <= REG_OS_IE_T3_SHIFT)
            {
                info = &OSi_IrqCallbackInfo[i - REG_OS_IE_T0_SHIFT + OSi_IRQCALLBACK_NO_TIMER0];
            }
#ifdef SDK_ARM7
            // ---- if vblank
            else if (REG_OS_IE_VB_SHIFT == i)
            {
                info = &OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NO_VBLANK];
            }
#endif
            //---- other interrupt
            else
            {
                OS_IRQTable[i] = function;
            }

            if (info)
            {
                info->func = (void (*)(void *))function;
                info->arg = 0;
                info->enable = TRUE;
            }

        }
        intrBit >>= 1;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetIrqFunction

  Description:  get IRQ handler function

  Arguments:    intrBit         IRQ mask bit

  Returns:      None
 *---------------------------------------------------------------------------*/
OSIrqFunction OS_GetIrqFunction(OSIrqMask intrBit)
{
    int     i;
    OSIrqFunction *funcPtr = &OS_IRQTable[0];

    for (i = 0; i < OS_IRQ_TABLE_MAX; i++)
    {
        if (intrBit & 1)
        {
            //---- if dma(0-3)
            if (REG_OS_IE_D0_SHIFT <= i && i <= REG_OS_IE_D3_SHIFT)
            {
                return (void (*)(void))OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT].func;
            }
            //---- if timer(0-3)
            else if (REG_OS_IE_T0_SHIFT <= i && i <= REG_OS_IE_T3_SHIFT)
            {
                return (void (*)(void))OSi_IrqCallbackInfo[i - REG_OS_IE_T0_SHIFT +
                                                           OSi_IRQCALLBACK_NO_TIMER0].func;
            }
#ifdef SDK_ARM7
            else if (REG_OS_IE_VB_SHIFT == i)
            {
                return (void (*)(void))OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NO_VBLANK].func;
            }
#endif

            //---- other interrupt
            return *funcPtr;
        }
        intrBit >>= 1;
        funcPtr++;
    }
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         OSi_EnterDmaCallback

  Description:  enter dma callback handler and arg

  Arguments:    dmaNo       dma number to set callback
                callback    callback for dma interrupt
                arg         its argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void OSi_EnterDmaCallback(u32 dmaNo, void (*callback) (void *), void *arg)
{
    OSIrqMask imask = (1UL << (REG_OS_IE_D0_SHIFT + dmaNo));

    //---- enter callback
    OSi_IrqCallbackInfo[dmaNo].func = callback;
    OSi_IrqCallbackInfo[dmaNo].arg = arg;

    //---- remember IRQ mask bit
    OSi_IrqCallbackInfo[dmaNo].enable = OS_EnableIrqMask(imask) & imask;
}


/*---------------------------------------------------------------------------*
  Name:         OSi_EnterTimerCallback

  Description:  enter timer callback handler and arg

  Arguments:    timerNo     timer number to set callback
                callback    callback for timer interrupt
                arg         its argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void OSi_EnterTimerCallback(u32 timerNo, void (*callback) (void *), void *arg)
{
    OSIrqMask imask = (1UL << (REG_OS_IE_T0_SHIFT + timerNo));

    //---- enter callback
    OSi_IrqCallbackInfo[timerNo + 4].func = callback;
    OSi_IrqCallbackInfo[timerNo + 4].arg = arg;

    //---- remember IRQ mask bit (force to be ENABLE)
    (void)OS_EnableIrqMask(imask);
    OSi_IrqCallbackInfo[timerNo + 4].enable = TRUE;
}

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_begin.h>
#endif
//================================================================================
//         IrqMask
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetIrqMask

  Description:  set irq factor

  Arguments:    intr        irq factor

  Returns:      previous factors
 *---------------------------------------------------------------------------*/
OSIrqMask OS_SetIrqMask(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

/*---------------------------------------------------------------------------*
  Name:         OS_EnableIrqMask

  Description:  set specified irq factor

  Arguments:    intr        irq factor

  Returns:      previous factors
 *---------------------------------------------------------------------------*/
OSIrqMask OS_EnableIrqMask(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = prep | intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

/*---------------------------------------------------------------------------*
  Name:         OS_DisableIrqMask

  Description:  unset specified irq factor

  Arguments:    intr        irq factor

  Returns:      previous factors
 *---------------------------------------------------------------------------*/
OSIrqMask OS_DisableIrqMask(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = prep & ~intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

/*---------------------------------------------------------------------------*
  Name:         OS_ResetRequestIrqMask

  Description:  reset IF bit
                (setting bit causes to clear bit for interrupt)

  Arguments:    intr        irq factor

  Returns:      previous factors
 *---------------------------------------------------------------------------*/
OSIrqMask OS_ResetRequestIrqMask(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IF;
    reg_OS_IF = intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_end.h>
#endif



//================================================================================
//              IRQ STACK CHECKER
//================================================================================
//---- irq stack size from LCF file
extern void SDK_IRQ_STACKSIZE(void);

//---- defs for irq stack
#ifdef SDK_ARM9
#  define  OSi_IRQ_STACK_TOP                (HW_DTCM_SVC_STACK - ((s32)SDK_IRQ_STACKSIZE))
#  define  OSi_IRQ_STACK_BOTTOM             HW_DTCM_SVC_STACK
#else
#  define  OSi_IRQ_STACK_TOP                (HW_PRV_WRAM_IRQ_STACK_END - ((s32)SDK_IRQ_STACKSIZE))
#  define  OSi_IRQ_STACK_BOTTOM             HW_PRV_WRAM_IRQ_STACK_END
#endif

//---- Stack CheckNumber
#ifdef SDK_ARM9
#  define  OSi_IRQ_STACK_CHECKNUM_BOTTOM     0xfddb597dUL
#  define  OSi_IRQ_STACK_CHECKNUM_TOP        0x7bf9dd5bUL
#  define  OSi_IRQ_STACK_CHECKNUM_WARN       0x597dfbd9UL
#else
#  define  OSi_IRQ_STACK_CHECKNUM_BOTTOM     0xd73bfdf7UL
#  define  OSi_IRQ_STACK_CHECKNUM_TOP        0xfbdd37bbUL
#  define  OSi_IRQ_STACK_CHECKNUM_WARN       0xbdf7db3dUL
#endif

static u32 OSi_IrqStackWarningOffset = 0;

/*---------------------------------------------------------------------------*
  Name:         OS_SetIrqStackChecker

  Description:  set irq stack check number to irq stack

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetIrqStackChecker(void)
{
    *(u32 *)(OSi_IRQ_STACK_BOTTOM - sizeof(u32)) = OSi_IRQ_STACK_CHECKNUM_BOTTOM;
    *(u32 *)(OSi_IRQ_STACK_TOP) = OSi_IRQ_STACK_CHECKNUM_TOP;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetIrqStackWarningOffset

  Description:  Set warning level for irq stack

  Arguments:    offset : offset from stack top. must be multiple of 4

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetIrqStackWarningOffset(u32 offset)
{
    SDK_ASSERTMSG((offset & 3) == 0, "Offset must be aligned by 4");
    SDK_ASSERTMSG(offset > 0, "Cannot set warning level to stack top.");
    SDK_ASSERTMSG(offset < ((u32)SDK_IRQ_STACKSIZE), "Cannot set warning level over stack bottom.");

    //---- remember warning offset
    OSi_IrqStackWarningOffset = offset;

    //---- set Stack CheckNum
    if (offset != 0)
    {
        *(u32 *)(OSi_IRQ_STACK_TOP + offset) = OSi_IRQ_STACK_CHECKNUM_WARN;
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_GetIrqStackStatus

  Description:  check irq stack. check each CheckNUM.
                return result.

  Arguments:    None

  Returns:      0 (OS_STACK_NO_ERROR)        no error
                OS_STACK_OVERFLOW            overflow
                OS_STACK_ABOUT_TO_OVERFLOW   about to overflow
                OS_STACK_UNDERFLOW           underflow
 *---------------------------------------------------------------------------*/
OSStackStatus OS_GetIrqStackStatus(void)
{
    //OS_Printf("CHECK OF %x\n", (*(u32 *)(OSi_IRQ_STACK_TOP) ) );
    //OS_Printf("CHECK AO %x\n", (*(u32 *)(OSi_IRQ_STACK_TOP + OSi_IrqStackWarningOffset) ) );
    //OS_Printf("CHECK UF %x\n", (*(u32 *)(OSi_IRQ_STACK_BOTTOM - sizeof(u32))) );

    //OS_Printf(" - OF %x\n", OSi_IRQ_STACK_CHECKNUM_TOP);
    //OS_Printf(" - AO %x\n", OSi_IRQ_STACK_CHECKNUM_WARN);
    //OS_Printf(" - UF %x\n", OSi_IRQ_STACK_CHECKNUM_BOTTOM);

    //---- Check if overflow
    if (*(u32 *)(OSi_IRQ_STACK_TOP) != OSi_IRQ_STACK_CHECKNUM_TOP)
    {
        return OS_STACK_OVERFLOW;
    }
    //---- Check if about to overflow
    else if (OSi_IrqStackWarningOffset
             && *(u32 *)(OSi_IRQ_STACK_TOP + OSi_IrqStackWarningOffset) !=
             OSi_IRQ_STACK_CHECKNUM_WARN)
    {
        return OS_STACK_ABOUT_TO_OVERFLOW;
    }
    //---- Check if underFlow
    else if (*(u32 *)(OSi_IRQ_STACK_BOTTOM - sizeof(u32)) != OSi_IRQ_STACK_CHECKNUM_BOTTOM)
    {
        return OS_STACK_UNDERFLOW;
    }
    //---- No Error, return.
    else
    {
        return OS_STACK_NO_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_CheckIrqStack

  Description:  check irq stack. check each CheckNUM.
                if changed, display warning and halt.

  Arguments:    file     file name displayed when stack overflow
                line     line number displayed when stack overflow

  Returns:      None
                ( if error occurred, never return )
 *---------------------------------------------------------------------------*/
static char *OSi_CheckIrqStack_mesg[] = {
    "overflow", "about to overflow", "underflow"
};

#ifndef SDK_FINALROM
#ifndef SDK_NO_MESSAGE
void OSi_CheckIrqStack(char *file, int line)
{
    OSStackStatus st = OS_GetIrqStackStatus();

    if (st == OS_STACK_NO_ERROR)
    {
        return;
    }

    OSi_Panic(file, line, "irq stack %s.\nirq stack area: %08x-%08x, warning offset: %x",
              OSi_CheckIrqStack_mesg[(int)st - 1],
              OSi_IRQ_STACK_TOP, OSi_IRQ_STACK_BOTTOM, OSi_IrqStackWarningOffset);
    // Never return
}
#endif
#endif
