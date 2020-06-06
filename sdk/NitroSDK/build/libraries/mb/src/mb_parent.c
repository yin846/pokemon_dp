/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_parent.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_parent.c,v $
  Revision 1.56  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.55  12/21/2005 14:25:56  yosizaki
  Fixed page-cache issue.

  Revision 1.54  12/13/2005 10:49:04  yosizaki
  Removed temporary hook for SEND_FAILED

  Revision 1.53  12/12/2005 07:32:18  yosizaki
  Fixed nextb.

  Revision 1.52  12/09/2005 08:17:53  yosizaki
  Added hook for SEND_FAILED (temporary)

  Revision 1.51  2005/03/14 07:09:58  sato_masaki
  Added MB_GetGameEntryBitmap() function.

  Revision 1.50  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.49  02/21/2005 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.48  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.47  01/27/2005 07:04:43  takano_makoto
  small fix.

  Revision 1.46  12/07/2004 10:20:37  yosizaki
  fix to reload cache from correct archive. (not only "rom")

  Revision 1.45  11/22/2004 12:44:14  takano_makoto
  Separated off the division-request transmission process as mb_block

  Revision 1.44  11/11/2004 11:47:26  yosizaki
  add cache-reloading.

  Revision 1.43  11/10/2004 13:15:48  yosizaki
  change to use cache-system.

  Revision 1.42  11/02/2004 18:12:04  takano_makoto
  fix comments.

  Revision 1.41  11/02/2004 10:23:01  yosizaki
  fix comment.

  Revision 1.40  10/21/2004 13:43:57  takano_makoto
  Changed the work release location at MB_END.

  Revision 1.39  2004/10/19 06:35:52  sato_masaki
  bug fix

  Revision 1.38  2004/10/12 05:39:21  sato_masaki
  Changed Callback error related to WM_APIID_RESET and WM_APIID_END to MB_ERRCODE_WM_FAILURE.

  Revision 1.37  2004/10/01 07:38:36  sato_masaki
  Added MB_COMM_PSTATE_WAIT_TO_SEND state. Changed so that DownloadFileInfo send to child does not occur if that child was received.    

  Revision 1.36  2004/09/29 13:03:15  sato_masaki
  Changed when the MB_COMM_PSTATE_BOOT_STARTABLE state is returned.

  Revision 1.35  2004/09/25 09:36:42  sato_masaki
  Change so that MB_CommGetChildUser() copies child information in buffer.    

  Revision 1.34  2004/09/25 05:30:24  sato_masaki
  Added process to prohibit interrupts while API called from main loop is executing.

  Revision 1.33  2004/09/22 09:41:32  sato_masaki
  Added a condition determination to MBi_CommParentSendBlock().

  Revision 1.32  2004/09/21 01:39:15  sato_masaki
  Standardized errors that originate in WM to MB_ERRCODE_WM_FAILURE. Abolished MB_ERRCODE_MP_SENT_FAILURE.

  Revision 1.31  2004/09/20 13:10:46  sato_masaki
  - Made changes related to MB_COMM_PSTATE_ERROR so that the value of the MBErrCode lister is passed to the arg.    
  -  In the function for sending, changed the MB_ERROR that was being used to MB_SENDFUNC_STATE_ERR (defined inside the library).

  Revision 1.30  2004/09/18 12:01:11  sato_masaki
  Set reservations in the Child Device send header definition.    

  Revision 1.29  2004/09/18 03:22:04  sato_masaki
  Handling for if an AID0 comes up when CHILD_CONNECTED or CHILD_DISCONNECTED.

  Revision 1.28  2004/09/16 12:49:19  sato_masaki
  In mb.h, changed the functions and structures defined as MBi_*** to MB_***

  Revision 1.27  2004/09/15 06:51:59  sato_masaki
  Deleted processes regarding WBT definitions.

  Revision 1.26  2004/09/15 06:25:29  sato_masaki
  Made variables of parts that were dependent on MB_COMM_P_SENDLEN, MB_COMM_C_SENDLEN definitions.

  Revision 1.25  2004/09/14 13:31:21  sato_masaki
  small fix

  Revision 1.24  2004/09/14 13:19:44  sato_masaki
  Changed to send request data in fragments.

  Revision 1.23  2004/09/14 06:06:50  sato_masaki
  Made MB_CommBootRequest**, MB_CommStartSending** in-line.-> Moved to mb.h.

  Revision 1.22  2004/09/13 13:08:55  sato_masaki
  remove member word from MB_CommBlockHeader

  Revision 1.21  2004/09/13 08:08:35  sato_masaki
  Added MBi_CommChangeParentStateCallbackOnly() and MBi_CommCallParentError().
  Made changes so that when receiving an entry request, the comparison is performed not only on the fileID but also on the requested GGID from the Child Device.

  Revision 1.20  2004/09/11 11:47:02  sato_masaki
  Fixed a bug that was being called even if there were no changes to the member of MB_UpdateGameInfoMember().

  Revision 1.19  2004/09/11 11:17:41  sato_masaki
  change mbc to pPwork

  Revision 1.18  2004/09/11 07:24:03  sato_masaki
  Changed MB_COMM_PSTATE_REQUESTED callback arg &req_data -> &req_data.userinfo
  
  Revision 1.17  09/10/2004 04:43:05  yosizaki
  change OS_Printf to MB_DEBUG_OUTPUT

  Revision 1.16  2004/09/10 02:54:44  sato_masaki
  small fix

  Revision 1.15  2004/09/10 02:54:06  sato_masaki
  Made changes to the CALLBACK_WM_STATE definition.

  Revision 1.14  2004/09/10 01:14:23  sato_masaki
  Added a process for setting the playerNo when receiving MbUserInfo.

  Revision 1.13  2004/09/09 14:01:47  sato_masaki
  In mb_gameinfo.c, changed the PlayerFlag format. (Ongoing task)

  Revision 1.12  2004/09/09 02:49:14  sato_masaki
  Changed the clear process structure related to the connection information and the entry information when MB_CALLBACK_CHILD_DISCONNECTED.

  Revision 1.11  2004/09/09 01:15:35  sato_masaki
  Added a process that forcibly KICKs the request to a fileNo that has not been registered.

  Revision 1.10  2004/09/08 07:19:18  sato_masaki
  small fix

  Revision 1.9  2004/09/08 04:38:53  sato_masaki
  - Deleted the fileid from MB_CommBlockHeader (mb_common.h). Changed the process along with that.
  - Added the MB_CommBootRequestAll() function. This function has features that are compatible with the MB_CommBootRequest() that existed before the change.

  Revision 1.8  2004/09/08 01:26:13  sato_masaki
  Made changes so that the MB_CommRequestData from the Child Device is stored at the point in time of the MB_COMM_PSTATE_REQUESTED callback.

  Revision 1.7  2004/09/07 11:55:14  sato_masaki
  - Changed MB_CommBootRequest() so that it specifies AID.
  - Name change: MB_COMM_PSTATE_CANCEL to MB_COMM_PSTATE_END.
    MB_COMM_PSTATE_END is now the only MB library end callback.
  - Changed the timing that clears the USER request.
  - Changed the processes surrounding the boot request.

  Revision 1.6  2004/09/07 04:36:00  sato_masaki
  Used IsChildAidValid() to make the check on the Child Device AID more strict.
  Changed so that the argument is passed in one portion of the callback.    

  Revision 1.5  2004/09/06 09:48:39  sato_masaki
  small fix

  Revision 1.4  2004/09/04 10:26:13  sato_masaki
  Made a revision regarding the number limitation process.

  Revision 1.3  2004/09/04 06:46:15  sato_masaki
  Added a callback state for when the number of members is exceeded (MB_COMM_*STATE_MEMBER_FULL).

  Revision 1.2  2004/09/04 04:38:24  sato_masaki
  Implemented a limitation check on the number of downloads.    
  MB_COMM_CSTATE_REQ_REFUSED returned on the Child Device.

  Revision 1.1  2004/09/03 07:04:36  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"

