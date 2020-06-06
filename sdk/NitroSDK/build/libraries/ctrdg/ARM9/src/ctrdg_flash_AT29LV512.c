/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_flash_AT29LV512.c

  Copyright 2003,2004,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_flash_AT29LV512.c,v $
  Revision 1.2  04/07/2006 03:27:28  okubata_ryoma
  small fix

  Revision 1.1  04/05/2006 10:48:30  okubata_ryoma
  changes made for AGB backup library's SDK collection


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

// definition data------------------------------
/* For debugging*/
//#ifndef   __FLASH_DEBUG
#define CTRDG_BACKUP_COM_ADR1   (CTRDG_AGB_FLASH_ADR+0x00005555)
#define CTRDG_BACKUP_COM_ADR2   (CTRDG_AGB_FLASH_ADR+0x00002aaa)
//#else
//#define   COM_ADR1    0x02003ffe
//#define COM_ADR2  0x02003fff
//#endif

#define FLASH_LOG_SECTOR_COUNT          16      // 64KB/4KB=16
#define FLASH_SECTOR_LOG_TO_PHYS        32      // 4KB/128B=32
#define FLASH_SECTOR_LOG_TO_PHYS_SHIFT  5       // 32=2^5

// extern data----------------------------------
extern u16 CTRDGi_PollingSR512kCOMMON(u16 phase,u8 *adr,u16 lastData);
/*exception controls*/
extern u16 ctrdgi_flash_lock_id;
extern BOOL ctrdgi_backup_irq;

// function's prototype declaration-------------
u16 CTRDGi_EraseFlashChipAT(void);
u16 CTRDGi_EraseFlashSectorAT(u16 p_secNo);
u16 CTRDGi_EraseFlash4KBAT(u16 l_secNo);
u16 CTRDGi_WriteFlashSectorAT(u16 p_secNo,u8 *src);
u16 CTRDGi_WriteFlash4KBAT(u16 l_secNo,u8 *src);

u32 CTRDGi_EraseFlashChipCoreAT(CTRDGTaskInfo *arg);
u32 CTRDGi_EraseFlash4KBCoreAT(CTRDGTaskInfo *arg);
u32 CTRDGi_WriteFlash4KBCoreAT(CTRDGTaskInfo *arg);
u32 CTRDGi_EraseFlashSectorCoreAT(CTRDGTaskInfo *arg);
u32 CTRDGi_WriteFlashSectorCoreAT(CTRDGTaskInfo *arg);

void CTRDGi_EraseFlashChipAsyncAT(CTRDG_TASK_FUNC callback);
void CTRDGi_EraseFlash4KBAsyncAT(u16 secNo, CTRDG_TASK_FUNC callback);
void CTRDGi_WriteFlash4KBAsyncAT(u16 secNo,u8 *src, CTRDG_TASK_FUNC callback);
void CTRDGi_EraseFlashSectorAsyncAT(u16 secNo, CTRDG_TASK_FUNC callback);
void CTRDGi_WriteFlashSectorAsyncAT(u16 secNo,u8 *src, CTRDG_TASK_FUNC callback);

// const data-----------------------------------
static const u16 atMaxTime[] = {
    10,       // common       10ms    (15.256us*66)*10
    40,       // program      20ms    (15.256us*66)*20
     0,       // sector erase 0ms
    40,       // chip erase   20ms
};

const CTRDGiFlashTypePlus AT29LV512_lib = {
    CTRDGi_WriteFlash4KBAT,
    CTRDGi_EraseFlashChipAT,
    CTRDGi_EraseFlash4KBAT,
    CTRDGi_WriteFlash4KBAsyncAT,
    CTRDGi_EraseFlashChipAsyncAT,
    CTRDGi_EraseFlash4KBAsyncAT,
    CTRDGi_PollingSR512kCOMMON,
    atMaxTime,
    {
/* For debugging*/
//#ifndef   __FLASH_DEBUG
        0x00010000,                         // ROM size
        {0x00001000, 12, 16,  0},           // sector
//#else
//      0x00002000,
//      {0x00000200,  9, 16,  0},
//#endif
        {MI_CTRDG_RAMCYCLE_18, MI_CTRDG_RAMCYCLE_18},   // agb read cycle=8, write cycle=8
        0x1f,                               // maker ID
        0x3d,                               // device ID
    },
};

const CTRDGiFlashTypePlus AT29LV512_org = {
    CTRDGi_WriteFlashSectorAT,  // cannot be used for untransferred DataPolling routines
    CTRDGi_EraseFlashChipAT,
    CTRDGi_EraseFlashSectorAT,    // cannot be used for untransferred DataPolling routines
    CTRDGi_WriteFlashSectorAsyncAT,
    CTRDGi_EraseFlashChipAsyncAT,
    CTRDGi_EraseFlashSectorAsyncAT,
    CTRDGi_PollingSR512kCOMMON,
    atMaxTime,
    {
/* For debugging*/
//#ifndef   __FLASH_DEBUG
        0x00010000,                         // ROM size
        {0x00000080,  7,512,  0},           // sector
//#else
//      0x00002000,
//      {0x00000010,  4,256,  0},
//#endif
        {MI_CTRDG_RAMCYCLE_18, MI_CTRDG_RAMCYCLE_18},   // agb read cycle=8, write cycle=8
        0x1f,                               // maker ID
        0x3d,                               // device ID
    },
};


