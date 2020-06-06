/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_wm_base.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_wm_base.c,v $
  Revision 1.13  2006/02/20 02:35:20  seiki_masashi
  Removed support for WM_GAMEINFO_TYPE_OLD 

  Revision 1.12  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.11  12/13/2005 10:41:29  yosizaki
  Removed temporary hook for SEND_FAILED

  Revision 1.10  12/09/2005 08:16:10  yosizaki
  Added hook for SEND_FAILED (temporary)

  Revision 1.9  11/14/2005 05:37:28  yosizaki
  fix about WM_ATTR_FLAG_ENTRY flag.

  Revision 1.8  11/07/2005 01:21:50  okubata_ryoma
  Switched from SDK_STATIC_ASSERT to SDK_COMPILER_ASSERT

  Revision 1.7  10/28/2005 11:23:41  seiki_masashi
  Supported addition of WM_STATECODE_DISCONNECTED_FROM_MYSELF.
  Supported addition of WM_STATECODE_PORT_INIT.

  Revision 1.6  07/29/2005 03:45:01  seiki_masashi
  Added to WM_STATECODE_INFORMATION

  Revision 1.5  04/14/2005 04:40:20  yosizaki
  fix timing of calling WM_GetNextTgid().

  Revision 1.4  04/11/2005 08:51:56  yosizaki
  move MBi_CreateTgid() to wm_sync.c as WM_GetNextTgid().

  Revision 1.3  04/11/2005 01:45:48  yosizaki
  add MB_SetLifeTime().
  add MB_SetPowerSaveMode().
  add MBi_CreateTgid().

  Revision 1.2  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.1  02/21/2005 00:40:08  yosizaki
  replace filename from tmp_lib_stub.


  Revision 1.62  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.61  01/27/2005 07:07:49  takano_makoto
  Revised code so that system enters wait when using ichneumon  

  Revision 1.60  2004/12/28 09:32:28  pair2
  fix around MB_EndToIdle. (check endReq == 1 or pWork == NULL )

  Revision 1.59  11/29/2004 04:47:41  yosizaki
  allocate sendBuf. (avoid to share with SetMPData's buffer)

  Revision 1.58  11/24/2004 05:14:33  yosizaki
  rename MB_StartParentEx/MB_EndEx..

  Revision 1.57  11/22/2004 12:47:26  takano_makoto
  Separated off the division-request transmission process as mb_block

  Revision 1.56  11/18/2004 07:28:46  takano_makoto
  Got rid of the argument for setting child transfer size in MB_SetParentCommSize, MB_SetParentCommParam

  Revision 1.55  11/11/2004 11:46:56  yosizaki
  change WM_End/Ex to finish task-thread before WM_Reset.

  Revision 1.54  11/10/2004 07:40:03  takano_makoto
  Eliminated MBw function from mb_gameinfo.c, and cut the scan parameter settings  

  Revision 1.53  11/10/2004 03:17:02  takano_makoto
  Added MBi_SetBeaconRecvStatusBufferDefault

  Revision 1.52  11/09/2004 12:02:23  yosizaki
  fix allocation of wmBuf.

  Revision 1.51  11/09/2004 01:05:39  seiki_masashi
  Implemented timeout on NULL response

  Revision 1.50  10/26/2004 08:09:54  takano_makoto
  Added MB_DisconnectChild

  Revision 1.49  10/19/2004 00:55:31  sato_masaki
  Fixed location in which a WM_API return value error was not returned with MB_CALLBACK_API_ERROR.  

  Revision 1.48  10/18/2004 11:44:36  yosizaki
  add MB_StartParentEx, MB_EndEx.

  Revision 1.47  10/15/2004 08:50:44  yosizaki
  add start_mp_busy.

  Revision 1.46  10/08/2004 10:32:49  takano_makoto
  Fixed problem of change to nameLength not being reflected in IPL startup screen.

  Revision 1.45  10/08/2004 03:04:17  takano_makoto
  In addition to the correction of 1.44, revised code so that it checks NUL character

  Revision 1.44  10/08/2004 02:44:46  takano_makoto
  Corrected it so the nameLength in the MBUserInfo can be seen by MB_Init  

  Revision 1.43  10/05/2004 09:45:29  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.42  10/04/2004 13:40:25  terui
  Standardized game group IDs as type u32.

  Revision 1.41  10/03/2004 08:17:31  ooe
  Support for changed specifications of StartMPEx. ignoreFatalError.

  Revision 1.40  09/28/2004 08:55:30  sato_masaki
  - Changed code so that OS_Panic is output when WM_STATECODE_FIFO_ERROR occurs with an Indication callback.  
  - Added judgment of size range to MBi_IsCommSizeValid.

  Revision 1.39  09/25/2004 05:51:47  sato_masaki
  Merged with NitroSDK_IPL_branch 1.27.2.7

  Revision 1.38  09/25/2004 05:43:49  sato_masaki
  Added process to prohibit interrupts while API called from main loop is executing.

  Revision 1.37  09/22/2004 09:41:18  sato_masaki
  Merged with IPL_branch 1.27.2.5

  Revision 1.36  09/22/2004 02:50:31  yosizaki
  optimize for code-size around MBi_ChildCallback().

  Revision 1.35  09/22/2004 02:31:57  yosizaki
  optimize for code-size around MB_Init().

  Revision 1.34  09/21/2004 01:39:15  sato_masaki
  Standardized errors that originate in WM to MB_ERRCODE_WM_FAILURE. Abolished MB_ERRCODE_MP_SENT_FAILURE.

  Revision 1.33  09/20/2004 12:56:17  sato_masaki
  Regarding MB_Init, added indication of whether initialization was completed. 

  Revision 1.32  09/18/2004 09:16:26  sato_masaki
  Changed so MB_End() does nothing if the flag for p_mbi_param->endReq is standing.

  Revision 1.31  09/18/2004 03:11:25  sato_masaki
  small fix

  Revision 1.30  09/18/2004 02:58:50  sato_masaki
  bug fix

  Revision 1.29  09/17/2004 11:36:30  sato_masaki
  - add function MBi_IsStarted()
  - change MP size check (it always returns 'TRUE')

  Revision 1.28  09/16/2004 12:48:35  sato_masaki
  - In mb.h, changed functions defined as MBi_*** to be MB_***.
  - Changed code so that size of incoming and outgoing data is checked for validity.  

  Revision 1.27  09/15/2004 06:51:59  sato_masaki
  Deleted processes regarding WBT definitions.

  Revision 1.26  09/15/2004 06:17:47  sato_masaki
  add MBi_SetParentCommSize, MBi_GetTgid functions

  Revision 1.25  09/14/2004 12:07:15  terui
  Deleted the rand function, and replaced with feature that uses MAC addresses to randomize beacon intervals.  

  Revision 1.24  09/14/2004 02:55:29  sato_masaki
  Corrected comments

  Revision 1.23  09/14/2004 02:41:38  sato_masaki
  Added mpBusy to MBiParam.

  Revision 1.21  09/13/2004 13:08:26  sato_masaki
  - Added MBi_ChildPortCallback().
  - Changed it so MP is not set when WM_STATECODE_CHILD_CONNECTED.

  Revision 1.20  09/13/2004 02:56:23  sato_masaki
  To retain compatibility with past specifications, reconfigured MBi_Init() to have tgid as an argument. 

  Revision 1.19  09/11/2004 12:49:06  yosiokat
  Changed the scan-time setting in MBi_StartCommon from the constant value 10 to MB_SCAN_TIME_NORMAL.

  Revision 1.18  09/11/2004 11:16:10  sato_masaki
  - For both parent and child, added validity testing of work size in SDK_STATIC_ASSERT.  
  - Separated the MB_CommWork structure into MB_CommPWork, MB_CommCWork and MB_CommCommonWork.

  Revision 1.17  09/11/2004 07:02:08  yosiokat
  - Stopped parent/child code separation of SDK_SMALL_BUILD definitions, and modified code so that definitions can separated after normal build.  
    - Took the part of the MBi_StartCore process that can be shared and made it MBi_StartCommon, and moved the rest to MB_StartParent and MB_StartChild.
    The callback specification is now p_mbi_param->callback_ptr, whereas before it was from MBi_ParentCallback.

  Revision 1.16  09/11/2004 05:01:26  terui
  Undid tentative measures for macros calculating the size of send and receive buffers. 

  Revision 1.15  09/11/2004 04:00:21  sato_masaki
  In the WM_Calc*******() macros, made the BOOL value that indicates the use of key sharing TRUE.
  (This is a temporary measure to avoid WM_StartMPEx() errors.))

  Revision 1.14  09/10/2004 12:32:47  seiki_masashi
  Changed it so the number of bytes sent for key sharing is concealed in the library.

  Revision 1.13  09/10/2004 07:50:15  sato_masaki
  Made changes so that in the changeScanChannel() function, ScanChannel will be selected from among the
  usable channels obtained from WM_GetAllowedChannel().
  Also, changed it so MBi_CommEnd() is called if there are no usable channel.    

  Revision 1.12  09/10/2004 02:32:23  sato_masaki
  Changed the value of scanParam.channel when WM_StartScan() is issued.

  Revision 1.11  09/09/2004 06:49:08  sato_masaki
  bug fix about changeScanChannel()

  Revision 1.10  09/09/2004 06:29:49  yosizaki
  change WM_SetMPData to use ToPort(WM_PORT_BT, WM_PRIORITY_LOW).

  Revision 1.9  09/09/2004 02:46:53  sato_masaki
  Restored CAM_LIFE_TIME to 40.

  Revision 1.8  09/09/2004 01:33:18  sato_masaki
  Restored FRAME_LIFE_TIME to 5, and set CAM_LIFE_TIME to 2*40.

  Revision 1.7  09/09/2004 00:50:03  sato_masaki
  Set FRAME_LIFE_TIME to 10.

  Revision 1.6  09/08/2004 13:10:09  sato_masaki
  Made it so MB_CALLBACK_MP_CHILD_RECV is returned also when

  Revision 1.5  09/08/2004 10:58:54  sato_masaki
  - Set LIFETIME to finite.
  - Changed the seek channels during scanning to 1, 7 and 13.
  - Changed WM_StartMP to Ex, and set mode to Do Not Resend.    

  Revision 1.4  09/06/2004 12:05:24  sato_masaki
  Deleted comments relating to StartConnect.

  Revision 1.3  09/06/2004 11:43:21  sato_masaki
  - Added tgid as an argument for the MB_Init() function.
  Added the MBi_GetTgid() function.

  Revision 1.2  09/04/2004 09:29:03  yosiokat
  Changed it so parent mode's code is deleted when SDK_SMALL_BUILD is defined.

  Revision 1.1  09/03/2004 07:04:36  sato_masaki
  Divided files according to features.

  Revision 1.25  09/02/2004 09:46:49  sato_masaki
  Added a zero-clear to mbc->fileinfo.

  Revision 1.24  08/30/2004 06:32:46  sato_masaki
  Changed the WMBT definition to WBT.

  Revision 1.23  08/26/2004 10:08:57  sato_masaki
  Added a state check when BeaconRecv.

  Revision 1.22  08/26/2004 09:54:59  sato_masaki
  small fix

  Revision 1.21  08/24/2004 12:20:06  sato_masaki
  SSID match countermeasure (temporarily fix)

  Revision 1.20  08/23/2004 04:45:17  ooe
  Supported changes to WM_StartConnect() specifications.

  Revision 1.19  08/19/2004 22:55:17  yosiokat
  Multiboot flag and GGID are no longer set by MB_Init, and instead they are set by WM_SetGameInfo when sending beacon.  

  Revision 1.18  08/19/2004 16:00:09  miya
  modified WM_SetGameInfo function

  Revision 1.17  08/19/2004 15:11:47  yosiokat
  - Added comments relating to previous correction. Changed p_mbi_param->parentParam->multiBootFlag= 1 under MB_Init.

  Revision 1.16  08/19/2004 15:07:37  yosiokat
  Revised so p_mbi_param->parentParam->userName, gameName is set only for the WM_GAMEINFO_TYPE_OLD definition.

  Revision 1.15  08/19/2004 12:27:20  sato_masaki
  Commented out the communications termination routine in "Beacon Indicate".  

  Revision 1.14  08/19/2004 08:00:53  ikedae
  Deleted #include <string.h>

  Revision 1.13  08/19/2004 07:51:40  sato_masaki
  Support for changes to WM specifications.

  Revision 1.12  08/19/2004 07:34:16  ooe
  Randomized beaconPeriod (Tentative for experimental use)

  Revision 1.11  08/19/2004 02:42:36  yosizaki
  change type of MBUserInfo.

  Revision 1.10  08/18/2004 01:25:09  miya
  small fix

  Revision 1.9  08/18/2004 00:05:14  sato_masaki
  Changed code to reflect change to WM_StartMP argument.  

  Revision 1.8  08/16/2004 13:02:40  sato_masaki
  Added the MBi_GetSendBufSize function

  Revision 1.7  08/12/2004 09:20:57  yosiokat
  - Added the MBi_RestartScan, MBi_SetMaxScanTime and MBi_GetScanChannel functions.
  - Changed code so that callback is returned to MB_Comm layer even when PARENT_NOT_FOUND occurs during scanning.  

  Revision 1.6  08/10/2004 12:11:55  sato_masaki
  Added the MBi_GetAid function.

  Revision 1.5  08/09/2004 14:56:43  yosizaki
  format all the symbol for API

  $NoKeywords: $
 *---------------------------------------------------------------------------*/