// ----------------------------------------------------------------------------
// definition

#define MB_SEND_THRESHOLD                       2       // During a send, send up to 2 blocks ahead    

// ----------------------------------------------------------------------------
// variable

/* Bit set indicating the file ID that received a transfer request from the child device in the latest MP*/
static int any_recv_bitmap = 0;

// ----------------------------------------------------------------------------
// static function's prototypes

// --- for parent
static void MBi_CommChangeParentState(u16 child, int state, void *arg);
static void MBi_CommChangeParentStateCallbackOnly(u16 child, int state, void *arg);
static void MBi_CommParentRecvDataPerChild(void *arg, u16 child);
static void MBi_CommParentRecvData(void *arg);
static int MBi_CommParentSendMsg(u8 type, u16 pollbmp);
static int MBi_CommParentSendDLFileInfo(void);
static int MBi_CommParentSendBlock(void);
static int MBi_CommParentSendData(void);
static void MBi_calc_sendblock(u8 file_id);
static u16 MBi_calc_nextsendblock(u16 next_block, u16 next_block_req);

// --- miscellany
static inline u16 max(u16 a, u16 b);
static BOOL IsChildAidValid(u16 child_aid);
static void MBi_CommCallParentError(u16 aid, u16 errcode);

/*  ============================================================================

    Parent Functions

    ============================================================================*/

/*---------------------------------------------------------------------------*
  Name:         MB_CommSetParentStateCallback

  Description:  Sets the parent event callback

  Arguments:    callback       Callback function to specify

  Returns:      None.
 *---------------------------------------------------------------------------*/

