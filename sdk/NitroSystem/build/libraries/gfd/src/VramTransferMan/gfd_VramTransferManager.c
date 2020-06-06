/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation

  File: gfd_VramTransferManager.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_VramTransferManager.c,v $
  Revision 1.6  10/18/2004 12:28:06 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.5  07/22/2004 11:32:13 AM  kitani_toshikazu
  Added assert.

  Revision 1.4  07/15/2004 09:43:15 AM  kitani_toshikazu
  Made the manager a singleton
  Added a method to find the sum of the transmission sizes

  Revision 1.3  2004/07/12 11:07:36 AM  hatamoto_minoru
  Added a transmission destination.
  Modified the queue operations

  Revision 1.2  07/08/2004 11:02:41 AM  kitani_toshikazu
  Changed the order of the NNS_GfdRegisterNewVramTransferTask() arguments

  Revision 1.1  07/08/2004 07:17:58 AM  kitani_toshikazu
  Added the VRAM transmission manager


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/gfd/VramTransferMan/gfd_VramTransferManager.h>

//------------------------------------------------------------------------------
// The only manager entity
//------------------------------------------------------------------------------
static NNSGfdVramTransferManager    s_VramTransferManager;


typedef void (*TransferFunc)( const void*, u32, u32 );

//------------------------------------------------------------------------------
static u16
GetNextIndex_( const NNSGfdVramTransferTaskQueue* pQueue, u16 idx )
{
    return (u16)( ( idx + 1 ) % pQueue->lengthOfArray );
}

//------------------------------------------------------------------------------
static BOOL
IsVramTransferTaskQueueFull_( const NNSGfdVramTransferTaskQueue* pQueue )
{
    return (pQueue->numTasks == pQueue->lengthOfArray) ? TRUE: FALSE;
}

//------------------------------------------------------------------------------
static BOOL
IsVramTransferTaskQueueEmpty_( const NNSGfdVramTransferTaskQueue* pQueue )
{
    return (pQueue->numTasks == 0) ? TRUE: FALSE;
}

//------------------------------------------------------------------------------
// Transfer
static void DoTransfer3dTex( const void* pSrc, u32 offset, u32 szByte )
{
    GX_BeginLoadTex();
    GX_LoadTex(pSrc, offset, szByte);
    GX_EndLoadTex();
}

static void DoTransfer3dTexPltt( const void* pSrc, u32 offset, u32 szByte )
{
    GX_BeginLoadTexPltt();
    GX_LoadTexPltt(pSrc, offset, szByte);
    GX_EndLoadTexPltt();
}

static void DoTransfer3dClearImageColor( const void* pSrc, u32 /*offset*/, u32 szByte )
{
    GX_BeginLoadClearImage();
    GX_LoadClearImageColor(pSrc, szByte);
    GX_EndLoadClearImage();
}

static void DoTransfer3dClearImageDepth( const void* pSrc, u32 /*offset*/, u32 szByte )
{
    GX_BeginLoadClearImage();
    GX_LoadClearImageDepth(pSrc, szByte);
    GX_EndLoadClearImage();
}

//------------
static void DoTransfer2dBG0CharMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG0Char(pSrc, offset, szByte);
}

static void DoTransfer2dBG1CharMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG1Char(pSrc, offset, szByte);
}

static void DoTransfer2dBG2CharMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG2Char(pSrc, offset, szByte);
}

static void DoTransfer2dBG3CharMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG3Char(pSrc, offset, szByte);
}

static void DoTransfer2dBG0ScrMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG0Scr(pSrc, offset, szByte);
}

static void DoTransfer2dBG1ScrMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG1Scr(pSrc, offset, szByte);
}

static void DoTransfer2dBG2ScrMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG2Scr(pSrc, offset, szByte);
}

static void DoTransfer2dBG3ScrMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG3Scr(pSrc, offset, szByte);
}

static void DoTransfer2dBG2BmpMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG2Bmp(pSrc, offset, szByte);
}

static void DoTransfer2dBG3BmpMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBG3Bmp(pSrc, offset, szByte);
}

static void DoTransfer2dObjPlttMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadOBJPltt(pSrc, offset, szByte);
}

static void DoTransfer2dBGPlttMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadBGPltt(pSrc, offset, szByte);
}

static void DoTransfer2dObjExtPlttMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_BeginLoadOBJExtPltt();
    GX_LoadOBJExtPltt(pSrc, offset, szByte);
    GX_EndLoadOBJExtPltt();
}

static void DoTransfer2dBGExtPlttMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt(pSrc, offset, szByte);
    GX_EndLoadBGExtPltt();
}

static void DoTransfer2dObjOamMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadOAM(pSrc, offset, szByte);
}