// program description**************************
u32 CTRDGi_EraseFlashChipCoreAT(CTRDGTaskInfo *arg)
{
    MICartridgeRamCycle ram_cycle;
    u32 result;
    (void)arg;
    
    /*exception controls (lock)*/
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*access cycle settings*/
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle( AgbFlash->agbWait[0]);
//    *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AT29LV512_org.type.agbWait[0];

    ctrdgi_backup_irq = OS_DisableIrq();

    *(vu8 *)CTRDG_BACKUP_COM_ADR1=0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2=0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1=0x80;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1=0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2=0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1=0x10;
/* For debugging*/
//#ifdef    __FLASH_DEBUG
//  adr=(u8 *)CTRDG_AGB_FLASH_ADR;
//  for(i=0;i<AT29LV512_org.type.romSize;i++)
//      *adr++=0xff;
//#endif
    (void)OS_RestoreIrq(ctrdgi_backup_irq);

    result=CTRDGi_PollingSR(CTRDG_BACKUP_PHASE_CHIP_ERASE,(u8 *)CTRDG_AGB_FLASH_ADR,0xff);
//  *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)| 3;

    /*access cycle settings*/
    MI_SetCartridgeRamCycle( ram_cycle);
    /*exception controls (unlock)*/
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}

u32 CTRDGi_EraseFlashSectorCoreAT(CTRDGTaskInfo *arg)
{
    u32 i;
    u8 *dst;
    BOOL shlet_ime;
    MICartridgeRamCycle ram_cycle;
    u32 result;
    CTRDGTaskInfo p = *arg;
    u16 p_secNo = p.sec_num;
    
    // ** This routine cannot be used alone if the DataPolling routine
    //      is not transferred.
    
    // calculates target address
    dst=(u8 *)(CTRDG_AGB_FLASH_ADR+(p_secNo<<AT29LV512_org.type.sector.shift));
    
    /*exception controls (lock)*/
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*access cycle settings*/
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle( AgbFlash->agbWait[0]);

    // interrupt prohibition
    shlet_ime = OS_DisableIrq();

    // data program
    *(vu8 *)CTRDG_BACKUP_COM_ADR1=0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2=0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1=0xa0;
    for(i=AT29LV512_org.type.sector.size;i>0;i--)
        *dst++=0xff;
    dst--;
    
    // interrupts allowed again
    (void)OS_RestoreIrq( shlet_ime);

    // Status check
    result=CTRDGi_PollingSR(CTRDG_BACKUP_PHASE_PROGRAM,dst,0xff);
    if(result)
        result=(u16)((result&0xff00)|CTRDG_BACKUP_PHASE_SECTOR_ERASE);
    
    /*access cycle settings*/
    MI_SetCartridgeRamCycle( ram_cycle);
    /*exception controls (unlock)*/
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}

u32 CTRDGi_EraseFlash4KBCoreAT(CTRDGTaskInfo *arg)
{
    u32 result;
    u16 i,p_secNo,retry;
    CTRDGTaskInfo p = *arg;
    u16 l_secNo = p.sec_num;

    /*Check the parameter*/
    if(l_secNo>=FLASH_LOG_SECTOR_COUNT)
        return CTRDG_BACKUP_RESULT_ERROR|CTRDG_BACKUP_PHASE_PARAMETER_CHECK;
    
//  *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AT29LV512_org.type.agbWait[0];
    p_secNo=(u16)(l_secNo<<FLASH_SECTOR_LOG_TO_PHYS_SHIFT);
    for(i=0;i<FLASH_SECTOR_LOG_TO_PHYS;i++) {
        retry=2;
        while(retry) {
            result=CTRDGi_EraseFlashSectorAT(p_secNo);
            if(result==0)
                break;
            retry--;
        }
        p_secNo++;
        if(result)
            break;
    }
//  *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)| 3;

    return result;
}

