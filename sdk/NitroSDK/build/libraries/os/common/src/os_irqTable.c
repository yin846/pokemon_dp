/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - OS
  File:     os_irqTable.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_irqTable.c,v $
  Revision 1.20  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.19  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.18  01/12/2005 11:11:30  takano_makoto
  fix copyright header.

  Revision 1.17  01/12/2005 01:04:34  yada
  add about SDK_TCM_APPLY

  Revision 1.16  12/20/2004 02:05:20  yada
  move irqHandler to DTCM

  Revision 1.15  08/16/2004 04:18:21  yada
  in ARM9, no hook in VBlank interrupt

  Revision 1.14  08/10/2004 12:13:32  yada
  VBlank counter is in shared area

  Revision 1.13  08/10/2004 08:00:50  yada
  add OS_GetVBlankCount(). make vblank hook

  Revision 1.12  07/08/2004 10:49:05  yada
  only arranged comment

  Revision 1.11  03/16/2004 07:07:41  yada
  Revised IRQ table. Added a dummy.

  Revision 1.10  03/01/2004 12:18:16  yada
  small bug fix

  Revision 1.9  2004/02/28 09:47:49  yasu
  expend ARM7 IRQ table

  Revision 1.8  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.7  02/03/2004 11:21:10  yada
  added comment

  Revision 1.6  02/02/2004 05:32:23  yada
  Take into consideration timer interrupt callback.
  Made to delete callback information before callback.

  Revision 1.5  01/18/2004 02:25:37  yada
  Reformed indents, etc.

  Revision 1.4  01/17/2004 11:44:20  yada
  Created Async mode for DMA transfers

  Revision 1.3  12/25/2003 07:31:26  yada
  Changes as per unified type rules

  Revision 1.2  2003/12/16 05:25:51  yasu
  Added missing jump vector

  Revision 1.1  2003/11/29 04:29:27  yasu
  Changed name    osInterruptTable.c -> os_irqTable.c

  Revision 1.4  11/25/2003 10:43:29  yada
  Revisions due to OS_xxxInterruptxxx -> OS_xxxIRQxxx name change.

  Revision 1.3  2003/11/25 06:28:47  yasu
  Limited include files

  Revision 1.2  2003/11/10 02:33:19  yasu
  Added  'interrupt table'

  Revision 1.1  11/06/2003 12:14:18  yada
  Provisional version


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>

void    OSi_IrqCallback(int dmaNo);

void    OSi_IrqTimer0(void);
void    OSi_IrqTimer1(void);
void    OSi_IrqTimer2(void);
void    OSi_IrqTimer3(void);

void    OSi_IrqDma0(void);
void    OSi_IrqDma1(void);
void    OSi_IrqDma2(void);
void    OSi_IrqDma3(void);

#ifdef SDK_ARM7
void    OSi_IrqVBlank(void);
#endif

//---------------- Default jump table for IRQ interrupt
#ifdef SDK_ARM9
#include    <nitro/dtcm_begin.h>
#endif
OSIrqFunction OS_IRQTable[OS_IRQ_TABLE_MAX] = {
#ifdef SDK_ARM9
    OS_IrqDummy,                       // VBlank (for ARM9)
#else
    OSi_IrqVBlank,                     // VBlank (for ARM7)
#endif
    OS_IrqDummy,                       // HBlank
    OS_IrqDummy,                       // VCounter
    OSi_IrqTimer0,                     // timer0
    OSi_IrqTimer1,                     // timer1
    OSi_IrqTimer2,                     // timer2
    OSi_IrqTimer3,                     // timer3
    OS_IrqDummy,                       // serial communication (will not occur)
    OSi_IrqDma0,                       // DMA0
    OSi_IrqDma1,                       // DMA1
    OSi_IrqDma2,                       // DMA2
    OSi_IrqDma3,                       // DMA3
    OS_IrqDummy,                       // key
    OS_IrqDummy,                       // cartridge
    OS_IrqDummy,                       // (not used)
    OS_IrqDummy,                       // (not used)
    OS_IrqDummy,                       // sub processor
    OS_IrqDummy,                       // sub processor send FIFO empty
    OS_IrqDummy,                       // sub processor receive FIFO not empty
    OS_IrqDummy,                       // card data transfer finish
    OS_IrqDummy,                       // card IREQ
#ifdef SDK_ARM9
    OS_IrqDummy,                       // geometry command FIFO
#else
    OS_IrqDummy,                       // (not used)
    OS_IrqDummy,                       // Power Management IC
    OS_IrqDummy,                       // SPI data transfer
    OS_IrqDummy                        // Wireless module
#endif
};
#ifdef SDK_ARM9
#include    <nitro/dtcm_end.h>
#endif

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/dtcm_begin.h>
#endif
//---------------- Jump table for DMA & TIMER & VBLANK interrupts
OSIrqCallbackInfo OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NUM] = {
    {NULL, 0, 0,},                     // dma0
    {NULL, 0, 0,},                     // dma1
    {NULL, 0, 0,},                     // dma2
    {NULL, 0, 0,},                     // dma3

    {NULL, 0, 0,},                     // timer0
    {NULL, 0, 0,},                     // timer1
    {NULL, 0, 0,},                     // timer2
    {NULL, 0, 0,},                     // timer3
#ifdef SDK_ARM7
    {NULL, 0, 0,}                      // vblank
#endif
};

