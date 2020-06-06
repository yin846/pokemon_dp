/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_common.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_common.c,v $
  Revision 1.18  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.17  11/07/2005 01:21:50  okubata_ryoma
  Switched from SDK_STATIC_ASSERT to SDK_COMPILER_ASSERT

  Revision 1.16  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.15  02/21/2005 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.14  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.13  11/22/2004 12:41:18  takano_makoto
  Fixed debug print bug.

  Revision 1.12  11/09/2004 12:02:50  yosizaki
  fix MB_Get*SystemBufSize().

  Revision 1.11  11/02/2004 10:11:43  yosizaki
  fix TYPE-string.

  Revision 1.10  2004/09/16 12:49:40  sato_masaki
  In mb.h, changed the functions and structures defined as MBi_*** to MB_***

  Revision 1.9  2004/09/15 06:51:20  sato_masaki
  Deleted the USE_POLLBMP definition.

  Revision 1.8  2004/09/14 13:19:44  sato_masaki
  Changed to send request data in fragments.

  Revision 1.7  2004/09/13 13:09:10  sato_masaki
  remove member word from MB_CommBlockHeader

  Revision 1.6  2004/09/13 02:59:47  sato_masaki
  Changed MB_GetParentSystemBufSize to MBi_GetParentSystemBufSize    
  Changed each MB_GetChildSystemBufSize to MBi_GetChildSystemBufSize.    

  Revision 1.5  2004/09/11 11:20:05  sato_masaki
  - Added sendbuf to the arguments of MBi_BlockHeaderBegin() and MBi_BlockHeaderEnd().
  - Added MB_GetParentSystemBufSize() and MB_GetChildSystemBufSize().
  - Added MBi_DebugPrint (for debug)

  Revision 1.4  2004/09/10 02:52:50  sato_masaki
  Made changes related to the PRINT_DEBUG definition.

  Revision 1.3  2004/09/09 02:50:50  sato_masaki
  Changed the argument type of the debug function and MBi_comm_type_output() to u16.

  Revision 1.2  2004/09/06 09:52:11  sato_masaki
  small fix

  Revision 1.1  2004/09/03 07:04:36  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"

// ----------------------------------------------------------------------------
// definition
#define MY_ROUND(n, a)      (((u32) (n) + (a) - 1) & ~((a) - 1))

#define MB_PARENT_WORK_SIZE_MIN (32 + sizeof(MBiParam) + 32 + sizeof(MB_CommPWork) + 32 + WM_SYSTEM_BUF_SIZE)
#define MB_CHILD_WORK_SIZE_MIN  (32 + sizeof(MBiParam) + 32 + sizeof(MB_CommCWork) + 32 + WM_SYSTEM_BUF_SIZE)
/*
 * Determine with requested size whether capacity is correct.    
 */
SDK_COMPILER_ASSERT(MB_PARENT_WORK_SIZE_MIN <= MB_SYSTEM_BUF_SIZE);
SDK_COMPILER_ASSERT(MB_CHILD_WORK_SIZE_MIN <= MB_CHILD_SYSTEM_BUF_SIZE);


/*
 *  Initializes the block header, and only configures the type.
 * In the interval after this until MBi_BlockHeaderEnd() sending,    
 * start to fill up the fragmented fields.     
 *  OK to leave as-is if there is no argument.
 */
void MBi_BlockHeaderBegin(u8 type, u32 *sendbuf)
{
    u8     *p = (u8 *)sendbuf;
    p[0] = type;                       /* Parent / child share type*/
}

/*
 *  Completes the block header configuration and calculates the checksum.
 * After doing so, actually sends to the partner designated by pollbmp.
 */
