/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  
  File: gfd_VramTransferManager.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_VramTransferManager.h,v $
  Revision 1.7  10/18/2004 12:28:23 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.6  07/28/2004 12:40:59 AM  kitani_toshikazu
  Correspondence with the padding warning

  Revision 1.5  2004/07/15 10:43:50  nishida
  Revised so that a function is void when there is no temporary argument

  Revision 1.4  07/15/2004 09:42:54 AM  kitani_toshikazu
  Made the manager a singleton
  Added a method to find the sum of the transmission sizes

  Revision 1.3  2004/07/12 11:07:42  hatamoto_minoru
  Added a transmission destination.
  Modified the queue operations

  Revision 1.2  07/08/2004 11:03:00 AM  kitani_toshikazu
  Changed the order of the NNS_GfdRegisterNewVramTransferTask() arguments

  Revision 1.1  07/08/2004 07:18:45 AM  kitani_toshikazu
  Added the VRAM transmission manager


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_GFD_VRAM_TRANSFER_MANAGER_H_
#define NNS_GFD_VRAM_TRANSFER_MANAGER_H_

#include <nitro.h>
#include <nnsys/gfd/gfd_common.h>

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         NNS_GFD_DST_TYPE

  Description:  The type of the transmission destination
 *---------------------------------------------------------------------------*/
// see also DoTransfer_()
typedef enum NNS_GFD_DST_TYPE
{
    NNS_GFD_DST_3D_TEX_VRAM = 0,        // 3D texture
    NNS_GFD_DST_3D_TEX_PLTT,            // 3D texture palette
    NNS_GFD_DST_3D_CLRIMG_COLOR,        // 3D clear image color
    NNS_GFD_DST_3D_CLRIMG_DEPTH,        // 3D clear image depth
    
    NNS_GFD_DST_2D_BG0_CHAR_MAIN,       // 2D Main BG0 character
    NNS_GFD_DST_2D_BG1_CHAR_MAIN,       // 2D Main BG1 character
    NNS_GFD_DST_2D_BG2_CHAR_MAIN,       // 2D Main BG2 character
    NNS_GFD_DST_2D_BG3_CHAR_MAIN,       // 2D Main BG3 character
    NNS_GFD_DST_2D_BG0_SCR_MAIN,        // 2D Main BG0 screen
    NNS_GFD_DST_2D_BG1_SCR_MAIN,        // 2D Main BG1 screen
    NNS_GFD_DST_2D_BG2_SCR_MAIN,        // 2D Main BG2 screen
    NNS_GFD_DST_2D_BG3_SCR_MAIN,        // 2D Main BG3 screen
    NNS_GFD_DST_2D_BG2_BMP_MAIN,        // 2D Main BG2 bitmap
    NNS_GFD_DST_2D_BG3_BMP_MAIN,        // 2D Main BG3 bitmap
    NNS_GFD_DST_2D_OBJ_PLTT_MAIN,       // 2D Main OBJ palette
    NNS_GFD_DST_2D_BG_PLTT_MAIN,        // 2D Main BG palette
    NNS_GFD_DST_2D_OBJ_EXTPLTT_MAIN,    // 2D Main OBJ extended palette
    NNS_GFD_DST_2D_BG_EXTPLTT_MAIN,     // 2D Main BG extended palette
    NNS_GFD_DST_2D_OBJ_OAM_MAIN,        // 2D Main OBJ OAM
    NNS_GFD_DST_2D_OBJ_CHAR_MAIN,       // 2D Main OBJ character
    
    NNS_GFD_DST_2D_BG0_CHAR_SUB,        // 2D Sub BG0 character
    NNS_GFD_DST_2D_BG1_CHAR_SUB,        // 2D Sub BG1 character
    NNS_GFD_DST_2D_BG2_CHAR_SUB,        // 2D Sub BG2 character
    NNS_GFD_DST_2D_BG3_CHAR_SUB,        // 2D Sub BG3 character
    NNS_GFD_DST_2D_BG0_SCR_SUB,         // 2D Sub BG0 screen
    NNS_GFD_DST_2D_BG1_SCR_SUB,         // 2D Sub BG1 screen
    NNS_GFD_DST_2D_BG2_SCR_SUB,         // 2D Sub BG2 screen
    NNS_GFD_DST_2D_BG3_SCR_SUB,         // 2D Sub BG3 screen
    NNS_GFD_DST_2D_BG2_BMP_SUB,         // 2D Sub BG2 bitmap
    NNS_GFD_DST_2D_BG3_BMP_SUB,         // 2D Sub BG3 bitmap
    NNS_GFD_DST_2D_OBJ_PLTT_SUB,        // 2D Sub OBJ palette
    NNS_GFD_DST_2D_BG_PLTT_SUB,         // 2D Sub BG palette
    NNS_GFD_DST_2D_OBJ_EXTPLTT_SUB,     // 2D Sub OBJ extended palette
    NNS_GFD_DST_2D_BG_EXTPLTT_SUB,      // 2D Sub BG extended palette
    NNS_GFD_DST_2D_OBJ_OAM_SUB,         // 2D Sub OBJ OAM
    NNS_GFD_DST_2D_OBJ_CHAR_SUB,        // 2D Sub OBJ character
    
    NNS_GFD_DST_MAX

}NNS_GFD_DST_TYPE;