void MB_CommSetParentStateCallback(MBCommPStateCallback callback)
{
    OSIntrMode enabled;

    SDK_ASSERT(pPwork != 0);

    enabled = OS_DisableInterrupts();  /* interrupts prohibited*/

    pPwork->parent_callback = callback;

    (void)OS_RestoreInterrupts(enabled);        /* Cancels interrupts prohibition*/
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetParentState

  Description:  Gets the Parent Device state for each Child Device

  Arguments:    child_aid(The WM AID expressions are handled from 1-15 in accordance with the formula)    

  Returns:      
 *---------------------------------------------------------------------------*/

int MB_CommGetParentState(u16 child_aid)
{

    if (pPwork && IsChildAidValid(child_aid))
    {
        return pPwork->p_comm_state[child_aid - 1];
    }
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildUser

  Description:  Gets the user information about the connected child devices.

  Arguments:    child(The WM AID expressions are handled from 1-15 in accordance with the formula)    

  Returns:      MbUser structure
 *---------------------------------------------------------------------------*/

const MBUserInfo *MB_CommGetChildUser(u16 child_aid)
{
    OSIntrMode enabled = OS_DisableInterrupts();        /* interrupts prohibited*/

    if (pPwork && IsChildAidValid(child_aid))
    {
        MI_CpuCopy8(&pPwork->childUser[child_aid - 1], &pPwork->childUserBuf, sizeof(MBUserInfo));
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return &pPwork->childUserBuf;
    }
    (void)OS_RestoreInterrupts(enabled);        /* Cancels interrupts prohibition*/
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildrenNumber

  Description:  gets the number of connected children

  Arguments:    

  Returns:      number of children
 *---------------------------------------------------------------------------*/

u8 MB_CommGetChildrenNumber(void)
{
    if (pPwork)
    {
        return pPwork->child_num;
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetGameEntryBitmap

  Description:  Obtains the AID bitmap being entered for the GameRegistry being distributed.
                

  Arguments:    game_req      - Pointer to the target GameRegistry

  Returns:      AID bitmap being entered in the specified GameRegistry
                (Parent AID: 0, Child AID: 1-15)
                If a game is not being distributed, the return value is 0.
               (If a game is being distributed, parent AID:0 is always included in the entry members.)
 *---------------------------------------------------------------------------*/

u16 MB_GetGameEntryBitmap(const MBGameRegistry *game_reg)
{
    int     i;
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        if ((pPwork->fileinfo[i].active) && ((u32)pPwork->fileinfo[i].game_reg == (u32)game_reg))
        {
            return pPwork->fileinfo[i].gameinfo_child_bmp;
        }
    }
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommIsBootable

  Description:  Determines whether a boot is possible.

  Arguments:    child_aid - AID of targeted child

  Returns:      yes - TRUE  no - FALSE
 *---------------------------------------------------------------------------*/

BOOL MB_CommIsBootable(u16 child_aid)
{
    SDK_ASSERT(pPwork != NULL);

    if (pPwork && IsChildAidValid(child_aid))
    {
        /* Return TRUE if child has completed transmission */
        if (pPwork->p_comm_state[child_aid - 1] == MB_COMM_PSTATE_SEND_COMPLETE)
        {
            return TRUE;
        }
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommResponseRequest

  Description:  Specifies a response to a connect request from a child.

  Arguments:    None.

  Returns:      success - TRUE  failed - FALSE
 *---------------------------------------------------------------------------*/

BOOL MB_CommResponseRequest(u16 child_aid, MBCommResponseRequestType ack)
{
    u16     req;
    int     state;
    OSIntrMode enabled;

    SDK_ASSERT(pPwork != NULL);

    enabled = OS_DisableInterrupts();  /* interrupts prohibited*/

    switch (ack)
    {
    case MB_COMM_RESPONSE_REQUEST_KICK:
        state = MB_COMM_PSTATE_REQUESTED;
        req = MB_COMM_USER_REQ_KICK;
        break;
    case MB_COMM_RESPONSE_REQUEST_ACCEPT:
        state = MB_COMM_PSTATE_REQUESTED;
        req = MB_COMM_USER_REQ_ACCEPT;
        break;

    case MB_COMM_RESPONSE_REQUEST_DOWNLOAD:
        state = MB_COMM_PSTATE_WAIT_TO_SEND;
        req = MB_COMM_USER_REQ_SEND_START;
        break;

    case MB_COMM_RESPONSE_REQUEST_BOOT:
        state = MB_COMM_PSTATE_SEND_COMPLETE;
        req = MB_COMM_USER_REQ_BOOT;
        break;
    default:
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return FALSE;
    }

    if (pPwork && IsChildAidValid(child_aid))
    {
        if (pPwork->p_comm_state[child_aid - 1] == state)
        {
            pPwork->req2child[child_aid - 1] = req;
            (void)OS_RestoreInterrupts(enabled);        /* Cancels interrupts prohibition*/
            return TRUE;
        }
    }

    (void)OS_RestoreInterrupts(enabled);        /* Cancels interrupts prohibition*/

    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeParentState

  Description:  Changes the Parent Device state

  Arguments:    child (The WM AID expressions are handled from 1-15 in accordance with the formula.      0 indicates the parent device).
                state

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeParentState(u16 child, int state, void *arg)
{
    SDK_ASSERT(pPwork && child >= 0 && child <= WM_NUM_MAX_CHILD);

    /* Change the state to that child if the child's number is specified */
    if (IsChildAidValid(child))
    {
        pPwork->p_comm_state[child - 1] = state;
    }

    MBi_CommChangeParentStateCallbackOnly(child, state, arg);

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeParentStateCallbackOnly

  Description:  Only performed when calling back the Child Device state notification     
                The internal state does not change

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeParentStateCallbackOnly(u16 child, int state, void *arg)
{
    if (pPwork->parent_callback)       // State changing callback
    {
        (*pPwork->parent_callback) (child, (u32)state, arg);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentCallback

  Description:  parent callback main unit

  Arguments:    type:WM_TYPE event arg:callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/

void MBi_CommParentCallback(u16 type, void *arg)
{
    MB_COMM_WMEVENT_OUTPUT(type, arg);

    switch (type)
    {
    case MB_CALLBACK_INIT_COMPLETE:
        /* Initialization complete*/
        /* Pass a WMStartParentCallback type arg to the Callback argument */
        MBi_CommChangeParentState(0, MB_COMM_PSTATE_INIT_COMPLETE, arg);
        break;

    case MB_CALLBACK_END_COMPLETE:
        /* End complete*/
        // Execute at the end of the function

        break;

    case MB_CALLBACK_CHILD_CONNECTED:
        {
            WMstartParentCallback *sparg = (WMstartParentCallback *)arg;

            /* Ignore if AID is 0 (parent)*/
            if (sparg->aid == 0)
                break;

            if (sparg->aid >= 16)
            {
                OS_TWarning("Connected Illegal AID No. ---> %2d\n", sparg->aid);
                break;
            }

            MB_DEBUG_OUTPUT("child %d connected to get bootimage!\n", sparg->aid);

            /* Pass a WMStartParentCallback type arg to the Callback argument */
            MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_CONNECTED, arg);
        }
        break;

    case MB_CALLBACK_CHILD_DISCONNECTED:
        {
            WMstartParentCallback *sparg = (WMstartParentCallback *)arg;

            /* Ignore if AID is 0 (parent)*/
            if (sparg->aid == 0)
                break;

            if (sparg->aid >= 16)
            {
                OS_TWarning("Disconnected Illegal AID No. ---> %2d\n", sparg->aid);
                break;
            }

            MB_DEBUG_OUTPUT("child %d disconnected \n", sparg->aid);

            /*  Delete child information
               (If disconnected, delete child information relating to that AID)*/
            pPwork->childversion[sparg->aid - 1] = 0;
            MI_CpuClear8(&pPwork->childggid[sparg->aid - 1], sizeof(u32));
            MI_CpuClear8(&pPwork->childUser[sparg->aid - 1], sizeof(MBUserInfo));

            /* Clear the Receiving buffer*/
            MBi_ClearParentPieceBuffer(sparg->aid);

            pPwork->req2child[sparg->aid - 1] = MB_COMM_USER_REQ_NONE;

            /* If the requested FileID has been set. Clear the ID with -1. */
            if (pPwork->fileid_of_child[sparg->aid - 1] != -1)
            {
                u8      fileID = (u8)pPwork->fileid_of_child[sparg->aid - 1];
                u16     nowChildFlag = pPwork->fileinfo[fileID].gameinfo_child_bmp;
                u16     child = sparg->aid;

                pPwork->fileinfo[fileID].gameinfo_child_bmp &= ~(MB_GAMEINFO_CHILD_FLAG(child));
                pPwork->fileinfo[fileID].gameinfo_changed_bmp |= MB_GAMEINFO_CHILD_FLAG(child);
                pPwork->fileid_of_child[child - 1] = -1;
                pPwork->fileinfo[fileID].pollbmp &= ~(0x0001 << (child));

                MB_DEBUG_OUTPUT("Update Member (AID:%2d)\n", child);
            }

            /* Clear the connection information*/
            if (pPwork->child_entry_bmp & (0x0001 << (sparg->aid)))
            {
                pPwork->child_num--;
                pPwork->child_entry_bmp &= ~(0x0001 << (sparg->aid));
            }

            /* If disconnected when MB_COMM_PSTATE_BOOT_REQUEST,
               it is assumed to be the end of the communication by booting, and the MB_COMM_PSTATE_BOOT_STARTABLE status is notified.*/
            if (pPwork->p_comm_state[sparg->aid - 1] == MB_COMM_PSTATE_BOOT_REQUEST)
            {
                MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_BOOT_STARTABLE, NULL);
            }

            /* Return WMStartParentCallback type arg to the Callback argument */
            MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_DISCONNECTED, arg);
            pPwork->p_comm_state[sparg->aid - 1] = MB_COMM_PSTATE_NONE;
        }
        break;

    case MB_CALLBACK_MP_PARENT_RECV:
        MBi_CommParentRecvData(arg);
        break;

    case MB_CALLBACK_MP_SEND_ENABLE:
        (void)MBi_CommParentSendData();
        break;

    case MB_CALLBACK_BEACON_SENT:
        {
            u8      i;
            /* For each game, update the GameInfo member information*/
            for (i = 0; i < MB_MAX_FILE; i++)
            {
                /* When the registration file is active,
                   and the game entry member change flag is enabled,
                   update the GameInfo member information.*/
                if (pPwork->fileinfo[i].active && pPwork->fileinfo[i].gameinfo_changed_bmp)
                {
                    MB_UpdateGameInfoMember(&pPwork->fileinfo[i].game_info,
                                            &pPwork->childUser[0],
                                            pPwork->fileinfo[i].gameinfo_child_bmp,
                                            pPwork->fileinfo[i].gameinfo_changed_bmp);
                    /* After update, clear the changed bmp*/
                    pPwork->fileinfo[i].gameinfo_changed_bmp = 0;
                }
            }
        }
        /* Set GameInfo in beacon and broadcast*/
        MB_SendGameInfoBeacon(MBi_GetGgid(), MBi_GetTgid(), MBi_GetAttribute());
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
                MBi_CommCallParentError(0, MB_ERRCODE_FATAL);
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
                MBi_CommCallParentError(0, MB_ERRCODE_WM_FAILURE);
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
                MBi_CommCallParentError(0, MB_ERRCODE_FATAL);
                break;
            case WM_APIID_RESET:
            case WM_APIID_END:
            default:
                /* Other errors are returned as callback errors*/
                MBi_CommCallParentError(0, MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;

    }


#if ( CALLBACK_WM_STATE == 1 )
    MBi_CommChangeParentState(0, (u32)(MB_COMM_PSTATE_WM_EVENT | type), arg);
#endif

    if (type == MB_CALLBACK_END_COMPLETE)
    {
        MBCommPStateCallback tmpCb = pPwork->parent_callback;

        /* Release work*/
        MI_CpuClearFast(pPwork, sizeof(MB_CommPWork));
        pPwork = NULL;
        if (tmpCb)                     // State changing callback
        {
            (*tmpCb) (0, MB_COMM_PSTATE_END, NULL);
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentRecvDataPerChild

  Description:  Processes the receiving data for each child

  Arguments:    arg      : pointer to callback argument, child : child AID

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommParentRecvDataPerChild(void *arg, u16 child)
{
    MBCommChildBlockHeader hd;
    int     state;
    u8     *p_data;

    // The range of a child is 1 to 15.
    if (child == 0 || child > WM_NUM_MAX_CHILD)
    {
        return;
    }

    p_data = MBi_SetRecvBufferFromChild((u8 *)&((WMMpRecvData *)arg)->cdata[0], &hd, child);

    state = pPwork->p_comm_state[child - 1];

    MB_DEBUG_OUTPUT("RECV (CHILD:%2d)", child);
    MB_COMM_TYPE_OUTPUT(hd.type);

    switch (hd.type)                   // Process for each data type
    {
    case MB_COMM_TYPE_CHILD_FILEREQ:
        {
            // Accept only when MB_COMM_PSTATE_CONNECTED
            if (state == MB_COMM_PSTATE_CONNECTED)
            {
                MBCommRequestData req_data;

                if (p_data == NULL)
                {
                    // Do not move to next state until all of child's request data is present.
                    break;
                }

                MI_CpuCopy8(p_data, &req_data, MB_COMM_REQ_DATA_SIZE);

                pPwork->childggid[child - 1] = req_data.ggid;
                pPwork->childversion[child - 1] = req_data.version;
                MB_DEBUG_OUTPUT("Child [%2d] MB_IPL_VERSION : %04x\n", child, req_data.version);
                MI_CpuCopy8(&req_data.userinfo, &pPwork->childUser[child - 1], sizeof(MBUserInfo));
                pPwork->childUser[child - 1].playerNo = child;
                /* Pass the received MBCommRequestData to the Callback argument */
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_REQUESTED, &req_data.userinfo);
            }

            if (state == MB_COMM_PSTATE_REQUESTED)
            {
                u8      i, entry_num = 0;
                u8      fileid = ((MBCommRequestData *)p_data)->fileid;

                /* KICK if an inactive fileNo is requested, the fileid is invalid or
                   the fileNo GGID does not match the requested GGID.
                   GGID do not match.    */
                if (fileid >= MB_MAX_FILE
                    || pPwork->fileinfo[fileid].active == 0
                    || pPwork->childggid[child - 1] != pPwork->fileinfo[fileid].game_reg->ggid)
                {
                    pPwork->req2child[child - 1] = MB_COMM_USER_REQ_KICK;
                }
                else
                {
                    /* KICK if exceeds number of persons*/
                    for (i = 0; i < WM_NUM_MAX_CHILD + 1; i++)
                    {
                        if (pPwork->fileinfo[fileid].gameinfo_child_bmp & (0x0001 << i))
                        {
                            entry_num++;
                        }
                    }

                    if (entry_num >= pPwork->fileinfo[fileid].game_reg->maxPlayerNum)
                    {
                        MB_DEBUG_OUTPUT("Member full (AID:%2d)\n", child);
                        /* Forcedly cancel the request*/
                        pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                        MBi_CommChangeParentState(child, MB_COMM_PSTATE_MEMBER_FULL, NULL);
                        break;
                    }
                }

                switch (pPwork->req2child[child - 1])
                {
                case MB_COMM_USER_REQ_ACCEPT:
                    {

                        if (0 == (pPwork->child_entry_bmp & (0x0001 << (child))))
                        {
                            pPwork->child_num++;
                            pPwork->child_entry_bmp |= (0x0001 << (child));
                            pPwork->fileid_of_child[child - 1] = (s8)fileid;

                            pPwork->fileinfo[fileid].gameinfo_child_bmp |=
                                MB_GAMEINFO_CHILD_FLAG(child);
                            pPwork->fileinfo[fileid].gameinfo_changed_bmp |=
                                MB_GAMEINFO_CHILD_FLAG(child);
                            MB_DEBUG_OUTPUT("Update Member (AID:%2d)\n", child);
                            pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;

                            MBi_CommChangeParentState(child, MB_COMM_PSTATE_REQ_ACCEPTED, NULL);
                        }
                    }
                    break;

                case MB_COMM_USER_REQ_KICK:
                    MB_DEBUG_OUTPUT("Kick (AID:%2d)\n", child);
                    pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                    MBi_CommChangeParentState(child, MB_COMM_PSTATE_KICKED, NULL);
                    break;
                }
            }
        }
        break;

    case MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO:

        /* When MB_COMM_PSTATE_REQ_ACCEPTED, only moves to MB_COMM_PSTATE_WAIT_TO_SEND*/
        if (state == MB_COMM_PSTATE_REQ_ACCEPTED)
        {
            MBi_CommChangeParentState(child, MB_COMM_PSTATE_WAIT_TO_SEND, NULL);
        }

        /* When MB_COMM_PSTATE_WAIT_TO_SEND,
           it adds pollbitmap to the requested file and
           changes to MB_COMM_PSTATE_SEND_PROCEED.*/
        else if (state == MB_COMM_PSTATE_WAIT_TO_SEND)
        {
            // If SendStart trigger was on, it changes to the block send state.
            if (pPwork->req2child[child - 1] == MB_COMM_USER_REQ_SEND_START)
            {
                u8      fid = (u8)pPwork->fileid_of_child[child - 1];
                pPwork->fileinfo[fid].pollbmp |= (0x0001 << (child));
                pPwork->fileinfo[fid].currentb = 0;

                pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_SEND_PROCEED, NULL);
            }
        }
        break;

    case MB_COMM_TYPE_CHILD_CONTINUE:
        if (state == MB_COMM_PSTATE_SEND_PROCEED)
        {
            u8      fileid = (u8)pPwork->fileid_of_child[child - 1];

            if (fileid == (u8)-1)
                break;

            // If nextSend is received from multiple children, send to the one having the most
            SDK_ASSERT(fileid < MB_MAX_FILE);
            SDK_ASSERT(pPwork->fileinfo[fileid].pollbmp);

            pPwork->fileinfo[fileid].nextb =
                MBi_calc_nextsendblock(pPwork->fileinfo[fileid].nextb, hd.data.req);
            any_recv_bitmap |= (1 << fileid);
        }
        break;

    case MB_COMM_TYPE_CHILD_STOPREQ:
        if (state == MB_COMM_PSTATE_SEND_PROCEED)
        {
            u8      fileid = (u8)pPwork->fileid_of_child[child - 1];

            if (fileid == (u8)-1)
                break;

            SDK_ASSERT(fileid < MB_MAX_FILE);

            pPwork->fileinfo[fileid].pollbmp &= ~(0x0001 << (child));

            MBi_CommChangeParentState(child, MB_COMM_PSTATE_SEND_COMPLETE, NULL);       // Send completed
        }
        else if (state == MB_COMM_PSTATE_SEND_COMPLETE)
        {
            if (pPwork->req2child[child - 1] == MB_COMM_USER_REQ_BOOT)
            {
                pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_BOOT_REQUEST, NULL);
                break;
            }
        }
        break;

    case MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED:
        if (state == MB_COMM_PSTATE_BOOT_REQUEST)
        {
            /* BOOTREQ_ACCEPTED from child is not used for state transition.*/
            break;
        }

        break;

    default:
        break;
    }

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentRecvData

  Description:  Receives parent data

  Arguments:    arg      : pointer to callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommParentRecvData(void *arg)
{
    // The argument arg contains a pointer to the receive buffer
    WMmpRecvHeader *mpHeader = (WMmpRecvHeader *)arg;

    u16     i;
    WMmpRecvData *datap;

    // Initialize at this point to evaluate statistically with MBi_CommParentRecvDataPerChild.
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        if (pPwork->fileinfo[i].active)
            pPwork->fileinfo[i].nextb = 0;
    }
    any_recv_bitmap = 0;

    // Display the received data for each child
    for (i = 1; i <= WM_NUM_MAX_CHILD; ++i)
    {
        // Get the starting address of the data for the child with an AID of i
        datap = WM_ReadMPData(mpHeader, (u16)i);
        // If the child with an AID of i has no data
        if (datap != NULL)
        {
            // Display the receive data
            if (datap->length == 0xffff)
            {
            }
            else if (datap->length != 0)
            {
                // Process regarding each child data
                MBi_CommParentRecvDataPerChild(datap, i);
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentSendMsg

  Description:  Sends a message from a parent

  Arguments:    pollbmp

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommParentSendMsg(u8 type, u16 pollbmp)
{
    MBCommParentBlockHeader hd;

    /* Notify the start of transmission (without arguments)*/
    hd.type = type;

    (void)MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);
    return MBi_BlockHeaderEnd(MB_COMM_PARENT_HEADER_SIZE, pollbmp, pPwork->common.sendbuf);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentSendDLFileInfo

  Description:  Sends DownloadFileInfo from the parent

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommParentSendDLFileInfo(void)
{
    MBCommParentBlockHeader hd;
    u8     *databuf = ((u8 *)pPwork->common.sendbuf) + MB_COMM_PARENT_HEADER_SIZE;
    u16     child;
    u8      i, fid;
    s8      send_candidate_fid = -1;
    static s8 prev_fid = -1;
    u8      file_req_num[MB_MAX_FILE];
    u16     pollbmp = 0;

    MI_CpuClear8(&file_req_num[0], sizeof(u8) * MB_MAX_FILE);

    // Count the FileID which the child is requesting
    for (child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {
        if (pPwork->p_comm_state[child - 1] == MB_COMM_PSTATE_REQ_ACCEPTED)
        {
            // Count only the child that is target for MB_COMM_PSTATE_REQ_ACCEPTED
            ++(file_req_num[pPwork->fileid_of_child[child - 1]]);
        }
    }

    fid = (u8)prev_fid;

    for (i = 0; i < MB_MAX_FILE; i++)  // Determine the send file ID
    {
#if 1
        fid = (u8)((fid + 1) % MB_MAX_FILE);

        if (pPwork->fileinfo[fid].active && file_req_num[fid] > 0)
        {
            send_candidate_fid = (s8)fid;
            break;
        }

#else
        if (pPwork->fileinfo[i].active)
        {
            if (file_req_num[i] > 0)
            {

                /* 
                   Majority decision
                   (  (In this case, when multiple child devices are waiting for the start of the download of different files,
                   transmission of DownloadFileInfo is locked to the file with more devices.
                   There will be no problem with the multiboot operation, but the state of the child application does not
                   move forward from MB_COMM_CSTATE_REQ_ENABLE.
                   (After download from the parent is determined, the entry process for those children is processed.)
                   The countermeasure may be necessary.)
                 */

                if (send_candidate_fid == -1 || file_req_num[i] > file_req_num[send_candidate_fid])
                {
                    send_candidate_fid = i;
                }

            }
        }
#endif

    }

    if (send_candidate_fid == -1)
        return MB_SENDFUNC_STATE_ERR;

    prev_fid = send_candidate_fid;

    // poll bitmap setting (only with child requesting the corresponding number for the File to be sent)
    for (child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {
        if (pPwork->p_comm_state[child - 1] == MB_COMM_PSTATE_REQ_ACCEPTED
            && pPwork->fileid_of_child[child - 1] == send_candidate_fid)
        {
            pollbmp |= (1 << child);
        }
    }

    MB_DEBUG_OUTPUT("DLinfo No %2d Pollbmp %04x\n", send_candidate_fid, pollbmp);

    // Send the FileInfo of the last child which accepted the request
    hd.type = MB_COMM_TYPE_PARENT_DL_FILEINFO;
    hd.fid = send_candidate_fid;

    databuf = MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);
    if (databuf)
    {
        // Copy data to the send buffer
        MI_CpuCopy8(&pPwork->fileinfo[send_candidate_fid].dl_fileinfo,
                    databuf, sizeof(MBDownloadFileInfo));
    }

    return MBi_BlockHeaderEnd(sizeof(MBDownloadFileInfo) + MB_COMM_PARENT_HEADER_SIZE, pollbmp,
                              pPwork->common.sendbuf);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_ReloadCache

  Description:  Reload data in specified destruction cache.

  Arguments:    p_task           task storing MBiCacheInfo in param[0]

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_ReloadCache(MBiTaskInfo * p_task)
{
    MBiCacheInfo *const p_info = (MBiCacheInfo *) p_task->param[0];
    MBiCacheList *const p_list = (MBiCacheList *) p_task->param[1];
    FSFile  file[1];

    FS_InitFile(file);

    /* Specify the target archive for MB_ReadSegment()*/
    if (FS_OpenFileDirect(file,
                          FS_FindArchive(p_list->arc_name, (int)p_list->arc_name_len),
                          p_info->src, p_info->src + p_info->len, (u32)~0))
    {
        if (FS_ReadFile(file, p_info->ptr, (int)p_info->len) == p_info->len)
        {
            /* In this case, it is all right to be READY*/
            p_info->state = MB_CACHE_STATE_READY;
        }
        (void)FS_CloseFile(file);
    }

    if (p_info->state != MB_CACHE_STATE_READY)
    {
        OS_TPanic("cache-reloading is failed!");
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentSendBlock

  Description:  Sends Block data from the parent

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommParentSendBlock(void)
{
    MBCommParentBlockHeader hd;
    u8     *databuf;
    u8      i;
    MB_BlockInfo bi;

    // Determine the send File number
    if (pPwork->file_num == 0)
        return MB_SENDFUNC_STATE_ERR;

    // block transfer body starts from here

    // Determine the send target file
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        pPwork->cur_fileid = (u8)((pPwork->cur_fileid + 1) % MB_MAX_FILE);
        if (pPwork->fileinfo[pPwork->cur_fileid].active
            && pPwork->fileinfo[pPwork->cur_fileid].pollbmp)
        {
            MB_DEBUG_OUTPUT("Send File ID:%2d\n", pPwork->cur_fileid);
            break;
        }
    }
    if (i == MB_MAX_FILE)              // No file to send
    {
        return MB_SENDFUNC_STATE_ERR;
    }

    /* Calculate the number of blocks being sent in the file being sent*/
    MBi_calc_sendblock(pPwork->cur_fileid);

    // Obtain the block information
    if (!MBi_get_blockinfo(&bi,
                           &pPwork->fileinfo[pPwork->cur_fileid].blockinfo_table,
                           pPwork->fileinfo[pPwork->cur_fileid].currentb,
                           &pPwork->fileinfo[pPwork->cur_fileid].dl_fileinfo.header))
    {
        return MB_SENDFUNC_STATE_ERR;
    }
    /* Prepare the packets to send*/
    hd.type = MB_COMM_TYPE_PARENT_DATA;
    hd.fid = pPwork->cur_fileid;
    hd.seqno = pPwork->fileinfo[pPwork->cur_fileid].currentb;
    databuf = MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);

    /* Access via cache. (Always hit if there is enough memory)*/
    {
        /* Calculate CARD address from block offset*/
        u32     card_addr = (u32)(bi.offset -
                                  pPwork->fileinfo[pPwork->cur_fileid].blockinfo_table.
                                  seg_src_offset[bi.segment_no] +
                                  pPwork->fileinfo[pPwork->cur_fileid].card_mapping[bi.segment_no]);
        /* Cache read for the specified CARD address*/
        MBiCacheList *const pl = pPwork->fileinfo[pPwork->cur_fileid].cache_list;
        if (!MBi_ReadFromCache(pl, card_addr, databuf, bi.size))
        {
            /* If cache miss, send reload request to task thread*/
            MBiTaskInfo *const p_task = &pPwork->cur_task;
            if (!MBi_IsTaskBusy(p_task))
            {
                /* lifetime value to avoid continuous page fault*/
                if (pl->lifetime)
                {
                    --pl->lifetime;
                }
                else
                {
                    /*
                     * Destroy the cache with the newest address.
                     * We continue to test the operation of this part.
                     */
                    MBiCacheInfo *pi = pl->list;
                    MBiCacheInfo *trg = NULL;
                    int     i;
                    for (i = 0; i < MB_CACHE_INFO_MAX; ++i)
                    {
                        if (pi[i].state == MB_CACHE_STATE_READY)
                        {
                            if (!trg || (trg->src > pi[i].src))
                                trg = &pi[i];
                        }
                    }
                    if (!trg)
                    {
                        OS_TPanic("cache-list is invalid! (all the pages are locked)");
                    }
                    pl->lifetime = 2;
                    trg->state = MB_CACHE_STATE_BUSY;
                    trg->src = (card_addr & ~31);
                    p_task->param[0] = (u32)trg;        /* MBiCacheInfo**/
                    p_task->param[1] = (u32)pl; /* MBiCacheList**/
                    MBi_SetTask(p_task, MBi_ReloadCache, NULL, 4);
                }
            }
            return MB_SENDFUNC_STATE_ERR;
        }
    }

    return MBi_BlockHeaderEnd((int)(bi.size + MB_COMM_PARENT_HEADER_SIZE),
                              pPwork->fileinfo[pPwork->cur_fileid].pollbmp, pPwork->common.sendbuf);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentSendData

  Description:  Sends parent data

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommParentSendData(void)
{
    struct bitmap
    {
        u16     connected;
        u16     req;
        u16     kick;
        u16     boot;
        u16     mem_full;
    };
    struct bitmap bmp;
    u16     child;
    int     errcode;

    MI_CpuClear16(&bmp, sizeof(struct bitmap));

    // Evaluate the parent status of each child
    for (child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {

        switch (pPwork->p_comm_state[child - 1])
        {
        case MB_COMM_PSTATE_CONNECTED:
            bmp.connected |= (1 << child);
            break;

        case MB_COMM_PSTATE_REQ_ACCEPTED:
            bmp.req |= (1 << child);
            break;

        case MB_COMM_PSTATE_KICKED:
            bmp.kick |= (1 << child);
            break;

        case MB_COMM_PSTATE_SEND_PROCEED:
            break;

        case MB_COMM_PSTATE_BOOT_REQUEST:
            bmp.boot |= (1 << child);
            break;

        case MB_COMM_PSTATE_MEMBER_FULL:
            bmp.mem_full |= (1 << child);
            break;

        default:
            break;                     // Does not count the state other than the above.
        }

    }
    /*
       Startmsg > DLFileInfo > Block
       Send in accordance to this priority:
     */
    if (bmp.boot)
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_BOOTREQ, bmp.boot);
    }
    else if (bmp.connected)            // Send entry request permitted message
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_SENDSTART, bmp.connected);
    }
    else if (bmp.mem_full)             // Send member exceeded message
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_MEMBER_FULL, bmp.mem_full);
    }
    else if (bmp.kick)                 // Send entry denied message
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_KICKREQ, bmp.kick);
    }
    else if (bmp.req)                  // Send MbDownloadFileInfo
    {
        errcode = MBi_CommParentSendDLFileInfo();
    }
    else                               // Send Block data
    {
        errcode = MBi_CommParentSendBlock();
    }

    // MP transmission for keeping Connection
    if (MB_SENDFUNC_STATE_ERR == errcode)
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_DUMMY, 0xffff);
    }

    return errcode;

}


/*---------------------------------------------------------------------------*
  Name:         MBi_calc_sendblock

  Description:  Calculates the block to be sent

  Arguments:    file_id - ID of the file to be sent

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_calc_sendblock(u8 file_id)
{
    /* no update if the request for the block specified by the child device has not been received*/
    if ((any_recv_bitmap & (1 << file_id)) == 0)
    {
        return;
    }

    if (pPwork->fileinfo[file_id].active && pPwork->fileinfo[file_id].pollbmp)
    {
        if (pPwork->fileinfo[file_id].nextb <= pPwork->fileinfo[file_id].currentb &&
            pPwork->fileinfo[file_id].currentb <=
            pPwork->fileinfo[file_id].nextb + MB_SEND_THRESHOLD)
        {
            pPwork->fileinfo[file_id].currentb++;
        }
        else
        {
            pPwork->fileinfo[file_id].currentb = pPwork->fileinfo[file_id].nextb;
        }
        MB_DEBUG_OUTPUT("**FILE %2d SendBlock %d\n", file_id, pPwork->fileinfo[file_id].currentb);
    }

}

/*---------------------------------------------------------------------------*
  Name:         MBi_calc_nextsendblock

  Description:  Returns the next block to be sent

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static u16 MBi_calc_nextsendblock(u16 next_block, u16 next_block_req)
{
    return max(next_block_req, next_block);
}


/*  ============================================================================

    miscellany functions

    ============================================================================*/

static inline u16 max(u16 a, u16 b)
{
    return (a > b) ? a : b;
}

static BOOL IsChildAidValid(u16 child_aid)
{
    return (child_aid >= 1 && child_aid <= WM_NUM_MAX_CHILD) ? TRUE : FALSE;
}

static void MBi_CommCallParentError(u16 aid, u16 errcode)
{
    MBErrorStatus e_stat;
    e_stat.errcode = errcode;

    MBi_CommChangeParentStateCallbackOnly(aid, MB_COMM_PSTATE_ERROR, &e_stat);
}