//----------------
static u16 OSi_IrqCallbackInfoIndex[OSi_IRQCALLBACK_NUM] = {
    REG_OS_IE_D0_SHIFT, REG_OS_IE_D1_SHIFT, REG_OS_IE_D2_SHIFT, REG_OS_IE_D3_SHIFT,
    REG_OS_IE_T0_SHIFT, REG_OS_IE_T1_SHIFT, REG_OS_IE_T2_SHIFT, REG_OS_IE_T3_SHIFT,
#ifdef SDK_ARM7
    REG_OS_IE_VB_SHIFT
#endif
};
#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/dtcm_end.h>
#endif


#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_begin.h>
#endif
/*---------------------------------------------------------------------------*
  Name:         OS_IrqDummy

  Description:  Dummy interrupt handler

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_IrqDummy(void)
{
    // do nothing
}


/*---------------------------------------------------------------------------*
  Name:         OSi_IrqCallback

  Description:  System interrupt handler

  Arguments:    system irq index

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_IrqCallback(int index)
{
    OSIrqMask imask = (1UL << OSi_IrqCallbackInfoIndex[index]);
    void    (*callback) (void *) = OSi_IrqCallbackInfo[index].func;

//OS_Printf( "irq %d\n", index  );
    //---- clear callback
    OSi_IrqCallbackInfo[index].func = NULL;

    //---- call callback
    if (callback)
    {
        (callback) (OSi_IrqCallbackInfo[index].arg);
    }

    //---- check IRQMask
    OS_SetIrqCheckFlag(imask);

    //---- restore IRQEnable
    if (!OSi_IrqCallbackInfo[index].enable)
    {
        (void)OS_DisableIrqMask(imask);
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_IrqDma0 - OSi_IrqDma3

  Description:  DMA0 - DMA3 interrupt handler

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_IrqDma0(void)
{
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_DMA0);
}

/*- - - - - - - - - - - - - - - - - - - - - - - */
void OSi_IrqDma1(void)
{
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_DMA1);
}

/*- - - - - - - - - - - - - - - - - - - - - - - */
void OSi_IrqDma2(void)
{
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_DMA2);
}

/*- - - - - - - - - - - - - - - - - - - - - - - */
void OSi_IrqDma3(void)
{
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_DMA3);
}

/*---------------------------------------------------------------------------*
  Name:         OSi_IrqTimer0 - OSi_IrqTimer3

  Description:  TIMER0 - TIMER3 interrupt handler

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
/*- - - - - - - - - - - - - - - - - - - - - - - */
void OSi_IrqTimer0(void)
{
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_TIMER0);
}

/*- - - - - - - - - - - - - - - - - - - - - - - */
void OSi_IrqTimer1(void)
{
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_TIMER1);
}

/*- - - - - - - - - - - - - - - - - - - - - - - */
void OSi_IrqTimer2(void)
{
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_TIMER2);
}

/*- - - - - - - - - - - - - - - - - - - - - - - */
void OSi_IrqTimer3(void)
{
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_TIMER3);
}

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_end.h>
#endif


/*---------------------------------------------------------------------------*
  Name:         OSi_VBlank

  Description:  VBLANK interrupt handler (for ARM7)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM7
void OSi_IrqVBlank(void)
{
    void    (*callback) (void) =
        (void (*)(void))OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NO_VBLANK].func;

    //---- vblank counter
    (*(u32 *)HW_VBLANK_COUNT_BUF)++;

    //---- call callback
    if (callback)
    {
        (callback) ();
    }

    //---- check IRQMask
    OS_SetIrqCheckFlag(1UL << REG_OS_IE_VB_SHIFT);
}
#endif