/*---------------------------------------------------------------------------*
  Name:         NNSGfdVramTransferTask

  Description:  The transmission task
 *---------------------------------------------------------------------------*/
typedef struct NNSGfdVramTransferTask
{
    NNS_GFD_DST_TYPE        type;       // The type of the transmission destination
    void*                   pSrc;       // The data address of the transmission source in Main memory
    u32                     dstAddr;    // The transmission destination address
    u32                     szByte;     // The size of the transmission
    
}NNSGfdVramTransferTask;

/*---------------------------------------------------------------------------*
  Name:         NNSGfdVramTransferTaskQueue

  Description:  The transmission task queue
 *---------------------------------------------------------------------------*/
typedef struct NNSGfdVramTransferTaskQueue
{
    NNSGfdVramTransferTask*     pTaskArray;     // The task array
    u32                         lengthOfArray;  // The length of the task array
    
    u16                         idxFront;       // The fetch position
    u16                         idxRear;        // The entry position
    u16                         numTasks;       // The number of tasks
    u16                         pad16_;         // The padding
    u32                         totalSize;      // The total amount to transmit
    
}NNSGfdVramTransferTaskQueue;

/*---------------------------------------------------------------------------*
  Name:         NNSGfdVramTransferManager

  Description:  The transmission task manager
 *---------------------------------------------------------------------------*/
typedef struct NNSGfdVramTransferManager
{
    NNSGfdVramTransferTaskQueue         taskQueue; // The transmission task queue

}NNSGfdVramTransferManager;





//------------------------------------------------------------------------------
// public ( internal )
//------------------------------------------------------------------------------
//
// The task queue-related handling relationship
//
BOOL 
NNSi_GfdPushVramTransferTaskQueue
( 
    NNSGfdVramTransferTaskQueue* pQueue 
);

NNSGfdVramTransferTask* 
NNSi_GfdGetFrontVramTransferTaskQueue
(
    NNSGfdVramTransferTaskQueue* pQueue
);

NNSGfdVramTransferTask* 
NNSi_GfdGetEndVramTransferTaskQueue
(
    NNSGfdVramTransferTaskQueue* pQueue
);

BOOL 
NNSi_GfdPopVramTransferTaskQueue
( 
    NNSGfdVramTransferTaskQueue* pQueue 
);




//------------------------------------------------------------------------------
// public
//------------------------------------------------------------------------------
//
// Initialization
//
void 
NNS_GfdInitVramTransferManager
( 
    NNSGfdVramTransferTask*    pTaskArray,
    u32                        lengthOfArray
);

//
// Start transmission
// Execution of the registration tasks
//
void 
NNS_GfdDoVramTransfer( void );

//
// Register the tasks
//
BOOL 
NNS_GfdRegisterNewVramTransferTask
(
    NNS_GFD_DST_TYPE            type,
    u32                         dstAddr,
    void*                       pSrc,
    u32                         szByte
);
//
// Get the total amount of transmission
//
u32 
NNS_GfdGetVramTransferTaskTotalSize( void );


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_VRAM_TRANSFER_MANAGER_H_