#include "mb_private.h"


/*
 * Determine whether capacity is valid with requested size.  
 */
SDK_COMPILER_ASSERT(32 + sizeof(MBiParam) + 32 + sizeof(MB_CommPWork) + 32 + WM_SYSTEM_BUF_SIZE <=
                    MB_SYSTEM_BUF_SIZE);
SDK_COMPILER_ASSERT(32 + sizeof(MBiParam) + 32 + sizeof(MB_CommCWork) + 32 + WM_SYSTEM_BUF_SIZE <=
                    MB_CHILD_SYSTEM_BUF_SIZE);


/*
 * Macro for computing size of parent/child send and receive buffers  
 * Reception buffer  parentMaxSize + WLHeader(48B) + WMHeader(2B)
 * Sending buffer (parentMaxSize + WMHeader(2B) + (KeySet(36B) only when KS) + 31) & 0xffffffe0 (Corrected alignment)
 * Reception buffer (childMaxSize + WLHeader(8B) + WMHeader(2B) + (KeyData(2B)(only when KeySharing)) is 15 sets worth + total Header(8B) + 31) & 0xffffffe0 (Corrected alignment)
 */
#define WL_HEADER_LENGTH_P  48         /* WL header length when parent receives*/
#define WM_HEADER_LENGTH    2          /* WM Header length*/
#define WL_HEADER_LENGTH_T  8          /* Total WL header length*/
#define WL_HEADER_LENGTH_C  8          /* WL header length of each child's data*/


// Macro for computing size of child Reception buffer

#define WM_CalcChildSendBufSize(_pInfo_)        (u16)(WM_SIZE_MP_CHILD_SEND_BUFFER(((WMGameInfo *)(&(((WMBssDesc*)(_pInfo_))->gameInfo)))->childMaxSize, FALSE))
#define WM_CalcChildRecvBufSize(_pInfo_)        (u16)(WM_SIZE_MP_CHILD_RECEIVE_BUFFER(((WMGameInfo *)(&(((WMBssDesc*)(_pInfo_))->gameInfo)))->parentMaxSize, FALSE))

