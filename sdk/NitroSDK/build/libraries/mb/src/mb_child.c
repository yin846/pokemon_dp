/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_child.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_child.c,v $
  Revision 1.49  03/08/2006 10:29:53  kitase_hirotake
  Changed so that magicNumber and ver are initialized with the MB_ReadMultiBootParentBssDesc function. 

  Revision 1.48  02/20/2006 02:30:49  seiki_masashi
  Changed name of WMGameInfo.gameNameCount_attribute to attribute 

  Revision 1.47  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.46  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.45  02/21/2005 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.44  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.43  11/22/2004 12:40:02  takano_makoto
  Separated off the division-request transmission process as mb_block

  Revision 1.42  11/10/2004 07:42:00  takano_makoto
  Removed WBw function from inside mb_gameinfo.c.  Separated routine that runs WM and routine that operates partition beacon.  

  Revision 1.41  11/02/2004 18:15:36  takano_makoto
  fix comments.

  Revision 1.39  10/28/2004 11:01:42  yada
  just fix comment

  Revision 1.38  10/12/2004 05:39:21  sato_masaki
  Changed Callback error related to WM_APIID_RESET and WM_APIID_END to MB_ERRCODE_WM_FAILURE.

  Revision 1.37  10/05/2004 10:05:43  sato_masaki
  Added process to identify the MB termination process when receiving BOOT_REQ

  Revision 1.36  10/04/2004 13:40:25  terui
  Standardized game group IDs as type u32.

  Revision 1.35  09/28/2004 08:53:10  sato_masaki
  - Added interrupt control to MB_GetChildProgressPercentage().
  - Added Warnings everywhere.

  Revision 1.34  09/25/2004 12:12:50  sato_masaki
  Changed name of IsAbleToRecv() to MBi_IsAbleToRecv(), and moved it to mb_fileinfo.c.

  Revision 1.33  09/25/2004 05:30:28  sato_masaki
  Added process to prohibit interrupts while API called from main loop is executing.

  Revision 1.32  09/25/2004 04:40:09  sato_masaki
  Merged with NitroSDK_IPL_branch 1.24.2.8.

  Revision 1.31  09/22/2004 09:42:00  sato_masaki
  Merged with IPL_branch 1.24.2.6.
  - rename get_minimum_blockno to get_next_blockno
  - optimize for code-size around MBi_CommChildRecvData().
  - Changed method of looking up not-yet-received data

  Revision 1.30  09/21/2004 01:39:15  sato_masaki
  Standardized errors that originate in WM to MB_ERRCODE_WM_FAILURE. Abolished MB_ERRCODE_MP_SENT_FAILURE.

  Revision 1.29  09/20/2004 13:11:10  sato_masaki
  - Made changes related to MB_COMM_PSTATE_ERROR so that the value of the MBErrCode enumerator is passed to 'arg'.  
  -  In the function for sending, changed the MB_ERROR that was being used to MB_SENDFUNC_STATE_ERR (defined inside the library).

  Revision 1.28  09/18/2004 12:01:11  sato_masaki
  Set 'reserved' in the Child Device send header definition.  

  Revision 1.27  09/17/2004 04:43:31  sato_masaki
  add function MB_ReadMultiBootParentBssDesc()

  Revision 1.26  09/17/2004 04:02:04  miya
  Removed unnecessary parts

  Revision 1.25  09/16/2004 12:49:27  sato_masaki
  In mb.h, changed the functions and structures defined as MBi_*** to MB_***

  Revision 1.24  09/15/2004 06:51:59  sato_masaki
  Deleted processes regarding WBT definitions.

  Revision 1.23  09/15/2004 06:25:29  sato_masaki
  Made variables of parts that were dependent on MB_COMM_P_SENDLEN, MB_COMM_C_SENDLEN definitions.

  Revision 1.22  09/15/2004 00:59:04  sato_masaki
  modify Recv Region check.

  Revision 1.21  09/14/2004 13:19:44  sato_masaki
  Changed to send request data in fragments.

  Revision 1.20  09/14/2004 06:16:19  sato_masaki
  Work clear is only valid when #ifdef SDK_FINALROM.  

  Revision 1.19  09/13/2004 13:14:05  sato_masaki
  - remove member word from MB_CommBlockHeader
  - change cast (WMmpRecvBuf*) -> (WMPortRecvCallback*) in MBi_CommChildRecvData()

  Revision 1.18  09/13/2004 09:26:56  yosiokat
  -  Added wmTypeEvent to the argument of MB_CountGameInfoLifetime as a means to prevent no lock target being found when scan locking.

  Revision 1.17  09/13/2004 08:10:41  sato_masaki
  - Renamed MBi_CommCallError() -> MBi_CommCallChildError()
  - Added GGID to the MBi_CommRequestFile() arguments

  Revision 1.16  09/13/2004 02:58:40  sato_masaki
  Added the MBi_CommCallError() function for calling error callback

  Revision 1.15  09/11/2004 11:17:59  sato_masaki
  change mbc to pCwork

  Revision 1.14  09/10/2004 05:26:51  sato_masaki
  Added MBi_CommChangeChildStateCallbackOnly() function, which sends only callback without changing internal state.
  Used in error notification, etc.

  Revision 1.13  09/10/2004 02:54:44  sato_masaki
  small fix

  Revision 1.12  09/10/2004 02:54:06  sato_masaki
  Made changes to the CALLBACK_WM_STATE definition.

  Revision 1.11  09/10/2004 01:58:17  sato_masaki
  Added a process for assigning mbc->user.playerNo and clearing.

  Revision 1.10  09/09/2004 14:01:47  sato_masaki
  In mb_gameinfo.c, changed the PlayerFlag format. (Ongoing task)

  Revision 1.9  09/09/2004 10:06:21  sato_masaki
  Added GameInfo -related items to MBCommCState

  Revision 1.8  09/08/2004 04:36:14  sato_masaki
  - Deleted field from MB_CommBlockHeader (mb_common.h), and changed Changed the process along with that.

  Revision 1.7  09/07/2004 06:50:12  sato_masaki
  Changed code to pass the arg argument to some callbacks.  
  Added function that evaluates the validity of the receive buffer address with IsAbleToRecv() during block receive.

  Revision 1.6  09/07/2004 04:40:16  sato_masaki
  Changed so that the argument is passed in some callbacks.  
  Added check string to MB_CommStartDownload().

  Revision 1.5  09/06/2004 09:52:39  sato_masaki
  small fix

  Revision 1.4  09/04/2004 11:02:02  sato_masaki
  Added function that clears region that can load code during cancellation.  

  Revision 1.3  09/04/2004 09:54:13  sato_masaki
  Changed code so that pointer to received MBDownloadFileInfo can be specified as an argument to the MB_COMM_CSTATE_DLINFO_ACCEPTED callback .   
   (mainly for debugging)
  
  Revision 1.2  09/04/2004 06:46:15  sato_masaki
  Added a callback state for when the number of members is exceeded (MB_COMM_*STATE_MEMBER_FULL).

  Revision 1.1  09/03/2004 07:04:36  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"


