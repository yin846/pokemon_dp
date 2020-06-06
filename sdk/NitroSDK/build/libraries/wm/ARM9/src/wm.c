/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm.c,v $
  Revision 1.92  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.91  2004/09/13 00:20:35  terui Improvement:
  Left only the change log and deleted code in accordance with split completion to separate file group.

  Revision 1.90  2004/09/10 12:05:40  seiki_masashi
  Changed it so the number of bytes sent for key sharing is concealed in the library

  Revision 1.89  2004/09/10 04:55:36  seiki_masashi
  Changed WM_GetReceiveBufferSize to the right value.

  Revision 1.88  2004/09/09 05:54:00  seiki_masashi
  Changed so that argument can be passed to callback of SetMPData callback.

  Revision 1.87  2004/09/09 01:56:11  terui Improvement:
  Moved some functions to separate file in accordance with split launch of wm.c file.
  Added several shared functions that reference from separate file.
  Renamed static variable and static function based on naming conventions.
  Added WM_Init and WM_Finish functions.

  Revision 1.86  2004/09/03 07:15:24  terui Improvement:
  Added CheckParentParameter function for use in debugging.

  Revision 1.85  2004/09/03 05:05:30  seiki_masashi
  Changed SetMPDataToPort's NO_CHILD judgment condition to child_bitmap==0.

  Revision 1.84  2004/09/03 04:46:49  seiki_masashi
  Changed so as to use WMStatus.mp_readyBitmap.

  Revision 1.83  2004/09/02 09:27:00  seiki_masashi
  fix typo.

  Revision 1.82  2004/09/02 09:24:42  seiki_masashi
  Added (again) cache processing in accordance with speed increase of port receive callback by MPEND_IND.

  Revision 1.81  2004/09/02 09:15:13  terui Improvement:
  Added WMi_GetStatusAddress function.

  Revision 1.78  2004/08/30 01:59:46  seiki_masashi
  Consolidated WM_STATECODE_CHILD_CONNECTED in WM_STATECODE_CONNECTED.

  Revision 1.77  2004/08/30 01:58:23  ooe
  Small fix

  Revision 1.76  2004/08/30 00:58:51  terui Improvement:
  Fixed so that link strength is consolidated on ARM7 side.

  Revision 1.75  2004/08/27 08:14:14  seiki_masashi
  Small fix.

  Revision 1.74  2004/08/27 06:43:50  seiki_masashi
  Added defaultRetryCount and fixFreqMode to WM_StartMPEx arguments.

  Revision 1.73  2004/08/27 04:42:20  terui Improvement:
  Revised WM_Disconnect parameter checking.

  Revision 1.72  2004/08/27 04:25:37  terui Improvement:
  Revised parameter checking for WM_Disconnect in the case of a child machine.

  Revision 1.71  2004/08/27 02:16:27  terui Improvement:
  Only fix comment.

  Revision 1.70  2004/08/25 08:17:32  seiki_masashi
  Added WM_StatMPEx.

  Revision 1.69  2004/08/25 05:52:44  terui Improvement:
  Rename parentRssi to lastRssi.

  Revision 1.68  2004/08/24 13:21:32  terui Improvement:
  Added function to get link level.

  Revision 1.67  2004/08/23 04:42:45  ooe
  Support for the changes to the specification of WM_StartConnect

  Revision 1.66  2004/08/21 06:56:50  ooe
  Added use of WMStartConnectReq type.

  Revision 1.65  2004/08/20 11:55:28  terui Improvement:
  Childs -> Children

  Revision 1.64  2004/08/20 06:48:17  terui Improvement:
  Added WM_DisconnectChilds.

  Revision 1.63  2004/08/19 15:25:07  miya
  modified SetGameInfo function.

  Revision 1.62  2004/08/19 05:55:21  miya
  WM_SetGameInfo

  Revision 1.61  2004/08/19 02:53:48  terui Improvement:
  Added WM_SetEntry function.

  Revision 1.60  2004/08/18 12:39:49  yosiokat
  WM_SetGameInfo's gameInfo size checking changed from 64 to WM_SIZE_USER_GAMEINFO

  Revision 1.59  2004/08/18 06:45:09  terui Improvement:
  Added WM_GetAllowedChannel function.

  Revision 1.58  2004/08/18 00:22:30  seiki_masashi
  Small change to WM_StartMP argument.

  Revision 1.57  2004/08/17 12:43:13  seiki_masashi
  Changed to specify send buffer in WM_StartMP argument.
  Revised in accordance with the removal of WM_ERRCODE_FRAME_GAP.

  Revision 1.56  2004/08/16 06:00:21  seiki_masashi
  Changes in support for dynamic connection/disconnection of DataSharing

  Revision 1.55  2004/08/12 14:22:37  seiki_masashi
  Support for DataSharing sync offset

  Revision 1.54  2004/08/11 10:26:55  seiki_masashi
  Added writing of reception buffer cache before passing to ARM7

  Revision 1.53  2004/08/11 10:03:03  seiki_masashi
  Corrected processing at start of DataSharing

  Revision 1.52  2004/08/11 07:26:00  seiki_masashi
  Corrected processing at start of DataSharing

  Revision 1.51  2004/08/11 04:26:13  seiki_masashi
  Added doubleMode to StartDataSharing arguments.
  Stabilized processing at start of DataSharing

  Revision 1.50  2004/08/10 14:16:52  seiki_masashi
  Temporarily stopped buffering of up to two data sets from child machine.

  Revision 1.49  2004/08/10 12:08:12  seiki_masashi
  Separated WmDataSharingSendDataSet from WmDataSharingReceiveData
  Added buffering of up to two data sets from child machine

  Revision 1.48  2004/08/10 10:12:33  terui Improvement:
  Only added comments and touched up source code.

  Revision 1.47  2004/08/10 04:14:35  seiki_masashi
  Port reception notify callback now indicates CONNECT, DISCONNECT
  Now continues operation dynamically even when child machine is dropped during DataSharing.

  Revision 1.46  2004/08/10 01:40:34  seiki_masashi
  Now operates normally if StepDataSharing is called when there is no child

  Revision 1.45  2004/08/09 10:29:33  seiki_masashi
  During DataSharing, machine no longer accepts DataSet if aidBitmap is not addressed to it.

  Revision 1.44  2004/08/07 13:39:39  terui Improvement:
  DmaCopy32->CpuCopyFast in WM_ReadStatus.

  Revision 1.43  2004/08/07 13:37:25  terui Improvement:
  Undefine WM_USE_TEMP_STATUS_BUF switch

  Revision 1.42  2004/08/07 10:23:19  seiki_masashi
  MPEND.ind, MP.ind now automatically invalidate the reception buffer cache
  Save seq number of DataSet to WMDataSharingInfo.

  Revision 1.41  2004/08/07 08:43:16  seiki_masashi
  32-byte aligned DataSharing's send data.

  Revision 1.40  2004/08/07 07:04:25  seiki_masashi
  Changed WM_StepDataSharing argument.

  Revision 1.39  2004/08/07 06:51:18  seiki_masashi
  Performs cache processing of reception data from 7

  Revision 1.38  2004/08/07 05:16:20  seiki_masashi
  Small fix

  Revision 1.37  2004/08/07 04:09:02  seiki_masashi
  Changed to send receivedBitmap, aidBitmap during DataSharing
  Corrected failure to initialize transmit buffer for DataSharing

  Revision 1.36  2004/08/07 02:58:28  terui Improvement:
  Moved callback exclusion control flag to shared region.
  Deleted WM_USE_BUF_REMAIN switch

  Revision 1.35  2004/08/07 00:19:40  seiki_masashi
  Implemented dataSharing

  Revision 1.34  2004/08/05 10:01:59  seiki_masashi
  Changed so that SetMPDataToPort() does not receive tmptt.

  Revision 1.33  2004/08/05 08:25:54  seiki_masashi
  Changes due to implementation of sendQueue.

  Revision 1.32  2004/08/04 07:12:39  seiki_masashi
  Added WMi_DebugPrintAllSendQueue

  Revision 1.31  2004/08/04 06:20:50  ooe
  Added InitWirelessCounter and GetWirelessCounter.

  Revision 1.30  2004/08/04 05:16:29  seiki_masashi
  Small fix

  Revision 1.29  2004/08/04 01:45:09  seiki_masashi
  Prearrangements for port introduction
  Added WM_SetMPDataToPort, WM_SetPortCallback

  Revision 1.28  2004/08/03 12:25:08  ooe
  Support for WL TestMode command changes.

  Revision 1.27  2004/08/02 06:29:47  terui Improvement:
  Standardized: WMbssDesc -> WMBssDesc
  Standardize: WMstatus -> WMStatus

  Revision 1.26  2004/08/02 02:52:51  terui Improvement:
  Standardized structure names based on naming conventions.

  Revision 1.25  2004/07/30 05:20:17  ooe
  Can now issue MeasureChannel command

  Revision 1.24  2004/07/29 12:09:59  ooe
  Added WM_MeasureChannel.

  Revision 1.23  2004/07/29 07:37:59  miya
  add error message

  Revision 1.22  2004/07/29 06:10:44  miya
  bug fix

  Revision 1.21  2004/07/29 05:13:46  miya
  added comment

  Revision 1.20  2004/07/29 04:20:50  miya
  add SetLifeTime function

  Revision 1.19  2004/07/28 04:42:27  miya
  added comment

  Revision 1.18  2004/07/28 04:21:18  miya
  add functions about ggid & tgid

  Revision 1.17  2004/07/27 00:06:26  terui Improvement:
  Stronger state checking for API calls

  Revision 1.16  2004/07/23 14:43:48  terui Improvement:
  Changed DMA copy size of WM_ReadStatus function

  Revision 1.15  2004/07/20 01:52:52  terui Improvement:
  Small fix

  Revision 1.14  2004/07/20 01:16:20  terui Improvement:
  Added various comments describing functions.
  Unified src pointer argument as const.
  Improved cache operations for section that performs data exchange with ARM7 via main memory.
  Unneeded functions deleted.
  WM_SetGameInfo specs changed to allow update only of user GameInfo
  Miscellaneous revisions.

  Revision 1.13  2004/07/16 11:26:09  terui Improvement:
  Add CheckStateEx.
  Update comments.

  Revision 1.12  2004/07/15 00:32:57  yasu
  fix a small bug

  Revision 1.11  2004/07/14 09:41:24  ooe
  Changed arguments of StartScan

  Revision 1.10  2004/07/14 05:47:23  seiki_masashi
  fix a bug.

  Revision 1.9  2004/07/13 08:34:24  yada
  sorry, fix a little

  Revision 1.8  2004/07/13 08:32:06  yada
  let WM_sp_init() return WM_ERRCODE_WM_DISABLE when WMsp is not ready

  Revision 1.7  2004/07/12 10:37:26  terui Improvement:
  Changed state checking at start of Scan

  Revision 1.6  2004/07/12 01:31:25  ooe
  Small fix

  Revision 1.5  2004/07/11 03:03:33  ooe
  Additions and other changes related WM_SetBeaconIndication  

  Revision 1.4  2004/07/08 10:07:50  terui Improvement:
  Changes reflecting start of WM7 overhaul

  Revision 1.15  2004/07/06 08:37:46  Ooe
  Deleted unused variables

  Revision 1.14  2004/07/06 01:53:33  Ooe
  Revised fifoFlag initialization

  Revision 1.13  2004/07/05 13:01:22  Ooe
  Revised cache control

  Revision 1.12  2004/07/02 04:53:01  Miya
  Revised cache control

  Revision 1.11  2004/06/30 03:08:53  ikedae
  Revised fifoBuf alignment

  Revision 1.10  2004/06/25 09:18:27  Ooe
  Support for Marionea-1.52.00

  Revision 1.9  2004/06/25 06:18:06  Ooe
  Added WM_SetMPData() argument (pollbmp)

  Revision 1.8  2004/06/24 11:22:03  Ooe
  Added WM_SetGameInfo() error handling

  Revision 1.7  2004/06/24 10:04:00  Ooe
  Added WM_SetGameInfo()

  Revision 1.6  2004/06/22 09:28:06  Ikedae
  (none)

  Revision 1.5  2004/06/22 05:19:16  Ikedae
  Changes related to WM_lib_Main deletion

  Revision 1.4  2004/06/19 05:56:29  Miya
  modified header file reference

  Revision 1.3  2004/06/19 04:05:10  miya
  ver. up Marionea-1.48.00

  Revision 1.2  2004/06/02 02:29:40  miya
  ver. up

  Revision 1.1  2004/05/26 08:55:07  terui Improvement:
  Additions reflecting embedding of wireless functionality

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/

