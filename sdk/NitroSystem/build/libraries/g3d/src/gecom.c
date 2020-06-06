/*---------------------------------------------------------------------------*
  Project:   NITRO-System - G3D(Hokotate) -
  File:     gecom.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gecom.c,v $
  Revision 1.18  02/25/2005 04:07:41 AM  takano_makoto
  small fix

  Revision 1.17  11/09/2004 02:15:48 AM  nishida_kenji
  fix comments.

  Revision 1.16  10/15/2004 01:32:24 AM  nishida_kenji
  add comments.

  Revision 1.15  09/22/2004 06:11:41 AM  nishida_kenji
  Add NNS_G3dGeUseFastDma(read warning).

  Revision 1.14  09/16/2004 07:03:29 AM  nishida_kenji
  bugfix NNS_G3dGeSendDL(freeze if GX_DMAID is GX_DMA_NOT_USE).

  Revision 1.13  09/02/2004 02:38:51 AM  nishida_kenji
  Do not DMA if DL is small.

  Revision 1.12  08/30/2004 11:45:55 AM  nishida_kenji
  deal with NITRO-CPU Geometry FIFO glitch(2004/08/26).

  Revision 1.11  08/27/2004 07:12:39 AM  nishida_kenji
  add NNS_G3D_USE_FASTGXDMA

  Revision 1.10  07/21/2004 04:27:17 AM  nishida_kenji
  small fix

  Revision 1.9  07/21/2004 02:16:58 AM  nishida_kenji
  use MI_CopyFast

  Revision 1.8  07/19/2004 10:56:34 AM  nishida_kenji
  add sendNB

  Revision 1.7  07/17/2004 12:59:20 PM  nishida_kenji
  revise NNS_G3dGeBufferOP_N

  Revision 1.6  07/12/2004 12:50:46 PM  nishida_kenji
  refactoring

  Revision 1.5  07/12/2004 02:24:53 AM  nishida_kenji
  (none)

  Revision 1.4  07/10/2004 10:45:28 AM  nishida_kenji
  (none)

  Revision 1.3  07/10/2004 08:48:00 AM  nishida_kenji
  (none)

  Revision 1.2  07/07/2004 12:38:32 PM  nishida_kenji
  REVISED

  Revision 1.1  07/02/2004 05:57:00 AM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/gecom.h>

//
// NOTICE:
// ONLY FOR SINGLE THREADED CODE
//


////////////////////////////////////////////////////////////////////////////////
//
// Static Variables
//

/*---------------------------------------------------------------------------*
    NNS_G3dFlagGXDmaAsync

    When the flag used by the MI_SendGXCommandAsync used with G3D
    is not zero, the DMA transfer has not completed.
 *---------------------------------------------------------------------------*/
static volatile int NNS_G3dFlagGXDmaAsync  = 0;

//
// NOTICE:
// Even if the buffer for the Ge -related API is NULL, this must operate as normal.
//

/*---------------------------------------------------------------------------*
    NNS_G3dGeBuffer

    Pointer to the geometry command buffer used by G3D.
    Allocated from the normal stack (DTCM) region memory.

    When a geometry command send request is received during a display list DMA transfer,
    the CPU processing can proceed by writing the command to this buffer.
    Valid for objects with large display lists.
 *---------------------------------------------------------------------------*/
static NNSG3dGeBuffer* NNS_G3dGeBuffer = NULL;


////////////////////////////////////////////////////////////////////////////////
//
// APIs
//


/*---------------------------------------------------------------------------*
    NNS_G3dGeIsSendDLBusy

    Returns whether the geometry command DMA transfer to the geometry engine executed
    by G3D is in progress.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGeIsSendDLBusy(void)
{
    return NNS_G3dFlagGXDmaAsync;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeIsBufferExist

    Returns whether or nor the command buffer exists.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGeIsBufferExist(void)
{
    return (NNS_G3dGeBuffer != NULL);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeSetBuffer

    If the command buffer is not yet set in the NNS_G3dGeBuffer,
    sets p as the buffer.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeSetBuffer(NNSG3dGeBuffer* p)
{
    NNS_G3D_NULL_ASSERT(p);

    if (NNS_G3dGeBuffer == NULL)
    {
        p->idx = 0;
        NNS_G3dGeBuffer = p;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeReleaseBuffer

    Safely removes the command buffer.
    The return value is a pointer to the removed command buffer.
 *---------------------------------------------------------------------------*/
NNSG3dGeBuffer*
NNS_G3dGeReleaseBuffer(void)
{
    NNSG3dGeBuffer* p;

    NNS_G3dGeFlushBuffer();

    p = NNS_G3dGeBuffer;
    NNS_G3dGeBuffer = NULL;
    return p;
}


