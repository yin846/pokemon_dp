/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_fileinfo.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_fileinfo.c,v $
  Revision 1.34  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.33  11/07/2005 01:21:50  okubata_ryoma
  Switched from SDK_STATIC_ASSERT to SDK_COMPILER_ASSERT

  Revision 1.32  05/20/2005 03:10:01  yosizaki
  support MBGameRegistry::userParam.

  Revision 1.31  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.30  02/21/2005 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.29  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.28  12/07/2004 10:20:37  yosizaki
  fix to reload cache from correct archive. (not only "rom")

  Revision 1.27  11/26/2004 09:16:42  yosizaki
  fix MB_ReadSegment (around Debugger's patch-code)

  Revision 1.26  11/22/2004 07:39:07  yosizaki
  fix MB_ReadSegment (around auth-code)

  Revision 1.25  11/11/2004 11:49:06  yosizaki
  fix MB_ReadSegment around cache-mode and auth-code.
  add task-thread system.

  Revision 1.24  11/10/2004 13:16:27  yosizaki
  change MB_ReadSegment to use cache-system.

  Revision 1.23  2004/10/06 05:47:58  sato_masaki
  small fix

  Revision 1.22  09/29/2004 07:15:56  yosizaki
  fix MB_ReadSegment. (add size of auth-code)

  Revision 1.21  2004/09/28 00:13:18  sato_masaki
  bug fix

  Revision 1.20  2004/09/27 11:24:02  sato_masaki
  - Added the IsAbleToLoad() function
  - Corrected the problem with MB_RegisterFile that GameInfo was registered with Beacon even when the block information table creation failed

  Revision 1.19  2004/09/25 12:11:53  sato_masaki
  Added MBi_IsAbleToRecv(). (Renamed IsAbleToRecv() and moved from mb_child.c.))

  Revision 1.18  2004/09/25 05:30:32  sato_masaki
  Added process to prohibit interrupts while API called from main loop is executing.

  Revision 1.17  2004/09/22 09:41:50  sato_masaki
  Added condition determination to MBi_get_blockinfo()

  Revision 1.16  2004/09/20 13:12:20  sato_masaki
  Changed the return value of MBi_get_blockinfo to a BOOL value.

  Revision 1.15  2004/09/17 11:37:17  sato_masaki
  Set so that in MB_RegisterFile(), if MB has not started, a FALSE will return.

  Revision 1.14  2004/09/16 12:50:37  sato_masaki
  - Changed the functions and types that have been defined with MBi_*** in mb.h to MB_***.
  - Changed the return value of MB_UnregisterFile into a pointer to the registered segment buffer.

  Revision 1.13  09/16/2004 08:44:21  miya
  Addition of members for file updates

  Revision 1.12  2004/09/15 06:51:59  sato_masaki
  Deleted processes regarding WBT definitions.

  Revision 1.11  2004/09/15 06:25:29  sato_masaki
  Made variables of parts that were dependent on MB_COMM_P_SENDLEN, MB_COMM_C_SENDLEN definitions.

  Revision 1.10  09/14/2004 15:38:45  yosizaki
  fix MB_ReadSegment

  Revision 1.9  2004/09/11 11:20:48  sato_masaki
  change mbc to pPwork

  Revision 1.8  09/10/2004 03:24:14  yosizaki
  unify file-access to register file.

  Revision 1.7  09/08/2004 11:09:46  miya
  small fix.

  Revision 1.6  2004/09/07 04:38:17  sato_masaki
  - renamed the const value and the defaultLoadSegList to MBi_defaultLoadSegList, and made non-static.
  - Added support for the addition of the MB_BlockInfo elements.

  Revision 1.5  2004/09/06 11:51:52  sato_masaki
  Made changes so that the configuration of the fileNo to GameInfo is performed inside MB_RegisterFile.

  Revision 1.4  09/06/2004 06:23:03  yosizaki
  fix around MBi_ReadSegmentHeader().

  Revision 1.3  09/06/2004 03:29:21  yosizaki
  -add routine about reading segment header.
  -change MB_RegisterFile() in order to send own program too.

  Revision 1.2  2004/09/04 11:03:18  sato_masaki
  Moved the definition of ROM_HEADER_SIZE_FULL to mb_fileinfo.h.

  Revision 1.1  2004/09/03 07:04:36  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"

// definition

/*
 * Cache page unit.
 * At first, the parent header part was 16kB, but there were
 * problems when crossing boundaries, so it was increased to 17 KB.
 */
#define	MB_CACHE_PAGE_SIZE	(17 * 1024)

/* Determine the correctness of the value of the MB_ReadSegment() required buffer definition*/
SDK_COMPILER_ASSERT(ROM_HEADER_SIZE_FULL +
                    MB_AUTHCODE_SIZE +
                    sizeof(MBiCacheList) +
                    sizeof(u32) * 4 + MB_CACHE_PAGE_SIZE * 3 <= MB_SEGMENT_BUFFER_MIN);

