/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_wm.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_block.h,v $
  Revision 1.5  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.4  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.3  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.2  11/24/2004 13:03:22  takano_makoto
  Removed MBCommParentBlockHeader and MBCommChildBlockHeader from mb_common.h.

  Revision 1.1  11/22/2004 12:47:52  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef _MB_BLOCK_H_
#define _MB_BLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/mb.h>
#include <nitro/wm.h>
#include "mb_common.h"


/* ----------------------------------------------------------------------- *
 *  Structure Definitions
 * ----------------------------------------------------------------------- */

/*
 * transfer block header (header added to each block)
   Actually sends/receives filled with Padding.
 */
typedef struct
{
    u8      type;                      // data type                             1B
    u8      pad1[1];
    u16     fid;                       // file number value                            4B
    u16     seqno;                     // parent use                                  6B
}
MBCommParentBlockHeader;


typedef struct
{
    u8      type;                      // data type                             1B
    u8      pad1[1];                   //                                          1B
    union                              //                              union total 16B
    {
        struct
        {
            u16     req;               // block number request value                       2B
            u8      reserved[MB_COMM_CALC_REQ_DATA_PIECE_SIZE(MB_COMM_P_RECVLEN_MAX)];  // 14B
        }
        data;

        struct
        {
            u8      piece;             //  number of fragmented data                1B     
            //  buffer storing fragmented data
            u8      data[MB_COMM_CALC_REQ_DATA_PIECE_SIZE(MB_COMM_P_RECVLEN_MAX)];      //  14B
            u8      pad2[1];           //  1B
        }
        req_data;
    };
    // Total is 18B
}
MBCommChildBlockHeader;


// split request receive buffer
typedef struct
{
    u32     data_buf[MB_MAX_CHILD][MB_COMM_CALC_REQ_DATA_BUF_SIZE(MB_COMM_P_RECVLEN_MAX) /
                                   sizeof(u32) + 1];
    u32     data_bmp[MB_MAX_CHILD];
}
MbRequestPieceBuf;

/* ----------------------------------------------------------------------- *
 *  Function Declarations
 * ----------------------------------------------------------------------- */

/* Set child MP transfer size*/
void    MBi_SetChildMPMaxSize(u16 childMaxSize);
/* Set parent buffer for receiving division request*/
void    MBi_SetParentPieceBuffer(MbRequestPieceBuf * buf);
/* Clear the buffer for receiving  division request*/
void    MBi_ClearParentPieceBuffer(u16 child_aid);

/* Build actual data for sending using parent's send header information*/
u8     *MBi_MakeParentSendBuffer(const MBCommParentBlockHeader * hdr, u8 *sendbuf);
/* Build structure from data buffer received from child*/
u8     *MBi_SetRecvBufferFromChild(const u8 *recvbuf, MBCommChildBlockHeader * hdr, u16 child_id);

/* Build actual data for sending using child's send header information*/
u8     *MBi_MakeChildSendBuffer(const MBCommChildBlockHeader * hdr, u8 *sendbuf);
/* Divides request data to send from child. */
u8      MBi_SendRequestDataPiece(u8 *pData, const MBCommRequestData *pReq);
/* Get the header portion of the packet received from parent and return pointer to the data portion */
u8     *MBi_SetRecvBufferFromParent(MBCommParentBlockHeader * hdr, const u8 *recvbuf);


#ifdef __cplusplus
}
#endif

#endif /*  _MB_BLOCK_H_    */
