/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_child.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_child.h,v $
  Revision 1.28  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.27  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.26  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.25  11/22/2004 12:53:17  takano_makoto
  Moved MbGameInfoXXX related type definitions and function definitions elsewhere.

  Revision 1.24  10/28/2004 11:01:42  yada
  just fix comment

  Revision 1.23  10/21/2004 08:52:09  takano_makoto
  Fixed errors in comments.

  Revision 1.22  09/28/2004 12:50:06  yosiokat
  Merged with IPL branch 1.17.2.4.

  Revision 1.17.2.4  2004/09/28 02:38:56  yosiokat
  Added beaconNo and sameBeaconRecvCount to MbGameInfoRecvList.

  Revision 1.21  09/20/2004 10:39:13  yosiokat
  Merged with IPL branch Rev.1.17.2.3

  Revision 1.17.2.3  2004/09/20 10:17:25  yosiokat
  Deleted seqNoFixed and seqNoVolat from MbGameInfoRecvList. (Revised so that it uses the same seqNoFixed and seqNoVolat inside the gameInfo on the receive side)

  Revision 1.17.2.2  2004/09/18 08:46:19  miya
  Merged with HEAD

  Revision 1.20  2004/09/16 12:51:02  sato_masaki
  - changed the functions and types that had been defined as MBi_*** to MB_***.

  Revision 1.19  09/16/2004 12:20:54  miya
  Conversions for file updates     

  Revision 1.18  09/16/2004 08:43:32  miya
  Addition of members for file updates

  Revision 1.17  09/15/2004 06:55:38  yosiokat
  Added u16 usefulGameInfoFlag to MbBeaconRecvStatus

  Revision 1.16  2004/09/13 13:18:10  sato_masaki
  modify pad bytes

  Revision 1.15  09/13/2004 09:25:39  yosiokat
  - Added notFoundLockTargetCount to MbBeaconRecvStatus as a means of dealing with a state where no lock targets are found at all when scan locking.
  - Revised MbGameInfoRecvList.lifeTimeCount to s16

  Revision 1.14  2004/09/13 08:11:41  sato_masaki
  MBiError, MBiErrorStatus are moved to mb.h

  Revision 1.13  2004/09/13 02:56:58  sato_masaki
  Added MBiErrorStatus and MBiError

  Revision 1.12  09/11/2004 12:39:27  yosiokat
  - Modified the MbBeaconRecvStatus element along with the revisions to the lock time and the parent device information lifespan count process

  Revision 1.11  2004/09/11 11:13:06  sato_masaki
  - defined the buffer size  MB_CHILD_SYSTEM_BUF_SIZE for the child device
  - added the MB_GetChildSystemBufSize() function which gets the used work memory size

  Revision 1.10  2004/09/10 06:32:13  sato_masaki
  Added comment.

  Revision 1.9  2004/09/09 13:52:26  sato_masaki
  Addition of the communication strength display linkLevel

  Revision 1.8  2004/09/09 10:05:10  sato_masaki
  Added GameInfo -related items to MBCommCState

  Revision 1.7  09/09/2004 04:34:56  yosiokat
  Added MB_CARD_ROM_HEADER_ADDRESS

  Revision 1.6  2004/09/08 12:52:22  sato_masaki
  Added MB_COMM_CSTATE_ERROR

  Revision 1.5  2004/09/07 04:34:22  sato_masaki
  small fix

  Revision 1.4  2004/09/06 11:12:25  sato_masaki
  moved the multiboot RAM address definition (used internally) from mb_private.h

  Revision 1.3  2004/09/04 09:55:26  sato_masaki
  Added a definition for the child device that remained in mb.h

  Revision 1.2  2004/09/03 12:05:13  sato_masaki
  Corrected comments

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/* ==========================================================================

    This is a header for the child device of the MBlibrary.
    When using this library in the multiboot child device and IPL,
    add it to nitro/mb.h and include this header.

   ==========================================================================*/


#ifndef _MB_CHILD_H_
#define _MB_CHILD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mb.h>
#include "mb_fileinfo.h"
#include "mb_gameinfo.h"

/* ---------------------------------------------------------------------

        define constants

   ---------------------------------------------------------------------*/

#define MB_CHILD_SYSTEM_BUF_SIZE        (0x6000)

#define MB_MAX_SEND_BUFFER_SIZE         (0x400)
#define MB_MAX_RECV_BUFFER_SIZE         (0x80)

/* ----------------------------------------------------------------------------

    multiboot RAM address definition (used internally)

   ----------------------------------------------------------------------------*/

