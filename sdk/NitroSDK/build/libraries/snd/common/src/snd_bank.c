/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - libraries
  File:     snd_bank.c

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd_bank.c,v $
  Revision 1.11  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.10  05/10/2005 01:52:16  ida
  Added SND_BANK_DISABLE_RELEASE

  Revision 1.9  05/10/2005 00:55:10  ida
  Note length was changed to unlimited when release 255 occurs.

  Revision 1.8  04/12/2005 06:16:12  ida
  Moved SND to NITRO-SDK

  Revision 1.21  03/17/2005 04:25:36  ida
  Added SND_INST_NULL

  Revision 1.20  02/21/2005 02:37:17  ida
  Warning measures

  Revision 1.19  02/02/2005 05:09:53  ida
  for SDK_FROM_TOOL

  Revision 1.18  01/24/2005 05:52:19  ida
  Added a cache store function after waveform archive editing.

  Revision 1.17  01/19/2005 23:45:33  ida
  Compile error handling.

  Revision 1.16  01/19/2005 06:19:58  ida
  Added functions related to external reference waveform archive implementation

  Revision 1.15  11/22/2004 04:30:17  ida
  workaround thread safe

  Revision 1.14  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.13  07/13/2004 05:42:55  ida
  change API name

  Revision 1.12  07/09/2004 06:50:48  ida
  change SND_NoteOn interface

  Revision 1.11  07/07/2004 05:06:11  ida
  add SND_WriteInstData

  Revision 1.10  07/06/2004 07:46:44  ida
  workaround multi wave archive for one bank

  Revision 1.9  07/05/2004 06:30:05  ida
  add assert

  Revision 1.8  06/23/2004 06:26:17  ida
  channel -> exchannel

  Revision 1.7  06/01/2004 00:19:14  ida
  add warning

  Revision 1.6  05/26/2004 04:18:38  ida
  check multiple assign wave archive

  Revision 1.5  05/13/2004 01:03:01  ida
  fix comments

  Revision 1.4  05/12/2004 06:39:58  ida
  modify data header structure

  Revision 1.3  04/30/2004 00:40:08  ida
  fix infinity loop
  add comments

  Revision 1.2  04/26/2004 02:54:55  ida
  add SND_Disposer*()

  Revision 1.1  03/26/2004 06:25:52  ida
  create snd_drv

  Revision 1.6  03/24/2004 06:21:43  ida
  add envelope command

  Revision 1.5  03/12/2004 02:17:06  ida
  untabify

  Revision 1.4  03/11/2004 01:52:13  ida
  use SND_CopyMemory16

  Revision 1.3  03/10/2004 06:32:08  ida
  SND_Snd -> SND_

  Revision 1.2  03/08/2004 05:40:27  ida
  rename files

  Revision 1.1  03/08/2004 03:02:50  ida
  ARM7/src -> common/src

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/snd/common/bank.h>

#ifndef SDK_FROM_TOOL

#include <nitro/misc.h>
#include <nitro/mi.h>
#include <nitro/snd/common/main.h>

#else

#define SDK_ASSERT(exp)           ((void) 0)
#define SDK_NULL_ASSERT(exp)           ((void) 0)
#define SDK_MINMAX_ASSERT(exp, min, max)           ((void) 0)

#define SNDi_LockMutex() ((void) 0)
#define SNDi_UnlockMutex() ((void) 0)

static u8 MI_ReadByte(const void *address)
{
    return *(u8 *)address;
}

#define HW_MAIN_MEM             0x02000000

#ifdef _MSC_VER
#pragma warning( disable : 4018 )      // warning: signed/unsigned mismatch
#pragma warning( disable : 4311 )      // warning: pointer truncation from 'type' to 'type'
#pragma warning( disable : 4312 )      // warning: conversion from 'type' to 'type' of greater size
#endif

#endif /* SDK_FROM_TOOL*/

/******************************************************************************
	static function declarations
 ******************************************************************************/