// ----------------------------------------------------------------------------
// definition

#define MY_ROUND(n, a)      (((u32) (n) + (a) - 1) & ~((a) - 1))

// --- for child
static int MBi_CommRequestFile(u8 file_no, u32 ggid);
static void MBi_CommChangeChildState(int state, void *arg);
static void MBi_CommChangeChildStateCallbackOnly(int state, void *arg);
static void MBi_CommChildRecvData(void *arg);
static int MBi_CommChildSendData(void);
static void MBi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index);

// --- miscellany
static BOOL checkRecvFlag(int x);
static void setRecvFlag(int x);
static u16 countRemainingblocks(void);
static u16 get_next_blockno(void);
static BOOL checkPollbmp(u16 pollbmp, u16 child_aid);
static void clearRecvRegion(void);
static void MBi_CommCallChildError(u16 errcode);


/*  ============================================================================

    Child Functions

    ============================================================================*/

/*---------------------------------------------------------------------------*
  Name:         MB_CommDownloadRequest

  Description:  Outputs download request (function becomes operation trigger of the sequence:  
                parent connection -> File request -> download)  
                Packs the following MB_CommDownloadRequest, and does connection request with  
                mbrs list index number.  
                Remembering index number enables clearing of that parent information internally when  
                connection fails.  

  Arguments:    index - The MbBeaconRecvStatus list index number being requested

  Returns:      
 *---------------------------------------------------------------------------*/

