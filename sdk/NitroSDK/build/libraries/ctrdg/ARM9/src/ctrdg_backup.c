/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_backup.c

  Copyright 2003,2004,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_backup.c,v $
  Revision 1.2  04/07/2006 03:27:28  okubata_ryoma
  small fix

  Revision 1.1  04/05/2006 10:48:30  okubata_ryoma
  changes made for AGB backup library's SDK collection


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

// extern data declaration----------------------
extern const CTRDGiFlashTypePlus defaultFlash1M;
extern const CTRDGiFlashTypePlus MX29L010;
extern const CTRDGiFlashTypePlus LE26FV10N1TS_10;

extern const CTRDGiFlashTypePlus defaultFlash512;
extern const CTRDGiFlashTypePlus LE39FW512;
extern const CTRDGiFlashTypePlus AT29LV512_lib;
extern const CTRDGiFlashTypePlus MN63F805MNP;

/*exception controls*/
extern u16 ctrdgi_flash_lock_id;
extern u16 ctrdgi_sram_lock_id;

// const data-----------------------------------
static const u8 AgbLib1MFlash_ver[]="AGBFLASH1M_V102";
static const u8 AgbLibFlash_ver[]="AGBFLASH512_V131";

static const CTRDGiFlashTypePlus *const flash1M_list[]={
    &MX29L010,
    &LE26FV10N1TS_10,
    &defaultFlash1M,
};

static const CTRDGiFlashTypePlus *const flash512_list[]={
    &LE39FW512,
    &AT29LV512_lib, //Atmel
    &MN63F805MNP,
    &defaultFlash512,
};

static const u16 readidtime[] = {
    20, //20ms
};

// global variables-----------------------------


/*******************************************************

    function's description

********************************************************/

// flash Identify functions---------------------

u16 CTRDG_IdentifyAgbBackup(CTRDGBackupType type)
{
    u16 result = 1;
    u16 flashID;
    const CTRDGiFlashTypePlus *const *flp;
    MICartridgeRamCycle ram_cycle;

    if(type == CTRDG_BACKUP_TYPE_FLASH_512K || type == CTRDG_BACKUP_TYPE_FLASH_1M)
    {
        /*exception controls*/
        ctrdgi_flash_lock_id = (u16)OS_GetLockID();
        
        /*exception controls (lock)*/
        (void)OS_LockCartridge(ctrdgi_flash_lock_id);
        
        /*access cycle settings*/
        ram_cycle = MI_GetCartridgeRamCycle();
        MI_SetCartridgeRamCycle(MI_CTRDG_RAMCYCLE_18);

        ctrdgi_fl_maxtime=readidtime;  //unless tentative settings are made, the timer within ReadflashID won't work
        flashID=CTRDGi_ReadFlashID();
        if(type == CTRDG_BACKUP_TYPE_FLASH_512K) flp=flash512_list;
        if(type == CTRDG_BACKUP_TYPE_FLASH_1M) flp=flash1M_list;
        
        /*access cycle settings*/
        MI_SetCartridgeRamCycle( ram_cycle);
        /*exception controls (unlock)*/
        (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

        result=1;
        while((*flp)->type.makerID!=0x00) {
            if((flashID & 0xff)==*(u16 *)&((*flp)->type.makerID)) {
                result=0;
                break;
            }
            flp++;
        }
        CTRDGi_WriteAgbFlashSector=(*flp)->CTRDGi_WriteAgbFlashSector;
        CTRDGi_EraseAgbFlashChip=(*flp)->CTRDGi_EraseAgbFlashChip;
        CTRDGi_EraseAgbFlashSector=(*flp)->CTRDGi_EraseAgbFlashSector;
        CTRDGi_WriteAgbFlashSectorAsync=(*flp)->CTRDGi_WriteAgbFlashSectorAsync;
        CTRDGi_EraseAgbFlashChipAsync=(*flp)->CTRDGi_EraseAgbFlashChipAsync;
        CTRDGi_EraseAgbFlashSectorAsync=(*flp)->CTRDGi_EraseAgbFlashSectorAsync;
        CTRDGi_PollingSR=(*flp)->CTRDGi_PollingSR;
        ctrdgi_fl_maxtime=(*flp)->maxtime;
        AgbFlash=&(*flp)->type;
    }
    else if(type == CTRDG_BACKUP_TYPE_SRAM)
    {
        ctrdgi_sram_lock_id = (u16)OS_GetLockID();
        result = 0;
    }
    return result;
}

