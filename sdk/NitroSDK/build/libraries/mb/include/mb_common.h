/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_common.h,v $
  Revision 1.22  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.21  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.20  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.19  01/27/2005 07:05:51  takano_makoto
  fix copyright header.

  Revision 1.18  01/27/2005 07:04:12  takano_makoto
  small fix.

  Revision 1.17  11/24/2004 13:03:54  takano_makoto
  Moved MBCommParentBlockHeader and MBCommChildBlockHeader to mb_block.h. Added MBErrorCallback.

  Revision 1.16  11/22/2004 12:54:59  takano_makoto
  Moved MBCommRequestData to $(NITROSDK_ROOT)/include/mb/mb.h.

  Revision 1.15  10/05/2004 14:41:06  yosizaki
  export some MB_COMM_P_* range consts.

  Revision 1.14  2004/09/28 08:56:17  sato_masaki
  Added MB_COMM_P_SENDLEN_MIN and MB_COMM_P_RECVLEN_MIN definitions.

  Revision 1.13  2004/09/20 12:54:11  sato_masaki
  Added the MB_SENDFUNC_STATE_ERR definition.

  Revision 1.12  2004/09/18 12:00:38  sato_masaki
  Set reservations in the Child Device send header definition.    

  Revision 1.11  2004/09/15 06:51:08  sato_masaki
  Deleted the USE_POLLBMP definition.

  Revision 1.10  2004/09/15 06:20:15  sato_masaki
  - Made the define definition that was dependent upon the definitions of MB_COMM_P_SENDLEN and MB_COMM_C_SENDLEN into a macro.
  - MB_COMM_P_SENDLEN_DEFAULT, MB_COMM_P_RECVLEN_DEFAULT        (8)     
    MB_COMM_P_SENDLEN_MAX and MB_COMM_P_RECVLEN_MAX.     

  Revision 1.9  2004/09/14 13:19:50  sato_masaki
  Changed to send request data in fragments.

  Revision 1.8  2004/09/13 13:06:29  sato_masaki
  remove member word from MB_CommBlockHeader

  Revision 1.7  2004/09/11 11:11:14  sato_masaki
  - Added a member to the MB_CommUserReq type.
  - Added sendbuf to the arguments of MBi_BlockHeaderBegin() and MBi_BlockHeaderEnd().
  - Added MBi_DebugPrint

  Revision 1.6  2004/09/10 02:59:10  sato_masaki
  PRINT_DEBUG, Made changes to the CALLBACK_WM_STATE definition.

  Revision 1.5  2004/09/09 02:50:29  sato_masaki
  Changed the argument type of the debug function and MBi_comm_type_output() to u16.

  Revision 1.4  2004/09/08 04:35:12  sato_masaki
  Deleted the fileid from MB_CommBlockHeader.

  Revision 1.3  2004/09/04 07:04:11  sato_masaki
  small fix

  Revision 1.2  2004/09/04 07:01:28  sato_masaki
  Added MB_COMM_TYPE_PARENT_MEMBER_FULL to MB_CommType.

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __MB_COMMON_H__
#define __MB_COMMON_H__

/* macro definition -------------------------------------------------------- */

// for debug
#ifdef	PRINT_DEBUG
#define MB_COMM_TYPE_OUTPUT             MBi_comm_type_output
#define MB_COMM_WMEVENT_OUTPUT          MBi_comm_wmevent_output
#else
#define MB_COMM_TYPE_OUTPUT( ... )      ((void)0)
#define MB_COMM_WMEVENT_OUTPUT( ... )   ((void)0)
#endif

/* Send / Receive size definitions*/
#define MB_COMM_P_SENDLEN_DEFAULT       (256)
#define MB_COMM_P_RECVLEN_DEFAULT       (8)

#define MB_COMM_P_SENDLEN_MAX           MB_COMM_PARENT_SEND_MAX
#define MB_COMM_P_RECVLEN_MAX           MB_COMM_PARENT_RECV_MAX

#define MB_COMM_P_SENDLEN_MIN           (sizeof(MBDownloadFileInfo))    // min size
#define MB_COMM_P_RECVLEN_MIN           MB_COMM_PARENT_RECV_MIN

#define MB_COMM_REQ_DATA_SIZE           (29)