NNS_G3D_INLINE void
sendNB(const void* src, void* dst, u32 szByte)
{
    MI_CpuSend32(src, dst, szByte);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeFlushBuffer

    If the geometry engine is performing DMA transfer, this waits until the process finishes.
    If there is a geometry command buffer, write the buffer contents to the geometry engine. 
     
    After this function ends, geometry commands can be sent directly to the geometry engine. 
    (The G3_XXX functions in the SDK can be used safely.)
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeFlushBuffer(void)
{
    if (NNS_G3dFlagGXDmaAsync)
    {
        NNS_G3dGeWaitSendDL();
    }

    if (NNS_G3dGeBuffer &&
        NNS_G3dGeBuffer->idx > 0)
    {
        sendNB(&NNS_G3dGeBuffer->data[0],
               (void*)&reg_G3X_GXFIFO,
               NNS_G3dGeBuffer->idx << 2);
        NNS_G3dGeBuffer->idx = 0;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeWaitSendDL

    Waits until geometry command DMA transfer (using NNS_G3dGeSendDL) is complete. 
    
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeWaitSendDL(void)
{
    while(NNS_G3dFlagGXDmaAsync)
        ;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeIsImmOK

    Returns whether it is OK to send a command directly to FIFO (g3imm.h). 
    Returns TRUE if it is OK to send the command.
    The conditions for this are that the command buffer does not exist or is empty, and GXDMA transfer is not currently underway.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGeIsImmOK(void)
{
    return (NNS_G3dGeBuffer == NULL || NNS_G3dGeBuffer->idx == 0) &&
           !NNS_G3dGeIsSendDLBusy();
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeIsBufferOK

    Returns whether or not an amount of data equal to numWord can be added to the geometry command buffer.
    Returns TRUE if the data can be added.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGeIsBufferOK(u32 numWord)
{
    return (NNS_G3dGeBuffer != NULL) &&
           (NNS_G3dGeBuffer->idx + numWord <= NNS_G3D_SIZE_COMBUFFER);
}


//
// Exclusively used by NNS_G3dGeSendDL
//
static void
simpleUnlock_(void* arg)
{
    *((volatile int*)arg) = 0;
}

#ifdef NNS_G3D_USE_FASTGXDMA
static BOOL NNS_G3dFlagUseFastDma = TRUE;
#else
static BOOL NNS_G3dFlagUseFastDma = FALSE;
#endif


/*---------------------------------------------------------------------------*
    NNS_G3dGeUseFastDma

    If a value other than FALSE is specified in the argument, the fast function is used for
    geometry command DMA transfer. Do not use this until use guidelines have been established.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeUseFastDma(BOOL cond)
{
#ifndef NNS_G3D_USE_FASTGXDMA
    //
    // WARNING:
    // YOU HAD BETTER NOT USE MI_SendGXCommandAsyncFast
    // UNTIL GUIDELINE FOR THIS FUNCTION IS WRITTEN.
    //
    NNS_G3dFlagUseFastDma = (cond);
#endif
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeSendDL

    Safely writes the display list to the Geometry Engine.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeSendDL(const void* src, u32 szByte)
{
    NNS_G3D_NULL_ASSERT(src);
    NNS_G3D_ASSERT(szByte >= 4);

    // Start transmission
    if (szByte < 256 || GX_DMAID == GX_DMA_NOT_USE)
    {
        NNS_G3dGeBufferOP_N(*(const u32*)src,
                            (const u32*)src + 1,
                            (szByte >> 2) - 1);
    }
    else
    {
        // Flush the command buffer
        NNS_G3dGeFlushBuffer();
        NNS_G3dFlagGXDmaAsync = 1;

        if (NNS_G3dFlagUseFastDma)
        {
            MI_SendGXCommandAsyncFast(GX_DMAID,
                                      src,
                                      szByte,
                                      &simpleUnlock_,
                                      (void*)&NNS_G3dFlagGXDmaAsync);
        }
        else
        {
            MI_SendGXCommandAsync(GX_DMAID,
                                  src,
                                  szByte,
                                  &simpleUnlock_,
                                  (void*)&NNS_G3dFlagGXDmaAsync);
        }
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeBufferOP_N

    The argument takes a number of N and sends the geometry command.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeBufferOP_N(u32 op, const u32* args, u32 num)
{
    if (NNS_G3dGeBuffer)
    {
        if (NNS_G3dFlagGXDmaAsync)
        {
            // Is there sufficient empty space in the buffer?
            if (NNS_G3dGeBuffer->idx + 1 + num <= NNS_G3D_SIZE_COMBUFFER)
            {
                NNS_G3dGeBuffer->data[NNS_G3dGeBuffer->idx++] = op;
                if (num > 0)
                {
                    MI_CpuCopyFast(args,
                                   &NNS_G3dGeBuffer->data[NNS_G3dGeBuffer->idx],
                                   num << 2);

                    NNS_G3dGeBuffer->idx += num;
                }
                // Buffering complete
                return;
            }
        }

        // In the end, nothing has been added to the buffer
        // Write immediately after emptying the buffer
        if (NNS_G3dGeBuffer->idx != 0)
        {
            NNS_G3dGeFlushBuffer();
        }
        else
        {
            if (NNS_G3dFlagGXDmaAsync)
            {
                NNS_G3dGeWaitSendDL();
            }
        }
    }
    else
    {
        if (NNS_G3dFlagGXDmaAsync)
        {
            NNS_G3dGeWaitSendDL();
        }
    }

    reg_G3X_GXFIFO = op;
    sendNB(args, (void*)&reg_G3X_GXFIFO, num << 2);
}