// constant variables

/* 
    default segment list
    the segment type and storage order is fixed with this.
 */
const MbSegmentType MBi_defaultLoadSegList[MB_DL_SEGMENT_NUM] = {
    MB_SEG_ROMHEADER,                  /* ROM HEADER*/
    MB_SEG_ARM9STATIC,                 /* ARM9 static*/
    MB_SEG_ARM7STATIC,                 /* ARM7 static*/
};


/* segment header region.*/
#define	MB_SEGMENT_HEADER_MIN	0x4000
#define	MB_SEGMENT_HEADER_MAX	0x8000


/* Valid segment header region (To make this changeable in the future, prepare a setting function)*/
static const CARDRomRegion mbi_seg_header_default[] = {
    {0x4000, 0x1000,},
    {0x7000, 0x1000,},
    {0, 0},
};
static const CARDRomRegion *mbi_seg_header = mbi_seg_header_default;

/* Information needed when reading the segment's header part*/
typedef struct
{
    u32     rom_src;
    u32     mem_src;
    u32     mem_dst;
    u32     len;
}
MBiSegmentHeaderInfo;


// ----------------------------------------------------------------------------
// static function's prototypes

static void MBi_MakeDownloadFileInfo(MbDownloadFileInfoHeader * mbdlFileInfop, const void *buf);
static void MBi_SetSegmentInfo(const RomHeader * mbRomHeaderp, const MbSegmentType * loadSegListp,
                               MbSegmentInfo * seg_infop, u32 *child_recv_buff_addr);
static void MBi_ReadSegmentHeader(const MBiSegmentHeaderInfo * p, u32 top, u32 bottom, BOOL clean);
static BOOL IsAbleToLoad(u8 segment_no, u32 address, u32 size);

/*---------------------------------------------------------------------------*
  Name:         MB_GetSegmentLength

  Description:  Acquires the length of the segment buffer that is required for the specified binary file.

  Arguments:    file : the FSFile structure that indicates the binary file
                       If NULL, uses itself. (Parent/Child shared binary)
  
  Returns:      Returns a positive value if it can properly acquire size. Otherwise returns 0.
 *---------------------------------------------------------------------------*/