static const SNDWaveData *GetWaveData(const SNDBankData *bank, int waveArcNo, int waveIndex);

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         SND_AssignWaveArc

  Description:  Assigns a waveform archive to bank (ThreadSafe)

  Arguments:    bank    - pointer to SNDBankData structure
                index   - wave archive index
                waveArc - pointer to SNDWaveArc structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_AssignWaveArc(SNDBankData *bank, int index, SNDWaveArc *waveArc)
{
    SNDWaveArcLink *next;
    SNDWaveArcLink *prev;

    SDK_NULL_ASSERT(bank);
    SDK_NULL_ASSERT(waveArc);
#ifdef SDK_ARM9
    SDK_ASSERTMSG(((u32)bank & 0x1f) == 0, "bank address must be aligned 32 bytes boundary.");
    SDK_ASSERTMSG(((u32)waveArc & 0x1f) == 0, "waveArc address must be aligned 32 bytes boundary.");
#endif
    SDK_MINMAX_ASSERT(index, 0, SND_BANK_TO_WAVEARC_MAX - 1);

    SNDi_LockMutex();

    if (bank->waveArcLink[index].waveArc != NULL)
    {
        // When already connected.

        // If it's the same as the waveform archive to which it is trying to connect, it does nothing.
        if (waveArc == bank->waveArcLink[index].waveArc)
        {
            SNDi_UnlockMutex();
            return;
        }

        // If a different waveform archive is connected, it disconnects once.
        if (&bank->waveArcLink[index] == bank->waveArcLink[index].waveArc->topLink)
        {
            bank->waveArcLink[index].waveArc->topLink = bank->waveArcLink[index].next;

#ifdef SDK_ARM9
            DC_StoreRange(bank->waveArcLink[index].waveArc, sizeof(SNDWaveArc));
#endif
        }
        else
        {
            prev = bank->waveArcLink[index].waveArc->topLink;
            while (prev != NULL)
            {
                if (&bank->waveArcLink[index] == prev->next)
                    break;
                prev = prev->next;
            }
            SDK_NULL_ASSERT(prev);
            prev->next = bank->waveArcLink[index].next;

#ifdef SDK_ARM9
            DC_StoreRange(prev, sizeof(SNDWaveArcLink));
#endif
        }
    }

    next = waveArc->topLink;
    waveArc->topLink = &bank->waveArcLink[index];
    bank->waveArcLink[index].next = next;
    bank->waveArcLink[index].waveArc = waveArc;

    SNDi_UnlockMutex();

#ifdef SDK_ARM9
    DC_StoreRange(bank, sizeof(SNDBankData));
    DC_StoreRange(waveArc, sizeof(SNDWaveArc));
#endif
}

/*---------------------------------------------------------------------------*
  Name:         SND_DestroyBank

  Description:  Destroys bank (ThreadSafe)

  Arguments:    bank    - pointer to SNDBankData structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_DestroyBank(SNDBankData *bank)
{
    SNDWaveArc *waveArc;
    SNDWaveArcLink *prev;
    int     i;

    SDK_NULL_ASSERT(bank);
#ifdef SDK_ARM9
    SDK_ASSERTMSG(((u32)bank & 0x1f) == 0, "bank address must be aligned 32 bytes boundary.");
#endif

    SNDi_LockMutex();

    for (i = 0; i < SND_BANK_TO_WAVEARC_MAX; i++)
    {
        waveArc = bank->waveArcLink[i].waveArc;
        if (waveArc == NULL)
            continue;

        if (&bank->waveArcLink[i] == waveArc->topLink)
        {
            waveArc->topLink = bank->waveArcLink[i].next;

#ifdef SDK_ARM9
            DC_StoreRange(waveArc, sizeof(SNDWaveArc));
#endif
        }
        else
        {
            prev = waveArc->topLink;
            while (prev != NULL)
            {
                if (&bank->waveArcLink[i] == prev->next)
                    break;
                prev = prev->next;
            }
            SDK_NULL_ASSERT(prev);
            prev->next = bank->waveArcLink[i].next;

#ifdef SDK_ARM9
            DC_StoreRange(prev, sizeof(SNDWaveArcLink));
#endif
        }
    }

    SNDi_UnlockMutex();
}

/*---------------------------------------------------------------------------*
  Name:         SND_DestroyWaveArc

  Description:  Destroys the waveform archive. (ThreadSafe)

  Arguments:    waveArc - pointer to SNDWaveArc structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_DestroyWaveArc(SNDWaveArc *waveArc)
{
    SNDWaveArcLink *link;
    SNDWaveArcLink *next;

    SDK_NULL_ASSERT(waveArc);
#ifdef SDK_ARM9
    SDK_ASSERTMSG(((u32)waveArc & 0x1f) == 0, "waveArc address must be aligned 32 bytes boundary.");
#endif

    SNDi_LockMutex();

    link = waveArc->topLink;
    while (link != NULL)
    {
        next = link->next;

        link->waveArc = NULL;
        link->next = NULL;

#ifdef SDK_ARM9
        DC_StoreRange(link, sizeof(SNDWaveArcLink));
#endif
        link = next;
    }

    SNDi_UnlockMutex();
}


/*---------------------------------------------------------------------------*
  Name:         SND_ReadInstData

  Description:  Gets Inst parameters from bank (ThreadSafe)

  Arguments:    bank    - pointer to SNDBankData structure
                prgNo - program number
                key - note key
                inst - pointer to SNDInstData structure

  Returns:      Whether it was successful or not
 *---------------------------------------------------------------------------*/