static void DoTransfer2dObjCharMain( const void* pSrc, u32 offset, u32 szByte )
{
    GX_LoadOBJ(pSrc, offset, szByte);
}
//------------
static void DoTransfer2dBG0CharSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG0Char(pSrc, offset, szByte);
}

static void DoTransfer2dBG1CharSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG1Char(pSrc, offset, szByte);
}

static void DoTransfer2dBG2CharSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG2Char(pSrc, offset, szByte);
}

static void DoTransfer2dBG3CharSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG3Char(pSrc, offset, szByte);
}

static void DoTransfer2dBG0ScrSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG0Scr(pSrc, offset, szByte);
}

static void DoTransfer2dBG1ScrSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG1Scr(pSrc, offset, szByte);
}

static void DoTransfer2dBG2ScrSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG2Scr(pSrc, offset, szByte);
}

static void DoTransfer2dBG3ScrSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG3Scr(pSrc, offset, szByte);
}

static void DoTransfer2dBG2BmpSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG2Bmp(pSrc, offset, szByte);
}

static void DoTransfer2dBG3BmpSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBG3Bmp(pSrc, offset, szByte);
}

static void DoTransfer2dObjPlttSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadOBJPltt(pSrc, offset, szByte);
}

static void DoTransfer2dBGPlttSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadBGPltt(pSrc, offset, szByte);
}

static void DoTransfer2dObjExtPlttSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_BeginLoadOBJExtPltt();
    GXS_LoadOBJExtPltt(pSrc, offset, szByte);
    GXS_EndLoadOBJExtPltt();
}

static void DoTransfer2dBGExtPlttSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt(pSrc, offset, szByte);
    GXS_EndLoadBGExtPltt();
}

static void DoTransfer2dObjOamSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadOAM(pSrc, offset, szByte);
}

static void DoTransfer2dObjCharSub( const void* pSrc, u32 offset, u32 szByte )
{
    GXS_LoadOBJ(pSrc, offset, szByte);
}
//------------


static void
DoTransfer_( const NNSGfdVramTransferTask* pTask )
{
    const static TransferFunc transFunc[NNS_GFD_DST_MAX] = {
        DoTransfer3dTex,
        DoTransfer3dTexPltt,
        DoTransfer3dClearImageColor,
        DoTransfer3dClearImageDepth,

        DoTransfer2dBG0CharMain,
        DoTransfer2dBG1CharMain,
        DoTransfer2dBG2CharMain,
        DoTransfer2dBG3CharMain,
        DoTransfer2dBG0ScrMain,
        DoTransfer2dBG1ScrMain,
        DoTransfer2dBG2ScrMain,
        DoTransfer2dBG3ScrMain,
        DoTransfer2dBG2BmpMain,
        DoTransfer2dBG3BmpMain,
        DoTransfer2dObjPlttMain,
        DoTransfer2dBGPlttMain,
        DoTransfer2dObjExtPlttMain,
        DoTransfer2dBGExtPlttMain,
        DoTransfer2dObjOamMain,
        DoTransfer2dObjCharMain,

        DoTransfer2dBG0CharSub,
        DoTransfer2dBG1CharSub,
        DoTransfer2dBG2CharSub,
        DoTransfer2dBG3CharSub,
        DoTransfer2dBG0ScrSub,
        DoTransfer2dBG1ScrSub,
        DoTransfer2dBG2ScrSub,
        DoTransfer2dBG3ScrSub,
        DoTransfer2dBG2BmpSub,
        DoTransfer2dBG3BmpSub,
        DoTransfer2dObjPlttSub,
        DoTransfer2dBGPlttSub,
        DoTransfer2dObjExtPlttSub,
        DoTransfer2dBGExtPlttSub,
        DoTransfer2dObjOamSub,
        DoTransfer2dObjCharSub,
    };
    TransferFunc doTransfer;
    NNS_GFD_NULL_ASSERT( pTask );
    SDK_MINMAX_ASSERT( pTask->type, 0, NNS_GFD_DST_MAX-1 );

    doTransfer = transFunc[pTask->type];

    DC_FlushRange( pTask->pSrc, pTask->szByte );
    doTransfer( pTask->pSrc, pTask->dstAddr, pTask->szByte );
}

//------------------------------------------------------------------------------
// Queue operation
static void ResetTaskQueue_( NNSGfdVramTransferTaskQueue* pQueue )
{
    NNS_GFD_NULL_ASSERT( pQueue );
    
    pQueue->idxFront    = pQueue->idxRear = 0;
    pQueue->numTasks    = 0;
    pQueue->totalSize   = 0;
}

//------------------------------------------------------------------------------
// Queue operation
BOOL
NNSi_GfdPushVramTransferTaskQueue( NNSGfdVramTransferTaskQueue* pQueue )
{
    NNS_GFD_NULL_ASSERT( pQueue );
    
    if( !IsVramTransferTaskQueueFull_( pQueue ) )
    {
        pQueue->idxRear = GetNextIndex_( pQueue, pQueue->idxRear );
        pQueue->numTasks++;
        return TRUE;
    }else{
        return FALSE;
    }
}

