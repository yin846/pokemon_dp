/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     ownerInfo.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_ownerInfo.c,v $
  Revision 1.11  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.10  04/26/2005 07:59:56  terui
  Fix comment

  Revision 1.9  04/18/2005 12:27:31  terui
  Deleted OSi_CheckOwnerInfo function

  Revision 1.8  04/12/2005 10:49:15  terui
  Made alterations to support Chinese language

  Revision 1.7  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.6  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.5  10/07/2004 10:19:27  yosizaki
  add favorite color API.

  Revision 1.4  2004/09/13 05:49:56  yasu
  merged with NitroSDK_2_00pr6_branch

  Revision 1.3.2.1  2004/09/13 05:43:02  yasu
  Support SDK_NVRAM_FORMAT

  Revision 1.3  09/03/2004 08:01:20  terui
  Added the OS_GetOwnerRtcOffset function.

  Revision 1.2  09/03/2004 04:29:15  terui
  Revised according to structure definition adjustments relating to user information in NVRAM.

  Revision 1.1  09/02/2004 06:21:20  terui
  Initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/os/common/ownerInfo.h>


/*---------------------------------------------------------------------------*
  Name:         OS_GetMacAddress

  Description:  Gets the MAC address of the wireless hardware.

  Arguments:    macAddress  -   Pointer to the buffer getting the MAC address.
                                Six bytes of data get written to this buffer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_GetMacAddress(u8 *macAddress)
{
    u8     *src;

    src =
        (u8 *)((u32)(OS_GetSystemWork()->nvramUserInfo) +
               ((sizeof(NVRAMConfig) + 3) & ~0x00000003));
    MI_CpuCopy8(src, macAddress, 6);
}

#ifdef  SDK_TS
#if ( SDK_TS_VERSION >= 200 || SDK_NVRAM_FORMAT >= 100 )

/*---------------------------------------------------------------------------*
  Name:         OS_GetOwnerInfo

  Description:  Gets the owner information.

  Arguments:    info    -   Pointer to the buffer getting the owner information.
                            Data gets written to this buffer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_GetOwnerInfo(OSOwnerInfo *info)
{
    NVRAMConfig *src;

    src = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);
    info->language = (u8)(src->ncd.option.language);
    info->favoriteColor = (u8)(src->ncd.owner.favoriteColor);
    info->birthday.month = (u8)(src->ncd.owner.birthday.month);
    info->birthday.day = (u8)(src->ncd.owner.birthday.day);
    info->nickNameLength = (u16)(src->ncd.owner.nickname.length);
    info->commentLength = (u16)(src->ncd.owner.comment.length);
    MI_CpuCopy16(src->ncd.owner.nickname.str,
                 info->nickName, OS_OWNERINFO_NICKNAME_MAX * sizeof(u16));
    MI_CpuCopy16(src->ncd.owner.comment.str, info->comment, OS_OWNERINFO_COMMENT_MAX * sizeof(u16));
}

#endif

/*---------------------------------------------------------------------------*
  Name:         OS_GetOwnerRtcOffset

  Description:  Gets the offset (in units of seconds) to use to increase
                or decrease the value every time the owner changes the RTC setting.
                Time gets added if RTC is set to a time in the future relative to
                the RTC prior to being reset. Time gets subtracted if the RTC is set back in the past relative to the RTC prior to being reset.

  Arguments:    None.

  Returns:      s64 -   Returns the offset value for RTC settings.
 *---------------------------------------------------------------------------*/
s64 OS_GetOwnerRtcOffset(void)
{
    NVRAMConfig *src = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);

#if ( SDK_TS_VERSION >= 200 || SDK_NVRAM_FORMAT >= 100 )
    return src->ncd.option.rtcOffset;
#else
    return src->ncd.rtcOffset;
#endif
}

#endif

/*---------------------------------------------------------------------------*
  Name:         OS_GetFavoriteColorTable

  Description:  Obtains a pointer to the "Favorite Color" array that corresponds to the favoriteColor
                member of the OSOwnerInfo structure.

  Arguments:    None.

  Returns:      Pointer to the GXRg type constant array that stores corresponding color value.
 *---------------------------------------------------------------------------*/
const GXRgb *OS_GetFavoriteColorTable(void)
{
    static const GXRgb col_table[OS_FAVORITE_COLOR_MAX] = {
        OS_FAVORITE_COLOR_VALUE_GRAY,
        OS_FAVORITE_COLOR_VALUE_BROWN,
        OS_FAVORITE_COLOR_VALUE_RED,
        OS_FAVORITE_COLOR_VALUE_PINK,
        OS_FAVORITE_COLOR_VALUE_ORANGE,
        OS_FAVORITE_COLOR_VALUE_YELLOW,
        OS_FAVORITE_COLOR_VALUE_LIME_GREEN,
        OS_FAVORITE_COLOR_VALUE_GREEN,
        OS_FAVORITE_COLOR_VALUE_DARK_GREEN,
        OS_FAVORITE_COLOR_VALUE_SEA_GREEN,
        OS_FAVORITE_COLOR_VALUE_TURQUOISE,
        OS_FAVORITE_COLOR_VALUE_BLUE,
        OS_FAVORITE_COLOR_VALUE_DARK_BLUE,
        OS_FAVORITE_COLOR_VALUE_PURPLE,
        OS_FAVORITE_COLOR_VALUE_VIOLET,
        OS_FAVORITE_COLOR_VALUE_MAGENTA,
    };
    return col_table;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
