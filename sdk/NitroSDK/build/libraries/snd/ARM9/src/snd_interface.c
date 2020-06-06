/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - libraries
  File:     snd_interface.c

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd_interface.c,v $
  Revision 1.4  03/20/2006 01:57:52  ida
  added the SND_SetMuteTrackEx function

  Revision 1.3  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.2  04/14/2005 06:52:44  ida
  Added SND_PrepareSeq function

  Revision 1.1  04/12/2005 06:16:12  ida
  Moved SND to NITRO-SDK

  Revision 1.19  03/17/2005 02:11:24  ida
  added the SNDi_SkipSeq function

  Revision 1.18  03/01/2005 02:20:53  ida
  Added the SND_SetTrackPanRange function

  Revision 1.17  02/16/2005 07:47:40  ida
  Added functions to obtain driver information

  Revision 1.16  01/06/2005 02:52:13  ida
  Updated copyright year notation

  Revision 1.15  01/05/2005 06:48:57  ida
  Added SND_SetTrackAllocatableChannel function

  Revision 1.14  10/21/2004 02:14:28  ida
  fix argument name

  Revision 1.13  10/05/2004 06:21:16  ida
  add SND_SetMasterPan

  Revision 1.12  08/19/2004 06:49:13  ida
  work around alarm

  Revision 1.11  08/10/2004 02:29:41  ida
  add SND_StopUnlockedChannel

  Revision 1.10  08/05/2004 03:42:23  ida
  for ensata

  Revision 1.9  08/02/2004 05:30:46  ida
  workaround sound alarm

  Revision 1.8  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.7  07/13/2004 05:44:47  ida
  SND_SetTrackAllocatableChannel become private

  Revision 1.6  07/09/2004 06:50:31  ida
  add SND_SetTrackAllocatableChannel

  Revision 1.5  07/05/2004 06:29:38  ida
  SNDCaptureId -> SNDCapture

  Revision 1.4  06/28/2004 07:14:34  ida
  change function interface

  Revision 1.3  06/23/2004 06:25:51  ida
  workaround shared work

  Revision 1.2  06/21/2004 01:15:51  ida
  change function interface

  Revision 1.1  06/11/2004 01:53:05  ida
  snd_command.c -> snd_interface.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/snd/ARM9/interface.h>

#include <nitro/misc.h>
#include <nitro/snd/common/command.h>
#include <nitro/snd/common/seq.h>
#include <nitro/snd/common/alarm.h>

/******************************************************************************
	macro definition
 ******************************************************************************/
#define PushCommand( c, a0, a1, a2, a3 ) PushCommand_impl( (c), (u32)(a0), (u32)(a1), (u32)(a2), (u32)(a3) )

#define GetOffset( type, member ) ( (u32)( &((type*)0)->member ) )
#define GetSize( type, member ) ( sizeof( ((type*)0)->member ) )

/******************************************************************************
	static function declaration
 ******************************************************************************/
static void PushCommand_impl(SNDCommandID command, u32 arg0, u32 arg1, u32 arg2, u32 arg3);

