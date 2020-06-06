/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_fileinfo.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_fileinfo.h,v $
  Revision 1.13  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.12  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.11  2004/09/25 12:10:38  sato_masaki
  Added MBi_IsAbleToRecv(). (Re-named from IsAbleToRecv())

  Revision 1.10  2004/09/20 12:53:46  sato_masaki
  Changed the return value of MBi_get_blockinfo() to a BOOL value.

  Revision 1.9  2004/09/14 06:34:21  sato_masaki
  Added a reserved region in MBDownloadFileInfo.

  Revision 1.8  2004/09/07 04:33:46  sato_masaki
  Added segment_no to MB_BlockInfo.

  Revision 1.7  09/06/2004 09:20:01  miya
  Deleted definitions

  Revision 1.6  09/06/2004 07:14:22  miya
  Changes due to specification changes.

  Revision 1.5  09/06/2004 03:48:08  miya
  Added an authentication library.

  Revision 1.4  09/06/2004 02:36:00  miya
  Definition changes due to specification changes.

  Revision 1.3  2004/09/04 11:03:00  sato_masaki
  Moved the definition of ROM_HEADER_SIZE_FULL from mb_fileinfo.c.

  Revision 1.2  2004/09/04 09:56:32  sato_masaki
  Moved the definitions related to the segment and the DownloadFileInfo to here.

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	__MB_FILEINFO_H__
#define	__MB_FILEINFO_H__

#include <nitro.h>
#include <nitro/mb.h>

/* macro definition -------------------------------------------------------- */

#define MB_DL_SEGMENT_NUM				(3)     //      three segments : RomHeader, ARM9Static, ARM7Static
#define MB_DOWNLOAD_FILEINFO_SIZE		(sizeof(MBDownloadFileInfo))

#define MB_AUTHCODE_SIZE				(136)   /* Authentication code size */

#define ROM_HEADER_SIZE_FULL			(0x160)

/* type structure definition --------------------------------------------------------- */

//---------------------------------------------------------
// download file information for multiboot sending from the Parent Device to the Child Device
//---------------------------------------------------------

/*
 *  segment type define
 */
typedef enum MbSegmentType
{
    MB_SEG_ARM9STATIC = 0,
    MB_SEG_ARM7STATIC,
    MB_SEG_ROMHEADER
}
MbSegmentType;

/*
 *  the multiboot initial boot segment information header structure
 */
typedef struct MbDownloadFileInfoHeader
{
    u32     arm9EntryAddr;             /* ARM9 entry address*/
    u32     arm7EntryAddr;             /* ARM7 entry address*/
    u32     padding;
}
MbDownloadFileInfoHeader;

/*
 *  segment composition information
 */
typedef struct MbSegmentInfo
{
    u32     recv_addr;                 // temporary storage address at the time of reception
    u32     load_addr;                 // load address  (execution address)
    u32     size;                      // segment size
    u32     target:1;                  // Target (MI_PROCESSOR_ARM9 or _ARM7. File without attribute is ARM9)
    u32     rsv:31;                    // reserved
}
MbSegmentInfo;                         // 16 byte


/*
 * the overall structure of the initial boot segment     
 */
typedef struct
{
    MbDownloadFileInfoHeader header;   // header information ( the entry address is stored here)
    MbSegmentInfo seg[MB_DL_SEGMENT_NUM];       // segment information
    u32     auth_code[MB_AUTHCODE_SIZE / sizeof(u32)];  // code for authentication
    u32     reserved[32 / sizeof(u32)]; // reserved region
}
MBDownloadFileInfo;


/* Table for referencing block information*/
typedef struct
{
    // the offset from the top of the image in each segment
    u32     seg_src_offset[MB_DL_SEGMENT_NUM];
    u16     seg_head_blockno[MB_DL_SEGMENT_NUM];        // the top block number of the segment
    u16     block_num;                 /* Total number of blocks*/
}
MB_BlockInfoTable;


/* Block transfer-related data*/
typedef struct
{
    u32     child_address;             // the storage address of the Child Device
    u32     size;                      // the block size
    u32     offset;                    // set so that it is held in the offset value from the top of the image
    u8      segment_no;
    u8      pad[3];
}
MB_BlockInfo;

/* const valriables -------------------------------------------------------- */

extern const MbSegmentType MBi_defaultLoadSegList[MB_DL_SEGMENT_NUM];

/* functions --------------------------------------------------------------- */

/*
 * Get the segment information from
 * the pointer to MbDownloadFileInfoHeader
 */
static inline MbSegmentInfo *MBi_GetSegmentInfo(MbDownloadFileInfoHeader * mdfi, int i)
{
    return ((MbSegmentInfo *) (mdfi + 1)) + i;
}

BOOL    MBi_MakeBlockInfoTable(MB_BlockInfoTable * table, MbDownloadFileInfoHeader * mdfi);
BOOL    MBi_get_blockinfo(MB_BlockInfo * bi, MB_BlockInfoTable * table, u32 block,
                          MbDownloadFileInfoHeader * mdfi);
u16     MBi_get_blocknum(MB_BlockInfoTable * table);
BOOL    MBi_IsAbleToRecv(u8 segment_no, u32 address, u32 size);

#endif /* __MB_FILEINFO_H__ */