// Macro for computing size of parent send/receive buffers
#define WM_CalcParentSendBufSize(_libParam_)    (u16)(WM_SIZE_MP_PARENT_SEND_BUFFER(((MBiParam*)(_libParam_))->parentParam.parentMaxSize, FALSE))
#define WM_CalcParentRecvBufSize(_libParam_)    (u16)(WM_SIZE_MP_PARENT_RECEIVE_BUFFER(((MBiParam*)(_libParam_))->parentParam.childMaxSize, WM_NUM_MAX_CHILD, FALSE))


/* Default value for Beacon interval (ms)*/
#define MB_BEACON_PERIOD_DEFAULT    (200)

/*
 * Debug switch for switching LifeTime.
 */
#define  NO_LIFETIME    0
#if (NO_LIFETIME == 1)
#define FRAME_LIFE_TIME 0xFFFF
#define CAM_LIFE_TIME   0xFFFF
#define MP_LIFE_TIME    0xFFFF
#else
/* Disable FRAME_LIFE_TIME so that disconnect is accomplished even under severely restricted environments*/
#define FRAME_LIFE_TIME 0xFFFF
#define CAM_LIFE_TIME   40
#define MP_LIFE_TIME    40
#endif

#define TABLE_NO    0xFFFF


static u16 mbi_life_table_no = TABLE_NO;
static u16 mbi_life_frame = FRAME_LIFE_TIME;
static u16 mbi_life_cam = CAM_LIFE_TIME;
static u16 mbi_life_mp = MP_LIFE_TIME;
static BOOL mbi_power_save_mode = TRUE;


//===========================================================
// Function Prototype Declarations
//===========================================================
static void MBi_ScanLock(u8 *macAddr); // Function for setting scan lock
static void MBi_ScanUnlock(void);      // Function for releasing scan lock

/*
 * Callback if error found when checking WM result
 */
static void MBi_CheckWmErrcode(u16 apiid, int errcode);
static void MBi_ParentCallback(void *arg);



/******************************************************************************/
/* Variables*/

static MBiParam *p_mbi_param;
static u16 WM_DMA_NO;

/*
 * WM system buffer.
 * Originally was a member of MBiParam, but made  
 * independent for MB_StartParentFromIdle().  
 */
static u8 *wmBuf;


MB_CommCommonWork *mbc = NULL;


/* Variables for child*/
static WMscanParam scanParam ATTRIBUTE_ALIGN(32);


/******************************************************************************/
/* functions*/


/* Cycle through ScanChannel, changing*/
static BOOL changeScanChannel(WMscanParam *p)
{
    u16     channel_bmp, channel, i;

    /* get channel bitmap*/
    channel_bmp = WM_GetAllowedChannel();

    /* If there is no available channel, return FALSE.*/
    if (channel_bmp == 0)
    {
        OS_TWarning("No Available Scan channel\n");
        return FALSE;
    }

    /* If usable channels exist*/
    for (i = 0, channel = p->channel;
         i < 16; i++, channel = (u16)((channel == 16) ? 1 : channel + 1))
    {
        if ((channel_bmp & (1 << (channel - 1))) == 0)
        {
            continue;
        }

        /* If the detected channel is the same one as before, search for another channel*/
        if (p->channel != channel)
        {
            p->channel = channel;
            break;
        }
    }

    return TRUE;

}


/*---------------------------------------------------------------------------*
  Name:         MB_SetLifeTime

  Description:  Explicitly specifies the lifetime of the MB wireless driver.
                Default value: ( 0xFFFF, 40, 0xFFFF, 40 )..

  Arguments:    tgid            Specified TGID

  Returns:      The argument is returned unchanged or a suitable TGID is returned.
 *---------------------------------------------------------------------------*/
void MB_SetLifeTime(u16 tableNumber, u16 camLifeTime, u16 frameLifeTime, u16 mpLifeTime)
{
    mbi_life_table_no = tableNumber;
    mbi_life_cam = camLifeTime;
    mbi_life_frame = frameLifeTime;
    mbi_life_mp = mpLifeTime;
}

/*---------------------------------------------------------------------------*
  Name:         MB_SetPowerSaveMode

  Description:  Sets constant power mode.
                This option is for stabilized operation in situations where power consumption can be ignored. It is disabled by default.
                This option should not be used by typical game applications unless they are being operated in an environment in which connection to power is guaranteed.

  Arguments:    enable           To enable, TRUE; To disable, FALSE.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_SetPowerSaveMode(BOOL enable)
{
    mbi_power_save_mode = enable;
}


/*
 * Assume processing has succeeded and return the converted WM result
 */
static inline int conv_errcode(int errcode)
{
    return (errcode == WM_ERRCODE_OPERATING) ? WM_ERRCODE_SUCCESS : errcode;
}


/*
 * Check if MP transmissions are possible.
   
   In order prevent another set of MP after SetMP and before the callback returns,  
   added an element to check the mpBusy flag, which is set when SetMP is executing.  
 
 */
static BOOL MBi_IsSendEnabled(void)
{
    return (p_mbi_param->mpStarted == 1) &&
        (p_mbi_param->mpBusy == 0) &&
        (p_mbi_param->endReq == 0) && (p_mbi_param->child_bitmap != 0);
}

static void MBi_OnInitializeDone(void)
{
    int     errcode;
    /* Issue API*/
    errcode = WM_SetIndCallback(MBi_ParentCallback);
    MBi_CheckWmErrcode(WM_APIID_INDICATION, errcode);
    errcode = WM_SetLifeTime(MBi_ParentCallback, mbi_life_table_no,
                             mbi_life_cam, mbi_life_frame, mbi_life_mp);
    MBi_CheckWmErrcode(WM_APIID_SET_LIFETIME, errcode);
}


/* 
 * If the MB parent is using ichneumon to operate wireless with VRAM, * the parent's MP data settings will be processed too fast if there are
 * not enough child units, and the IPL will not follow the parent's
 * MP communications.
 * As a countermeasure to this, insert a temporary wait.
 */
static inline void MbWaitForWvr(u32 cycles)
{
    u32     child_cnt = 0;
    u32     i;

    for (i = 0; i < MB_MAX_CHILD; i++)
    {
        if (pPwork->p_comm_state[i] != MB_COMM_PSTATE_NONE)
        {
            if (++child_cnt >= 2)
                break;
        }
    }
    // If there is only one child, insert a wait process.
    if (child_cnt == 1)
    {
        OS_SpinWait(cycles);
    }
}

/*
 * Parent callback
 */
