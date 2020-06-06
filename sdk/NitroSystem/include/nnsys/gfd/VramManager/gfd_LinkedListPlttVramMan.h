/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_LinkedListPlttVramMan.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_LinkedListPlttVramMan.h,v $
  Revision 1.6  08/17/2005 11:53:48 PM  kitani_toshikazu
  Revised copyright notation

  Revision 1.5  08/17/2005 08:08:55 AM  kitani_toshikazu
  Added a function for debug display

  Revision 1.4  10/18/2004 12:28:23 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.3  09/24/2004 09:37:18 AM  kitani_toshikazu
  Name of NNS_GfdGetLnkPlttVramManagerManagementWorkSize() was changed to
  =>NNS_GfdGetLnkPlttVramManagerWorkSize(). 

  Revision 1.2  09/17/2004 11:35:12 AM  kitani_toshikazu
  Erased the header include for internal module

  Revision 1.1  09/10/2004 10:46:31 AM  kitani_toshikazu
  Registration of the initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_LINKEDLISTPLTTVRAMMAN_H_ 
#define NNS_GFD_LINKEDLISTPLTTVRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_PlttVramMan_Types.h>



//------------------------------------------------------------------------------
// Function Declaration
//------------------------------------------------------------------------------
void NNS_GfdDumpLnkPlttVramManager();

u32 NNS_GfdGetLnkPlttVramManagerWorkSize( u32 numMemBlk );

void NNS_GfdInitLnkPlttVramManager
( 
    u32     szByte, 
    void*   pManagementWork, 
    u32     szByteManagementWork,
    BOOL    useAsDefault
);

NNSGfdPlttKey   NNS_GfdAllocLnkPlttVram( u32 szByte, BOOL b4Pltt, u32 opt );
int             NNS_GfdFreeLnkPlttVram( NNSGfdPlttKey plttKey );
void            NNS_GfdResetLnkPlttVramState( void );


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_LINKEDLISTPLTTVRAMMAN_H_