/******************************************************************************
	public function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         SND_StartSeq

  Description:  Starts the sequence.

  Arguments:    playerNo - Player number
                seqBase  - Sequence data base address.
                seqOffset - Sequence data offset.
                bank_p - Bank pointer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_StartSeq(int playerNo, const void *base, u32 offset, const struct SNDBankData *bank)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_NULL_ASSERT(base);
    SDK_NULL_ASSERT(bank);

    PushCommand(SND_COMMAND_START_SEQ, playerNo, base, offset, bank);
}

/*---------------------------------------------------------------------------*
  Name:         SND_StopSeq

  Description:  Stops the sequence.

  Arguments:    playerNo - Player number

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_StopSeq(int playerNo)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);

    PushCommand(SND_COMMAND_STOP_SEQ, playerNo, 0, 0, 0);
}

/*---------------------------------------------------------------------------*
  Name:         SND_PrepareSeq

  Description:  Prepares for sequence playback.

  Arguments:    playerNo - Player number
                seqBase  - Sequence data base address.
                seqOffset - Sequence data offset.
                bank_p - Bank pointer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_PrepareSeq(int playerNo, const void *base, u32 offset, const struct SNDBankData *bank)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_NULL_ASSERT(base);
    SDK_NULL_ASSERT(bank);

    PushCommand(SND_COMMAND_PREPARE_SEQ, playerNo, base, offset, bank);
}

/*---------------------------------------------------------------------------*
  Name:         SND_StartPreparedSeq

  Description:  Begins playback of the prepared sequence.

  Arguments:    playerNo - Player number

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_StartPreparedSeq(int playerNo)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);

    PushCommand(SND_COMMAND_START_PREPARED_SEQ, playerNo, 0, 0, 0);
}

/*---------------------------------------------------------------------------*
  Name:         SND_PauseSeq

  Description:  Pauses or restarts the sequence.

  Arguments:    playerNo - Player number
                flag - TRUE to pauseÅAFALSE to restart

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_PauseSeq(int playerNo, BOOL flag)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);

    PushCommand(SND_COMMAND_PAUSE_SEQ, playerNo, flag, 0, 0);
}

void SND_SetPlayerTempoRatio(int playerNo, int ratio)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MIN_ASSERT(ratio, 1);

    SNDi_SetPlayerParam(playerNo,
                        GetOffset(SNDPlayer, tempo_ratio),
                        (u32)ratio, GetSize(SNDPlayer, tempo_ratio));
}

void SND_SetPlayerVolume(int playerNo, int volume)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(volume, -32768, 0);

    SNDi_SetPlayerParam(playerNo,
                        GetOffset(SNDPlayer, extFader), (u32)volume, GetSize(SNDPlayer, extFader));
}

void SND_SetPlayerChannelPriority(int playerNo, int prio)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(prio, 0, 127);

    SNDi_SetPlayerParam(playerNo, GetOffset(SNDPlayer, prio), (u32)prio, GetSize(SNDPlayer, prio));
}

void SND_SetPlayerLocalVariable(int playerNo, int varNo, s16 var)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(varNo, 0, SND_PLAYER_VARIABLE_NUM - 1);

    PushCommand(SND_COMMAND_PLAYER_LOCAL_VAR, playerNo, varNo, var, 0);
}

void SND_SetPlayerGlobalVariable(int varNo, s16 var)
{
    SDK_MINMAX_ASSERT(varNo, 0, SND_GLOBAL_VARIABLE_NUM - 1);

    PushCommand(SND_COMMAND_PLAYER_GLOBAL_VAR, varNo, var, 0, 0);
}

void SND_SetTrackVolume(int playerNo, u32 trackBitMask, int volume)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(volume, -32768, 0);

    SNDi_SetTrackParam(playerNo, trackBitMask,
                       GetOffset(SNDTrack, extFader), (u32)volume, GetSize(SNDTrack, extFader));
}

void SND_SetTrackPitch(int playerNo, u32 trackBitMask, int pitch)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(pitch, -32768, 32767);

    SNDi_SetTrackParam(playerNo, trackBitMask,
                       GetOffset(SNDTrack, ext_pitch), (u32)pitch, GetSize(SNDTrack, ext_pitch));
}

void SND_SetTrackPan(int playerNo, u32 trackBitMask, int pan)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(pan, -128, 127);

    SNDi_SetTrackParam(playerNo, trackBitMask,
                       GetOffset(SNDTrack, ext_pan), (u32)pan, GetSize(SNDTrack, ext_pan));
}

void SND_SetTrackPanRange(int playerNo, u32 trackBitMask, int panRange)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(panRange, 0, 127);

    SNDi_SetTrackParam(playerNo, trackBitMask,
                       GetOffset(SNDTrack, pan_range), (u32)panRange, GetSize(SNDTrack, pan_range));
}

void SND_SetTrackModDepth(int playerNo, u32 trackBitMask, int depth)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(depth, 0, 255);

    SNDi_SetTrackParam(playerNo, trackBitMask,
                       GetOffset(SNDTrack, mod.depth), (u32)depth, GetSize(SNDTrack, mod.depth));
}

void SND_SetTrackModSpeed(int playerNo, u32 trackBitMask, int speed)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(speed, 0, 255);

    SNDi_SetTrackParam(playerNo, trackBitMask,
                       GetOffset(SNDTrack, mod.speed), (u32)speed, GetSize(SNDTrack, mod.speed));
}

void SND_SetTrackAllocatableChannel(int playerNo, u32 trackBitMask, u32 chBitMask)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);
    SDK_MINMAX_ASSERT(chBitMask, 0, 0xffff);

    PushCommand(SND_COMMAND_ALLOCATABLE_CHANNEL, playerNo, trackBitMask, chBitMask, 0);
}

void SND_StartTimer(u32 chBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags)
{
    PushCommand(SND_COMMAND_START_TIMER, chBitMask, capBitMask, alarmBitMask, flags);
}

void SND_StopTimer(u32 chBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags)
{
    int     alarmNo;
    u32     mask = alarmBitMask;

    for (alarmNo = 0; alarmNo < SND_ALARM_NUM && mask != 0; alarmNo++, mask >>= 1)
    {
        if ((mask & 0x01) == 0)
            continue;
        SNDi_IncAlarmId(alarmNo);
    }

    PushCommand(SND_COMMAND_STOP_TIMER, chBitMask, capBitMask, alarmBitMask, flags);
}

void SND_SetupCapture(SNDCapture capture,
                      SNDCaptureFormat format,
                      void *buffer_p, u32 length, BOOL loopFlag, SNDCaptureIn in, SNDCaptureOut out)
{
    PushCommand(SND_COMMAND_SETUP_CAPTURE,
                buffer_p,
                length, capture << 31 | format << 30 | loopFlag << 29 | in << 28 | out << 27, 0);
}

void SND_SetupAlarm(int alarmNo, u32 tick, u32 period, SNDAlarmHandler handler, void *arg)
{
    u8      id = SNDi_SetAlarmHandler(alarmNo,
                                      handler,
                                      arg);

    PushCommand(SND_COMMAND_SETUP_ALARM, alarmNo, tick, period, id);
}

void SND_SetTrackMute(int playerNo, u32 trackBitMask, BOOL flag)
{
    PushCommand(SND_COMMAND_MUTE_TRACK, playerNo, trackBitMask, flag ? SND_SEQ_MUTE_STOP : SND_SEQ_MUTE_OFF, 0);
}

void SND_SetTrackMuteEx(int playerNo, u32 trackBitMask, SNDSeqMute mute)
{
    PushCommand(SND_COMMAND_MUTE_TRACK, playerNo, trackBitMask, mute, 0);
}

void SND_StopUnlockedChannel(u32 chBitMask, u32 flags)
{
    PushCommand(SND_COMMAND_STOP_UNLOCKED_CHANNEL, chBitMask, flags, 0, 0);
}

void SND_LockChannel(u32 chBitMask, u32 flags)
{
    PushCommand(SND_COMMAND_LOCK_CHANNEL, chBitMask, flags, 0, 0);
}

void SND_UnlockChannel(u32 chBitMask, u32 flags)
{
    PushCommand(SND_COMMAND_UNLOCK_CHANNEL, chBitMask, flags, 0, 0);
}

void SND_SetChannelTimer(u32 chBitMask, int timer)
{
    PushCommand(SND_COMMAND_CHANNEL_TIMER, chBitMask, timer, 0, 0);
}

void SND_SetChannelVolume(u32 chBitMask, int volume, SNDChannelDataShift shift)
{
    PushCommand(SND_COMMAND_CHANNEL_VOLUME, chBitMask, volume, shift, 0);
}

void SND_SetChannelPan(u32 chBitMask, int pan)
{
    PushCommand(SND_COMMAND_CHANNEL_PAN, chBitMask, pan, 0, 0);
}

void SND_SetupChannelPcm(int chNo,
                         SNDWaveFormat format,
                         const void *dataaddr,
                         SNDChannelLoop loop,
                         int loopStart,
                         int loopLen, int volume, SNDChannelDataShift shift, int timer, int pan)
{
    PushCommand(SND_COMMAND_SETUP_CHANNEL_PCM,
                timer << 16 | chNo,
                dataaddr,
                volume << 24 | shift << 22 | loopLen,
                loop << 26 | format << 24 | pan << 16 | loopStart);
}

void SND_SetupChannelPsg(int chNo,
                         SNDDuty duty, int volume, SNDChannelDataShift shift, int timer, int pan)
{
    PushCommand(SND_COMMAND_SETUP_CHANNEL_PSG, chNo, shift << 8 | volume, timer << 8 | pan, duty);
}

void SND_SetupChannelNoise(int chNo, int volume, SNDChannelDataShift shift, int timer, int pan)
{
    PushCommand(SND_COMMAND_SETUP_CHANNEL_NOISE, chNo, shift << 8 | volume, timer << 8 | pan, 0);
}

void SND_InvalidateSeqData(const void *start, const void *end)
{
    PushCommand(SND_COMMAND_INVALIDATE_SEQ, start, end, 0, 0);
}

void SND_InvalidateBankData(const void *start, const void *end)
{
    PushCommand(SND_COMMAND_INVALIDATE_BANK, start, end, 0, 0);
}

void SND_InvalidateWaveData(const void *start, const void *end)
{
    PushCommand(SND_COMMAND_INVALIDATE_WAVE, start, end, 0, 0);
}

void SND_SetMasterVolume(int volume)
{
    PushCommand(SND_COMMAND_MASTER_VOLUME, volume, 0, 0, 0);
}

void SND_SetOutputSelector(SNDOutput left,
                           SNDOutput right, SNDChannelOut channel1, SNDChannelOut channel3)
{
    PushCommand(SND_COMMAND_OUTPUT_SELECTOR, left, right, channel1, channel3);
}

void SND_SetMasterPan(int pan)
{
    SDK_MINMAX_ASSERT(pan, SND_CHANNEL_PAN_MIN, SND_CHANNEL_PAN_MAX);

    PushCommand(SND_COMMAND_MASTER_PAN, pan, 0, 0, 0);
}

void SND_ResetMasterPan(void)
{
    PushCommand(SND_COMMAND_MASTER_PAN, -1, 0, 0, 0);
}

void SND_ReadDriverInfo(struct SNDDriverInfo *info)
{
    PushCommand(SND_COMMAND_READ_DRIVER_INFO, info, 0, 0, 0);
}

/******************************************************************************
	private function
 ******************************************************************************/