static void MBi_ParentCallback(void *arg)
{
    int     errcode;
    WMCallback *buf = (WMCallback *)arg;

    switch (buf->apiid)
    {
    case WM_APIID_INITIALIZE:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }
            MBi_OnInitializeDone();
        }
        break;

    case WM_APIID_SET_LIFETIME:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            // Issue API
            errcode = WM_SetParentParameter(MBi_ParentCallback, &p_mbi_param->parentParam);
            MBi_CheckWmErrcode(WM_APIID_SET_P_PARAM, errcode);
        }
        break;

    case WM_APIID_SET_P_PARAM:
        {
            p_mbi_param->callback(MB_CALLBACK_INIT_COMPLETE, arg);
            // Error code does not get returned here
            errcode = WM_SetBeaconIndication(MBi_ParentCallback, 1 /* 1:ON, 0:OFF*/ );
            MBi_CheckWmErrcode(WM_APIID_SET_BEACON_IND, errcode);
        }
        break;

        // Check whether Beacon Send/Recv Indication has occurred
    case WM_APIID_SET_BEACON_IND:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            errcode = WMi_StartParentEx(MBi_ParentCallback, mbi_power_save_mode);
            MBi_CheckWmErrcode(WM_APIID_START_PARENT, errcode);
        }
        break;

    case WM_APIID_START_PARENT:
        {
            WMstartParentCallback *callback = (WMstartParentCallback *)arg;

            if (callback->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            switch (callback->state)
            {
            case WM_STATECODE_PARENT_START:
                p_mbi_param->child_bitmap = 0;
                p_mbi_param->mpStarted = 0;
                break;
            case WM_STATECODE_CHILD_CONNECTED:

                /* Forced break after MB_End() call*/
                if (p_mbi_param->endReq == 1)
                    break;

                p_mbi_param->child_bitmap |= (0x1 << callback->aid);
                p_mbi_param->callback(MB_CALLBACK_CHILD_CONNECTED, arg);

                // If before start of MP, then start MP
                if ((p_mbi_param->mpStarted == 0) && (!mbc->start_mp_busy))
                {
                    mbc->start_mp_busy = TRUE;
                    errcode = WM_StartMPEx(MBi_ParentCallback, (u16 *)p_mbi_param->recvBuf, p_mbi_param->recvBufSize, (u16 *)p_mbi_param->sendBuf, p_mbi_param->sendBufSize, (u16)(p_mbi_param->contSend ? 0 : 1), 0,   /* defaultRetryCount*/
                                           FALSE, FALSE, 1,     /* fixFreqMode*/
                                           TRUE /* ignoreFatalError*/
                        );
                    MBi_CheckWmErrcode(WM_APIID_START_MP, errcode);

                }
                else
                {
                    // MP send-enabled callback
                    if (MBi_IsSendEnabled())
                    {
                        p_mbi_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
                    }
                }
                break;

            case WM_STATECODE_DISCONNECTED:
                p_mbi_param->child_bitmap &= ~(0x1 << callback->aid);
                p_mbi_param->callback(MB_CALLBACK_CHILD_DISCONNECTED, arg);
                break;

            case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
                // Does nothing if the unit disconnects from itself
                break;

            case WM_STATECODE_BEACON_SENT:
                /* Forced break after MB_End() call*/
                if (p_mbi_param->endReq == 1)
                    break;

                // Notify end of beacon transmission
                p_mbi_param->callback(MB_CALLBACK_BEACON_SENT, arg);
                break;

            default:
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
        }
        break;

    case WM_APIID_START_MP:
        {
            // Error code does not get returned here
            WMstartMPCallback *callback = (WMstartMPCallback *)arg;
            mbc->start_mp_busy = FALSE;
            switch (callback->state)
            {
            case WM_STATECODE_MP_START:
                // Set flag indicating that MP has started MP
                p_mbi_param->mpStarted = 1;
                {
                    // MP send-enabled callback
//                    if( MBi_IsSendEnabled() ) {
                    p_mbi_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
//                    }
                }
                break;

            case WM_STATECODE_MPEND_IND:
                // MP reception callback
                p_mbi_param->callback(MB_CALLBACK_MP_PARENT_RECV, (void *)(callback->recvBuf));
                break;

            default:
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
            break;
        }
        break;

    case WM_APIID_SET_MP_DATA:
        {
            /* If MB parent is using ichneumon, enter a wait*/
            if (pPwork->useWvrFlag)
            {
                MbWaitForWvr(13000);   // ARM9 made to wait roughly 3 lines during IRQ interrupt  
            }
            p_mbi_param->mpBusy = 0;
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_MP_PARENT_SENT, arg);
                p_mbi_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);        // Next transmission is allowed
            }
            else if (buf->errcode == WM_ERRCODE_SEND_QUEUE_FULL)
            {
                p_mbi_param->callback(MB_CALLBACK_SEND_QUEUE_FULL_ERR, arg);
            }
            else
            {
                p_mbi_param->callback(MB_CALLBACK_MP_PARENT_SENT_ERR, arg);
                p_mbi_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);        // Next transmission is allowed
            }
        }
        break;

    case WM_APIID_RESET:
        if (!mbc->is_started_ex)
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->endReq = 0;
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }
            p_mbi_param->child_bitmap = 0;
            p_mbi_param->mpStarted = 0;

            errcode = WM_End(MBi_ParentCallback);
            MBi_CheckWmErrcode(WM_APIID_END, errcode);
            break;
        }
        else
        {
            (void)WM_SetPortCallback(WM_PORT_BT, NULL, NULL);
            (void)WM_SetIndCallback(NULL);
            /*
             * |
             * | Fall.
             * V
             */
        }
        // don't break;
    case WM_APIID_END:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->endReq = 0;
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            p_mbi_param->mbIsStarted = 0;
            mbc->isMbInitialized = 0;
            p_mbi_param->callback(MB_CALLBACK_END_COMPLETE, arg);
        }
        break;

    case WM_APIID_DISCONNECT:
        {
            WMDisconnectCallback *callback = (WMDisconnectCallback *)arg;

            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                return;
            }

            // Update the child information
            p_mbi_param->child_bitmap &= ~(callback->disconnectedBitmap);
        }
        break;

    case WM_APIID_INDICATION:
        {
            WMindCallback *cb = (WMindCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_BEACON_RECV:     // Indicate beacon received
                p_mbi_param->callback(MB_CALLBACK_BEACON_RECV, arg);
                break;
            case WM_STATECODE_DISASSOCIATE:    // Indicate disconnection
                p_mbi_param->callback(MB_CALLBACK_DISASSOCIATE, arg);
                break;
            case WM_STATECODE_REASSOCIATE:     // Indicate reconnection
                p_mbi_param->callback(MB_CALLBACK_REASSOCIATE, arg);
                break;
            case WM_STATECODE_AUTHENTICATE:    // Indicate confirmed authentication
                p_mbi_param->callback(MB_CALLBACK_AUTHENTICATE, arg);
                break;

            case WM_STATECODE_FIFO_ERROR:
                OS_TPanic("FIFO Error\n");
                break;
            case WM_STATECODE_INFORMATION:
                // Does nothing.
                break;
            }
        }
        break;

    default:
        p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
        break;
    }
}

/*
 * Child callback
 */
static void MBi_ChildPortCallback(void *arg)
{
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_PORT_RECV:
        // Indicate reception of data
        p_mbi_param->callback(MB_CALLBACK_MP_CHILD_RECV, (void *)(arg));
        break;
    case WM_STATECODE_CONNECTED:
        // Connection notification
        break;
    case WM_STATECODE_PORT_INIT:
    case WM_STATECODE_DISCONNECTED:
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        break;
    }
}


