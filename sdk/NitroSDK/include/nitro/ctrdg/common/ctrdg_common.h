/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - include
  File:     ctrdg_common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_common.h,v $
  Revision 1.18  04/28/2006 11:02:17  yada
  add define of PXI command for PHI setting.
  MIPhiClock enum -> CTRDGPhiClock enum

  Revision 1.17  02/22/2006 04:06:35  kitase_hirotake
  Revised to eliminate unlimited waiting for locks

  Revision 1.16  02/07/2006 08:12:18  kitase_hirotake
  Added CTRDG_CheckPulledOut

  Revision 1.15  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.14  11/30/2005 12:22:58  yosizaki
  add CTRDG_IsEnabled(), CTRDG_Enable(), CTRDG_CheckEnabled().

  Revision 1.13  11/01/2005 01:16:12  okubata_ryoma
  Made changes due to addition of CTRDG.

  Revision 1.12  04/12/2005 06:23:03  yosizaki
  add pulled-out checkers.

  Revision 1.11  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.10  02/21/2005 02:47:07  seiki_masashi
  Revised Copyright notation

  Revision 1.9  02/18/2005 13:17:26  seiki_masashi
  warning measures

  Revision 1.8  09/17/2004 12:25:18  yada
  add data forwarding functions

  Revision 1.7  09/17/2004 06:44:09  yada
  add line in CTRDG_CpuCopy8()

  Revision 1.6  09/17/2004 05:12:17  yada
  add CTRDG_IsOptionCartridge()

  Revision 1.5  09/14/2004 13:01:50  yada
  CallbackForPulledOut->PulledOutCallback

  Revision 1.4  09/14/2004 10:41:04  yada
  fix comment

  Revision 1.3  09/14/2004 07:23:08  yada
  fix a little

  Revision 1.2  09/14/2004 06:09:44  yada
  check cartridge pulled out in ARM7

  Revision 1.1  09/13/2004 10:49:35  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CTRDG_COMMON_H_
#define NITRO_CTRDG_COMMON_H_


#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>


#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------------------------------------------

// Definitions related to PXI communications protocol
#define CTRDG_PXI_COMMAND_MASK              0x0000003f  // Command part for start word
#define CTRDG_PXI_COMMAND_SHIFT             0
#define CTRDG_PXI_COMMAND_PARAM_MASK        0x01ffffc0  // Parameter part for start word
#define CTRDG_PXI_COMMAND_PARAM_SHIFT       6
#define CTRDG_PXI_FIXLEN_DATA_MASK          0x03ffffff  // Data part for fixed-length continuous packets
#define CTRDG_PXI_FIXLEN_DATA_SHIFT         0
#define CTRDG_PXI_FLEXLEN_DATA_MASK         0x01ffffff  // Data part for variable-length continuous packets
#define CTRDG_PXI_FLEXLEN_DATA_SHIFT        0
#define CTRDG_PXI_FLEXLEN_CONTINUOUS_BIT    0x02000000  // Sending variable-length continuous packets
#define CTRDG_PXI_PACKET_MAX                4   // Largest continuous number of packets

// Commands issued from ARM9 to the ARM7 via PXI
#define CTRDG_PXI_COMMAND_INIT_MODULE_INFO  0x0001
#define CTRDG_PXI_COMMAND_TERMINATE         0x0002
#define CTRDG_PXI_COMMAND_SET_PHI           0x0003

#define CTRDG_PXI_COMMAND_IMI_PACKET_SIZE   2

// Commands issued from ARM7 to the ARM9 via PXI
#define CTRDG_PXI_COMMAND_PULLED_OUT        0x0011
#define CTRDG_PXI_COMMAND_SET_PHI_RESULT    0x0012


#ifdef SDK_ARM9
#define CTRDG_LOCKED_BY_MYPROC_FLAG     OS_MAINP_LOCKED_FLAG
#else
#define CTRDG_LOCKED_BY_MYPROC_FLAG     OS_SUBP_LOCKED_FLAG
#endif

#define CTRDG_SYSROM9_NINLOGO_ADR       0xffff0020

#define CTRDG_AGB_ROMHEADER_SIZE        0xc0
#define CTRDG_AGB_NINLOGO_SIZE          0x9c

#define CTRDG_IS_ROM_CODE               0x96