int MB_CommDownloadRequest(int index)
{
    const MbBeaconRecvStatus *mbrsp;   // parent information receive status
    const WMBssDesc *bssDescp;         // connection destination parent information pointer
    u8      fileNo;                    //    "        in requested file number
    int     ret;
    OSIntrMode enabled = OS_DisableInterrupts();        /* Interrupts disabled*/

    mbrsp = MB_GetBeaconRecvStatus();  // parent information receive status
    bssDescp = &(mbrsp->list[index].bssDesc);
    fileNo = mbrsp->list[index].gameInfo.fileNo;

    pCwork->connectTargetNo = index;   // Saves parent list number trying to connect
    pCwork->fileid = fileNo;           // register request file ID in advance
    pCwork->last_recv_seq_no = -1;     //initialize last received block sequence number

    // copy bssDescription in work
    MI_CpuCopy16(bssDescp, &pCwork->bssDescbuf, WM_BSS_DESC_SIZE);

    ret = MBi_CommConnectToParent((const WMBssDesc *)&pCwork->bssDescbuf);      // first, enter connection  
    (void)OS_RestoreInterrupts(enabled);        /* Restore interrupts */

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommRequestFile

  Description:  Specifies file number and outputs request.  

  Arguments:    file_no

  Returns:      
 *---------------------------------------------------------------------------*/

static int MBi_CommRequestFile(u8 file_no, u32 ggid)
{
    int     errcode;
    MBCommChildBlockHeader hd;
    u8     *databuf;
    MBCommRequestData req_data;

    /* Register the requested game's GGID, UserInfo and Version*/
    req_data.ggid = ggid;
    MI_CpuCopy8(&pCwork->common.user, &req_data.userinfo, sizeof(MBUserInfo));
    req_data.version = MB_IPL_VERSION;
    req_data.fileid = file_no;

    hd.type = MB_COMM_TYPE_CHILD_FILEREQ;
    // Copy data to the send buffer
    hd.req_data.piece = MBi_SendRequestDataPiece(hd.req_data.data, &req_data);

    databuf = MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);

    if (!databuf)
    {
        return MB_SENDFUNC_STATE_ERR;
    }
    errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, 0xFFFF, pCwork->common.sendbuf);
    return errcode;
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommSetChildStateCallback

  Description:  child event callback settings

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