static void MBi_ChildCallback(void *arg)
{
    WMCallback *buf = (WMCallback *)arg;
    MBiParam *const p_param = p_mbi_param;
    int     errcode;

    switch (buf->apiid)
    {
        //---------------------------------------------------------------
    case WM_APIID_INITIALIZE:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            p_param->callback(MB_CALLBACK_INIT_COMPLETE, arg);

            // Issue API
            errcode =
                WM_SetLifeTime(MBi_ChildCallback, mbi_life_table_no, mbi_life_cam, mbi_life_frame,
                               mbi_life_mp);
            MBi_CheckWmErrcode(WM_APIID_SET_LIFETIME, errcode);
        }
        break;

    case WM_APIID_SET_LIFETIME:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            // Issue WM_StartScan
            scanParam.scanBuf = &p_param->parentInfoBuf;
            if (scanParam.channel == 0)
                scanParam.channel = 1;
            if (scanParam.maxChannelTime == 0)
            {
                scanParam.maxChannelTime = 200;
            }
            scanParam.bssid[0] = 0xff;
            scanParam.bssid[1] = 0xff;
            scanParam.bssid[2] = 0xff;
            scanParam.bssid[3] = 0xff;
            scanParam.bssid[4] = 0xff;
            scanParam.bssid[5] = 0xff;
            p_param->scanning_flag = TRUE;
            p_param->scan_channel_flag = TRUE;
            errcode = WM_StartScan(MBi_ChildCallback, &scanParam);
            MBi_CheckWmErrcode(WM_APIID_START_SCAN, errcode);
        }
        break;

    case WM_APIID_START_SCAN:
        {
            WMstartScanCallback *callback = (WMstartScanCallback *)arg;

            if (callback->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            switch (callback->state)
            {
            case WM_STATECODE_SCAN_START:
                break;

            case WM_STATECODE_PARENT_FOUND:
                // Store the parent information obtained with scan
                {
                    WMstartScanCallback *buf = (WMstartScanCallback *)arg;
                    ParentInfo *p = &p_param->parent_info[0];

                    int     i;
                    for (i = 0; i < p_param->found_parent_count; ++i)
                    {

                        if (WM_IsBssidEqual(buf->macAddress, p[i].scan_data.macAddress))
                        {
                            p[i].scan_data.gameInfoLength = buf->gameInfoLength;
                            p[i].scan_data.gameInfo = buf->gameInfo;
                            DC_InvalidateRange(p_param->parent_info[i].parentInfo,
                                               WM_BSS_DESC_SIZE);
                            MI_DmaCopy16(WM_DMA_NO, &p_param->parentInfoBuf,
                                         p_param->parent_info[i].parentInfo, WM_BSS_DESC_SIZE);
                            p_param->last_found_parent_no = i;
                            goto scan_end;
                        }
                    }

                    if (i < MB_NUM_PARENT_INFORMATIONS)
                    {
                        p_param->found_parent_count = (u16)(i + 1);
                        // Newly discovered parent, so store data
                        MI_CpuCopy16(buf, &p[i].scan_data, sizeof(p[i].scan_data));
                        DC_InvalidateRange(p_param->parent_info[i].parentInfo, WM_BSS_DESC_SIZE);
                        MI_DmaCopy16(WM_DMA_NO, &p_param->parentInfoBuf,
                                     p_param->parent_info[i].parentInfo, WM_BSS_DESC_SIZE);
                        p_param->last_found_parent_no = i;
                    }
                }
              scan_end:
                p_param->callback(MB_CALLBACK_PARENT_FOUND, arg);
                if (!p_param->scanning_flag)
                {
                    return;
                }

                if (p_param->scan_channel_flag)
                {
                    if (FALSE == changeScanChannel(&scanParam))
                    {
                        (void)MBi_CommEnd();
                    }
                }
                errcode = WM_StartScan(MBi_ChildCallback, &scanParam);
                MBi_CheckWmErrcode(WM_APIID_START_SCAN, errcode);
                break;

            case WM_STATECODE_PARENT_NOT_FOUND:
                p_param->callback(MB_CALLBACK_PARENT_NOT_FOUND, arg);   // This callback to ParentInfoLifeTimeCount
                if (!p_param->scanning_flag)
                {
                    return;
                }

                if (p_param->scan_channel_flag)
                {
                    if (FALSE == changeScanChannel(&scanParam))
                    {
                        (void)MBi_CommEnd();
                    }
                }
                errcode = WM_StartScan(MBi_ChildCallback, &scanParam);
                MBi_CheckWmErrcode(WM_APIID_START_SCAN, errcode);
                break;

            default:
                p_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
        }
        break;

    case WM_APIID_END_SCAN:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            errcode = WM_StartConnect(MBi_ChildCallback, p_param->pInfo, NULL);
            MBi_CheckWmErrcode(WM_APIID_START_CONNECT, errcode);
        }
        break;

    case WM_APIID_START_CONNECT:
        {
            WMstartConnectCallback *callback = (WMstartConnectCallback *)arg;

            if (callback->errcode != WM_ERRCODE_SUCCESS)
            {
                /* Reset the number of parents*/
                p_param->found_parent_count = 0;
                // Notify of callback error
                p_param->callback(MB_CALLBACK_CONNECT_FAILED, arg);
                return;
            }

            switch (callback->state)
            {
            case WM_STATECODE_CONNECT_START:
                p_param->child_bitmap = 0;
                p_param->mpStarted = 1;
                break;

            case WM_STATECODE_CONNECTED:
                p_param->my_aid = (u16)callback->aid;
                p_param->callback(MB_CALLBACK_CONNECTED_TO_PARENT, arg);
                p_param->child_bitmap = 1;

                errcode = WM_SetPortCallback(WM_PORT_BT, MBi_ChildPortCallback, NULL);

                if (errcode != WM_ERRCODE_SUCCESS)
                {
                    break;
                }

                errcode = WM_StartMPEx(MBi_ChildCallback, (u16 *)p_param->recvBuf, p_param->recvBufSize, (u16 *)p_param->sendBuf, p_param->sendBufSize, (u16)(p_param->contSend ? 0 : 1), 0,    /* defaultRetryCount*/
                                       FALSE, FALSE, 1, /* fixFreqMode*/
                                       TRUE     /* ignoreFatalError*/
                    );
                MBi_CheckWmErrcode(WM_APIID_START_MP, errcode);
                break;

            case WM_STATECODE_DISCONNECTED:
                p_param->callback(MB_CALLBACK_DISCONNECTED_FROM_PARENT, arg);
                p_param->child_bitmap = 0;
                p_param->mpStarted = 0;
                break;

            case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
                // Does nothing if the unit disconnects from itself
                break;

            default:
                p_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
        }
        break;

    case WM_APIID_START_MP:
        {
            WMstartMPCallback *callback = (WMstartMPCallback *)arg;

            switch (callback->state)
            {
            case WM_STATECODE_MP_START:
                p_param->mpStarted = 1; // Set flag indicating that MP has started MP
                {
                    // MP send-enabled callback
                    if (MBi_IsSendEnabled())
                    {
                        p_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
                    }
                }
                break;

            case WM_STATECODE_MP_IND:
                if (callback->errcode == WM_ERRCODE_INVALID_POLLBITMAP)
                {
//                  p_param->callback( MB_CALLBACK_MP_CHILD_RECV, (void*)(callback->recvBuf) );
                }
                else
                {
//                    p_param->callback( MB_CALLBACK_MP_CHILD_RECV, (void*)(callback->recvBuf) );
                }
                break;

            case WM_STATECODE_MPACK_IND:
                {
                    //p_param->callback( MB_CALLBACK_MPACK_IND, NULL );
                }
                break;

            default:
                p_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
        }
        break;

    case WM_APIID_SET_MP_DATA:
        {
            p_param->mpBusy = 0;
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_MP_CHILD_SENT, arg);
            }
            else if (buf->errcode == WM_ERRCODE_TIMEOUT)
            {
                p_param->callback(MB_CALLBACK_MP_CHILD_SENT_TIMEOUT, arg);
            }
            else
            {
                p_param->callback(MB_CALLBACK_MP_CHILD_SENT_ERR, arg);
            }
            // Next transmission is allowed
            p_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_RESET:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->endReq = 0;
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }
            p_mbi_param->child_bitmap = 0;
            p_mbi_param->mpStarted = 0;

            errcode = WM_End(MBi_ChildCallback);
            MBi_CheckWmErrcode(WM_APIID_END, errcode);
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_END:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->endReq = 0;
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            p_param->mbIsStarted = 0;
            mbc->isMbInitialized = 0;
            p_param->callback(MB_CALLBACK_END_COMPLETE, arg);
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_KS:
        {
            // MP send-enabled callback
            if (MBi_IsSendEnabled())
            {
                p_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
            }
        }
        break;

        //---------------------------------------------------------------        
    case WM_APIID_INDICATION:
        {
            WMindCallback *cb = (WMindCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_FIFO_ERROR:
                OS_TPanic("FIFO Error\n");
                break;
            case WM_STATECODE_INFORMATION:
                // Does nothing.
                break;
            }
        }
        break;
    default:
        p_param->callback(MB_CALLBACK_ERROR, arg);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_GetBeaconPeriodDispersion

  Description:  Gets a sufficiently randomized value to add to the beacon interval
                for that machine's MAC address. 

  Arguments:    None.

  Returns:      u32 - A sufficiently randomized value (0 to 19) for that machine.  
 *---------------------------------------------------------------------------*/
static u32 MBi_GetBeaconPeriodDispersion(void)
{
    u8      mac[6];
    u32     ret;
    s32     i;

    OS_GetMacAddress(mac);
    for (i = 0, ret = 0; i < 6; i++)
    {
        ret += mac[i];
    }
    ret += OS_GetVBlankCount();
    ret *= 7;
    return (ret % 20);
}


/******************************************************************************/
/* API*/

/* initialization*/
int MB_Init(void *work, const MBUserInfo *user, u32 ggid, u32 tgid, u32 dma)
{

    SDK_ASSERT(user != NULL);
    SDK_ASSERT(work != NULL);
    SDK_ASSERT((dma < 4));
    SDK_ASSERT(user->nameLength <= MB_USER_NAME_LENGTH);

    if (mbc && mbc->isMbInitialized)
    {
        return MB_ERRCODE_INVALID_STATE;
    }
    else
    {
        MBiParam *const p_parm = (MBiParam *) ((((u32)work) + 0x1F) & ~0x1F);
        MB_CommCommonWork *const p_com =
            (MB_CommCommonWork *) ((((u32)p_parm) + sizeof(*p_parm) + 0x1F) & ~0x1F);
        OSIntrMode enabled;

        /*
         * Automatic retrieval is performed if TGID is automatically specified.
         * (Since RTC is used internally, this is retrieved before interrupts are disabled.)
         */
        if (tgid == MB_TGID_AUTO)
        {
            tgid = WM_GetNextTgid();
        }

        enabled = OS_DisableInterrupts();

        /* Initialize DMA channel and work region*/
        WM_DMA_NO = (u16)dma;
        p_mbi_param = p_parm;
        mbc = p_com;
        MI_CpuClear32(p_parm, sizeof(*p_parm));
        MI_CpuClear16(p_com, sizeof(*p_com));

        {                              /* Save user name and game name*/
            int     i;
            static const u16 *game = L"multiboot";
            u16    *c;
            c = (u16 *)p_parm->uname;
            for (i = 0; i < user->nameLength; ++i)
            {
                *c++ = user->name[i];
            }
            c = (u16 *)p_parm->gname;
            for (i = 0; i < WM_SIZE_GAMENAME; ++i)
            {
                if (*game == 0)
                {
                    break;
                }
                *c++ = *game++;
            }
            MI_CpuCopy8(user, &p_com->user, sizeof(MBUserInfo));
            if (user->nameLength < MB_USER_NAME_LENGTH)
            {
                p_com->user.name[user->nameLength] = 0;
            }
        }

        {
            p_parm->p_sendlen = MB_COMM_P_SENDLEN_DEFAULT;
            p_parm->p_recvlen = MB_COMM_P_RECVLEN_DEFAULT;

            /* Set the parent information (information obtained by child when scanned)*/
            p_parm->sendBufSize = 0;
            p_parm->recvBufSize = 0;

            /* Unnecessary ? */
            p_parm->contSend = 1;

            p_parm->recvBuf = (WMmpRecvBuf *)p_com->recvbuf;

            {
                WMParentParam *const p_parent = &p_parm->parentParam;
                /*
                 * The first beacon is invalid in entry and mb.
                 * Turned on with SetGameInfo in MbBeacon
                 */
                p_parent->entryFlag = 0;
                p_parent->multiBootFlag = 0;
                p_parent->CS_Flag = 1;
                p_parent->KS_Flag = 0;
                /* Set own unique GGID TGID*/
                p_parent->ggid = ggid;
                p_parent->tgid = (u16)tgid;
                p_parent->beaconPeriod =
                    (u16)(MB_BEACON_PERIOD_DEFAULT + MBi_GetBeaconPeriodDispersion());
                p_parent->maxEntry = 15;
            }
        }

        p_parm->mpBusy = 0;
        p_parm->mbIsStarted = 0;
        p_com->isMbInitialized = 1;

        p_com->start_mp_busy = FALSE;

        (void)OS_RestoreInterrupts(enabled);
    }

    return MB_ERRCODE_SUCCESS;
}

#define MP_USEC_TIME_LIMIT  (5600)

static BOOL MBi_IsCommSizeValid(u16 sendSize, u16 recvSize, u16 entry_num)
{
    int     usec;

    SDK_ASSERT(entry_num > 0 && entry_num <= 15);

    /* If sendSize is outside the defined boundary, judge it to be an invalid size setting*/
    if (sendSize > MB_COMM_P_SENDLEN_MAX || sendSize < MB_COMM_P_SENDLEN_MIN)
    {
        OS_TWarning("MB Parent send buffer size is out of the range.[%3d - %3d Bytes]\n",
                    MB_COMM_P_SENDLEN_MIN, MB_COMM_P_SENDLEN_MAX);
        return FALSE;
    }

    /* If recvSize is outside the defined boundary, judge it to be an invalid size setting*/
    if (recvSize > MB_COMM_P_RECVLEN_MAX || recvSize < MB_COMM_P_RECVLEN_MIN)
    {
        OS_TWarning
            ("MB Parent receive buffer size per child is out of the range.[%3d - %3d Bytes]\n",
             MB_COMM_P_RECVLEN_MIN, MB_COMM_P_RECVLEN_MAX);
        return FALSE;
    }

    /* Evaluate for the time it takes to perform MP communications once*/
    usec = 330 + 4 * (sendSize + 38) + entry_num * (112 + 4 * (recvSize + 32));

    /* If needed time exceeds 5600us, judge it to be an invalid size setting. */
    if (usec >= MP_USEC_TIME_LIMIT)
    {
        OS_TWarning("These send receive sizes require lower than %4dusec\n"
                    "it exceeds %4d usec\n", MP_USEC_TIME_LIMIT, usec);
        return FALSE;
    }

    return TRUE;
}


BOOL MB_SetParentCommParam(u16 sendSize, u16 maxChildren)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    /* Cannot change if WM has started*/
    if (p_mbi_param->mbIsStarted)
    {
        OS_TWarning("MB has Already started\n");

        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    /* Judge whether the send/receive data sizes are correct*/
    if (FALSE == MBi_IsCommSizeValid(sendSize, MB_COMM_P_RECVLEN_DEFAULT, maxChildren))
    {
        OS_TWarning("MP data sizes have not changed\n");
        // 
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    /* Set the maximum number of people who can connect*/
    p_mbi_param->parentParam.maxEntry = maxChildren;

    /* Set the size of the data being sent/received*/
    p_mbi_param->p_sendlen = sendSize;
    p_mbi_param->p_recvlen = MB_COMM_P_RECVLEN_DEFAULT;

    (void)OS_RestoreInterrupts(enabled);

    return TRUE;
}


BOOL MB_SetParentCommSize(u16 sendSize)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    BOOL    ret;

    /* Maximum number of connected persons inherit the current settings*/
    ret = MB_SetParentCommParam(sendSize, p_mbi_param->parentParam.maxEntry);

    (void)OS_RestoreInterrupts(enabled);

    return ret;
}

u16 MB_GetTgid(void)
{
    return p_mbi_param->parentParam.tgid;
}

/* Initialization (The part common to parent and child)*/
static int MBi_StartCommon(void)
{
    int     errcode;

    p_mbi_param->mpStarted = 0;
    p_mbi_param->child_bitmap = 0;
    p_mbi_param->endReq = 0;
    p_mbi_param->currentTgid = 0;
    MBi_SetMaxScanTime(MB_SCAN_TIME_NORMAL);

    if (!mbc->is_started_ex)
    {
        do
        {
            errcode = WM_Initialize(wmBuf, p_mbi_param->callback_ptr, WM_DMA_NO);
        }
        while (errcode == WM_ERRCODE_WM_DISABLE);
        if (errcode != WM_ERRCODE_OPERATING)
        {
            OS_TWarning("WM_Initialize failed!\n");
            return MB_ERRCODE_WM_FAILURE;
        }
        else
        {
            (void)WM_SetIndCallback(p_mbi_param->callback_ptr);
            p_mbi_param->mbIsStarted = 1;
            return MB_ERRCODE_SUCCESS;
        }
    }
    else
    {
        (void)WM_SetIndCallback(p_mbi_param->callback_ptr);
        p_mbi_param->mbIsStarted = 1;
        MBi_OnInitializeDone();
        return MB_ERRCODE_SUCCESS;
    }
}


/* Shared process for setting parent parameters and starting*/
static int MBi_StartParentCore(int channel)
{
    int     i, ret;
    MBCommPStateCallback cb_tmp;
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    p_mbi_param->parentParam.channel = (u16)channel;
    wmBuf = (u8 *)((((u32)mbc) + sizeof(MB_CommPWork) + 31) & ~31);

    /* Save temporarily for times when callback is already set*/
    cb_tmp = pPwork->parent_callback;

    /* Clear the parent's unique work region*/
    MI_CpuClear16((void *)((u32)pPwork + sizeof(MB_CommCommonWork)),
                  sizeof(MB_CommPWork) - sizeof(MB_CommCommonWork));
    MB_CommSetParentStateCallback(cb_tmp);

    /* Calculate parameters that depend on the size of the send/receive*/
    mbc->block_size_max = MB_COMM_CALC_BLOCK_SIZE(p_mbi_param->p_sendlen);

    MBi_SetChildMPMaxSize(p_mbi_param->p_recvlen);
    MBi_SetParentPieceBuffer(&pPwork->req_data_buf);

    for (i = 0; i < MB_MAX_CHILD; i++)
    {
        pPwork->p_comm_state[i] = MB_COMM_PSTATE_NONE;
        pPwork->fileid_of_child[i] = -1;        /* Initialize the FileID of request from child*/
    }
    pPwork->file_num = 0;

    MI_CpuClear16(&pPwork->fileinfo[0], sizeof(pPwork->fileinfo));
    MI_CpuClear8(&pPwork->req2child[0], sizeof(pPwork->req2child));

    p_mbi_param->mode = MB_MODE_PARENT;
    p_mbi_param->callback = MBi_CommParentCallback;
    p_mbi_param->callback_ptr = MBi_ParentCallback;

    /* Parent max transmission size (Byte)*/
    p_mbi_param->parentParam.parentMaxSize = p_mbi_param->p_sendlen;
    p_mbi_param->sendBufSize = WM_CalcParentSendBufSize(p_mbi_param);
    /* child max transmission size (Byte)*/
    p_mbi_param->parentParam.childMaxSize = p_mbi_param->p_recvlen;
    p_mbi_param->recvBufSize = WM_CalcParentRecvBufSize(p_mbi_param);

    OS_TPrintf("Parent sendSize : %4d\n", p_mbi_param->parentParam.parentMaxSize);
    OS_TPrintf("Parent recvSize : %4d\n", p_mbi_param->parentParam.childMaxSize);
    OS_TPrintf("Parent sendBufSize : %4d\n", p_mbi_param->sendBufSize);
    OS_TPrintf("Parent recvBufSize : %4d\n", p_mbi_param->recvBufSize);

    MB_InitSendGameInfoStatus();

    ret = MBi_StartCommon();

    (void)OS_RestoreInterrupts(enabled);

    pPwork->useWvrFlag = PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM7);

    return ret;
}

