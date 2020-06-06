/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     NNSMcsRingBuffer.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ringBuffer.c,v $
  Revision 1.8  04/04/2005 09:14:11  kitamurs
  Changed so that it compiles in C++ mode in Win32

  Revision 1.7  01/21/2005 04:50:10  kitamurs
  Changed year

  Revision 1.6  11/12/2004 02:03:04  kitamurs
  Added support for divided transfer

  Revision 1.5  10/04/2004 05:57:53  kitamurs
  Added the state clear

  Revision 1.4  10/04/2004 05:44:55  kitamurs
  Added the buffer clear function

  Revision 1.3  09/08/2004 06:00:14  kitamurs
  Supported ensata

  Revision 1.2  08/05/2004 05:52:18  kitamurs
  Interrupt provisions

  Revision 1.1.4.1  2004/07/27 01:36:44  kitamurs
  Correspondence to the interrupt prohibition

  Revision 1.1  05/25/2004 06:59:11  kitamurs
  Moved directories

  Revision 1.1  05/24/2004 02:14:08  kitamurs
  Moved directories

  Revision 1.1  04/28/2004 05:11:45  kitamurs
  Added load functions

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#if ! defined(_WIN32)

    #include <nitro.h>

#endif

#include <nnsys/mcs/ringBuffer.h>


/* ========================================================================
    Compiler switches
   ======================================================================== */

#define USE_DISABLE_IRQ 0


/* ========================================================================
    Macro constant
   ======================================================================== */

#if defined(_WIN32)

    // c++
    #define INLINE  inline

#else

    // c99
    #define INLINE  inline

#endif


/* ========================================================================
    Macro functions
   ======================================================================== */

#if USE_DISABLE_IRQ
    #define DISABLE_IRQ_BLOCK(s)                    \
        do                                          \
        {                                           \
            const BOOL preIRQ = OS_DisableIrq();    \
            (s) ;                                   \
            (void)OS_RestoreIrq(preIRQ);            \
        }while(FALSE)
#else
    #define DISABLE_IRQ_BLOCK(s)    (s)
#endif

#if defined(_WIN32)
    #define GetVarFromRBMem(app, rb)                    NNSi_McsReadFromRingBufferMem(&(app), (uint32_t)&(rb), sizeof(app))
    #define SetVarToRBMem(rb, app)                      NNSi_McsWriteToRingBufferMem((uint32_t)&(rb), &(app), sizeof(rb))

    #define ReadFromRBMem(appAddr, rbAddr, bytes)       NNSi_McsReadFromRingBufferMem(appAddr, rbAddr, bytes)
    #define WriteToRBMem(rbAddr, appAddr, bytes)        NNSi_McsWriteToRingBufferMem(rbAddr, appAddr, bytes)
#else
    #define GetVarFromRBMem(app, rb)                    ((app) = (rb))
    #define SetVarToRBMem(rb, app)                      ((rb) = (app))

    #define ReadFromRBMem(appAddr, rbAddr, bytes)       MI_CpuCopy8((const void*)rbAddr, appAddr, bytes)
    #define WriteToRBMem(rbAddr, appAddr, bytes)        MI_CpuCopy8(appAddr, (void*)rbAddr, bytes)
#endif

#define RefU32(p)   (*(uint32_t*)(p))
#define RefCU32(p)  (*(const uint32_t*)(p))


/* ========================================================================
    static functions
   ======================================================================== */

static INLINE uint32_t
RoundUp(
    uint32_t    value,
    uint32_t    alignment
)
{
    return (value + alignment -1) & ~(alignment -1);
}

static INLINE uint32_t
RoundDown(
    uint32_t    value,
    uint32_t    alignment
)
{
    return value & ~(alignment -1);
}

static INLINE uint32_t
GetMin(
    uint32_t    a,
    uint32_t    b
)
{
    return a < b ? a: b;
}

