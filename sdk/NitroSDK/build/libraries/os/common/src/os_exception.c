/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_exception.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_exception.c,v $
  Revision 1.37  05/08/2006 09:26:16  okubata_ryoma
  Made revisions relating to the return of OS_CONSOLE_NITRO even on the debugger in the case of a FINALROM build.

  Revision 1.36  04/04/2006 06:05:53  yosizaki
  revisions related to the 8 byte alignment of the call stack

  Revision 1.35  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.34  02/28/2005 05:26:28  yosizaki
  do-indent.

  Revision 1.33  11/02/2004 10:35:45  yada
  just fix comment

  Revision 1.32  10/03/2004 09:59:16  yasu
  Avoid sdkindent in inline-asm{}

  Revision 1.31  09/02/2004 02:53:42  yada
  only change comment

  Revision 1.30  08/26/2004 00:20:10  yada
  change method to distinguish running on debugger

  Revision 1.29  06/11/2004 06:24:43  yada
  close SDK_BB support

  Revision 1.28  06/10/2004 06:25:06  yada
  consider for occurring exception in THUMB mode

  Revision 1.27  06/07/2004 10:56:35  yada
  make difference to set handler according to exec environment

  Revision 1.26  05/12/2004 11:00:48  yada
  OSi_ExPrintf always uses OS_Printf

  Revision 1.25  05/12/2004 05:34:29  yada
  some fix

  Revision 1.24  05/12/2004 05:31:46  yada
  modification for DEBUGGER

  Revision 1.23  05/07/2004 02:11:06  takano_makoto
  Change OSiExContext offset definition.

  Revision 1.22  04/16/2004 09:36:19  yada
  fix temporary exception vector
  0x27fffd9c->0x027ffd9c

  Revision 1.21  04/14/2004 12:48:20  yada
  Added provisional support for IS-NITRO-DEBUGGER

  Revision 1.20  03/30/2004 06:09:29  yada
  Changed ldconst and lda to the description that uses ldr =.

  Revision 1.19  03/18/2004 12:56:43  yada
  Corrected a mistake in the exception vector settings

  Revision 1.18  03/11/2004 09:47:14  yada
  The prohibition of FIQ and IRQ interrupts at the time of the exception was changed so that only IRQ interrupts are prohibited

  Revision 1.17  03/09/2004 02:37:59  yada
  Corrected the debugger monitor handler call assessment section

  Revision 1.16  03/08/2004 02:54:13  yada
  Gave consideration to the debugger monitor exception handler

  Revision 1.15  03/01/2004 11:54:24  yada
  Creation of function comments in English.

  Revision 1.14  02/19/2004 08:43:05  yada
  Added user exception routine type.

  Revision 1.13  02/17/2004 06:04:38  yada
  Added support for ARM7

  Revision 1.12  02/16/2004 09:40:12  yada
  Set so that interrupts on the ARM7-side also hook

  Revision 1.11  02/16/2004 00:49:30  yasu
  Don't switch ISD/noISD by SDK_LINK_ISD.
  Please use stubs library to resolve it at link time.

  Revision 1.10  02/13/2004 04:05:28  yada
  Thought of ways to divide ARM9 and ARM7 processing.

  Revision 1.9  02/12/2004 11:08:30  yada
  Worked on distribution of processing for ARM9/ARM7.

  Revision 1.8  02/09/2004 11:17:22  yasu
  include code32.h

  Revision 1.7  02/05/2004 10:50:59  yada
  Set so that undefined commands will cause a stop

  Revision 1.6  02/05/2004 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  02/02/2004 05:34:54  yada
  OS_SetIrqMask() -> OS_EnableIrqMask()

  Revision 1.4  01/30/2004 08:05:36  yada
  Corrected so that there is no exception display with FINALROM

  Revision 1.3  01/30/2004 04:31:47  yada
  Built in exception display. xyxyxy

  Revision 1.2  01/16/2004 01:18:05  yasu
  Support SDK_BB

  Revision 1.1  01/14/2004 12:03:05  yada
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/hw/common/armArch.h>
#include <nitro/memorymap.h>
#include <nitro/os.h>


//---- displaying function OSi_ExPrintf
#ifdef SDK_ARM9
#define OSi_ExPrintf OS_Printf
//#define OSi_ExPrintf ISDPrintf
//void ISDPrintf( const char *pBuf, ... );
#else
#define OSi_ExPrintf OS_Printf
#endif


static asm void OSi_ExceptionHandler( void );
static asm void OSi_GetAndDisplayContext( void );
static asm void OSi_SetExContext( void );
static     void OSi_DisplayExContext( void );


//---- context for exception display
typedef struct
{
    OSContext   context;
    u32         cp15;
    u32         spsr;
    u32         exinfo;
    u32         debug[4];
} OSiExContext;

static OSiExContext OSi_ExContext;

//---- user's exception handler
static OSExceptionHandler OSi_UserExceptionHandler;
static void *OSi_UserExceptionHandlerArg;