/* Set parent parameters & start (Initialized internally in WM)*/
int MB_StartParent(int channel)
{
    mbc->is_started_ex = FALSE;
    return MBi_StartParentCore(channel);
}

/* Set parent parameters & start (Started externally from WM)*/
int MB_StartParentFromIdle(int channel)
{
    mbc->is_started_ex = TRUE;
    return MBi_StartParentCore(channel);
}

/* Set child parameters and start*/
int MB_StartChild(void)
{
    int     ret;
    MBCommCStateCallbackFunc cb_tmp;
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    mbc->is_started_ex = FALSE;

    wmBuf = (u8 *)((((u32)mbc) + sizeof(MB_CommCWork) + 31) & ~31);

    /* Save temporarily for times when callback is already set*/
    cb_tmp = pCwork->child_callback;

    /* Clear the child's unique work region*/
    MI_CpuClear16((void *)((u32)pCwork + sizeof(MB_CommCommonWork)),
                  sizeof(MB_CommCWork) - sizeof(MB_CommCommonWork));
    MB_CommSetChildStateCallback(cb_tmp);

    pCwork->c_comm_state = MB_COMM_CSTATE_NONE;

    p_mbi_param->mode = MB_MODE_CHILD;
    p_mbi_param->callback = MBi_CommChildCallback;
    p_mbi_param->callback_ptr = MBi_ChildCallback;

    p_mbi_param->scanning_flag = FALSE;
    p_mbi_param->scan_channel_flag = TRUE;
    p_mbi_param->last_found_parent_no = -1;

    MBi_SetBeaconRecvStatusBufferDefault();
    MBi_SetScanLockFunc(MBi_ScanLock, MBi_ScanUnlock);
    MB_InitRecvGameInfoStatus();

    ret = MBi_StartCommon();

    (void)OS_RestoreInterrupts(enabled);

    return ret;
}