/* Calculation macros relating to fragment request data*/
#define MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ )       (( __P_RECVLEN__ ) - 2)
#define MB_COMM_CALC_REQ_DATA_PIECE_NUM( __P_RECVLEN__ )        \
                                ( (MB_COMM_REQ_DATA_SIZE + 1) / (MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ )) )
#define MB_COMM_CALC_REQ_DATA_BUF_SIZE( __P_RECVLEN__ )     (MB_COMM_REQ_DATA_SIZE + 1)
/*
 *  There are logical errors, but the IPL runs on these specifications, so
 the errors cannot be corrected. *   It's supposed to be:
 * #define MB_COMM_CALC_REQ_DATA_PICE_NUM( __P_RECV_LEN__)                                                            \
 *                                  ( (MB_COMM_REQ_DATA_SIZE + MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ ) - 1) \
 *                                    / MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ ) )
 * #define MB_COMM_CALC_REQ_DATA_BUF_SIZE( __P_RECV_LEN__ )                                 \
 *                                  ( MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ )     \
 *                                    * MB_COMM_CALC_REQ_DATA_PIECE_NUM( __P_RECVLEN__ ) )
 */


/* Block header size*/
#define MB_COMM_PARENT_HEADER_SIZE      (6)     // the size of MB_CommParentBlockHeader  (without the padding)
#define MB_COMM_CHILD_HEADER_SIZE       (8)     // the size of MB_CommChildBlockHeader  (without the padding)

#define MB_COMM_CALC_BLOCK_SIZE( __P_SENDLEN__ )                (( __P_SENDLEN__ ) - MB_COMM_PARENT_HEADER_SIZE)

/* Definition of error returned by data-transfer function
   This is defined with a value that will not take the WM ERRCODE.     */
#define MB_SENDFUNC_STATE_ERR           (WM_ERRCODE_MAX + 1)

/* Block transfer data types*/
typedef enum MBCommType
{
    MB_COMM_TYPE_DUMMY = 0,            //  0

    MB_COMM_TYPE_PARENT_SENDSTART,     //  1
    MB_COMM_TYPE_PARENT_KICKREQ,       //  2
    MB_COMM_TYPE_PARENT_DL_FILEINFO,   //  3
    MB_COMM_TYPE_PARENT_DATA,          //  4
    MB_COMM_TYPE_PARENT_BOOTREQ,       //  5
    MB_COMM_TYPE_PARENT_MEMBER_FULL,   //  6

    MB_COMM_TYPE_CHILD_FILEREQ,        //  7
    MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO, //  8
    MB_COMM_TYPE_CHILD_CONTINUE,       //  9
    MB_COMM_TYPE_CHILD_STOPREQ,        //  10
    MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED //  11
}
MBCommType;

/* Request from user types*/
typedef enum MBCommUserReq
{
    MB_COMM_USER_REQ_NONE = 0,         //  0
    MB_COMM_USER_REQ_DL_START,         //  1
    MB_COMM_USER_REQ_SEND_START,       //  2
    MB_COMM_USER_REQ_ACCEPT,           //  3
    MB_COMM_USER_REQ_KICK,             //  4
    MB_COMM_USER_REQ_BOOT              //  5
}
MBCommUserReq;

/* structure definition ---------------------------------------------------- */

/*
 * Error notifying callback
 */
typedef struct
{
    u16     apiid;                     // API code
    u16     errcode;                   // error code
}
MBErrorCallback;


/* functions --------------------------------------------------------------- */

/*
 *  Initializes the block header, and only configures the type.
 * In the interval after this until MBi_BlockHeaderEnd() sending,    
 * start to fill up the fragmented fields.     
 *  OK to leave as-is if there is no argument.
 */
void    MBi_BlockHeaderBegin(u8 type, u32 *sendbuf);

/*
 *  Completes the block header configuration and calculates the checksum.
 * After doing so, actually sends to the partner designated by pollbmp.
 */
int     MBi_BlockHeaderEnd(int body_len, u16 pollbmp, u32 *sendbuf);


/*
 *  checksum calculation
 */
u16     MBi_calc_cksum(const u16 *buf, int length);

// --- for debug
void    MBi_DebugPrint(const char *file, int line, const char *func, const char *fmt, ...);
void    MBi_comm_type_output(u16 type);
void    MBi_comm_wmevent_output(u16 type, void *arg);

#endif /* __MB_COMMON_H__ */