//---- debugger's exception handler
static void *OSi_DebuggerHandler = NULL;

//================================================================================
//  exception vector
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetExceptionVectorUpper

  Description:  set exception vector to high address

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm void OS_SetExceptionVectorUpper( void )
{
    mrc     p15, 0, r0, c1, c0, 0
    orr     r0, r0, #HW_C1_EXCEPT_VEC_UPPER
    mcr     p15, 0, r0, c1, c0, 0
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetExceptionVectorLower

  Description:  set exception vector to low address

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_SetExceptionVectorLower( void )
{
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #HW_C1_EXCEPT_VEC_UPPER
    mcr     p15, 0, r0, c1, c0, 0
    bx      lr
}
#include <nitro/codereset.h>


//================================================================================
//  exception handling
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitException

  Description:  Initialize exception handling system

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/

// In the future, HW_EXCP_VECTOR_BUF will be set to this value.
#define HW_EXCP_VECTOR_BUF_FOR_DEBUGGER  0x027ffd9c

void OS_InitException(void)
{
    //---- consider for debugger exception handler
    if (0x2600000 <= *(u32 *)HW_EXCP_VECTOR_BUF_FOR_DEBUGGER
        && *(u32 *)HW_EXCP_VECTOR_BUF_FOR_DEBUGGER < 0x2800000)
    {
        OSi_DebuggerHandler = *(void **)HW_EXCP_VECTOR_BUF_FOR_DEBUGGER;
    }
    else
    {
        OSi_DebuggerHandler = NULL;
    }

    //---- exception hook
    if (!OSi_DebuggerHandler && !(OS_GetConsoleType() & OS_CONSOLE_ISDEBUGGER))
    {
        *(u32 *)(HW_EXCP_VECTOR_BUF_FOR_DEBUGGER) = (u32)OSi_ExceptionHandler;

        // for not-debugger, store handler address into original hook
        *(u32 *)(HW_EXCP_VECTOR_BUF) = (u32)OSi_ExceptionHandler;
    }

    //---- user's handler
    OSi_UserExceptionHandler = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetUserExceptionHandler

  Description:  set user exception handler and its argument

  Arguments:    handler    exception hander
                arg         its argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetUserExceptionHandler(OSExceptionHandler handler, void *arg)
{
    OSi_UserExceptionHandler = handler;
    OSi_UserExceptionHandlerArg = arg;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ExceptionHandler

  Description:  system exception handler
                user handler is called from here

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
//
//      status of stack is {cp15,spsr,r12,lr}
//      LSB = 1 means coming by reset
//
#include <nitro/code32.h>
asm void OSi_ExceptionHandler( void )
{
    //---- call debugger monitor handler (if exists)
    ldr       r12, =OSi_DebuggerHandler // only r12 can be destructed
    ldr       r12, [r12]
    cmp       r12, #0
    movne     lr, pc
    bxne      r12

    //---- setting stack pointer <------------------------- consider later
#ifdef SDK_ARM9
    //---- ARM9 stack
    ldr       r12, =HW_ITCM_END
#else
    //---- ARM7 stack
    ldr       r12, =0x3806000
#endif
    stmfd     r12!, {r0-r3,sp,lr}

    and       r0, sp, #1
    mov       sp, r12
    
    mrs       r1, CPSR
    and       r1, r1, #0x1f

    //---- if ABORT exception, stop
    teq       r1, #0x17
    bne       @10
    bl        OSi_GetAndDisplayContext
    b         usr_return

@10:
    //---- if UNDEF exception, stop
    teq       r1, #0x1b
    bne       usr_return
    bl        OSi_GetAndDisplayContext

usr_return:
    ldr       r12, =OSi_DebuggerHandler
    ldr       r12, [r12]
    cmp       r12, #0
@1: beq       @1

//( stop now even if debugger )
@2: 
    mov       r0,r0 // nop
    b         @2

    ldmfd     sp!, {r0-r3, r12, lr}
    mov       sp, r12
    bx        lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetAndDisplayContext

  Description:  stop after displaying registers

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static asm void OSi_GetAndDisplayContext( void )
{
    stmfd     sp!, {r0, lr} /* 8 byte alignment of the call stack*/

    //---- set exception context
    bl        OSi_SetExContext
    //---- display exception context (and call user callback)
    bl        OSi_DisplayExContext

    ldmfd     sp!, {r0, lr} /* 8 byte alignment of the call stack*/
    bx        lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_SetExContext

  Description:  set context when exception occurred

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
//       explanation registers at the top of this function.
//
//       in r12, {r0-r3,sp} (sp is exception sp) stored.
//       in this sp, {cp15,spsr,r12,lr} (cp15,spsr,r12,lr is registers when exception occurred) stored.
//       r4-r11 is registers then exception occurred.
//       if you want to know spsr,sp,lr, please switch bank and read.
//
static asm void OSi_SetExContext( void )
{
    //---- pointer ExContext structure
    ldr    r1, =OSi_ExContext;

    mrs    r2, CPSR
    str    r2,  [r1, #OSiExContext.debug[1] ]

    //---- store bit which means which is the reason, reset or exception
    str    r0, [r1, #OSiExContext.exinfo ]

    //---- store r0 - r3
    ldr    r0, [r12,#0]
    str    r0, [r1, #OS_CONTEXT_R0]
    ldr    r0, [r12,#4]
    str    r0, [r1, #OS_CONTEXT_R1]
    ldr    r0, [r12,#8]
    str    r0, [r1, #OS_CONTEXT_R2]
    ldr    r0, [r12, #12]
    str    r0, [r1, #OS_CONTEXT_R3]
    ldr    r2, [r12, #16]
    bic    r2, r2, #1

    //---- store r4 - r11
    add    r0, r1, #OS_CONTEXT_R4
    stmia  r0, {r4-r11}

    //---- (for debug)
    str    r12, [r1, #OSiExContext.debug[0] ]

#ifdef SDK_ARM9
    //---- get {cp15,cpsr,r12,pc} from stack
    ldr    r0, [r2, #0]
    str    r0, [r1, #OSiExContext.cp15 ]
    ldr    r3, [r2, #4]
    str    r3, [r1, #OS_CONTEXT_CPSR]
    ldr    r0, [r2, #8]
    str    r0, [r1, #OS_CONTEXT_R12]
    ldr    r0, [r2, #12]
    str    r0, [r1, #OS_CONTEXT_PC_PLUS4]
#else // ifdef SDK_ARM9
    //---- get {cpsr,r12,pc} from stack
    mov    r0, #0
    str    r0, [r1, #OSiExContext.cp15]
    ldr    r3, [r2, #0]
    str    r3, [r1, #OS_CONTEXT_CPSR]
    ldr    r0, [r2, #4]
    str    r0, [r1, #OS_CONTEXT_R12]
    ldr    r0, [r2, #8]
    str    r0, [r1, #OS_CONTEXT_PC_PLUS4]
#endif // ifdef SDK_ARM9

    //---- set mode to one which exception occurred
    //     but, disable IRQ
    mrs    r0, CPSR
    orr    r3, r3, #0x80
    bic    r3, r3, #0x20
    msr    CPSR_cxsf, r3
   
    //---- get sp, lr, spsr
    str    sp, [r1, #OS_CONTEXT_R13]
    str    lr, [r1, #OS_CONTEXT_R14]
    mrs    r2, SPSR

    //---- debugger
    str    r2, [r1, #OSiExContext.debug[3] ]

    //---- restore mode
    msr    CPSR_cxsf, r0
    bx     lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_DisplayExContext

  Description:  stop after display exception context

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void OSi_DisplayExContext()
{
#ifndef SDK_FINALROM
    int     i;

    OSi_ExPrintf("**** Exception Occurred ****\n");

    //---------------- displaying registers
    //---- for R0-15 Registers
    for (i = 0; i < 13; i++)
    {
        OSi_ExPrintf("R%02d=%08X  %c", i, OSi_ExContext.context.r[i], ((i & 3) == 3) ? '\n' : ' ');
    }
    OSi_ExPrintf("SP =%08X   ", OSi_ExContext.context.sp);
    OSi_ExPrintf("LR =%08X   ", OSi_ExContext.context.lr);
    OSi_ExPrintf("PC =%08X\n", OSi_ExContext.context.pc_plus4);

    //---- for status Registers
#ifdef SDK_ARM9
    OSi_ExPrintf("  CPSR=%08X  SPSR=%08X  CP15=%08X\n",
                 OSi_ExContext.context.cpsr, OSi_ExContext.spsr, OSi_ExContext.cp15);
#else
    OSi_ExPrintf("  CPSR=%08X  SPSR=%08X\n", OSi_ExContext.context.cpsr, OSi_ExContext.spsr);
#endif

    //---- for Debug Values
#if 0
    for (i = 0; i < 4; i++)
    {
        OSi_ExPrintf("DEBUG%02d=%08X  ", i, OSi_ExContext.debug[i]);
    }
#endif
    OSi_ExPrintf("\n\n");
#endif

    //---------------- user's callback
    if (OSi_UserExceptionHandler)
    {
        //---- force to become SYS mode
        //     but, use current stack not SYS mode stack
        asm
        {
          /* *INDENT-OFF**/
          mov      r0, sp
          ldr      r1, =0x9f
          msr      CPSR_cxsf, r1
          mov      sp, r0
          /* *INDENT-ON**/
        }

#ifdef SDK_ARM9
        OS_EnableProtectionUnit();
#endif

        //---- call user handler
        ((void (*)(u32, void *))OSi_UserExceptionHandler) ((u32)&OSi_ExContext,
                                                           OSi_UserExceptionHandlerArg);

#ifdef SDK_ARM9
        OS_DisableProtectionUnit();
#endif

    }

}

#include <nitro/codereset.h>