/* Process for ending wireless communications if the task thread has ended or if it never started.*/
static int MBi_CallReset(void)
{
    int     errcode;
    errcode = WM_Reset(p_mbi_param->callback_ptr);
    MBi_CheckWmErrcode(WM_APIID_RESET, errcode);
    return conv_errcode(errcode);
}

static void MBi_OnReset(MBiTaskInfo * p_task)
{
    (void)p_task;
    (void)MBi_CallReset();
}

/* End communications*/
int MBi_CommEnd(void)
{
    int     ret = WM_ERRCODE_FAILED;
    OSIntrMode enabled = OS_DisableInterrupts();

    if (p_mbi_param->endReq == 0)
    {
        p_mbi_param->scanning_flag = FALSE;
        p_mbi_param->endReq = 1;
        /* If the task thread is running, stop this and then reset*/
        if (MBi_IsTaskAvailable())
        {
            MBi_EndTaskThread(MBi_OnReset);
            ret = WM_ERRCODE_SUCCESS;
        }
        /* If not, reset*/
        else
        {
            ret = MBi_CallReset();
        }
    }

    (void)OS_RestoreInterrupts(enabled);

    return ret;
}

void MB_End(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    if (mbc->is_started_ex)
    {
        OS_TPanic("MB_End called after MB_StartParentFromIdle! (please call MB_EndToIdle)");
    }

    (void)MBi_CommEnd();

    (void)OS_RestoreInterrupts(enabled);
}

void MB_EndToIdle(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    if (!mbc->is_started_ex)
    {
        OS_TPanic("MB_EndToIdle called after MB_StartParent! (please call MB_End)");
    }

    (void)MBi_CommEnd();

    (void)OS_RestoreInterrupts(enabled);
}

//-------------------------------------
// Disconnecting child
//-------------------------------------
void MB_DisconnectChild(u16 aid)
{
    SDK_NULL_ASSERT(pPwork);
    SDK_ASSERT(p_mbi_param->endReq != 1);

    if (WM_Disconnect(MBi_ParentCallback, aid) != WM_ERRCODE_OPERATING)
    {
        OS_TWarning("MB_DisconnectChild failed disconnect child %d\n", aid);
    }

    if (aid == 0 || aid >= 16)
    {
        OS_TWarning("Disconnected Illegal AID No. ---> %2d\n", aid);
        return;
    }

    /*  Delete child information
       (If disconnected, delete child information relating to that AID)*/
    pPwork->childversion[aid - 1] = 0;
    MI_CpuClear8(&pPwork->childggid[aid - 1], sizeof(u32));
    MI_CpuClear8(&pPwork->childUser[aid - 1], sizeof(MBUserInfo));

    /* Clear the Receiving buffer*/
    MBi_ClearParentPieceBuffer(aid);

    pPwork->req2child[aid - 1] = MB_COMM_USER_REQ_NONE;

    /* If the requested FileID has been set. Clear the ID with -1. */
    if (pPwork->fileid_of_child[aid - 1] != -1)
    {
        u8      fileID = (u8)pPwork->fileid_of_child[aid - 1];
        u16     nowChildFlag = pPwork->fileinfo[fileID].gameinfo_child_bmp;
        u16     child = aid;

        pPwork->fileinfo[fileID].gameinfo_child_bmp &= ~(MB_GAMEINFO_CHILD_FLAG(child));
        pPwork->fileinfo[fileID].gameinfo_changed_bmp |= MB_GAMEINFO_CHILD_FLAG(child);
        pPwork->fileid_of_child[child - 1] = -1;
        pPwork->fileinfo[fileID].pollbmp &= ~(0x0001 << child);

        MB_DEBUG_OUTPUT("Update Member (AID:%2d)\n", child);
    }

    /* Clear the connection information*/
    if (pPwork->child_entry_bmp & (0x0001 << aid))
    {
        pPwork->child_num--;
        pPwork->child_entry_bmp &= ~(0x0001 << aid);
    }
    pPwork->p_comm_state[aid - 1] = MB_COMM_PSTATE_NONE;
}