//---- PHI output control
typedef enum
{
    CTRDG_PHI_CLOCK_LOW = MIi_PHI_CLOCK_LOW,      // Low level
    CTRDG_PHI_CLOCK_4MHZ = MIi_PHI_CLOCK_4MHZ,    //  4.19 MHz
    CTRDG_PHI_CLOCK_8MHZ = MIi_PHI_CLOCK_8MHZ,    //  8.38 MHz
    CTRDG_PHI_CLOCK_16MHZ = MIi_PHI_CLOCK_16MHZ   // 16.76 MHz
}
CTRDGPhiClock;

//----------------------------------------------------------------------------

// Game Pak header

typedef struct
{
    u32     startAddress;              // Start address (for AGB)
    u8      nintendoLogo[0x9c];        // NINTENDO logo data (for AGB)

    char    titleName[12];             // Soft title name (for AGB)
    u32     gameCode;                  // Game code (for AGB)
    u16     makerCode;                 // Maker code (for AGB)

    u8      isRomCode;                 // Is ROM code

    u8      machineCode;               // Machine code (for AGB)
    u8      deviceType;                // Device type (for AGB)

    u8      exLsiID[3];                // Extended LSI-ID

    u8      reserved_A[4];             // Reserved A (Set 0)
    u8      softVersion;               // Soft version (for AGB)
    u8      complement;                // Complement (for AGB)

    u16     moduleID;                  // Module ID
}
CTRDGHeader;

// Module ID

typedef struct
{
    union
    {
        struct
        {
            u8      bitID;             // Bit ID
            u8      numberID:5;        // Number ID
            u8:     2;
            u8      disableExLsiID:1;  // Invalidate expanded LSI-ID region
        };
        u16     raw;
    };
}
CTRDGModuleID;

// Module information

typedef struct
{
    CTRDGModuleID moduleID;            // Module ID
    u8      exLsiID[3];                // Extended LSI-ID
    u8      isAgbCartridge:1;          // Is AgbCartridge
    u8      detectPullOut:1;           // Detect pull out cartridge
    u8:     0;
    u16     makerCode;                 // Maker code (for AGB)
    u32     gameCode;                  // Game code (for AGB)
}
CTRDGModuleInfo;


//---- callback type for pulled out cartridge
#ifdef SDK_ARM9
typedef BOOL (*CTRDGPulledOutCallback) (void);
#endif
/*---------------------------------------------------------------------------*

  Description:  About the CTRDG library

                ARM7 periodically checks to see if Game Pak has been removed.
                However, this cannot be done when the ARM9's access rights to
                the Game Pak bus are not released, so do not keep locked for
                longer than 10 continuous frames.

                The processor tries the Game Pak lock internally, so if the
                lock has been set by the other processor, it will not return
                until the lock is released.

                However, if the processor itself has locked the Game Pak, then
                it is OK to call without releasing the lock.

 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Init

  Description:  Initilize Game Pak function

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CTRDG_Init(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsPulledOut

  Description:  Checks to see if Game Pak has been removed

                If the Game Pak was never inserted, the function returns FALSE.

                Note that if a low-resistance adapter cartridge is not attached
                to IS-NITRO-DEBUGGER, the function sometimes reacts like the
                Game Pak has been removed, even though there was never one attached in the first place.

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsPulledOut(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsExisting

  Description:  Checks whether there is a Game Pak

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsExisting(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsBitID

  Description:  Checks whether there is a bit-ID module to the Game Pak


  Arguments:    bitID  bit ID

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsBitID(u8 bitID);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsNumberID

  Description:  Checks whether there is a number-ID module to the Game Pak

  Arguments:    numberID  number ID

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsNumberID(u8 numberID);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsAgbCartridge

  Description:  Checks whether there is an AGB Game Pak

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsAgbCartridge(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsOptionCartridge

  Description:  Checks whether there is an option cartridge

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsOptionCartridge(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_GetAgbGameCode

  Description:  Gets the AGB Game Pak's game code

  Arguments:    None

  Returns:      Game code if exist, FALSE if not exist
 *---------------------------------------------------------------------------*/