void MB_CommSetChildStateCallback(MBCommCStateCallbackFunc callback)
{
    OSIntrMode enabled;

    SDK_ASSERT(pCwork != 0);

    enabled = OS_DisableInterrupts();  /* Disable interrupts */

    pCwork->child_callback = callback;

    (void)OS_RestoreInterrupts(enabled);        /* Restore interrupts */
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildState

  Description:  Get Download state

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

int MB_CommGetChildState(void)
{
    if (pCwork)
    {
        return pCwork->c_comm_state;
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetChildProgressPercentage

  Description:  Gets Download progress percentage.

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

u16 MB_GetChildProgressPercentage(void)
{
    OSIntrMode enabled;
    u16     ret = 0;

    enabled = OS_DisableInterrupts();  /* Disable interrupts */

    if (pCwork->total_block > 0)
    {
        ret = (u16)((u32)(pCwork->got_block * 100) / pCwork->total_block);
    }

    (void)OS_RestoreInterrupts(enabled);        /* Restore interrupts */

    return ret;                        // 0% is returned when TotalBlock count is zero  
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommStartDownload

  Description:  Starts download.

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MB_CommStartDownload(void)
{
    OSIntrMode enabled;
    BOOL    ret = FALSE;

    enabled = OS_DisableInterrupts();  /* Disable interrupts */

    if (pCwork
        && pCwork->c_comm_state == MB_COMM_CSTATE_DLINFO_ACCEPTED
        && pCwork->user_req == MB_COMM_USER_REQ_NONE)
    {

        pCwork->user_req = MB_COMM_USER_REQ_DL_START;

        ret = TRUE;
    }

    (void)OS_RestoreInterrupts(enabled);        /* Restore interrupts */
    return ret;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeChildState

  Description:  child state change and callback call

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeChildState(int state, void *arg)
{
    pCwork->c_comm_state = state;

    MBi_CommChangeChildStateCallbackOnly(state, arg);

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeChildStateCallbackOnly

  Description:  Only performed when invoking the callback for child state notification   
                The internal state does not change

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeChildStateCallbackOnly(int state, void *arg)
{
    if (pCwork->child_callback)        // State changing callback
    {
        (*pCwork->child_callback) ((u32)state, arg);
    }

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildCallback

  Description:  child callback

  Arguments:    type:WM_TYPE event arg:callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_CommChildCallback(u16 type, void *arg)
{
    MB_COMM_WMEVENT_OUTPUT(type, arg);

    switch (type)
    {
    case MB_CALLBACK_INIT_COMPLETE:
        /* Initialization complete*/
        MBi_CommChangeChildState(MB_COMM_CSTATE_INIT_COMPLETE, arg);

        break;

    case MB_CALLBACK_PARENT_FOUND:
        {
            WMStartScanCallback *pSsc = (WMStartScanCallback *)arg;
            int     parent_no = MBi_GetLastFountParent();
            WMBssDesc *bssDescp = MBi_GetParentBssDesc(parent_no);

            /* Discover parents*/
            /* Get parent game information via beacon*/
            (void)MB_RecvGameInfoBeacon(MBi_CommBeaconRecvCallback, pSsc->linkLevel, bssDescp);

            MB_CountGameInfoLifetime(MBi_CommBeaconRecvCallback, TRUE);
        }
        break;
    case MB_CALLBACK_PARENT_NOT_FOUND:
        /* Lifetime count of parent game information*/
        MB_CountGameInfoLifetime(MBi_CommBeaconRecvCallback, FALSE);
        break;

    case MB_CALLBACK_CONNECTED_TO_PARENT:
        MBi_CommChangeChildState(MB_COMM_CSTATE_CONNECT, arg);
        break;

    case MB_CALLBACK_MP_CHILD_RECV:
        MBi_CommChildRecvData(arg);
        break;

    case MB_CALLBACK_MP_SEND_ENABLE:
        (void)MBi_CommChildSendData();
        break;

    case MB_CALLBACK_END_COMPLETE:
        /* End complete*/

        /* If MBi_CommEnd() called in BOOT request reception state*/
        if (pCwork->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED && pCwork->boot_end_flag == 1)
        {
            // Set DownloadFileInfo and bssDesc in fixed region -> use with loader
            MI_CpuCopy16(&pCwork->dl_fileinfo,
                         (void *)MB_DOWNLOAD_FILEINFO_ADDRESS, sizeof(MBDownloadFileInfo));
            MI_CpuCopy16(&pCwork->bssDescbuf, (void *)MB_BSSDESC_ADDRESS, MB_BSSDESC_SIZE);
            MBi_CommChangeChildState(MB_COMM_CSTATE_BOOT_READY, NULL);  // Boot preparation completed
        }
        /* In other cases (cancel)*/
        else
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_CANCELLED, NULL);   // Cancel

            /* Zero-clear PlayerNo*/
            pCwork->common.user.playerNo = 0;
            clearRecvRegion();

            pCwork->c_comm_state = MB_COMM_CSTATE_NONE;
        }

        break;

    case MB_CALLBACK_CONNECT_FAILED:
        /* Connection failure*/
        /* Pass WMstartConnectCallback type arg in Callback argument */
        MBi_CommChangeChildState(MB_COMM_CSTATE_CONNECT_FAILED, arg);
        MB_DeleteRecvGameInfo(pCwork->connectTargetNo); // Delete game information of parent that failed to connect.
        pCwork->connectTargetNo = 0;
        (void)MBi_RestartScan();       // Resume scan after callback notification
        (void)MBi_CommEnd();

        break;

    case MB_CALLBACK_DISCONNECTED_FROM_PARENT:
        /* Disconnect notification*/
        /* Pass WMstartConnectCallback type arg in Callback argument */
        MBi_CommChangeChildState(MB_COMM_CSTATE_DISCONNECTED_BY_PARENT, arg);
        (void)MBi_RestartScan();       // Resume scan after callback notification
        (void)MBi_CommEnd();
        break;

    case MB_CALLBACK_API_ERROR:
        /* Error values returned when WM API called in ARM9*/
        {
            u16     apiid, errcode;

            apiid = ((u16 *)arg)[0];
            errcode = ((u16 *)arg)[1];

            switch (errcode)
            {
            case WM_ERRCODE_INVALID_PARAM:
            case WM_ERRCODE_FAILED:
            case WM_ERRCODE_WM_DISABLE:
            case WM_ERRCODE_NO_DATASET:
            case WM_ERRCODE_FIFO_ERROR:
            case WM_ERRCODE_TIMEOUT:
                MBi_CommCallChildError(MB_ERRCODE_FATAL);
                break;
            case WM_ERRCODE_OPERATING:
            case WM_ERRCODE_ILLEGAL_STATE:
            case WM_ERRCODE_NO_CHILD:
            case WM_ERRCODE_OVER_MAX_ENTRY:
            case WM_ERRCODE_NO_ENTRY:
            case WM_ERRCODE_INVALID_POLLBITMAP:
            case WM_ERRCODE_NO_DATA:
            case WM_ERRCODE_SEND_QUEUE_FULL:
            case WM_ERRCODE_SEND_FAILED:
            default:
                MBi_CommCallChildError(MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;
    case MB_CALLBACK_ERROR:
        {
            /* Errors when callback returned after WM API call*/
            WMCallback *pWmcb = (WMCallback *)arg;
            switch (pWmcb->apiid)
            {
            case WM_APIID_INITIALIZE:
            case WM_APIID_SET_LIFETIME:
            case WM_APIID_SET_P_PARAM:
            case WM_APIID_SET_BEACON_IND:
            case WM_APIID_START_PARENT:
            case WM_APIID_START_MP:
            case WM_APIID_SET_MP_DATA:
            case WM_APIID_START_DCF:
            case WM_APIID_SET_DCF_DATA:
            case WM_APIID_DISCONNECT:
            case WM_APIID_START_KS:
                /* The above errors are important to WM initialization*/
                MBi_CommCallChildError(MB_ERRCODE_FATAL);
                break;
            case WM_APIID_RESET:
            case WM_APIID_END:
            default:
                /* Other errors are returned as callback errors*/
                MBi_CommCallChildError(MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;

    default:
        break;
    }

#if ( CALLBACK_WM_STATE == 1 )
    if (pCwork->child_callback)
    {
        (*pCwork->child_callback) ((u32)(MB_COMM_CSTATE_WM_EVENT | type), arg);
    }
#endif

    if (type == MB_CALLBACK_END_COMPLETE)
    {
        // start work
        MI_CpuClear16(pCwork, sizeof(MB_CommCWork));
        pCwork = NULL;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildRecvData

  Description:  Receives child data.

  Arguments:    arg      : pointer to callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommChildRecvData(void *arg)
{
    WMPortRecvCallback *bufp = (WMPortRecvCallback *)arg;
    MB_CommCWork *const p_child = pCwork;
    MBCommParentBlockHeader hd;
    u8     *databuf;
    u16     aid = MBi_GetAid();

    // destroy receive buffer cache
//  DC_InvalidateRange( bufp->data, MY_ROUND(bufp->length, 32) );

    databuf = MBi_SetRecvBufferFromParent(&hd, (u8 *)bufp->data);

    MB_DEBUG_OUTPUT("RECV ");
    MB_COMM_TYPE_OUTPUT(hd.type);

    switch (hd.type)                   // state transition depending on receive block type
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:
        // send start message from parent
        if (p_child->c_comm_state == MB_COMM_CSTATE_CONNECT)
        {
            MB_DEBUG_OUTPUT("Allowed to request file from parent!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_REQ_ENABLE, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_KICKREQ: // kick message from parent
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MB_DEBUG_OUTPUT("Kicked from parent!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_REQ_REFUSED, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_MEMBER_FULL:      // members from parent exceeded message   
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MB_DEBUG_OUTPUT("Member full!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_MEMBER_FULL, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_DL_FILEINFO:
        // receive MbDownloadFileInfoHeader
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {

            // stores MbDownloadFileInfoHeader in exclusive buffer
            MI_CpuCopy8(databuf, &p_child->dl_fileinfo, sizeof(MBDownloadFileInfo));

            MB_DEBUG_OUTPUT("Download File Info has received (Total block num = %d)\n",
                            p_child->total_block);
            if (!MBi_MakeBlockInfoTable(&p_child->blockinfo_table,
                                        (MbDownloadFileInfoHeader *) & p_child->dl_fileinfo))
            {
                /* Something invalid in received DownloadFileInfo*/
                MBi_CommCallChildError(MB_ERRCODE_INVALID_DLFILEINFO);
                OS_TWarning("The received DownloadFileInfo is illegal.\n");
                return;
            }

            /* Assign PlayerNo*/
            p_child->common.user.playerNo = aid;

            // store total block number
            p_child->total_block = MBi_get_blocknum(&p_child->blockinfo_table);

            /* Pass the received MbDownloadFileInfo as an argument. */
            MBi_CommChangeChildState(MB_COMM_CSTATE_DLINFO_ACCEPTED, (void *)&p_child->dl_fileinfo);
        }

        break;

    case MB_COMM_TYPE_PARENT_DATA:
        /* Receive block data*/
        if (p_child->c_comm_state == MB_COMM_CSTATE_DLINFO_ACCEPTED
            && p_child->user_req == MB_COMM_USER_REQ_DL_START)
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_RECV_PROCEED, NULL);
            p_child->user_req = MB_COMM_USER_REQ_NONE;
            /* Once state has changed, can immediately get received data*/
        }

        if (p_child->c_comm_state == MB_COMM_CSTATE_RECV_PROCEED)
        {
            u16     block_num;
            MB_BlockInfo bi;

            // Routine used only when MB_COMM_TYPE_DATA   
            block_num = p_child->total_block;

            if (block_num == 0 || block_num >= MB_MAX_BLOCK)
            {

                MBi_CommCallChildError(MB_ERRCODE_INVALID_BLOCK_NUM);

                OS_TWarning("Illegal Number of Block! [%d]\n", block_num);
                return;
            }

            /* Evaluations relating to block No.*/
            if (hd.seqno < 0 ||
                hd.seqno >= block_num ||
                MBi_get_blockinfo(&bi, &p_child->blockinfo_table, hd.seqno,
                                  &p_child->dl_fileinfo.header) == FALSE)
            {
                /* Block number is invalid*/
                MBi_CommCallChildError(MB_ERRCODE_INVALID_BLOCK_NO);
                OS_TWarning("The illegal block No.[%d] has been received! (maxnum %d)\n",
                            hd.seqno, block_num);
                goto CheckRemainBlock;
            }

            if (hd.fid != p_child->fileid)
            {
                /* FileID is different from the requested one*/
                MBi_CommCallChildError(MB_ERRCODE_INVALID_FILE);
                OS_TWarning("Received File ID [%d] differs from what was requested!\n", hd.fid);
                goto CheckRemainBlock;
            }

            if (!MBi_IsAbleToRecv(bi.segment_no, bi.child_address, bi.size))
            {
                /* Receive address invalid*/
                MBi_CommCallChildError(MB_ERRCODE_INVALID_RECV_ADDR);
                OS_TWarning("The receive address of Block No.%d is illegal. [%08x - %08x]\n",
                            hd.seqno, bi.child_address, bi.child_address + bi.size);
                goto CheckRemainBlock;
            }

            /* Copy to specified address after checking receive address*/
            if (checkRecvFlag(hd.seqno) == FALSE)
            {
                MB_DEBUG_OUTPUT("DATA : BLOCK(%d)/REMAIN(%d), Recv address[%x] size[%x]\n",
                                hd.seqno, countRemainingblocks(), bi.child_address, bi.size);
                MI_CpuCopy8(databuf, (void *)bi.child_address, bi.size);
                p_child->got_block++;
                setRecvFlag(hd.seqno);
            }

          CheckRemainBlock:
            /* Have all blocks been received? */
            if (0 == countRemainingblocks())
            {
                MBi_CommChangeChildState(MB_COMM_CSTATE_RECV_COMPLETE, NULL);   // receive complete
            }
        }
        break;

    case MB_COMM_TYPE_PARENT_BOOTREQ:
        if (p_child->c_comm_state == MB_COMM_CSTATE_RECV_COMPLETE)
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_BOOTREQ_ACCEPTED, NULL);
        }
        else if (p_child->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED)
        {
            p_child->boot_end_flag = 1;
            (void)MBi_CommEnd();       // end communication
        }
        break;
    default:
        break;
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildSendData

  Description:  Sends child data.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommChildSendData(void)
{
    u16     block_num = pCwork->total_block;
    MBCommChildBlockHeader hd;
    int     errcode = 0;
    u16     pollbmp = 0xffff;

    switch (pCwork->c_comm_state)
    {
    default:
        // send DUMMY MP to establish MP communication
        hd.type = MB_COMM_TYPE_DUMMY;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_REQ_ENABLE:
        {
            const MbBeaconRecvStatus *mbrsp = MB_GetBeaconRecvStatus();
            // send FileRequest
            errcode = MBi_CommRequestFile(pCwork->fileid,
                                          mbrsp->list[pCwork->connectTargetNo].gameInfo.ggid);
            MB_DEBUG_OUTPUT("Requested File (errcode:%d)\n", errcode);
            // MP is set by RequestFile  
        }
        break;

    case MB_COMM_CSTATE_DLINFO_ACCEPTED:
        // DownloadInfo reception message
        hd.type = MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_RECV_PROCEED:
        // block transfer continuation message
        MI_CpuClear8(&hd, sizeof(MBCommChildBlockHeader));
        hd.type = MB_COMM_TYPE_CHILD_CONTINUE;
        hd.data.req = get_next_blockno();
        hd.data.reserved[0] = (u8)(0x00ff & pCwork->got_block); // Lo
        hd.data.reserved[1] = (u8)((0xff00 & pCwork->got_block) >> 8);  // Hi
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_RECV_COMPLETE:
        // block transfer stop message (send continues until BOOTREQ comes from parent)
        hd.type = MB_COMM_TYPE_CHILD_STOPREQ;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_BOOTREQ_ACCEPTED:
        hd.type = MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;
    }

    return errcode;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommBeaconRecvCallback

  Description:  child beacon receive callback

  Arguments:    msg    : beacon receive message
                gInfop : parent game information
                index  : beacon index

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index)
{
#pragma unused(index)

    switch (msg)
    {
    case MB_BC_MSG_GINFO_VALIDATED:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_VALIDATED,
                                             (void *)&gInfop->gameInfo);
        MB_DEBUG_OUTPUT("Parent Info Enable\n");
        break;
    case MB_BC_MSG_GINFO_INVALIDATED:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_INVALIDATED,
                                             (void *)&gInfop->gameInfo);
        break;
    case MB_BC_MSG_GINFO_LOST:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_LOST,
                                             (void *)&gInfop->gameInfo);
        break;
    case MB_BC_MSG_GINFO_LIST_FULL:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_LIST_FULL,
                                             (void *)&gInfop->gameInfo);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MB_ReadMultiBootParentBssDesc

  Description:  setup WMBssDesc structure information
                with MB_GetMultiBootParentBssDesc() data,
                in order to use by WM_StartConnect().

  Arguments:    p_desc            pointer to destination WMBssDesc
                parent_max_size   max packet length of parent in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                child_max_size    max packet length of child in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                ks_flag           if use key-sharing mode, TRUE.
                                    (must be equal to parent's WMParentParam!)
                cs_flag           if use continuous mode, TRUE.
                                    (must be equal to parent's WMParentParam!)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_ReadMultiBootParentBssDesc(WMBssDesc *p_desc,
                                   u16 parent_max_size, u16 child_max_size, BOOL ks_flag,
                                   BOOL cs_flag)
{
    const MBParentBssDesc *parentInfo = MB_GetMultiBootParentBssDesc();
    SDK_NULL_ASSERT(parentInfo);
    MI_CpuCopy8(parentInfo, p_desc, sizeof(MBParentBssDesc));

    p_desc->gameInfoLength = 0x10;
    p_desc->gameInfo.magicNumber = 0x0001;
    p_desc->gameInfo.ver = 0;
    p_desc->gameInfo.ggid =
        (u32)(*(const u16 *)(&p_desc->ssid[0]) | (*(const u16 *)(&p_desc->ssid[2]) << 16));
    p_desc->gameInfo.tgid = *(const u16 *)(&p_desc->ssid[4]);
    p_desc->gameInfo.userGameInfoLength = 0;
    p_desc->gameInfo.parentMaxSize = parent_max_size;
    p_desc->gameInfo.childMaxSize = child_max_size;
    MI_WriteByte(&p_desc->gameInfo.attribute,
                 (u8)((ks_flag ? WM_ATTR_FLAG_KS : 0) |
                      (cs_flag ? WM_ATTR_FLAG_CS : 0) | WM_ATTR_FLAG_ENTRY));
}


/*  ============================================================================

    miscellany functions

    ============================================================================*/

static BOOL checkRecvFlag(int x)
{
    int     pos = (x >> 3);
    int     bit = (x & 0x7);

    SDK_ASSERT(x < MB_MAX_BLOCK);

    if (pCwork->recvflag[pos] & (1 << bit))
    {
        return TRUE;
    }
    return FALSE;
}

static void setRecvFlag(int x)
{
    int     pos = (x >> 3);
    int     bit = (x & 0x7);

    SDK_ASSERT(x < MB_MAX_BLOCK);

    pCwork->recvflag[pos] |= (1 << bit);

    pCwork->last_recv_seq_no = x;
}


static u16 countRemainingblocks(void)
{
    return (u16)(pCwork->total_block - pCwork->got_block);
}

static u16 get_next_blockno(void)
{
    int     req;
    int     search_count = 0;
    req = pCwork->last_recv_seq_no;
    req++;

    while (1)
    {
        if (req < 0 || req >= pCwork->total_block)
        {
            req = 0;
        }
        else if (checkRecvFlag(req))
        {
            req++;
        }
        else
        {
            return (u16)req;
        }

        if (pCwork->last_recv_seq_no == req)
        {
            return (u16)(pCwork->total_block);
        }
        search_count++;

        if (search_count > 1000)
        {
            pCwork->last_recv_seq_no = req;
            return (u16)req;
        }
    }
}

// check whether child AID bit specified in pollbmp is enabled
static BOOL checkPollbmp(u16 pollbmp, u16 child_aid)
{
    if (pollbmp & (u16)(1 << child_aid))
    {
        return TRUE;
    }
    return FALSE;
}

/* Clear the reception region*/
static void clearRecvRegion(void)
{
    /* When the build is not FINALROM, do not clear the downloadable region.*/
#ifdef SDK_FINALROM
    /* Clear the ARM9 downloadable region*/
    MI_CpuClearFast((void *)MB_LOAD_AREA_LO, MB_LOAD_MAX_SIZE);
    /* Clear the ARM7 load buffer address region*/
    MI_CpuClearFast((void *)MB_ARM7_STATIC_RECV_BUFFER, MB_ARM7_STATIC_RECV_BUFFER_SIZE);
    /* Clear the MBbssDesc and MBDownloadFileInfo regions*/
    MI_CpuClear16((void *)MB_BSSDESC_ADDRESS,
                  MB_DOWNLOAD_FILEINFO_ADDRESS + MB_DOWNLOAD_FILEINFO_SIZE - MB_BSSDESC_ADDRESS);
    /* Clear the ROM Header region*/
    MI_CpuClear16((void *)MB_ROM_HEADER_ADDRESS, ROM_HEADER_SIZE_FULL);
#endif

}


static void MBi_CommCallChildError(u16 errcode)
{
    MBErrorStatus e_stat;
    e_stat.errcode = errcode;

    MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_ERROR, &e_stat);

}