BOOL SND_ReadInstData(const SNDBankData *bank, int prgNo, int key, SNDInstData *inst)
{
    u32     instOffset;

    SDK_NULL_ASSERT(bank);
    SDK_NULL_ASSERT(inst);

    if (prgNo < 0)
        return FALSE;

#ifdef SDK_FROM_TOOL
    if (bank->fileHeader.signature[0] == 'S' &&
        bank->fileHeader.signature[1] == 'B' &&
        bank->fileHeader.signature[2] == 'C' && bank->fileHeader.signature[3] == 'B')
    {
        const SNDBankDataCallback *bankcb = (const SNDBankDataCallback *)bank;
        return bankcb->readInstDataFunc(bankcb, prgNo, key, inst);
    }
#endif

    SNDi_LockMutex();

    if (prgNo >= bank->instCount)
    {                                  // Note: read from MainMemory
        SNDi_UnlockMutex();
        return FALSE;
    }

    instOffset = bank->instOffset[prgNo];       // Note: read from MainMemory
    inst->type = (u8)(instOffset & 0xff);
    instOffset >>= 8;

    // Process for each Inst type.
    switch (inst->type)
    {
    case SND_INST_PCM:
    case SND_INST_PSG:
    case SND_INST_NOISE:
    case SND_INST_DIRECTPCM:
    case SND_INST_NULL:
        {
            const SNDInstParam *param = (const SNDInstParam *)((u8 *)bank + instOffset);

            inst->param = *param;      // structure copy
            break;
        }

    case SND_INST_DRUM_SET:
        {
            const SNDDrumSet *drumSet = (const SNDDrumSet *)((u8 *)bank + instOffset);
            u8      min = MI_ReadByte(&drumSet->min);
            u8      max = MI_ReadByte(&drumSet->max);

            if (key < min || key > max)
            {
                SNDi_UnlockMutex();
                return FALSE;
            }

            *inst = drumSet->instOffset[key - min];     // structure copy
            break;
        }

    case SND_INST_KEY_SPLIT:
        {
            int     index = 0;
            const SNDKeySplit *keySplit = (const SNDKeySplit *)((u8 *)bank + instOffset);

            while (key > MI_ReadByte(&keySplit->key[index]))
            {
                index++;
                if (index >= SND_INST_KEYSPLIT_MAX)
                {
                    SNDi_UnlockMutex();
                    return FALSE;
                }
            }

            *inst = keySplit->instOffset[index];        // structure copy
            break;
        }

    case SND_INST_INVALID:
    default:
        SNDi_UnlockMutex();
        return FALSE;
    }

    SNDi_UnlockMutex();

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SND_WriteInstData

  Description:  Writes Inst data (ThreadSafe)

  Arguments:    bank    - pointer to SNDBankData structure
                prgNo - program number
                key - note key
                inst - pointer to SNDInstData structure

  Returns:      Returns whether it was successful.
 *---------------------------------------------------------------------------*/
BOOL SND_WriteInstData(SNDBankData *bank, int prgNo, int key, const struct SNDInstData *inst)
{
    u32     instOffset;
    u8      type;

    SDK_NULL_ASSERT(bank);
    SDK_NULL_ASSERT(inst);

    if (prgNo < 0)
        return FALSE;

    SNDi_LockMutex();
    if (prgNo >= bank->instCount)
    {                                  // Note: read from MainMemory
        SNDi_UnlockMutex();
        return FALSE;
    }

    instOffset = bank->instOffset[prgNo];       // Note: read from MainMemory
    type = (u8)(instOffset & 0xff);
    instOffset >>= 8;

    // Process for each Inst type.
    switch (type)
    {
    case SND_INST_PCM:
    case SND_INST_PSG:
    case SND_INST_NOISE:
    case SND_INST_DIRECTPCM:
    case SND_INST_NULL:
        {
            SNDInstParam *param = (SNDInstParam *)((u8 *)bank + instOffset);

            bank->instOffset[prgNo] = (instOffset << 8) | inst->type;
            *param = inst->param;      // structure copy
            break;
        }

    case SND_INST_DRUM_SET:
        {
            SNDDrumSet *drumSet = (SNDDrumSet *)((u8 *)bank + instOffset);
            u8      min = MI_ReadByte(&drumSet->min);
            u8      max = MI_ReadByte(&drumSet->max);

            if (key < min || key > max)
            {
                SNDi_UnlockMutex();
                return FALSE;
            }

            drumSet->instOffset[key - min] = *inst;     // structure copy
            break;
        }

    case SND_INST_KEY_SPLIT:
        {
            int     index = 0;
            SNDKeySplit *keySplit = (SNDKeySplit *)((u8 *)bank + instOffset);

            while (key > MI_ReadByte(&keySplit->key[index]))
            {
                index++;
                if (index >= SND_INST_KEYSPLIT_MAX)
                {
                    SNDi_UnlockMutex();
                    return FALSE;
                }
            }

            keySplit->instOffset[index] = *inst;        // structure copy
            break;
        }

    case SND_INST_INVALID:
    default:
        SNDi_UnlockMutex();
        return FALSE;
    }

    SNDi_UnlockMutex();

#ifdef SDK_ARM9
    DC_StoreRange(bank, bank->fileHeader.fileSize);
#endif

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetFirstInstDataPos

  Description:  Gets SNDInstPos for Inst data scanning.

  Arguments:    bank - Pointer to bank data

  Returns:      SNDInstPos structure.
 *---------------------------------------------------------------------------*/
SNDInstPos SND_GetFirstInstDataPos(const SNDBankData *bank)
{
    SNDInstPos pos;

    SDK_NULL_ASSERT(bank);

    (void)bank;

    pos.prgNo = 0;
    pos.index = 0;

    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetNextInstData

  Description:  Gets the next Inst data.

  Arguments:    bank - Pointer to bank data
                inst - Pointer that stores the acquired Inst data.
                pos - Pointer to the SNDInstPos structure.

  Returns:      Whether information was obtained or not
 *---------------------------------------------------------------------------*/
BOOL SND_GetNextInstData(const SNDBankData *bank, SNDInstData *inst, SNDInstPos *pos)
{
    SDK_NULL_ASSERT(bank);
    SDK_NULL_ASSERT(inst);
    SDK_NULL_ASSERT(pos);

    for (; pos->prgNo < bank->instCount; pos->prgNo++, pos->index = 0)
    {
        u32     instOffset = bank->instOffset[pos->prgNo];
        inst->type = (u8)(instOffset & 0xff);
        instOffset >>= 8;

        switch (inst->type)
        {
        case SND_INST_PCM:
        case SND_INST_PSG:
        case SND_INST_NOISE:
        case SND_INST_DIRECTPCM:
        case SND_INST_NULL:
            {
                const SNDInstParam *param = (const SNDInstParam *)((u8 *)bank + instOffset);
                inst->param = *param;
                pos->prgNo++;
                return TRUE;
            }

        case SND_INST_DRUM_SET:
            {
                const SNDDrumSet *drumSet = (const SNDDrumSet *)((u8 *)bank + instOffset);
                for (; pos->index < drumSet->max - drumSet->min + 1; pos->index++)
                {
                    *inst = drumSet->instOffset[pos->index];
                    pos->index++;
                    return TRUE;
                }
                break;
            }

        case SND_INST_KEY_SPLIT:
            {
                const SNDKeySplit *keySplit = (const SNDKeySplit *)((u8 *)bank + instOffset);
                for (; pos->index < SND_INST_KEYSPLIT_MAX; pos->index++)
                {
                    if (keySplit->key[pos->index] == 0)
                        break;
                    *inst = keySplit->instOffset[pos->index];
                    pos->index++;
                    return TRUE;
                }
                break;
            }

        default:
            break;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetWaveDataCount

  Description:  Gets the number of waveform data in the waveform archive.

  Arguments:    waveArc - Pointer to the waveform archive.

  Returns:      Number of waveform data.
 *---------------------------------------------------------------------------*/
u32 SND_GetWaveDataCount(const struct SNDWaveArc *waveArc)
{
    SDK_NULL_ASSERT(waveArc);

    return waveArc->waveCount;
}

/*---------------------------------------------------------------------------*
  Name:         SND_SetWaveDataAddress

  Description:  Records waveform data to the waveform archive.

  Arguments:    waveArc - Pointer to the waveform archive.
                index - Index number of waveform data.
                address - Address of waveform data.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_SetWaveDataAddress(SNDWaveArc *waveArc, int index, const SNDWaveData *address)
{
    SDK_NULL_ASSERT(waveArc);
    SDK_MINMAX_ASSERT(index, 0, waveArc->waveCount);
    SDK_ASSERT(address == NULL || address >= (void *)HW_MAIN_MEM);

    SNDi_LockMutex();

    waveArc->waveOffset[index] = (u32)address;

#ifdef SDK_ARM9
    DC_StoreRange(&waveArc->waveOffset[index], sizeof(u32));
#endif

    SNDi_UnlockMutex();
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetWaveDataAddress

  Description:  Gets the waveform data recorded in the waveform archive.

  Arguments:    waveArc - Pointer to the waveform archive.
                index - Index number of waveform data.

  Returns:      Pointer to the acquired waveform data.
 *---------------------------------------------------------------------------*/
const SNDWaveData *SND_GetWaveDataAddress(const SNDWaveArc *waveArc, int index)
{
    const SNDWaveData *wave;
    u32     offset;

    SDK_NULL_ASSERT(waveArc);
    SDK_MINMAX_ASSERT(index, 0, waveArc->waveCount);

    SNDi_LockMutex();

    offset = waveArc->waveOffset[index];

    if (offset != 0)
    {
        if (offset < HW_MAIN_MEM)
            wave = (const SNDWaveData *)((u8 *)waveArc + offset);
        else
            wave = (const SNDWaveData *)offset;
    }
    else
    {
        wave = NULL;
    }

    SNDi_UnlockMutex();

    return wave;
}

//-----------------------------------------------------------------------------
// ARM7 only

#ifdef SDK_ARM7

/*---------------------------------------------------------------------------*
  Name:         SND_NoteOn

  Description:  Performs note-on using the bank.

  Arguments:    

  Returns:      pointer to SNDChannel structure
 *---------------------------------------------------------------------------*/
BOOL SND_NoteOn(SNDExChannel *ch_p,
                int key, int velocity, s32 length, const SNDBankData *bank, const SNDInstData *inst)
{
    const SNDWaveData *wave_data;
    int     release;
    BOOL    result;

    SDK_NULL_ASSERT(ch_p);
    SDK_MINMAX_ASSERT(key, 0, 127);
    SDK_MINMAX_ASSERT(velocity, 0, 127);
    SDK_NULL_ASSERT(inst);

    release = inst->param.release;
    if (inst->param.release == SND_BANK_DISABLE_RELEASE)
    {
        length = -1;
        release = 0;
    }

    switch (inst->type)
    {
    case SND_INST_PCM:
    case SND_INST_DIRECTPCM:
        if (inst->type == SND_INST_PCM)
        {
            wave_data = GetWaveData(bank, inst->param.wave[1], inst->param.wave[0]);
        }
        else
        {
            wave_data = (const SNDWaveData *)(inst->param.wave[1] << 16 | inst->param.wave[0]);
        }

        if (wave_data != NULL)
        {
            result = SND_StartExChannelPcm(ch_p, &wave_data->param, wave_data->samples, length);
        }
        else
        {
            result = FALSE;
        }
        break;

    case SND_INST_PSG:
        result = SND_StartExChannelPsg(ch_p, (SNDDuty)inst->param.wave[0], length);
        break;

    case SND_INST_NOISE:
        result = SND_StartExChannelNoise(ch_p, length);
        break;

    default:
        result = FALSE;
        break;
    }

    if (!result)
        return FALSE;

    ch_p->key = (u8)key;
    ch_p->original_key = inst->param.original_key;
    ch_p->velocity = (u8)velocity;

    SND_SetExChannelAttack(ch_p, inst->param.attack);
    SND_SetExChannelDecay(ch_p, inst->param.decay);
    SND_SetExChannelSustain(ch_p, inst->param.sustain);
    SND_SetExChannelRelease(ch_p, release);

    ch_p->init_pan = (s8)(inst->param.pan - 64);

    return TRUE;
}

#endif

/******************************************************************************
	static functions
 ******************************************************************************/

//-----------------------------------------------------------------------------
// ARM7 only

#ifdef SDK_ARM7

/*---------------------------------------------------------------------------*
  Name:         GetWaveData

  Description:  Gets waveform data from the waveform archive.

  Arguments:    bank - pointer to SNDBankData
                waveArcNo - wave archive number
                waveIndex - wave index

  Returns:      pointer to SNDWaveData structure
 *---------------------------------------------------------------------------*/
static const SNDWaveData *GetWaveData(const SNDBankData *bank, int waveArcNo, int waveIndex)
{
    const SNDWaveArc *arc;

    SDK_MINMAX_ASSERT(waveArcNo, 0, SND_BANK_TO_WAVEARC_MAX - 1);

    arc = bank->waveArcLink[waveArcNo].waveArc;

    if (arc == NULL)
        return NULL;
    if (waveIndex >= arc->waveCount)
        return NULL;

    return SND_GetWaveDataAddress(arc, waveIndex);
}

#endif

/*====== End of snd_bank.c ======*/