u32     CTRDG_GetAgbGameCode(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_GetAgbMakerCode

  Description:  Gets the AGB Game Pak's maker code

  Arguments:    None

  Returns:      Maker code if exist, FALSE if not exist
 *---------------------------------------------------------------------------*/
u16     CTRDG_GetAgbMakerCode(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsAgbCartridgePulledOut

  Description:  get whether system has detected pulled out AGB cartridge

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsAgbCartridgePulledOut(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsOptionCartridgePulledOut

  Description:  get whether system has detected pulled out option cartridge

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsOptionCartridgePulledOut(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetPulledOutCallback

  Description:  set user callback for being pulled out cartridge

  Arguments:    callback : callback

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM9
void    CTRDG_SetPulledOutCallback(CTRDGPulledOutCallback callback);
#endif

/*---------------------------------------------------------------------------*
  Name:         CTRDG_TerminateForPulledOut

  Description:  terminate for pulling out cartridge

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM9
void    CTRDG_TerminateForPulledOut(void);
#endif

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SendToARM7

  Description:  Sends data to ARM7

  Arguments:    arg      : data to send

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CTRDG_SendToARM7(void *arg);

//================================================================================
//       ACCESS DATA IN CARTRIDGE
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CTRDG_DmaCopy16 / 32

  Description:  read cartridge data via DMA

  Arguments:    dmaNo : DMA No.
                src: source address (in cartridge)
                dest  : destination address (in memory)
                size   : forwarding size

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size);
BOOL    CTRDG_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CpuCopy8 / 16 / 32

  Description:  read cartridge data by CPU access

  Arguments:    src: source address (in cartridge)
                dest  : destination address (in memory)
                size   : forwarding size

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_CpuCopy8(const void *src, void *dest, u32 size);
BOOL    CTRDG_CpuCopy16(const void *src, void *dest, u32 size);
BOOL    CTRDG_CpuCopy32(const void *src, void *dest, u32 size);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Read8 / 16 / 32

  Description:  read cartridge data by CPU access

  Arguments:    address  : source address (in cartridge)
                rdata    : address to store read data

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_Read8(const u8 *address, u8 *rdata);
BOOL    CTRDG_Read16(const u16 *address, u16 *rdata);
BOOL    CTRDG_Read32(const u32 *address, u32 *rdata);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Write8 / 16 / 32

  Description:  write data to cartridge

  Arguments:    address  : destination address (in cartridge)
                data   : data to write

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_Write8(u8 *address, u8 data);
BOOL    CTRDG_Write16(u16 *address, u16 data);
BOOL    CTRDG_Write32(u32 *address, u32 data);


/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsEnabled

  Description:  check if CTRDG is accessible

  Arguments:    None.

  Returns:      Return cartridge access permission.
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsEnabled(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Enable

  Description:  Set cartridge access permission mode.

  Arguments:    enable       permission mode to be set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDG_Enable(BOOL enable);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CheckEnabled

  Description:  Terminate program if CTRDG is not accessible

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDG_CheckEnabled(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CheckPulledOut

  Description:  cartridge is pulled out

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CTRDG_CheckPulledOut(void);


/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetPhiClock

  Description:  set ARM7 and ARM9 PHI output clock

  Arguments:    clock :    Phi clock to set

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM9
void CTRDG_SetPhiClock(CTRDGPhiClock clock);
#endif

//================================================================================
//       PRIVATE FUNCTION
//================================================================================
//---- private function ( don't use these CTRDGi_* function )

// ROM access-cycle structure
typedef struct CTRDGRomCycle
{
    MICartridgeRomCycle1st c1;
    MICartridgeRomCycle2nd c2;

}
CTRDGRomCycle;

// Lock-by-processor information structure
typedef struct CTRDGLockByProc
{
    BOOL    locked;
    OSIntrMode irq;

}
CTRDGLockByProc;

void    CTRDGi_InitCommon(void);
void    CTRDGi_InitModuleInfo(void);
void    CTRDGi_SendtoPxi(u32 data);
#if defined(SDK_ARM7)
BOOL    CTRDGi_LockByProcessor(u16 lockID, CTRDGLockByProc *info);
#else
void    CTRDGi_LockByProcessor(u16 lockID, CTRDGLockByProc *info);
#endif
void    CTRDGi_UnlockByProcessor(u16 lockID, CTRDGLockByProc *info);
void    CTRDGi_ChangeLatestAccessCycle(CTRDGRomCycle *r);
void    CTRDGi_RestoreAccessCycle(CTRDGRomCycle *r);

BOOL    CTRDGi_IsBitIDAtInit(u8 bitID);
BOOL    CTRDGi_IsNumberIDAtInit(u8 numberID);
BOOL    CTRDGi_IsAgbCartridgeAtInit(void);
u32     CTRDGi_GetAgbGameCodeAtInit(void);
u16     CTRDGi_GetAgbMakerCodeAtInit(void);

// Acquire the Game Pak header address
#define CTRDGi_GetHeaderAddr()          ((CTRDGHeader *)HW_CTRDG_ROM)

// Acquire the ROM image address of the Game Pak module ID
#define CTRDGi_GetModuleIDImageAddr()   ((u16 *)(HW_CTRDG_ROM + 0x0001fffe))

// Acquire the address of the Game Pak module information
#define CTRDGi_GetModuleInfoAddr()      ((CTRDGModuleInfo *)HW_CTRDG_MODULE_INFO_BUF)




#define CTRDGi_FORWARD_TYPE_DMA   0x00000000
#define CTRDGi_FORWARD_TYPE_CPU   0x00000001
#define CTRDGi_FORWARD_TYPE_MASK  0x00000001

#define CTRDGi_FORWARD_WIDTH_8    0x00000000
#define CTRDGi_FORWARD_WIDTH_16   0x00000010
#define CTRDGi_FORWARD_WIDTH_32   0x00000020
#define CTRDGi_FORWARD_WIDTH_MASK 0x00000030

#define CTRDGi_FORWARD_DMA16   ( CTRDGi_FORWARD_TYPE_DMA | CTRDGi_FORWARD_WIDTH_16 )
#define CTRDGi_FORWARD_DMA32   ( CTRDGi_FORWARD_TYPE_DMA | CTRDGi_FORWARD_WIDTH_32 )
#define CTRDGi_FORWARD_CPU8    ( CTRDGi_FORWARD_TYPE_CPU | CTRDGi_FORWARD_WIDTH_8  )
#define CTRDGi_FORWARD_CPU16   ( CTRDGi_FORWARD_TYPE_CPU | CTRDGi_FORWARD_WIDTH_16 )
#define CTRDGi_FORWARD_CPU32   ( CTRDGi_FORWARD_TYPE_CPU | CTRDGi_FORWARD_WIDTH_32 )

#define CTRDGi_ACCESS_DIR_WRITE   0x00000000
#define CTRDGi_ACCESS_DIR_READ    0x00000001
#define CTRDGi_ACCESS_DIR_MASK    0x00000001

#define CTRDGi_ACCESS_WIDTH_8     0x00000010
#define CTRDGi_ACCESS_WIDTH_16    0x00000020
#define CTRDGi_ACCESS_WIDTH_32    0x00000040
#define CTRDGi_ACCESS_WIDTH_MASK  0x000000f0

#define CTRDGi_ACCESS_WRITE8  ( CTRDGi_ACCESS_DIR_WRITE | CTRDGi_ACCESS_WIDTH_8  )
#define CTRDGi_ACCESS_WRITE16 ( CTRDGi_ACCESS_DIR_WRITE | CTRDGi_ACCESS_WIDTH_16 )
#define CTRDGi_ACCESS_WRITE32 ( CTRDGi_ACCESS_DIR_WRITE | CTRDGi_ACCESS_WIDTH_32 )
#define CTRDGi_ACCESS_READ8   ( CTRDGi_ACCESS_DIR_READ  | CTRDGi_ACCESS_WIDTH_8  )
#define CTRDGi_ACCESS_READ16  ( CTRDGi_ACCESS_DIR_READ  | CTRDGi_ACCESS_WIDTH_16 )
#define CTRDGi_ACCESS_READ32  ( CTRDGi_ACCESS_DIR_READ  | CTRDGi_ACCESS_WIDTH_32 )

//---- internal function
BOOL    CTRDGi_CopyCommon(u32 dmaNo, const void *src, void *dest, u32 size, u32 forwardType);

//---- internal function
BOOL    CTRDGi_AccessCommon(void *address, u32 data, void *rdata, u32 accessType);

#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_CTRDG_COMMON_H_*/
#endif