u32 CTRDGi_WriteFlashSectorCoreAT(CTRDGTaskInfo *arg)
{
    u32 i;
    u8 *dst;
    BOOL shlet_ime;
    MICartridgeRamCycle ram_cycle;
    u32 result;
    CTRDGTaskInfo p = *arg;
    u16 p_secNo = p.sec_num;
    u8 *src = p.data;
    
    // ** This routine cannot be used alone if the DataPolling routine
    //      is not transferred.
    
    /*exception controls (lock)*/
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*access cycle settings*/
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle( AgbFlash->agbWait[0]);

    // calculates target address
    dst=(u8 *)(CTRDG_AGB_FLASH_ADR+(p_secNo<<AT29LV512_org.type.sector.shift));
    // interrupt prohibition
    shlet_ime = OS_DisableIrq();

    // data program
    //*(vu16 *)REG_WAITCNT=(
    *(vu8 *)CTRDG_BACKUP_COM_ADR1=0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2=0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1=0xa0;
    for(i=AT29LV512_org.type.sector.size;i>0;i--)
        *dst++=*src++;
    dst--;
    src--;
    // interrupts allowed again
    (void)OS_RestoreIrq( shlet_ime);

    //*(vu16 *)REG_WAITCNT=(*(vu16 *)REG_WAITCNT & 0xfffc)|AT29LV512_org.type.agbWait[0];

    // Status check
    result = CTRDGi_PollingSR(CTRDG_BACKUP_PHASE_PROGRAM,dst,*src);
    
    /*access cycle settings*/
    MI_SetCartridgeRamCycle( ram_cycle);
    /*exception controls (unlock)*/
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}

u32 CTRDGi_WriteFlash4KBCoreAT(CTRDGTaskInfo *arg)
{
    u32 result;
    u16 p_secNo,retry;
    CTRDGTaskInfo p = *arg;
    u16 l_secNo = p.sec_num;
    u8 *src = p.data;

    /*Check the parameter*/
    if(l_secNo>=FLASH_LOG_SECTOR_COUNT)
        return CTRDG_BACKUP_RESULT_ERROR|CTRDG_BACKUP_PHASE_PARAMETER_CHECK;
    
//  *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AT29LV512_org.type.agbWait[0];
    p_secNo=(u16)(l_secNo<<FLASH_SECTOR_LOG_TO_PHYS_SHIFT);
    ctrdg_flash_remainder=(u16)(AT29LV512_lib.type.sector.size);
    while(ctrdg_flash_remainder) {
        retry=2;
        while(retry) {
            result=CTRDGi_WriteFlashSectorAT(p_secNo,src);
            if(result==0)
                break;
            retry--;
        }
        if(result)
            break;
        ctrdg_flash_remainder-=AT29LV512_org.type.sector.size;
        src+=AT29LV512_org.type.sector.size;
        p_secNo++;
    }
//  *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)| 3;
    
    return result;
}

u16 CTRDGi_EraseFlashChipAT(void)
{
    u16 result;
    CTRDGTaskInfo p;
    result = (u16)CTRDGi_EraseFlashChipCoreAT(&p);
    
    return result;
}

u16 CTRDGi_EraseFlash4KBAT(u16 l_secNo)
{
    u16 result;
    CTRDGTaskInfo p;
    p.sec_num = l_secNo;
    result = (u16)CTRDGi_EraseFlash4KBCoreAT(&p);
    
    return result;
}

u16 CTRDGi_WriteFlash4KBAT(u16 l_secNo,u8 *src)
{
    u16 result;
    CTRDGTaskInfo p;
    p.sec_num = l_secNo;
    p.data = src;
    result = (u16)CTRDGi_WriteFlash4KBCoreAT(&p);
    
    return result;
}

void CTRDGi_EraseFlashChipAsyncAT(CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    
    CTRDGi_SetTask(&p, CTRDGi_EraseFlashChipCoreAT, callback);
}

void CTRDGi_EraseFlash4KBAsyncAT(u16 l_secNo, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    
    p.sec_num = l_secNo;
    CTRDGi_SetTask(&p, CTRDGi_EraseFlash4KBCoreAT, callback);
}

void CTRDGi_WriteFlash4KBAsyncAT(u16 l_secNo, u8 *src, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    
    p.sec_num = l_secNo;
    p.data = src;
    CTRDGi_SetTask(&p, CTRDGi_WriteFlash4KBCoreAT, callback);
}

u16 CTRDGi_EraseFlashSectorAT(u16 p_secNo)
{
    u16 result;
    CTRDGTaskInfo p;
    p.sec_num = p_secNo;
    result = (u16)CTRDGi_EraseFlashSectorCoreAT(&p);
    
    return result;
}

u16 CTRDGi_WriteFlashSectorAT(u16 p_secNo,u8 *src)
{
    u16 result;
    CTRDGTaskInfo p;
    p.sec_num = p_secNo;
    p.data = src;
    result = (u16)CTRDGi_WriteFlashSectorCoreAT(&p);

    return result;
}

void CTRDGi_EraseFlashSectorAsyncAT(u16 p_secNo, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    
    p.sec_num = p_secNo;
    CTRDGi_SetTask(&p, CTRDGi_EraseFlashSectorCoreAT, callback);
}

void CTRDGi_WriteFlashSectorAsyncAT(u16 p_secNo, u8 *src, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    
    p.sec_num = p_secNo;
    p.data = src;
    CTRDGi_SetTask(&p, CTRDGi_WriteFlashSectorCoreAT, callback);
}