void SNDi_SetPlayerParam(int playerNo, u32 offset, u32 data, int size)
{
    PushCommand(SND_COMMAND_PLAYER_PARAM, playerNo, offset, data, size);
}

void SNDi_SetTrackParam(int playerNo, u32 trackBitMask, u32 offset, u32 data, int size)
{
    PushCommand(SND_COMMAND_TRACK_PARAM, size << 24 | playerNo, trackBitMask, offset, data);
}

void SNDi_SetSurroundDecay(int decay)
{
    PushCommand(SND_COMMAND_SURROUND_DECAY, decay, 0, 0, 0);
}

void SNDi_SkipSeq(int playerNo, u32 tick)
{
    SDK_MINMAX_ASSERT(playerNo, SND_PLAYER_MIN, SND_PLAYER_MAX);

    PushCommand(SND_COMMAND_SKIP_SEQ, playerNo, tick, 0, 0);
}

/******************************************************************************
	static function
 ******************************************************************************/

static void PushCommand_impl(SNDCommandID id, u32 arg0, u32 arg1, u32 arg2, u32 arg3)
{
    SNDCommand *command = SND_AllocCommand(SND_COMMAND_BLOCK);
    if (command == NULL)
        return;

    command->id = id;
    command->arg[0] = arg0;
    command->arg[1] = arg1;
    command->arg[2] = arg2;
    command->arg[3] = arg3;

    SND_PushCommand(command);
}

/*====== End of snd_interface.c ======*/