/* Used only with mb_child.c ***********************************************/

int MBi_GetLastFountParent(void)
{
    return p_mbi_param->last_found_parent_no;
}

WMBssDesc *MBi_GetParentBssDesc(int id)
{
    return p_mbi_param->parent_info[id].parentInfo;
}

static void MBi_ScanLock(u8 *macAddr)
{
    scanParam.maxChannelTime = MB_SCAN_TIME_LOCKING;
    p_mbi_param->scan_channel_flag = FALSE;
    WM_CopyBssid(macAddr, scanParam.bssid);
}

static void MBi_ScanUnlock(void)
{
    static const u8 bss_fill[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    scanParam.maxChannelTime = MB_SCAN_TIME_NORMAL;
    p_mbi_param->scan_channel_flag = TRUE;
    WM_CopyBssid(bss_fill, scanParam.bssid);
}

/* Used only with mb_ *************************************************/

// Settings at scan time
void MBi_SetMaxScanTime(u16 time)
{
    scanParam.maxChannelTime = time;
}

static int
MBi_SetMPData(WMCallbackFunc callback, const u16 *sendData, u16 sendDataSize, u16 tmptt,
              u16 pollbmp)
{
#pragma unused( tmptt )
    int     errcode;
    errcode =
        WM_SetMPDataToPort(callback, sendData, sendDataSize, pollbmp, WM_PORT_BT, WM_PRIORITY_LOW);
    MBi_CheckWmErrcode(WM_APIID_SET_MP_DATA, errcode);
    return errcode;
}


int MBi_SendMP(const void *buf, int len, int pollbmp)
{
    int     errcode;

    u16     size = (u16)len;
    u16     pollbitmap = (u16)pollbmp;

    // MP data cannot be sent before start of MP and after request to end MP has been issued.
    if ((p_mbi_param->mpStarted == 0) || (p_mbi_param->endReq == 1))
    {
        return WM_ERRCODE_FAILED;
    }

    switch (p_mbi_param->mode)
    {
    case MB_MODE_PARENT:
        errcode = MBi_SetMPData(p_mbi_param->callback_ptr, (u16 *)buf, size,    // Note: If MBi_ParentCallback is used, even mb_child gets linked to ParentCallback, so substituted with callback_ptr.    
                                (u16)((!p_mbi_param->contSend) ? 1000 : 0), pollbitmap);

        if (errcode == WM_ERRCODE_OPERATING)
        {
            p_mbi_param->mpBusy = 1;
        }

        return conv_errcode(errcode);

    case MB_MODE_CHILD:
        errcode = MBi_SetMPData(MBi_ChildCallback, (u16 *)buf, size, 0, pollbitmap);

        if (errcode == WM_ERRCODE_OPERATING)
        {
            p_mbi_param->mpBusy = 1;
        }

        return conv_errcode(errcode);

    default:
        return WM_ERRCODE_FAILED;
    }
}

int MBi_GetSendBufSize(void)
{
    return (int)p_mbi_param->sendBufSize;
}

int MBi_GetRecvBufSize(void)
{
    return (int)p_mbi_param->recvBufSize;
}

int MBi_CommConnectToParent(const WMBssDesc *bssDescp)
{
    WMgameInfo *gameInfo;
    int     errcode;
    SDK_ASSERT(bssDescp != 0);

    gameInfo = (WMgameInfo *)(&(bssDescp->gameInfo));
    p_mbi_param->p_sendlen = gameInfo->parentMaxSize;
    p_mbi_param->p_recvlen = gameInfo->childMaxSize;
    /* Calculate parameters that depend on the size of the send/receive*/
    mbc->block_size_max = MB_COMM_CALC_BLOCK_SIZE(p_mbi_param->p_sendlen);

    MBi_SetChildMPMaxSize(p_mbi_param->p_recvlen);

    OS_TPrintf("\trecv size : %d\n", p_mbi_param->p_sendlen);
    OS_TPrintf("\tsend size : %d\n", p_mbi_param->p_recvlen);
    OS_TPrintf("\tblock size: %d\n", mbc->block_size_max);

    p_mbi_param->recvBufSize = (u16)WM_CalcChildRecvBufSize(bssDescp);
    p_mbi_param->sendBufSize = (u16)WM_CalcChildSendBufSize(bssDescp);
    p_mbi_param->pInfo = bssDescp;
    p_mbi_param->currentTgid = ((WMGameInfo *)&(bssDescp->gameInfo))->tgid;
    p_mbi_param->scanning_flag = FALSE;

    errcode = WM_EndScan(p_mbi_param->callback_ptr);
    MBi_CheckWmErrcode(WM_APIID_END_SCAN, errcode);
    return conv_errcode(errcode);
}

u32 MBi_GetGgid(void)
{
    return p_mbi_param->parentParam.ggid;
}

u16 MBi_GetTgid(void)
{
    return (p_mbi_param->parentParam.tgid);
}

u8 MBi_GetAttribute(void)
{
    return ((u8)(((p_mbi_param->parentParam.entryFlag) ? WM_ATTR_FLAG_ENTRY : 0) |      // entryFlag lowest bit
                 ((p_mbi_param->parentParam.multiBootFlag) ? WM_ATTR_FLAG_MB : 0) |     // multiBootFlag second bit
                 ((p_mbi_param->parentParam.KS_Flag) ? WM_ATTR_FLAG_KS : 0) |   // KS_Flag third bit
                 ((p_mbi_param->parentParam.CS_Flag) ? WM_ATTR_FLAG_CS : 0)     // CS_Flag fourth bit
            ));
}


// Restart scanning
int MBi_RestartScan(void)
{
    int     errcode;

    p_mbi_param->scanning_flag = TRUE;

    if (p_mbi_param->scan_channel_flag)
    {
        if (FALSE == changeScanChannel(&scanParam))
        {
            (void)MBi_CommEnd();
        }
    }

    errcode = WM_StartScan(MBi_ChildCallback, &scanParam);
    MBi_CheckWmErrcode(WM_APIID_START_SCAN, errcode);
    return conv_errcode(errcode);
}


// Get scan channel
int MBi_GetScanChannel(void)
{
    return scanParam.channel;
}

// Get own AID
u16 MBi_GetAid(void)
{
    return p_mbi_param->my_aid;
}

BOOL MBi_IsStarted(void)
{
    return (p_mbi_param->mbIsStarted == 1) ? TRUE : FALSE;
}

// Check WM_API's returned value
static void MBi_CheckWmErrcode(u16 apiid, int errcode)
{
    u16     arg[2];

    if (errcode != WM_ERRCODE_OPERATING && errcode != WM_ERRCODE_SUCCESS)
    {
        arg[0] = apiid;
        arg[1] = (u16)errcode;
        p_mbi_param->callback(MB_CALLBACK_API_ERROR, arg);
    }
}