int MBi_BlockHeaderEnd(int body_len, u16 pollbmp, u32 *sendbuf)
{
    /*
     * MY_ROUND may be needed if sendbuf is 32 aligned.
     * A check has already been added in Init().
     * Take more at the end and try to align internally.
     */
    DC_FlushRange(sendbuf, MY_ROUND(body_len, 32));
    DC_WaitWriteBufferEmpty();

    MB_DEBUG_OUTPUT("SEND (BMP:%04x)", pollbmp);
    MB_COMM_TYPE_OUTPUT(((u8 *)sendbuf)[0]);

    return MBi_SendMP(sendbuf, body_len, pollbmp);
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetParentSystemBufSize

  Description:  Gets the size of the work memory used by MB.( Parent Device )

  Arguments:    None.
  
  Returns:      The size of the work memory used by the MB Parent Device.
 *---------------------------------------------------------------------------*/
int MB_GetParentSystemBufSize(void)
{
    return MB_PARENT_WORK_SIZE_MIN;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetChildSystemBufSize

  Description:  Gets the size of the work memory used by MB. (Child Device )

  Arguments:    None.
  
  Returns:      the size of the work memory used by the MB parent device
 *---------------------------------------------------------------------------*/
int MB_GetChildSystemBufSize(void)
{
    return MB_CHILD_WORK_SIZE_MIN;
}


u16 MBi_calc_cksum(const u16 *buf, int length)
{
    u32     sum;
    int     nwords = length >> 1;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (u16)(sum ^ 0xffff);
}

/*  ============================================================================

    for debug

    ============================================================================*/

#ifdef	PRINT_DEBUG

void MBi_DebugPrint(const char *file, int line, const char *func, const char *fmt, ...)
{
    va_list vlist;

    OS_TPrintf("func: %s [%s:%d]:\n", func, file, line);

    va_start(vlist, fmt);
    OS_TVPrintf(fmt, vlist);
    va_end(vlist);

    OS_TPrintf("\n");
}

void MBi_comm_type_output(u16 type)
{
    enum
    { MB_TYPE_STRING_NUM = 12 };
    static const char *const mb_type_string[MB_TYPE_STRING_NUM] = {
        "MB_COMM_TYPE_DUMMY",          //      0

        "MB_COMM_TYPE_PARENT_SENDSTART",        //      1
        "MB_COMM_TYPE_PARENT_KICKREQ", //      2
        "MB_COMM_TYPE_PARENT_DL_FILEINFO",      //      3
        "MB_COMM_TYPE_PARENT_DATA",    //      4
        "MB_COMM_TYPE_PARENT_BOOTREQ", //      5
        "MB_COMM_TYPE_PARENT_MEMBER_FULL",      //      6

        "MB_COMM_TYPE_CHILD_FILEREQ",  //      7
        "MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO",   //      8
        "MB_COMM_TYPE_CHILD_CONTINUE", //      9
        "MB_COMM_TYPE_CHILD_STOPREQ",  //      10
        "MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED",  //      11
    };
    if (type >= MB_TYPE_STRING_NUM)
    {
        MB_OUTPUT("TYPE: unknown\n");
        return;
    }
    MB_OUTPUT("TYPE: %s\n", mb_type_string[type]);
}

void MBi_comm_wmevent_output(u16 type, void *arg)
{
    enum
    { MB_CB_STRING_NUM = 43 };
    static const char *const mb_cb_string[MB_CB_STRING_NUM + 2] = {
        "MB_CALLBACK_CHILD_CONNECTED", //              0
        "MB_CALLBACK_CHILD_DISCONNECTED",       //              1
        "MB_CALLBACK_MP_PARENT_SENT",  //              2
        "MB_CALLBACK_MP_PARENT_RECV",  //              3
        "MB_CALLBACK_PARENT_FOUND",    //              4
        "MB_CALLBACK_PARENT_NOT_FOUND", //              5
        "MB_CALLBACK_CONNECTED_TO_PARENT",      //              6
        "MB_CALLBACK_DISCONNECTED",    //              7
        "MB_CALLBACK_MP_CHILD_SENT",   //              8
        "MB_CALLBACK_MP_CHILD_RECV",   //              9
        "MB_CALLBACK_DISCONNECTED_FROM_PARENT", //              10
        "MB_CALLBACK_CONNECT_FAILED",  //              11
        "MB_CALLBACK_DCF_CHILD_SENT",  //              12
        "MB_CALLBACK_DCF_CHILD_SENT_ERR",       //              13
        "MB_CALLBACK_DCF_CHILD_RECV",  //              14
        "MB_CALLBACK_DISCONNECT_COMPLETE",      //              15
        "MB_CALLBACK_DISCONNECT_FAILED",        //              16
        "MB_CALLBACK_END_COMPLETE",    //              17
        "MB_CALLBACK_MP_CHILD_SENT_ERR",        //              18
        "MB_CALLBACK_MP_PARENT_SENT_ERR",       //              19
        "MB_CALLBACK_MP_STARTED",      //              20
        "MB_CALLBACK_INIT_COMPLETE",   //              21
        "MB_CALLBACK_END_MP_COMPLETE", //              22
        "MB_CALLBACK_SET_GAMEINFO_COMPLETE",    //              23
        "MB_CALLBACK_SET_GAMEINFO_FAILED",      //              24
        "MB_CALLBACK_MP_SEND_ENABLE",  //              25
        "MB_CALLBACK_PARENT_STARTED",  //              26
        "MB_CALLBACK_BEACON_LOST",     //              27
        "MB_CALLBACK_BEACON_SENT",     //              28
        "MB_CALLBACK_BEACON_RECV",     //              29
        "MB_CALLBACK_MP_SEND_DISABLE", //              30
        "MB_CALLBACK_DISASSOCIATE",    //              31
        "MB_CALLBACK_REASSOCIATE",     //              32
        "MB_CALLBACK_AUTHENTICATE",    //              33
        "MB_CALLBACK_SET_LIFETIME",    //              34
        "MB_CALLBACK_DCF_STARTED",     //              35
        "MB_CALLBACK_DCF_SENT",        //              36
        "MB_CALLBACK_DCF_SENT_ERR",    //              37
        "MB_CALLBACK_DCF_RECV",        //              38
        "MB_CALLBACK_DCF_END",         //              39
        "MB_CALLBACK_MPACK_IND",       //              40
        "MB_CALLBACK_MP_CHILD_SENT_TIMEOUT",    //              41
        "MB_CALLBACK_SEND_QUEUE_FULL_ERR",      //              42
        "MB_CALLBACK_API_ERROR",       //              255
        "MB_CALLBACK_ERROR",           //              256
    };

    if (type == MB_CALLBACK_API_ERROR)
        type = MB_CB_STRING_NUM;
    else if (type == MB_CALLBACK_ERROR)
        type = MB_CB_STRING_NUM + 1;
    else if (type >= MB_CB_STRING_NUM)
    {
        MB_OUTPUT("EVENTTYPE: unknown\n");
        return;
    }

    MB_OUTPUT("EVENTTYPE:%s\n", mb_cb_string[type]);
    if (arg)
    {
        MB_OUTPUT("\tAPPID:%04x ERRCODE:%04x\n", ((u16 *)arg)[0], ((u16 *)arg)[1]);
        MB_OUTPUT("\twlCmd:%04x wlResult:%04x\n", ((u16 *)arg)[2], ((u16 *)arg)[3]);
    }
}

#endif
