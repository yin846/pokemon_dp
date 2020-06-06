/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     ringBuffer.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ringBuffer.h,v $
  Revision 1.8  04/04/2005 09:14:19  kitamurs
  Changed so that it compiles in C++ mode in Win32

  Revision 1.7  01/21/2005 04:58:57  kitamurs
  Changed year

  Revision 1.6  11/12/2004 02:03:20  kitamurs
  Added support for divided transfer

  Revision 1.5  10/04/2004 05:45:13  kitamurs
  Added the buffer clear function

  Revision 1.4  09/08/2004 06:27:05  kitamurs
  Added header for BOOL type

  Revision 1.3  09/08/2004 06:05:17  kitamurs
  Supported ensata

  Revision 1.2  08/05/2004 05:52:28  kitamurs
  Interrupt provisions

  Revision 1.1.4.1  2004/07/27 01:47:42  kitamurs
  Correspondence to the interrupt prohibition

  Revision 1.1  05/25/2004 07:00:25  kitamurs
  Moved directories

  Revision 1.1  05/24/2004 02:17:12  kitamurs
  Moved directories

  Revision 1.1  04/28/2004 05:12:40  kitamurs
  Added load functions

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_RINGBUFFER_H_
#define NNS_MCS_RINGBUFFER_H_

#if defined(_MSC_VER)
    #include "mcsStdInt.h"
#else
    #include <stdint.h>
#endif

#if defined(_WIN32)
    #include <windef.h>
#else
    #include <nitro/types.h>
#endif

#if !defined(_WIN32) && defined(__cplusplus)
extern "C" {
#endif


/* =======================================================================
    Constant definitions
   ======================================================================== */

#define NNS_MCS_RINGBUF_SIGNATURE   'MCRB'

#define NNS_MCS_RINGBUF_OVERFLOW    (1 << 0)

enum
{
    NNS_MCS_RINGBUF_DTYPE_START,
    NNS_MCS_RINGBUF_DTYPE_MIDDLE,
    NNS_MCS_RINGBUF_DTYPE_END
};


/* ========================================================================
    Type Definitions
   ======================================================================== */

#if defined(_WIN32)
    typedef uint32_t    NNSiMcsBufPtr;
    typedef uint32_t    NNSiMcsUIntPtr;
#else
    typedef void*       NNSiMcsBufPtr;
    typedef uintptr_t   NNSiMcsUIntPtr;
#endif

typedef struct NNSiMcsBufRgn NNSiMcsBufRgn;
struct NNSiMcsBufRgn
{
    NNSiMcsUIntPtr  buf;
    uint32_t        bufSize;
};

typedef struct NNSiMcsMsgRange NNSiMcsMsgRange;
struct NNSiMcsMsgRange
{
    NNSiMcsUIntPtr  start;
    NNSiMcsUIntPtr  end;
};

typedef struct NNSiMcsRingBufferHeader NNSiMcsRingBufferHeader;
struct NNSiMcsRingBufferHeader
{
    uint32_t        signature;
    uint32_t        state;
    NNSiMcsMsgRange mrng;
    NNSiMcsBufRgn   brgn;
};

typedef NNSiMcsUIntPtr      NNSMcsRingBuffer;


/* =======================================================================
    Function Prototype
   ======================================================================== */

#if defined(_WIN32)
    void                NNSi_McsReadFromRingBufferMem(
                            void*       appAddr,
                            uint32_t    rbAddr,
                            uint32_t    size);

    void                NNSi_McsWriteToRingBufferMem(
                            uint32_t    rbAddr,
                            const void* appAddr,
                            uint32_t    size);
#endif

NNSMcsRingBuffer    NNS_McsInitRingBuffer(
                        NNSiMcsBufPtr       buf,
                        uint32_t            bufSize);

uint32_t            NNS_McsGetRingBufferReadableBytes(
                        NNSMcsRingBuffer    rb);

uint32_t            NNS_McsGetRingBufferTotalReadableBytes(
                        NNSMcsRingBuffer    rb);

uint32_t            NNS_McsGetRingBufferWritableBytes(
                        NNSMcsRingBuffer    rb);

BOOL                NNS_McsReadRingBuffer(
                        NNSMcsRingBuffer    rb,
                        void*               buf,
                        uint32_t            size,
                        uint32_t*           pReadBytes);

BOOL                NNS_McsWriteRingBuffer(
                        NNSMcsRingBuffer    rb,
                        const void*         buf,
                        uint32_t            size);

BOOL                NNS_McsWriteRingBufferEx(
                        NNSMcsRingBuffer    rb,
                        const void*         buf,
                        uint32_t            offset,
                        uint32_t            size,
                        uint32_t            totalSize);

void                NNS_McsClearRingBuffer(
                        NNSMcsRingBuffer    rb);

BOOL                NNS_McsCheckRingBuffer(
                        NNSMcsRingBuffer    rb);

uint32_t            NNS_McsGetRingBufferState(
                        NNSMcsRingBuffer    rb);

void                NNS_McsClearRingBufferState(
                        NNSMcsRingBuffer    rb);


#if !defined(_WIN32) && defined(__cplusplus)
} /* extern "C"*/
#endif

/* NNS_MCS_RINGBUFFER_H_*/
#endif