//------------------------------------------------------------------------------
// Queue operation
NNSGfdVramTransferTask*
NNSi_GfdGetFrontVramTransferTaskQueue
(
    NNSGfdVramTransferTaskQueue* pQueue
)
{
    NNS_GFD_NULL_ASSERT( pQueue );
    
    return &pQueue->pTaskArray[ pQueue->idxFront ];
}

//------------------------------------------------------------------------------
// Queue operation
NNSGfdVramTransferTask*
NNSi_GfdGetEndVramTransferTaskQueue
(
    NNSGfdVramTransferTaskQueue* pQueue
)
{
    NNS_GFD_NULL_ASSERT( pQueue );
    
    return &pQueue->pTaskArray[ pQueue->idxRear ];
}

//------------------------------------------------------------------------------
// Queue operation
BOOL
NNSi_GfdPopVramTransferTaskQueue( NNSGfdVramTransferTaskQueue* pQueue )
{
    NNS_GFD_NULL_ASSERT( pQueue );
    
    if( !IsVramTransferTaskQueueEmpty_( pQueue ) )
    {
        pQueue->idxFront = GetNextIndex_( pQueue, pQueue->idxFront );
        pQueue->numTasks--;
        return TRUE;
    }else{
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_GfdInitVramTransferManager

  Description:  This initializes the VRAM transfer manager.

  Arguments:    pTaskArray:       VRAM transfer task array  (used as an internal buffer)
                lengthOfArray:       Length of VRAM transfer task array (used as internal buffer)

  Returns:      None

 *---------------------------------------------------------------------------*/
void
NNS_GfdInitVramTransferManager
(
    NNSGfdVramTransferTask*    pTaskArray,
    u32                        lengthOfArray
)
{
    NNS_GFD_NULL_ASSERT( pTaskArray );
    NNS_GFD_NON_ZERO_ASSERT( lengthOfArray );

    {
        NNSGfdVramTransferManager* pMan = &s_VramTransferManager;
        
        pMan->taskQueue.pTaskArray      = pTaskArray;
        pMan->taskQueue.lengthOfArray   = lengthOfArray;

        ResetTaskQueue_( &pMan->taskQueue );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdDoVramTransfer

  Description:  Runs registered VRAM transfer tasks.

  Arguments:    None


  Returns:      None

 *---------------------------------------------------------------------------*/
void
NNS_GfdDoVramTransfer( )
{
    NNSGfdVramTransferManager* pMan = &s_VramTransferManager;
    NNSGfdVramTransferTask* pTask
        = NNSi_GfdGetFrontVramTransferTaskQueue( &pMan->taskQueue );

    while( NNSi_GfdPopVramTransferTaskQueue( &pMan->taskQueue ) )
    {
        DoTransfer_( pTask );
        
        pMan->taskQueue.totalSize -= pTask->szByte;
        
        pTask = NNSi_GfdGetFrontVramTransferTaskQueue( &pMan->taskQueue );
    }    
}


/*---------------------------------------------------------------------------*
  Name:         NNS_GfdRegisterNewVramTransferTask

  Description:  Registers new VRAM transfer tasks.

  Arguments:    type    :       The type of the transmission destination
                dstAddr :       The transmission destination address
                pSrc    :       Pointer to the transfer source data
                szByte:       The size of the transmission

  Returns:      Register succeeded or failed (If succeeded TRUE).

 *---------------------------------------------------------------------------*/
BOOL
NNS_GfdRegisterNewVramTransferTask
(
    
    NNS_GFD_DST_TYPE            type,
    u32                         dstAddr,
    void*                       pSrc,
    u32                         szByte
)
{
    NNSGfdVramTransferManager*  pMan = &s_VramTransferManager;
    
    if( !IsVramTransferTaskQueueFull_( &pMan->taskQueue ) )
    {
        NNSGfdVramTransferTask* pTask =
            NNSi_GfdGetEndVramTransferTaskQueue( &pMan->taskQueue );

        pTask->type     = type;
        pTask->pSrc     = pSrc;
        pTask->dstAddr  = dstAddr;
        pTask->szByte   = szByte;

        (void)NNSi_GfdPushVramTransferTaskQueue( &pMan->taskQueue );
        
        pMan->taskQueue.totalSize += pTask->szByte;

        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdGetVramTransferTaskTotalSize

  Description:  Obtains total size of registered transfer tasks.

  Arguments:    None

  Returns:      Total size of registered transfer tasks

 *---------------------------------------------------------------------------*/
u32 
NNS_GfdGetVramTransferTaskTotalSize()
{
    return s_VramTransferManager.taskQueue.totalSize;
}