static INLINE NNSiMcsUIntPtr
GetRingBufferBufferEnd(const NNSiMcsBufRgn* pBRgn)
{
    return pBRgn->buf + pBRgn->bufSize;
}

static INLINE NNSiMcsUIntPtr
GetRingBufferPtrInc(
    NNSiMcsUIntPtr          ptr,
    uint32_t                val,
    const NNSiMcsBufRgn*    pBRgn
)
{
    ptr += val;

    if (ptr >= GetRingBufferBufferEnd(pBRgn))
    {
        ptr -= pBRgn->bufSize;
    }

    return ptr;
}

static INLINE NNSiMcsUIntPtr
GetRingBufferPtrDec(
    NNSiMcsUIntPtr          ptr,
    uint32_t                val,
    const NNSiMcsBufRgn*    pBRgn
)
{
    ptr -= val;

    if (ptr < pBRgn->buf)
    {
        ptr += pBRgn->bufSize;
    }

    return ptr;
}

static uint32_t
GetRingBufferReadableBytes(const NNSiMcsMsgRange* pMRng)
{
    if(pMRng->start == pMRng->end)
    {
        return 0;
    }

    {
        uint32_t size;
        GetVarFromRBMem(size, RefCU32(pMRng->start));
        return size;
    }
}

static uint32_t
GetRingBufferWritableBytes(
    const NNSiMcsMsgRange*  pMRng,
    uint32_t                bufSize
)
{
    uint32_t writableBytes = 0;

    if (pMRng->start > pMRng->end)
    {
        writableBytes = pMRng->start - pMRng->end;
    }
    else
    {
        writableBytes = bufSize - (pMRng->end - pMRng->start);
    }

    writableBytes -= sizeof(uint32_t);

    if (writableBytes > 0)
    {
        writableBytes -= sizeof(uint32_t);
    }

    return writableBytes;
}

static void
GetRingBufferHeader(
    NNSiMcsBufRgn*      pBrgn,
    NNSiMcsMsgRange*    pMrng,
    NNSMcsRingBuffer    rb
)
{
    const NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address

    // Copy to local
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(pBrgn->buf    , pRbh->brgn.buf    ),
            GetVarFromRBMem(pBrgn->bufSize, pRbh->brgn.bufSize),
            GetVarFromRBMem(pMrng->end    , pRbh->mrng.end    ),

            GetVarFromRBMem(pMrng->start  , pRbh->mrng.start  )
        )
    );
}

static NNSiMcsUIntPtr
WriteBuffer(
    NNSiMcsUIntPtr          pDst,
    const uint8_t*          pSrc,
    uint32_t                size,
    const NNSiMcsBufRgn*    pBrgn
)
{
    while (size > 0)
    {
        const uint32_t writeByte = GetMin(GetRingBufferBufferEnd(pBrgn) - pDst, size);
        WriteToRBMem(pDst, pSrc, writeByte);
        pDst = GetRingBufferPtrInc(pDst, RoundUp(writeByte, 4), pBrgn);
        pSrc += writeByte;
        size -= writeByte;
    }

    return pDst;
}

static void
SetState(
    NNSMcsRingBuffer    rb,
    uint32_t            addState
)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address
    uint32_t state;

    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(state      , pRbh->state),
            state |= addState,
            SetVarToRBMem  (pRbh->state, state      )
        )
    );
}


/* ========================================================================
    External functions
   ======================================================================== */

NNSMcsRingBuffer
NNS_McsInitRingBuffer(
    NNSiMcsBufPtr   buf,
    uint32_t        bufSize
)
{
    const uint32_t signature = NNS_MCS_RINGBUF_SIGNATURE;
    const uint32_t state = 0;
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)buf;
    NNSiMcsUIntPtr startBuf = (NNSiMcsUIntPtr)buf + sizeof(NNSiMcsRingBufferHeader);
    bufSize -= sizeof(NNSiMcsRingBufferHeader);

    SetVarToRBMem(pRbh->state       , state);
    SetVarToRBMem(pRbh->brgn.buf    , startBuf);
    SetVarToRBMem(pRbh->brgn.bufSize, bufSize);
    SetVarToRBMem(pRbh->mrng.start  , startBuf);
    SetVarToRBMem(pRbh->mrng.end    , startBuf);

    SetVarToRBMem(pRbh->signature   , signature);   // Lastly, set signature (completed initialization)

    return (NNSMcsRingBuffer)pRbh;
}