u32 MB_GetSegmentLength(FSFile *file)
{
    enum
    { ROM_HEADER_SIZE_SMALL = 0x60 };
    u8      rom[ROM_HEADER_SIZE_SMALL];
    const RomHeader *p = NULL;

    u32     ret = 0;

    SDK_ASSERT(!file || FS_IsFile(file));

    /* Specify a stand-alone binary file*/
    if (file)
    {
        /* Remember the current position*/
        const u32 bak_pos = FS_GetPosition(file);
        /* For now read the ROM header part*/
        if (FS_ReadFile(file, &rom, sizeof(rom)) >= sizeof(rom))
        {
            p = (const RomHeader *)rom;
        }
        /* Return to initial position*/
        (void)FS_SeekFile(file, (int)bak_pos, FS_SEEK_SET);
    }
    /* Binary shared by parent & child*/
    else
    {
        p = (const RomHeader *)HW_ROM_HEADER_BUF;
    }
    if (p)
    {
        ret = (u32)(ROM_HEADER_SIZE_FULL +      /* ROM header*/
                    MB_AUTHCODE_SIZE + /* auth code*/
                    sizeof(MBiCacheList) +      /* Cache structure*/
                    sizeof(u32) * 4 +  /* segment - CARD mapping*/
                    p->main_size +     /* ARM9 static segment*/
                    p->sub_size        /* ARM7 static segment*/
            );
        if (ret < MB_SEGMENT_BUFFER_MIN)
            ret = MB_SEGMENT_BUFFER_MIN;
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         MB_ReadSegment

  Description:  Reads the required segment data from the specified binary file.

  Arguments:    file : the FSFile structure that indicates the binary file
                       If NULL, uses itself. (Parent/Child shared binary)
                buf : The buffer that reads the segment data
				len    : the buf size
  
  Returns:      Returns TRUE if it properly reads segment data. Otherwise returns FALSE.
 *---------------------------------------------------------------------------*/

BOOL MB_ReadSegment(FSFile *file, void *buf, u32 len)
{
    BOOL    ret = FALSE;
    SDK_ASSERT(!file || FS_IsFile(file));

    /*
     * Cut the specified buffer by the following format.
     *
     * [RomHeader]
     * [AuthCode]
     * [CacheList]
     * [CardMapping]
     * [ARM9 static]
     * [ARM7 static]
     *
     * If the size of the last two segments is insufficient, you can get
     * around this by realizing virtual memory in cache format.
     */

    if (len >= ROM_HEADER_SIZE_FULL + 4)
    {

        BOOL    is_own_binary = FALSE;

        u32     rest = len;
        u8     *dst = (u8 *)buf;
        u32     bak_pos;
        FSFile  own_bin[1];
        u32     own_size;
        RomHeader *p_rom;

        MBiCacheList *p_cache = NULL;
        u32    *p_map = NULL;

        p_rom = (RomHeader *) dst;
        dst += ROM_HEADER_SIZE_FULL, rest -= ROM_HEADER_SIZE_FULL;

        /* Normal multiboot*/
        if (file)
        {
            bak_pos = FS_GetPosition(file);
            if (FS_ReadFile(file, p_rom, ROM_HEADER_SIZE_FULL) < ROM_HEADER_SIZE_FULL)
                rest = 0;
            own_size = p_rom->own_size;
            if (!own_size)
                own_size = 16 * 1024 * 1024;
        }
        /* Clone boot*/
        else
        {
            const RomHeader *mem_rom = (RomHeader *) HW_ROM_HEADER_BUF;
            own_size = mem_rom->own_size;
            if (!own_size)
                own_size = 16 * 1024 * 1024;
            is_own_binary = TRUE;
            /* Directly open file indicating self*/
            FS_InitFile(own_bin);
            (void)FS_OpenFileDirect(own_bin,
                                    FS_FindArchive("rom", 3), 0, (u32)(own_size + MB_AUTHCODE_SIZE),
                                    (u32)~0);
            file = own_bin;
            bak_pos = FS_GetPosition(file);
            /* For now, copy the ROM header part*/
            MI_CpuCopy8(mem_rom, p_rom, ROM_HEADER_SIZE_FULL);
            /* Correct the converted place*/
            *(u32 *)((u32)p_rom + 0x60) |= 0x00406000;
        }

        /* Read binary value of size MB_AUTHCODE_SIZE*/
        if (rest >= MB_AUTHCODE_SIZE)
        {
            (void)FS_SeekFile(file, (int)(bak_pos + own_size), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_AUTHCODE_SIZE);
            dst += MB_AUTHCODE_SIZE, rest -= MB_AUTHCODE_SIZE;
        }
        else
        {
            rest = 0;
        }

        /* Secure a cache list structure*/
        if (rest >= sizeof(MBiCacheList))
        {
            p_cache = (MBiCacheList *) dst;
            MBi_InitCache(p_cache);
            dst += sizeof(MBiCacheList), rest -= sizeof(MBiCacheList);
            /* Register ROM header in first cache*/
            MBi_AttachCacheBuffer(p_cache, 0, ROM_HEADER_SIZE_FULL, p_rom, MB_CACHE_STATE_LOCKED);
            /* Memorizes target archive name (specified when delayed load)*/
            {
                const FSArchive *const p_arc = FS_GetAttachedArchive(file);
                int     i = 0;
                while ((i < FS_ARCHIVE_NAME_LEN_MAX) && p_arc->name.ptr[i])
                    ++i;
                MI_CpuCopy8(p_arc->name.ptr, p_cache->arc_name, (u32)i);
                p_cache->arc_name_len = (u32)i;
            }
        }
        else
        {
            rest = 0;
        }

        /* Secure segment-CARD mapping region*/
        if (rest >= sizeof(u32) * 4)
        {
            p_map = (u32 *)dst;
            p_map[0] = 0;              /* ROM header*/
            p_map[1] = FS_GetFileImageTop(file) + bak_pos + p_rom->main_rom_offset;     /* ARM9 static*/
            p_map[2] = FS_GetFileImageTop(file) + bak_pos + p_rom->sub_rom_offset;      /* ARM7 static*/
            dst += sizeof(u32) * 4, rest -= sizeof(u32) * 4;
        }
        else
        {
            rest = 0;
        }

        /* Read each segment*/
        if (rest >= p_rom->main_size + p_rom->sub_size)
        {
            /* If there is enough room to read all at once*/
            const u32 base = FS_GetFileImageTop(file);
            (void)FS_SeekFile(file, (int)(p_map[1] - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, (int)p_rom->main_size);
            MBi_AttachCacheBuffer(p_cache, p_map[1], p_rom->main_size, dst, MB_CACHE_STATE_LOCKED);
            dst += p_rom->main_size, rest -= p_rom->main_size;
            (void)FS_SeekFile(file, (int)(p_map[2] - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, (int)p_rom->sub_size);
            MBi_AttachCacheBuffer(p_cache, p_map[2], p_rom->sub_size, dst, MB_CACHE_STATE_LOCKED);
            dst += p_rom->sub_size, rest -= p_rom->sub_size;
            /* Preparations complete*/
            ret = TRUE;
        }
        else if (rest >= MB_CACHE_PAGE_SIZE * 3)
        {
            /* If there is enough room to support 3-page cache method*/
            const u32 base = FS_GetFileImageTop(file);
            u32     offset = p_map[1];
            /* First page (fixed)*/
            (void)FS_SeekFile(file, (int)(offset - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_CACHE_PAGE_SIZE);
            MBi_AttachCacheBuffer(p_cache, offset, MB_CACHE_PAGE_SIZE, dst, MB_CACHE_STATE_LOCKED);
            dst += MB_CACHE_PAGE_SIZE, rest -= MB_CACHE_PAGE_SIZE;
            offset += MB_CACHE_PAGE_SIZE;
            /* Second page*/
            (void)FS_SeekFile(file, (int)(offset - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_CACHE_PAGE_SIZE);
            MBi_AttachCacheBuffer(p_cache, offset, MB_CACHE_PAGE_SIZE, dst, MB_CACHE_STATE_READY);
            dst += MB_CACHE_PAGE_SIZE, rest -= MB_CACHE_PAGE_SIZE;
            offset += MB_CACHE_PAGE_SIZE;
            /* Third page*/
            (void)FS_SeekFile(file, (int)(offset - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_CACHE_PAGE_SIZE);
            MBi_AttachCacheBuffer(p_cache, offset, MB_CACHE_PAGE_SIZE, dst, MB_CACHE_STATE_READY);
            dst += MB_CACHE_PAGE_SIZE, rest -= MB_CACHE_PAGE_SIZE;
            /* Preparations complete*/
            ret = TRUE;
        }

        /* Return to initial position*/
        (void)FS_SeekFile(file, (int)bak_pos, FS_SEEK_SET);

        /* Clone boot*/
        if (is_own_binary)
        {
            (void)FS_CloseFile(own_bin);
            if (ret)
            {
                const CARDRomRegion *p_header = mbi_seg_header;
                MBiSegmentHeaderInfo info;
                info.rom_src = p_rom->main_rom_offset;
                info.mem_src = (u32)((u32)p_rom->main_ram_address - p_rom->main_rom_offset);
                info.mem_dst = (u32)((u32)p_cache->list[1].ptr - p_rom->main_rom_offset);
                info.len = len;
                /* Initialize the first appropriate region*/
                MBi_ReadSegmentHeader(&info, MB_SEGMENT_HEADER_MIN, MB_SEGMENT_HEADER_MAX, TRUE);
                /* Sequentially transfer header*/
                for (; p_header->length != 0; ++p_header)
                {
                    MBi_ReadSegmentHeader(&info,
                                          p_header->offset,
                                          (u32)(p_header->offset + p_header->length), FALSE);
                }
                {
                    /* Restore the debugger's autoload patch process*/
                    extern u32 _start_AutoloadDoneCallback[1];
                    u8     *dst = (u8 *)p_cache->list[1].ptr;
                    dst += ((u32)&_start_AutoloadDoneCallback - (u32)p_rom->main_ram_address);
                    *(u32 *)dst = 0xE12FFF1E;   /* asm { bx, lr } (always code32)*/
                }
            }
        }

        if (ret)
        {
            DC_FlushRange(buf, len);
        }
    }

    return ret;
}

/*
 * load the header section of the segment
 */
static void MBi_ReadSegmentHeader(const MBiSegmentHeaderInfo * p, u32 top, u32 bottom, BOOL clean)
{
    /* Adjust the header range*/
    if (top < MB_SEGMENT_HEADER_MIN)
        top = MB_SEGMENT_HEADER_MIN;
    if (bottom > MB_SEGMENT_HEADER_MAX)
        bottom = MB_SEGMENT_HEADER_MAX;
    /* Adjust the actual transfer region*/
    if (top < p->rom_src)
        top = p->rom_src;
    if (bottom > p->rom_src + p->len)
        bottom = p->rom_src + p->len;
    /* Transfer if the region is valid*/
    if (top < bottom)
    {
        if (clean)
        {                              /* 0 Initialize*/
            MI_CpuClear8((void *)(p->mem_dst + top), (u32)(bottom - top));
        }
        else
        {                              /* Transfer*/
            MI_CpuCopy8((const void *)(p->mem_src + top),
                        (void *)(p->mem_dst + top), (u32)(bottom - top));
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         MB_RegisterFile

  Description:  Registers the download source file in the Parent Device.

  Arguments:    game_reg : the game registration information
                buf : the segment buffer already loaded by MB_ReadSegment
  
  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MB_RegisterFile(const MBGameRegistry *game_reg, const void *buf)
{
    MbDownloadFileInfoHeader *mdfi;
    static u8 update = 0;
    u8     *appname = (u8 *)game_reg->gameNamep;
    u8      fileID = 0xff, i;

    OSIntrMode enabled = OS_DisableInterrupts();        /* interrupts prohibited*/

    /* Return FALSE if MB has not been started*/
    if (!MBi_IsStarted())
    {
        OS_TWarning("MB has not started yet. Cannot Register File\n");
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return FALSE;
    }

    if (pPwork->file_num + 1 > MB_MAX_FILE)
    {
        OS_TWarning("The number of registered files has exceeded.\n");
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return FALSE;
    }

    /* Search for empty fileinfo*/
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        /* Return FALSE if this GameRegistry has been registered already*/
        if (pPwork->fileinfo[i].game_reg == (MBGameRegistry *)game_reg)
        {
            MB_DEBUG_OUTPUT("Already Registered \"%s\"\n", game_reg->gameNamep);
            (void)OS_RestoreInterrupts(enabled);        /* Cancels interrupts prohibition*/
            return FALSE;
        }

        if (pPwork->fileinfo[i].active == 0)
        {
            fileID = i;
            break;
        }
    }

    /* Abnormal end if empty fileinfo cannot be found*/
    if (i == MB_MAX_FILE)
    {
        OS_TWarning("Too many Files! \"%s\"\n", game_reg->gameNamep);
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return FALSE;
    }


    // register the GameRegistry pointer
    pPwork->fileinfo[fileID].game_reg = (MBGameRegistry *)game_reg;

    // set the pointer to the DownloadFileInfo
    mdfi = &pPwork->fileinfo[fileID].dl_fileinfo.header;

    // generate the MBDownloadFileInfo
    MBi_MakeDownloadFileInfo(mdfi, buf);
    /* Copy user-defined extended parameters*/
    MI_CpuCopy8(game_reg->userParam,
                ((MBDownloadFileInfo *) mdfi)->reserved, HW_DOWNLOAD_PARAMETER_SIZE);

    // Create block information table
    if (FALSE == MBi_MakeBlockInfoTable(&pPwork->fileinfo[fileID].blockinfo_table, mdfi))
    {
        OS_TWarning("Cannot make block information!\n");
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return FALSE;
    }

    // generate the MBGameInfo
    MBi_MakeGameInfo(&pPwork->fileinfo[fileID].game_info, game_reg, &pPwork->common.user);

    /* Set the file number in GameInfo*/
    pPwork->fileinfo[fileID].game_info.fileNo = fileID;

    /* Register GameInfo in the Beacon list */
    MB_AddGameInfo(&pPwork->fileinfo[fileID].game_info);

    /* Set the file update number in GameInfo*/
    pPwork->fileinfo[fileID].game_info.seqNoFixed = update++;

    pPwork->fileinfo[fileID].gameinfo_child_bmp = MB_GAMEINFO_PARENT_FLAG;

    pPwork->fileinfo[fileID].src_adr = (u32 *)buf;

    /* Save the various pointers for cache*/
    pPwork->fileinfo[fileID].cache_list = (MBiCacheList *)
        ((u32)buf + ROM_HEADER_SIZE_FULL + MB_AUTHCODE_SIZE);
    pPwork->fileinfo[fileID].card_mapping = (u32 *)
        ((u32)buf + ROM_HEADER_SIZE_FULL + MB_AUTHCODE_SIZE + sizeof(MBiCacheList));

    /* Start task thread if needed*/
    if (pPwork->fileinfo[fileID].cache_list->list[3].state != MB_CACHE_STATE_EMPTY)
    {
        if (!MBi_IsTaskAvailable())
        {
            MBi_InitTaskInfo(&pPwork->cur_task);
            MBi_InitTaskThread(pPwork->task_work, sizeof(pPwork->task_work));
        }
    }

    /* Make the registered file active */
    pPwork->fileinfo[fileID].active = 1;

    // increment the number of files
    pPwork->file_num++;

    MB_DEBUG_OUTPUT("Register Game \"%s\"\n", game_reg->gameNamep);

    (void)OS_RestoreInterrupts(enabled);        /* Cancels interrupts prohibition*/

    return TRUE;

}

/*---------------------------------------------------------------------------*
  Name:         MB_UnregisterFile

  Description:  Deletes the specified file from the download list.

  Arguments:    game_reg : the game registration information
  
  Returns:      
 *---------------------------------------------------------------------------*/

/* Deletes the specified file from the download list.*/
void   *MB_UnregisterFile(const MBGameRegistry *game_reg)
{
    u8      fileID, i;
    void   *ret_bufp;
    OSIntrMode enabled = OS_DisableInterrupts();        /* interrupts prohibited*/

    /* Search for fileinfo that matches GameRegistry*/
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        if (pPwork->fileinfo[i].game_reg == (MBGameRegistry *)game_reg)
        {
            fileID = i;
            break;
        }
    }

    if (fileID != pPwork->fileinfo[fileID].game_info.fileNo)
    {
        OS_TWarning("Registerd File ID does not correspond with File ID in Registry List.\n");
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return NULL;
    }

    /* Abnormal end if cannot find matching fileinfo*/
    if (i == MB_MAX_FILE)
    {
        OS_TWarning("Cannot find corresponding GameRegistry\n");
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return NULL;
    }

    /* Delete from GameInfo*/
    if (FALSE == MB_DeleteGameInfo(&pPwork->fileinfo[fileID].game_info))
    {
        OS_TWarning("Cannot delete GameInfo %s\n",
                    pPwork->fileinfo[fileID].game_info.fixed.gameName);
        (void)OS_RestoreInterrupts(enabled);    /* Cancels interrupts prohibition*/
        return NULL;
    }

    ret_bufp = pPwork->fileinfo[fileID].src_adr;

    pPwork->fileinfo[fileID].active = 0;

    /* Delete the target fileinfo*/
    MI_CpuClear16(&pPwork->fileinfo[fileID], sizeof(pPwork->fileinfo[0]));

    pPwork->file_num--;

    MB_DEBUG_OUTPUT("Delete Game \"%s\"\n", game_reg->gameNamep);

    (void)OS_RestoreInterrupts(enabled);        /* Cancels interrupts prohibition*/

    return ret_bufp;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_MakeDownloadFileInfo

  Description:  Builds the download file information from the designated multiboot game registration.

  Arguments:    mbdlFileInfop - the pointer to the MbDownloadFileInfo to be built
                buf           - the pointer to the file image buffer
  
  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_MakeDownloadFileInfo(MbDownloadFileInfoHeader * mbdlFileInfop, const void *buf)
{
    const RomHeader *mbRomHeaderp;
    const MbSegmentType *loadSegListp;
    MbSegmentInfo *seg_infop;
    int     seg_num;
    const u16 *auth_code;
    u32     child_recv_buff_addr = MB_ARM7_STATIC_RECV_BUFFER;

    mbRomHeaderp = (const RomHeader *)buf;
    auth_code = (const u16 *)((u32)buf + ROM_HEADER_SIZE_FULL);

    /*
       NOTE: maybe it would be good to check the ROM header and determine if it is a ROM binary? 
     */

    // build the initial download file information based on the multiboot game information
    mbdlFileInfop->arm9EntryAddr = (u32)mbRomHeaderp->main_entry_address;
    mbdlFileInfop->arm7EntryAddr = (u32)mbRomHeaderp->sub_entry_address;
    seg_infop = (MbSegmentInfo *) (mbdlFileInfop + 1);
    loadSegListp = MBi_defaultLoadSegList;
    MB_DEBUG_OUTPUT("\t<segment list>         recv_adr load_adr     size  rom_adr (base)\n");

    /*
     *  Delete all because we reached an agreement to make the segments to static.
     * overlay : impossible, since management for loading/unloading cannot be done.
     * file    : Impossible in terms of schedule because the specification such as capacity must be formulated and written as a reference.
     */
    for (seg_num = 0; seg_num < MB_DL_SEGMENT_NUM; seg_num++)
    {
        /* Process for getting each segment's load address and size information*/
        MB_DEBUG_OUTPUT("\t SEG%2d : ", seg_num);
        MBi_SetSegmentInfo(mbRomHeaderp, loadSegListp, seg_infop, &child_recv_buff_addr);
        seg_infop++;
        loadSegListp++;
    }

    /* Set authentication-use code in MBDownloadFileInfo*/
    {
        MBDownloadFileInfo *pMdfi = (MBDownloadFileInfo *) mbdlFileInfop;
        MI_CpuCopy8(auth_code, &pMdfi->auth_code[0], MB_AUTHCODE_SIZE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_SetSegmentInfo

  Description:  Sets the segment information.

  Arguments:    mbRomHeaderp
                loadSegListp
				seg_infop
				child_recv_buffer

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_SetSegmentInfo(const RomHeader * mbRomHeaderp,
                               const MbSegmentType * loadSegListp,
                               MbSegmentInfo * seg_infop, u32 *child_recv_buff_addr)
{
    CARDRomRegion *romRegp;

    /* Process for getting each segment's load address and size information*/

    switch (*loadSegListp)
    {
    case MB_SEG_ARM9STATIC:
        romRegp = (CARDRomRegion *)(&mbRomHeaderp->main_ram_address);

        if (((u32)romRegp->offset >= MB_LOAD_AREA_LO)
            && ((u32)romRegp->offset < MB_LOAD_AREA_HI)
            && (((u32)romRegp->offset + romRegp->length) <= MB_LOAD_AREA_HI))
        {

            seg_infop->size = romRegp->length;
            seg_infop->load_addr = (u32)romRegp->offset;
            /* Note: Considering data compression and uncompression,  load_addr == recv_addr should not be used in the future*/
            seg_infop->recv_addr = seg_infop->load_addr;
            seg_infop->target = MI_PROCESSOR_ARM9;
            MB_DEBUG_OUTPUT("arm9 static :");
        }
        else
        {
            // the address or the address + size of the load program exceeds the loadable area.
            OS_Panic("ARM9 boot code out of the load area. : addr = %x  size = %x\n",
                     (u32)romRegp->offset, romRegp->length);
        }
        break;

    case MB_SEG_ARM7STATIC:
        {
            BOOL    error_flag = FALSE;
            BOOL    reload_flag = FALSE;
            u32     load_last_addr;

            romRegp = (CARDRomRegion *)(&mbRomHeaderp->sub_ram_address);
            load_last_addr = (u32)((u32)romRegp->offset + romRegp->length);

            // placement address,  size determination
            /* For the address of the starting position (when placed in main memory)*/
            if (((u32)romRegp->offset >= MB_LOAD_AREA_LO)
                && ((u32)romRegp->offset < MB_BSSDESC_ADDRESS))
            {
                /* Regarding the end of the placement address*/
                if (load_last_addr <= MB_ARM7_STATIC_LOAD_AREA_HI)      // Set as is if relocation is not necessary
                {
                }
                else if ((load_last_addr < MB_BSSDESC_ADDRESS)
                         && (romRegp->length <= MB_ARM7_STATIC_RECV_BUFFER_SIZE))
                {                      // in that way if there is a need to relocate
                    reload_flag = TRUE;
                }
                else
                {
                    error_flag = TRUE;
                }
            }
            else
                /* For the address of the starting position (when placed in WRAM)*/
            if (((u32)romRegp->offset >= HW_WRAM)
                    && ((u32)romRegp->offset < HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE))
            {

                if (load_last_addr <= (HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE))
                {                      // decide to relocate.
                    reload_flag = TRUE;
                }
                else
                {
                    error_flag = TRUE;
                }
            }
            else
            {                          // an error when in regions other than this
                error_flag = TRUE;
            }

            // error determination
            if (error_flag == TRUE)
            {
                OS_Panic("ARM7 boot code out of the load area. : addr = %x  size = %x\n",
                         (u32)romRegp->offset, romRegp->length);
            }

            // segment information set
            {
                seg_infop->size = romRegp->length;
                seg_infop->load_addr = (u32)romRegp->offset;
                if (!reload_flag)
                {
                    seg_infop->recv_addr = seg_infop->load_addr;
                }
                else
                {
                    /*
                     * If the execution address on ARM7 is a region that needs to be relocated,
                     * set the receive address to the main memory receive buffer.
                     */
                    seg_infop->recv_addr = *child_recv_buff_addr;
                    *child_recv_buff_addr += seg_infop->size;
                }
            }

            seg_infop->target = MI_PROCESSOR_ARM7;
            MB_DEBUG_OUTPUT("arm7 static :");
        }
        break;

    case MB_SEG_ROMHEADER:
        seg_infop->size = ROM_HEADER_SIZE_FULL;
        seg_infop->load_addr = (u32)MB_ROM_HEADER_ADDRESS;
        seg_infop->recv_addr = seg_infop->load_addr;
        // target for ROMHEADER is MI_PROCESSOR_ARM9
        seg_infop->target = MI_PROCESSOR_ARM9;
        MB_DEBUG_OUTPUT("rom header  :");
        break;
    }

    MB_DEBUG_OUTPUT(" %8x %8x %8x %8x\n",
                    seg_infop->recv_addr, seg_infop->load_addr,
                    seg_infop->size,
                    (*loadSegListp == MB_SEG_ROMHEADER) ? 0 : *((u32 *)romRegp - 2));

}

/*	----------------------------------------------------------------------------

	Block information control function (for Parent)

	----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MBi_MakeBlockInfoTable

  Description:  Creates a table used to get the block information

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/


BOOL MBi_MakeBlockInfoTable(MB_BlockInfoTable * table, MbDownloadFileInfoHeader * mdfi)
{
    u16    *seg_headB = table->seg_head_blockno;
    u32    *seg_src = table->seg_src_offset;
    u8      i;
    u32     src_ofs = 0;

    if (!mdfi)
        return FALSE;

    for (i = 0; i < MB_DL_SEGMENT_NUM; ++i)
    {
        MbSegmentInfo *si = MBi_GetSegmentInfo(mdfi, i);
        seg_src[i] = src_ofs;
        src_ofs += si->size;
    }

    seg_headB[0] = 0;                  // Segment 0

    for (i = 0; i < MB_DL_SEGMENT_NUM; ++i)
    {
        MbSegmentInfo *si = MBi_GetSegmentInfo(mdfi, i);
        u16     next_block_head =
            (u16)(seg_headB[i] + (u16)((si->size + mbc->block_size_max - 1) / mbc->block_size_max));

        if (FALSE == IsAbleToLoad(i, si->load_addr, si->size))
        {
            return FALSE;
        }

        if (i < MB_DL_SEGMENT_NUM - 1)
        {
            seg_headB[i + 1] = next_block_head;
        }
        else
        {
            table->block_num = next_block_head;
        }

    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_get_blockinfo

  Description:  Gets the block information

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MBi_get_blockinfo(MB_BlockInfo * bi, MB_BlockInfoTable * table,
                       u32 block, MbDownloadFileInfoHeader * mdfi)
{
    s8      i;
    u32     seg_block, block_adr_offset;

    if (block >= table->block_num)
    {
        return FALSE;
    }

    for (i = MB_DL_SEGMENT_NUM - 1; i >= 0; i--)
    {
        if (block >= table->seg_head_blockno[i])
        {
            break;
        }
    }

    if (i < 0)
        return FALSE;

    seg_block = block - table->seg_head_blockno[i];
    block_adr_offset = seg_block * mbc->block_size_max;

    {
        MbSegmentInfo *si = MBi_GetSegmentInfo(mdfi, i);
        bi->size = si->size - block_adr_offset;
        if (bi->size > mbc->block_size_max)
            bi->size = (u32)mbc->block_size_max;
        bi->offset = (u32)(block_adr_offset + table->seg_src_offset[i]);
        bi->child_address = (u32)(block_adr_offset + (u32)(si->recv_addr));
        bi->segment_no = (u8)i;
    }

    MB_DEBUG_OUTPUT("blockNo:%d \n", block);
    MB_DEBUG_OUTPUT("Segment %d [block:%d offset(%08x) destination(%08x) size(%04x)]\n",
                    i, seg_block, bi->offset, bi->child_address, bi->size);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_get_blocknum

  Description:  Gets the total number of blocks.

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

u16 MBi_get_blocknum(MB_BlockInfoTable * table)
{
    SDK_ASSERT(table != 0);
    return table->block_num;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_IsAbleToRecv

  Description:  Check for validity of the receive address

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MBi_IsAbleToRecv(u8 segment_no, u32 address, u32 size)
{
    MbSegmentType seg_type;

    SDK_ASSERT(segment_no < MB_DL_SEGMENT_NUM);

    seg_type = MBi_defaultLoadSegList[segment_no];

    switch (seg_type)
    {
    case MB_SEG_ROMHEADER:
        if (address >= MB_ROM_HEADER_ADDRESS
            && address + size <= MB_ROM_HEADER_ADDRESS + ROM_HEADER_SIZE_FULL)
        {
            return TRUE;
        }
        break;

    case MB_SEG_ARM9STATIC:
        if (address >= MB_LOAD_AREA_LO && address + size <= MB_LOAD_AREA_HI)
        {
            return TRUE;
        }
        break;

    case MB_SEG_ARM7STATIC:
        /* wram mapping*/
        if (address >= MB_ARM7_STATIC_RECV_BUFFER
            && address + size <= MB_ARM7_STATIC_RECV_BUFFER_END)
        {
            return TRUE;
        }
        else
            /* main memory mapping*/
        if (address >= MB_LOAD_AREA_LO && address + size <= MB_ARM7_STATIC_RECV_BUFFER_END)
        {
            return TRUE;
        }
        break;

    default:
        return FALSE;
    }

    return FALSE;
}

static BOOL IsAbleToLoad(u8 segment_no, u32 address, u32 size)
{
    MbSegmentType seg_type;

    SDK_ASSERT(segment_no < MB_DL_SEGMENT_NUM);

    seg_type = MBi_defaultLoadSegList[segment_no];

    switch (seg_type)
    {
    case MB_SEG_ROMHEADER:
    case MB_SEG_ARM9STATIC:
        return MBi_IsAbleToRecv(segment_no, address, size);

    case MB_SEG_ARM7STATIC:
        /* main memory*/
        if (address >= MB_LOAD_AREA_LO && address < MB_BSSDESC_ADDRESS)
        {
            u32     end_adr = address + size;

            if (MB_ARM7_STATIC_RECV_BUFFER_END > address
                && MB_ARM7_STATIC_RECV_BUFFER_END < end_adr)

            {
                return FALSE;          // NG
            }
            else if (end_adr <= MB_ARM7_STATIC_LOAD_AREA_HI)
            {
                return TRUE;           // OK
            }
            else if (end_adr < MB_BSSDESC_ADDRESS && size <= MB_ARM7_STATIC_RECV_BUFFER_SIZE)
            {
                return TRUE;           // OK (needs relocate)
            }
            else
            {
                return FALSE;          // NG
            }
        }
        else
            /* wram*/
        if (address >= HW_WRAM && address < HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE)
        {
            u32     end_adr = address + size;
            if (end_adr <= (HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE))
            {
                return TRUE;           // OK
            }
            else
            {
                return FALSE;          // NG
            }
        }
        break;

    default:
        return FALSE;
    }

    return FALSE;
}