/* 
    Multiboot program allocatable range

    The maximum initial load size of the ARM9 code in wireless multiboot is 2.5 MB.
    
    The loadable range of the ARM9 code is
    MB_LOAD_AREA_LO to MB_LOAD_AREA_HI
     
 */
#define MB_LOAD_AREA_LO                         ( HW_MAIN_MEM )
#define MB_LOAD_AREA_HI                         ( HW_MAIN_MEM + 0x002c0000 )
#define MB_LOAD_MAX_SIZE                        ( MB_LOAD_AREA_HI - MB_LOAD_AREA_LO )


/*
    Temporary receive buffer address definition on child side of ARM7 static during multiboot     
    
    The loadable range of ARM7 code is any of the following:

    a) 0x02000000 - 0x02300000
       (MB_LOAD_AREA_LO - MB_ARM7_STATIC_RECV_BUFFER_END)
    b) 0x02300000 - 0x023fe000
       (MB_ARM7_STATIC_RECV_BUFFER_END - 0x023fe000)
    c) 0x037f8000 - 0x0380f000
       (internal WRAM)

     
    
    * NOTE
    When load ARM7 code in internal WRAM or 0x02300000 or later,    

    be sure create buffer in 0x022c0000 - 0x02300000     
    (MB_LOAD_AREA_HI - MB_ARM7_STATIC_RECV_BUFFER_END),
    receive to this buffer, and subsequently reallocate to specified address     
    during boot.    
    
    ARM7 code arranged across 0x02300000 address    
    For 0x02300000 and later addresses, we cannot guarantee normal operation of    
      ARM7 code exceeding size of 0x40000 ( MB_ARM7_STATIC_RECV_BUFFER_SIZE ).    
      Therefore the creation of this kind of code has been prohibited.    
 */

#define MB_ARM7_STATIC_RECV_BUFFER              MB_LOAD_AREA_HI
#define MB_ARM7_STATIC_RECV_BUFFER_END          ( HW_MAIN_MEM + 0x00300000 )
#define MB_ARM7_STATIC_RECV_BUFFER_SIZE         ( MB_ARM7_STATIC_RECV_BUFFER_END - MB_LOAD_AREA_HI )
#define MB_ARM7_STATIC_LOAD_AREA_HI             ( MB_LOAD_AREA_HI + MB_ARM7_STATIC_RECV_BUFFER_SIZE )
#define MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE       ( 0x18000 - 0x1000 )    // 0x18000 = HW_WRAM_SIZE + HW_PRV_WRAM_SIZE

/* Place for temporary storage of BssDesc*/
#define MB_BSSDESC_ADDRESS                      ( HW_MAIN_MEM + 0x003fe800 )
/* Place for temporary storage of MBDownloadFileInfo*/
#define MB_DOWNLOAD_FILEINFO_ADDRESS            ( MB_BSSDESC_ADDRESS + ( ( ( MB_BSSDESC_SIZE ) + (32) - 1 ) & ~((32) - 1) ) )
/* Place for saving ROM header of inserted ROM card at time of multiboot startup*/
#define MB_CARD_ROM_HEADER_ADDRESS              ( MB_DOWNLOAD_FILEINFO_ADDRESS + ( ( ( MB_DOWNLOAD_FILEINFO_SIZE ) + (32) - 1 ) & ~((32) - 1) ) )
/* Place for storage of ROM Header*/
#define MB_ROM_HEADER_ADDRESS                   HW_ROM_HEADER_BUF


/* ---------------------------------------------------------------------

        The API for the multiboot library (MB) child device. Used by IPL.

   ---------------------------------------------------------------------*/

/* Set child parameters and start*/
int     MB_StartChild(void);

/* Get the size of the work memory being used by the MB child*/
int     MB_GetChildSystemBufSize(void);

/* Set the callback for posting the state of the child*/
void    MB_CommSetChildStateCallback(MBCommCStateCallbackFunc callback);

/* Get the state of the child*/
int     MB_CommGetChildState(void);

/* Start download*/
BOOL    MB_CommStartDownload(void);

/* Get the extent of progress of the child's download process (percentage 0 to 100)*/
u16     MB_GetChildProgressPercentage(void);

/* Try connecting to parent and request a download file*/
int     MB_CommDownloadRequest(int index);      // designate the parent device that requests the connection with the list number of MbBeaconRecvStatus

#ifdef __cplusplus
}
#endif

#endif /*  _MB_CHILD_H_    */
