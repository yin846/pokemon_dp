/********************************************************************/
/*      rom_header.h*/
/*          NITRO-IPL*/
/*                                                                  */
/*              Copyright (C) 2003-2004 NINTENDO Co.,Ltd.*/
/********************************************************************/
/*
	ROM header region define


	$Log: mb_rom_header.h,v $
	Revision 1.4  01/18/2006 02:11:30  kitase_hirotake
	do-indent
	
	Revision 1.3  02/28/2005 05:26:25  yosizaki
	do-indent.
	
	Revision 1.2  09/10/2004 03:24:29  yosizaki
	add own_size member.
	
	Revision 1.1  2004/09/07 04:26:13  sato_masaki
	Renamed rom_header.h to mb_rom_header.h.
	
	Revision 1.6  2004/09/03 12:07:59  sato_masaki
	small fix
	
	Revision 1.5  2004/09/03 11:48:54  yasu
	fix typo
	
	Revision 1.4  2004/09/03 07:06:28  sato_masaki
	Divided files according to features.
	
	Revision 1.2  08/07/2004 08:20:31  yosiokat
	Corrected one part of the information in the RomHeader.    
	
	Revision 1.1  08/07/2004 07:15:20  yosizaki
	(none)
	
	Revision 1.1  07/18/2004 11:49:47  Sato
	(none)
	
	Revision 1.1  07/14/2004 07:40:16  Yosiokat
	new additions
	
	Revision 1.5  06/23/2004 08:48:38  Yosiokat
	Added the disableClearMemoryPad memory clear control flag for when starting up in the debugger.
	
	Revision 1.4  06/17/2004 12:27:32  Yosiokat
	Added the inspectCard flag, which is used for inspecting the card in the ROM header.    
	
	Revision 1.3  05/21/2004 06:12:18  Yosiokat
	Deleted the semicolon "};" in the c++ compatibility portion of the header, since it was an error.    
	
	Revision 1.2  05/19/2004 08:25:28  yosiokat
	Added update log.
	

*/

#ifndef _ROM_HEADER_H_
#define _ROM_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <nitro.h>
#include <nitro/fs.h>



// ROM header
//----------------------------------------------------------------------
typedef struct
{
    //
    // 0x000 System Reserved
    //
    char    title_name[12];            // Soft title name
    u32     game_code;                 // Game code

    u16     maker_code;                // Maker code
    u8      machine_code;              // Machine code
    u8      rom_type;                  // Rom type
    u8      rom_size;                  // Rom size

    u8      reserved_A[9];             // System Reserved A ( Set ALL 0 )

    u8      soft_version;              // Soft version
    u8      comp_arm9_boot_area:1;     // Compress arm9 boot area
    u8      comp_arm7_boot_area:1;     // Compress arm7 boot area
    u8      inspectCard:1;             // the inspect card flag
    u8      disableClearMemoryPad:1;   // the IPL memory pad clear - disable flag
    u8:     0;

    //
    // 0x020 for Static modules (Section:B)
    //
    //      ARM9
    u32     main_rom_offset;           // ROM offset
    void   *main_entry_address;        // Entry point
    void   *main_ram_address;          // RAM address
    u32     main_size;                 // Module size

    //      ARM7
    u32     sub_rom_offset;            // ROM offset
    void   *sub_entry_address;         // Entry point
    void   *sub_ram_address;           // RAM address
    u32     sub_size;                  // Module size

    //
    // 0x040 for File Name Table[FNT] (Section:C)
    //
    u32     fnt_offset;                // ROM offset
    u32     fnt_size;                  // Table size

    //
    // 0x048 for File Allocation Table[FAT] (Section:E)
    //
    u32     fat_offset;                // ROM offset
    u32     fat_size;                  // Table size

    //
    // 0x050 for Overlay Tables[OVT] (Section:D)
    //
    //      ARM9
    u32     main_ovt_offset;           // ROM offset
    u32     main_ovt_size;             // Table size

    //      ARM7
    u32     sub_ovt_offset;            // ROM offset
    u32     sub_ovt_size;              // Table size

    // 0x060 for ROM control parameter
    u8      reserved_A2[32];

    // 0x080
    u32     own_size;

    // 0x084 - 0x0C0 System Reserved
    u8      reserved_B[60];            // System Reserved B (Set 0)

    // 0x0C0 for NINTENDO logo data
    u8      nintendo_logo[0x9c];       // NINTENDO logo data
    u16     nintendo_logo_crc16;       //            CRC-16

    // 0x15E ROM header CRC-16
    u16     header_crc16;              // ROM header CRC-16

    //
    // 0x0160 - 0x0180 System Reserved
    //
    u8      reserved_C[32];            // Debugger Reserved (Set ALL 0)

}
RomHeader;


//----------------------------------------------------------------------
// procurement of the ROM header address    
//----------------------------------------------------------------------

#define GetRomHeaderAddr()  ((RomHeader *)HW_ROM_HEADER_BUF)

//- procures the ROM header address    



#ifdef __cplusplus
}
#endif

#endif // _ROM_HEADER_H_