uint32_t
NNS_McsGetRingBufferReadableBytes(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb;
    NNSiMcsMsgRange mrng;

    // Copy to local
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(mrng.start, pRbh->mrng.start),

            GetVarFromRBMem(mrng.end  , pRbh->mrng.end  )
        )
    );

    return GetRingBufferReadableBytes(&mrng);
}

uint32_t
NNS_McsGetRingBufferTotalReadableBytes(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRgh = (NNSiMcsRingBufferHeader*)rb;
    NNSiMcsBufRgn brgn;
    NNSiMcsMsgRange mrng;

    // Copy to local
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(brgn.buf    , pRgh->brgn.buf    ),
            GetVarFromRBMem(brgn.bufSize, pRgh->brgn.bufSize),
            GetVarFromRBMem(mrng.start  , pRgh->mrng.start  ),

            GetVarFromRBMem(mrng.end    , pRgh->mrng.end    )
        )
    );

    if (mrng.start == mrng.end)
    {
        return 0;
    }

    {
        NNSiMcsUIntPtr msgPtr = mrng.start;
        uint32_t total = 0;

        while (msgPtr != mrng.end)
        {
            uint32_t size;
            GetVarFromRBMem(size, RefCU32(msgPtr));
            total += size;
            msgPtr = GetRingBufferPtrInc(msgPtr, sizeof(uint32_t) + RoundUp(size, 4), &brgn);
        }

        return total;
    }
}

uint32_t
NNS_McsGetRingBufferWritableBytes(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address
    NNSiMcsMsgRange mrng;
    uint32_t bufSize;

    // Copy to local
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(bufSize   , pRbh->brgn.bufSize),
            GetVarFromRBMem(mrng.end  , pRbh->mrng.end  ),

            GetVarFromRBMem(mrng.start, pRbh->mrng.start)
        )
    );

    return GetRingBufferWritableBytes(&mrng, bufSize);
}

BOOL
NNS_McsReadRingBuffer(
    NNSMcsRingBuffer    rb,
    void*               buf,
    uint32_t                 size,
    uint32_t*                pReadBytes
)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address
    NNSiMcsBufRgn brgn;
    NNSiMcsMsgRange mrng;
    uint32_t readableBytes;

    // Copy to local
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(brgn.buf    , pRbh->brgn.buf    ),
            GetVarFromRBMem(brgn.bufSize, pRbh->brgn.bufSize),
            GetVarFromRBMem(mrng.start  , pRbh->mrng.start),

            GetVarFromRBMem(mrng.end    , pRbh->mrng.end  )
        )
    );

    readableBytes = GetRingBufferReadableBytes(&mrng);
    if (readableBytes == 0)
    {
        *pReadBytes = 0;
        return TRUE;
    }

    {
        BOOL bRet = TRUE;

        uint8_t* pDst = (uint8_t*)buf;
        NNSiMcsUIntPtr pSrc = GetRingBufferPtrInc(mrng.start, sizeof(uint32_t), &brgn);
        uint32_t restSize;

        if (readableBytes > size)
        {
            // Because size header is added, when total cannot be read, use 4-byte alignment
            restSize = RoundDown(size, 4);
        }
        else
        {
            restSize = readableBytes;
        }
        *pReadBytes = restSize;

        while (restSize > 0)
        {
            const uint32_t readByte = GetMin(GetRingBufferBufferEnd(&brgn) - pSrc, restSize);
            ReadFromRBMem(pDst, pSrc, readByte);
            pSrc = GetRingBufferPtrInc(pSrc, RoundUp(readByte, 4), &brgn);
            pDst += readByte;
            restSize -= readByte;
        }

        if (*pReadBytes != readableBytes)
        {
            const uint32_t newSize = readableBytes - *pReadBytes;
            pSrc = GetRingBufferPtrDec(pSrc, sizeof(uint32_t), &brgn);   // Go back by uint32_t
            SetVarToRBMem(RefU32(pSrc), newSize);                   // Set the remaining size

            bRet = FALSE;
        }

        DISABLE_IRQ_BLOCK( SetVarToRBMem(pRbh->mrng.start, pSrc) );

        return bRet;
    }
}

