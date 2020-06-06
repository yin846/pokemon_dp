/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_block.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_block.c,v $
  Revision 1.4  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.3  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.2  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.1  11/22/2004 12:38:39  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "mb_common.h"
#include "mb_block.h"
#include "mb_private.h"


//============================================================================
//  Function Prototype Declarations
//============================================================================
static u8 *MBi_ReceiveRequestDataPiece(const MBCommChildBlockHeader * hdr, u16 child);
static BOOL IsGetAllRequestData(u16 child);


//============================================================================
//  Declare variables
//============================================================================

// division request fragment size and number    
static struct
{
    int     size;
    int     num;
    int     bufSize;
}
req_data_piece;

// for child
// division request send index
static u8 req_data_piece_idx = 0;

// for parent
// split request receive buffer
static MbRequestPieceBuf *req_buf;


//============================================================================
//  function definitions
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         MBi_SetChildMPMaxSize
  
  Description:  Sets the transmission data size for child and calculates
                on the production prototype of the machine.
  
  Arguments:    childMaxSize child send size
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_SetChildMPMaxSize(u16 childMaxSize)
{
    req_data_piece.size = MB_COMM_CALC_REQ_DATA_PIECE_SIZE(childMaxSize);
    req_data_piece.num = MB_COMM_CALC_REQ_DATA_PIECE_NUM(childMaxSize);
    req_data_piece.bufSize = MB_COMM_CALC_REQ_DATA_BUF_SIZE(childMaxSize);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_SetParentPieceBuffer

  Description:  Sets division request receive buffer of parent.
  
  Arguments:    buf   pointer to receive buffer
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_SetParentPieceBuffer(MbRequestPieceBuf * buf)
{
    req_buf = buf;
    MI_CpuClear8(req_buf, sizeof(MbRequestPieceBuf));
}


/*---------------------------------------------------------------------------*
  Name:         MBi_ClearParentPieceBuffer

  Description:  Clears division request receive buffer.
  
  Arguments:    child_aid   child AID that clears buffer
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_ClearParentPieceBuffer(u16 child_aid)
{
    if (req_buf == NULL)
    {
        return;
    }

    MI_CpuClear8(req_buf->data_buf[child_aid - 1],
                 MB_COMM_CALC_REQ_DATA_BUF_SIZE(req_data_piece.bufSize));
    req_buf->data_bmp[child_aid - 1] = 0;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_MakeParentSendBuffer

  Description:  Builds data to actually send from send header information of parent.    

  Arguments:    hdr     pointer to parent send header
                sendbuf   pointer to buffer that creates send data

  Returns:      pointer to send buffer
 *---------------------------------------------------------------------------*/
u8     *MBi_MakeParentSendBuffer(const MBCommParentBlockHeader * hdr, u8 *sendbuf)
{
    u8     *ptr = sendbuf;

    *ptr++ = hdr->type;

    switch (hdr->type)
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:        //  1
        break;
    case MB_COMM_TYPE_PARENT_KICKREQ: //  2
        break;
    case MB_COMM_TYPE_PARENT_DL_FILEINFO:      //  3
        break;
    case MB_COMM_TYPE_PARENT_DATA:    //  4
        *ptr++ = (u8)(0x00ff & hdr->fid);       // Lo
        *ptr++ = (u8)((0xff00 & hdr->fid) >> 8);        // Hi
        *ptr++ = (u8)(0x00ff & hdr->seqno);     // Lo
        *ptr++ = (u8)((0xff00 & hdr->seqno) >> 8);      // Hi
        break;
    case MB_COMM_TYPE_PARENT_BOOTREQ: //  5
        break;
    case MB_COMM_TYPE_PARENT_MEMBER_FULL:      //  6
        break;
    default:
        return NULL;
    }

    return ptr;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_SetRecvBufferFromChild

  Description:  Gets header part from packet received from child, and returns pointer to data part.
                the nth block header, where n = "idx".

  Arguments:    hdr     pointer to variable for getting header
                recvbuf      pointer to receive buffer

  Returns:      pointer to data part
 *---------------------------------------------------------------------------*/
u8     *MBi_SetRecvBufferFromChild(const u8 *recvbuf, MBCommChildBlockHeader * hdr, u16 child_id)
{
    u8     *ptr = (u8 *)recvbuf;

    hdr->type = *ptr++;

    switch (hdr->type)
    {
    case MB_COMM_TYPE_CHILD_FILEREQ:
        // restores fragmented data
        if (IsGetAllRequestData(child_id))
        {
            return (u8 *)req_buf->data_buf[child_id - 1];
        }

        hdr->req_data.piece = *ptr++;
        if (hdr->req_data.piece > req_data_piece.num)
        {
            return NULL;               // error
        }
        MI_CpuCopy8(ptr, hdr->req_data.data, (u32)req_data_piece.size);
        ptr = MBi_ReceiveRequestDataPiece(hdr, child_id);
        break;
    case MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO:
        hdr->data.req = (u16)(0x00ff & (*ptr++));
        hdr->data.req |= (((u16)(*ptr++) << 8) & 0xff00);
        break;
    case MB_COMM_TYPE_CHILD_CONTINUE:
        hdr->data.req = (u16)(0x00ff & (*ptr++));
        hdr->data.req |= (((u16)(*ptr++) << 8) & 0xff00);
        MI_CpuCopy8(ptr, hdr->data.reserved, (u32)req_data_piece.size);
        ptr += req_data_piece.size;
        break;
    default:
        return NULL;
    }

    return ptr;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_ReceiveRequestDataPiece

  Description:  Builds entirety from division request data received from child.    

  Arguments:    hdr     pointer to variable for getting header
                recvbuf      pointer to receive buffer

  Returns:      pointer to data when all division request data is together
                NULL when not yet together
 *---------------------------------------------------------------------------*/
static u8 *MBi_ReceiveRequestDataPiece(const MBCommChildBlockHeader * hdr, u16 child)
{
    u8      piece;
    u8     *ptr;

    if (req_buf == NULL)
    {
        return NULL;
    }

    piece = hdr->req_data.piece;

    if (piece > req_data_piece.num)
    {
        return NULL;
    }

    ptr = ((u8 *)req_buf->data_buf[child - 1]) + (piece * req_data_piece.size);

    MI_CpuCopy8(&hdr->req_data.data[0], ptr, (u32)req_data_piece.size);

    req_buf->data_bmp[child - 1] |= (1 << piece);

    MB_DEBUG_OUTPUT(" %02x %02x %02x %02x %02x %02x\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4],
                    ptr[5]);

    if (IsGetAllRequestData(child))
    {
        return (u8 *)req_buf->data_buf[child - 1];
    }

    return NULL;
}


/*---------------------------------------------------------------------------*
  Name:         

  Description:  Builds entirety from division request data received from child.    

  Arguments:    hdr     pointer to variable for getting header
                recvbuf      pointer to receive buffer

  Returns:      TRUE when all division request data is together    
                FALSE when not yet together    
 *---------------------------------------------------------------------------*/
static BOOL IsGetAllRequestData(u16 child)
{
    u16     i;

    /* Determine whether Pieces are collected*/
    for (i = 0; i < req_data_piece.num; i++)
    {
        if ((req_buf->data_bmp[child - 1] & (1 << i)) == 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}



/*---------------------------------------------------------------------------*
  Name:         MBi_MakeChildSendBuffer

  Description:  Builds data to actually send from send header information of child.    

  Arguments:    hdr     pointer to child send header
                sendbuf   pointer to buffer that creates send data

  Returns:      pointer to send buffer
 *---------------------------------------------------------------------------*/
u8     *MBi_MakeChildSendBuffer(const MBCommChildBlockHeader * hdr, u8 *sendbuf)
{
    u8     *pbuf = sendbuf;

    *pbuf++ = hdr->type;
    switch (hdr->type)
    {
    case MB_COMM_TYPE_CHILD_FILEREQ:
        *pbuf++ = hdr->req_data.piece;
        if (hdr->req_data.piece > req_data_piece.num)
        {
            return NULL;               // error
        }

        MI_CpuCopy8((void *)(hdr->req_data.data), (void *)pbuf, (u32)req_data_piece.size);
        pbuf += req_data_piece.size;
        break;
    case MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO:
        break;
    case MB_COMM_TYPE_CHILD_CONTINUE:
        *pbuf++ = (u8)(0x00ff & hdr->data.req); // Lo
        *pbuf++ = (u8)((0xff00 & hdr->data.req) >> 8);  // Hi
        MI_CpuCopy8((void *)(hdr->data.reserved), (void *)pbuf, (u32)req_data_piece.size);
        pbuf += req_data_piece.size;
        break;
    case MB_COMM_TYPE_CHILD_STOPREQ:
        break;
    case MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED:
        break;
    default:
        return NULL;
    }

    return pbuf;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_SendRequestDataPiece

  Description:  Divides request data to send from child.

  Arguments:    pData     pointer that gets division data
                pReq      request data to divide

  Returns:      index value of obtained division data
 *---------------------------------------------------------------------------*/
u8 MBi_SendRequestDataPiece(u8 *pData, const MBCommRequestData *pReq)
{
    const u8 *ptr = (u8 *)pReq;

    /* Determine the send Piece*/
    req_data_piece_idx = (u8)((req_data_piece_idx + 1) % req_data_piece.num);
    MB_DEBUG_OUTPUT("req_data piece : %d\n", req_data_piece_idx);

    // Copy data to the send buffer
    MI_CpuCopy8((void *)&ptr[req_data_piece_idx * req_data_piece.size],
                pData, (u32)req_data_piece.size);

    MB_DEBUG_OUTPUT(" %02x %02x %02x %02x %02x %02x\n", pData[0], pData[1], pData[2], pData[3],
                    pData[4], pData[5]);

    return req_data_piece_idx;
}



/*---------------------------------------------------------------------------*
  Name:         MBi_SetRecvBufferFromParent

  Description:  Gets header part from packet received from parent and returns
                the nth block header, where n = "idx".

  Arguments:    hdr     pointer to variable for getting header
                recvbuf      pointer to receive buffer

  Returns:      pointer to data part
 *---------------------------------------------------------------------------*/
u8     *MBi_SetRecvBufferFromParent(MBCommParentBlockHeader * hdr, const u8 *recvbuf)
{

    hdr->type = *recvbuf++;

    switch (hdr->type)
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:
        break;
    case MB_COMM_TYPE_PARENT_KICKREQ:
        break;
    case MB_COMM_TYPE_PARENT_DL_FILEINFO:
        break;
    case MB_COMM_TYPE_PARENT_DATA:
        hdr->fid = (u16)(*recvbuf++);  // Lo
        hdr->fid |= ((u16)(*recvbuf++) << 8);   // Hi
        hdr->seqno = (u16)(*recvbuf++); // Lo
        hdr->seqno |= ((u16)(*recvbuf++) << 8); // Hi
        break;
    case MB_COMM_TYPE_PARENT_BOOTREQ:
        break;
    case MB_COMM_TYPE_PARENT_MEMBER_FULL:
        break;
    default:
        return NULL;
    }
    return (u8 *)recvbuf;
}