/*
    Make sure that end point is not buf + bufSize. In this case, indicate buf.
    
    When writing, end should have a different value than start. There should always be a space of 4 bytes between start and end.
*/
BOOL
NNS_McsWriteRingBuffer(
    NNSMcsRingBuffer    rb,
    const void*         buf,
    uint32_t            size
)
{
    NNSiMcsBufRgn brgn;
    NNSiMcsMsgRange mrng;

    // Copy to local
    GetRingBufferHeader(&brgn, &mrng, rb);

    if (size > GetRingBufferWritableBytes(&mrng, brgn.bufSize))
    {
        SetState(rb, NNS_MCS_RINGBUF_OVERFLOW);
        return FALSE;
    }

    SetVarToRBMem(RefU32(mrng.end), size);

    {
        NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address
        NNSiMcsUIntPtr pDst = GetRingBufferPtrInc(mrng.end, sizeof(uint32_t), &brgn);
        pDst = WriteBuffer(pDst, (const uint8_t*)buf, size, &brgn);
        DISABLE_IRQ_BLOCK( SetVarToRBMem(pRbh->mrng.end, pDst) );
    }

    return TRUE;
}

BOOL
NNS_McsWriteRingBufferEx(
    NNSMcsRingBuffer    rb,
    const void*         buf,
    uint32_t            offset,
    uint32_t            size,
    uint32_t            totalSize
)
{
    NNSiMcsBufRgn brgn;
    NNSiMcsMsgRange mrng;

    // Copy to local
    GetRingBufferHeader(&brgn, &mrng, rb);

    if (offset + size > GetRingBufferWritableBytes(&mrng, brgn.bufSize))
    {
        SetState(rb, NNS_MCS_RINGBUF_OVERFLOW);
        return FALSE;
    }

    SetVarToRBMem(RefU32(mrng.end), totalSize);

    {
        NNSiMcsUIntPtr pDst = GetRingBufferPtrInc(mrng.end, sizeof(uint32_t) + offset, &brgn);
        pDst = WriteBuffer(pDst, (const uint8_t*)buf, size, &brgn);
        if (offset + size == totalSize)
        {
            NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address
            DISABLE_IRQ_BLOCK( SetVarToRBMem(pRbh->mrng.end, pDst) );
        }
    }

    return TRUE;
}

void
NNS_McsClearRingBuffer(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb;
    NNSiMcsUIntPtr endPtr;
    const uint32_t state = 0;

    // Set end pointer value to start pointer and clear the state
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(endPtr          , pRbh->mrng.end),
            SetVarToRBMem  (pRbh->mrng.start, endPtr        ),
            SetVarToRBMem  (pRbh->state     , state         )
        )
    );
}

BOOL
NNS_McsCheckRingBuffer(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address
    uint32_t signature;

    DISABLE_IRQ_BLOCK( GetVarFromRBMem(signature, pRbh->signature) );

    return signature == NNS_MCS_RINGBUF_SIGNATURE;
}

uint32_t
NNS_McsGetRingBufferState(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address
    uint32_t state;

    DISABLE_IRQ_BLOCK( GetVarFromRBMem(state, pRbh->state) );

    return state;
}

void
NNS_McsClearRingBufferState(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // Target address
    const uint32_t state = 0;

    DISABLE_IRQ_BLOCK( SetVarToRBMem(pRbh->state, state) );
}


