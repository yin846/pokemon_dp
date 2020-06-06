/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PM - include - common
  File:     pm_common.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: pm_common.h,v $
  Revision 1.18  10/26/2005 11:11:11 AM  yada
  add definition for PMIC_BL_CTL register

  Revision 1.17  02/28/2005 05:26:03 AM  yosizaki
  do-indent.

  Revision 1.16  10/04/2004 11:23:01 AM  yada
  treat backlight status in sleeping, and recover

  Revision 1.15  09/30/2004 11:10:45 AM  yada
  not change LED status in switching LCD power

  Revision 1.14  09/10/2004 09:38:44 AM  yada
  add PM_LED_PATTERN_WIRELESS

  Revision 1.13  09/09/2004 09:54:27 AM  yada
  add blink pattern

  Revision 1.12  08/26/2004 11:41:36 AM  yada
  change order of LED pattern definition

  Revision 1.11  08/26/2004 08:11:58 AM  yada
  add LED definition

  Revision 1.10  08/24/2004 02:44:08 AM  ooe
  Added LED flashing pattern.

  Revision 1.9  08/23/2004 08:07:51 AM  yada
  add LED self blink

  Revision 1.8  08/07/2004 07:02:46 AM  yada
  PM_KEY_LOGIC_* -> PM_PAD_LOGIC_*

  Revision 1.7  08/06/2004 12:41:11 PM  yada
  add key logic definition

  Revision 1.6  08/04/2004 12:44:43 AM  yada
  PM_SetBackLightTop/Bottom -> PM_SetBackLight

  Revision 1.5  08/02/2004 12:54:07 PM  yada
  change wake up factor name

  Revision 1.4  08/02/2004 06:15:35 AM  yada
  add WL_INTR

  Revision 1.3  08/02/2004 12:37:05 AM  yada
  push out some defines to pm.h

  Revision 1.2  07/31/2004 01:31:24 AM  yada
  add parameters about PM_SetAmp, PM_SetAmpGain, PM_SetVDETVoltage

  Revision 1.1  07/29/2004 09:00:50 AM  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_PM_COMMON_H_
#define NITRO_PM_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#ifdef SDK_ARM9
#include <nitro/hw/ARM9/ioreg_PAD.h>
#else  // SDK_ARM9
#include <nitro/hw/ARM7/ioreg_PAD.h>
#endif // SDK_ARM9

//================================================================
//    PMIC register parameter
//================================================================
//---------------- address
#define REG_PMIC_CTL_ADDR        0     // R/W
#define REG_PMIC_STAT_ADDR       1     // R
#define REG_PMIC_OP_CTL_ADDR     2     // R/W
#define REG_PMIC_PGA_GAIN_ADDR   3     // R/W
#define REG_PMIC_BL_CTL_ADDR     4     // R/W(bit0-3), R(bit4,5,7)
#define PMIC_REG_NUMS            5     // nums of register

//---------------- each register spec
//---- PMIC_CTL
#define PMIC_CTL_SND_PWR        (1<< 0)
#define PMIC_CTL_SND_VOLCTRL    (1<< 1)
#define PMIC_CTL_BKLT1          (1<< 2)
#define PMIC_CTL_BKLT2          (1<< 3)
#define PMIC_CTL_LED_SW         (1<< 4)
#define PMIC_CTL_LED_SP         (1<< 5)
#define PMIC_CTL_PWR_OFF        (1<< 6)

//---- PMIC_STAT
#define PMIC_STAT_VDET          (1<< 0)

//---- PMIC_OP_CTL
#define PMIC_OP_CTL             (1<< 0)

//---- PMIC_PGA_GAIN
#define PMIC_PGA_GAIN_SHIFT     0
#define PMIC_PGA_GAIN_MASK      (3<< PMIC_PGA_GAIN_SHIFT)

//---- PMIC_BL_CTL
#define PMIC_BL_CTL_BL_SHIFT    0
#define PMIC_BL_CTL_BL_MASK     (3<< PMIC_BL_CTL_BL_SHIFT)
#define PMIC_BL_CTL_ADPT_SW     (1<< 2)
#define PMIC_BL_CTL_ADPT_DETECT (1<< 3)
#define PMIC_BL_CTL_VERSION     (1<< 6)


//---------------- PMIC register access spec
#define PMIC_REG_READ           1
#define PMIC_REG_WRITE          0
#define PMIC_REG_OP_SHIFT       7
#define PMIC_REG_OP_MASK        1

//================================================================
//    UTILITY command to access PMIC
//================================================================
//---- pm ulitity proc number
enum
{
    PM_UTIL_DUMMY = 0,
    PM_UTIL_LED_ON,
    PM_UTIL_LED_BLINK_HIGH_SPEED,
    PM_UTIL_LED_BLINK_LOW_SPEED,
    PM_UTIL_LCD1_BACKLIGHT_ON,
    PM_UTIL_LCD1_BACKLIGHT_OFF,
    PM_UTIL_LCD2_BACKLIGHT_ON,
    PM_UTIL_LCD2_BACKLIGHT_OFF,
    PM_UTIL_LCD12_BACKLIGHT_ON,
    PM_UTIL_LCD12_BACKLIGHT_OFF,
    PM_UTIL_SOUND_POWER_ON,
    PM_UTIL_SOUND_POWER_OFF,
    PM_UTIL_SOUND_VOL_CTRL_ON,
    PM_UTIL_SOUND_VOL_CTRL_OFF,
    PM_UTIL_FORCE_POWER_OFF,
    PM_UTIL_FORCE_POWER_ON
};

//================================================================
//    BAUDRATE parameter
//================================================================
#define PM_BAUDRATE_4MHZ          0
#define PM_BAUDRATE_2MHZ          1
#define PM_BAUDRATE_1MHZ          2
#define PM_BAUDRATE_512KHZ        3

//---- default
#define PM_BAUDRATE_PMIC_DEFAULT  PM_BAUDRATE_1MHZ

//================================================================
//    SLEEP parameter
//================================================================
//---- factors to wake up from sleep mode
#define PM_TRIGGER_KEY              (1 << 0)
#define PM_TRIGGER_RTC_ALARM        (1 << 1)
#define PM_TRIGGER_COVER_OPEN       (1 << 2)
#define PM_TRIGGER_CARD             (1 << 3)
#define PM_TRIGGER_CARTRIDGE        (1 << 4)
typedef u32 PMWakeUpTrigger;

//---- logic for key interrupt
#define PM_PAD_LOGIC_OR    (0 << REG_PAD_KEYCNT_LOGIC_SHIFT)
#define PM_PAD_LOGIC_AND   (1 << REG_PAD_KEYCNT_LOGIC_SHIFT)
typedef u32 PMLogic;

//---- backlight to recover from sleep  (uses in library)
#define PM_BACKLIGHT_RECOVER_TOP_SHIFT      5
#define PM_BACKLIGHT_RECOVER_BOTTOM_SHIFT   6
#define PM_BACKLIGHT_RECOVER_TOP_ON      (1<<PM_BACKLIGHT_RECOVER_TOP_SHIFT)
#define PM_BACKLIGHT_RECOVER_TOP_OFF     (0<<PM_BACKLIGHT_RECOVER_TOP_SHIFT)
#define PM_BACKLIGHT_RECOVER_BOTTOM_ON   (1<<PM_BACKLIGHT_RECOVER_BOTTOM_SHIFT)
#define PM_BACKLIGHT_RECOVER_BOTTOM_OFF  (0<<PM_BACKLIGHT_RECOVER_BOTTOM_SHIFT)

//================================================================
//     LED pattern parameter
//================================================================
//---- LED blink pattern
typedef enum
{
    PM_LED_PATTERN_NONE = 0,           // don't care
    PM_LED_PATTERN_ON = 1,
    PM_LED_PATTERN_BLINK_LOW = 2,
    PM_LED_PATTERN_BLINK_HIGH = 3,
    PM_LED_PATTERN_BLINK1 = 4,
    PM_LED_PATTERN_BLINK2 = 5,
    PM_LED_PATTERN_BLINK3 = 6,
    PM_LED_PATTERN_BLINK4 = 7,
    PM_LED_PATTERN_BLINK5 = 8,
    PM_LED_PATTERN_BLINK6 = 9,
    PM_LED_PATTERN_BLINK8 = 10,
    PM_LED_PATTERN_BLINK10 = 11,
    PM_LED_PATTERN_PATTERN1 = 12,
    PM_LED_PATTERN_PATTERN2 = 13,
    PM_LED_PATTERN_PATTERN3 = 14,
    PM_LED_PATTERN_WIRELESS = 15
}
PMLEDPattern;

#define PM_LED_PATTERN_MAX  PM_LED_PATTERN_WIRELESS


//---- LED status
typedef enum
{
    PM_LED_NONE = 0,                   // dummy
    PM_LED_ON = 1,
    PM_LED_BLINK_LOW = 2,
    PM_LED_BLINK_HIGH = 3
}
PMLEDStatus;

//================================================================
//     PXI format
//================================================================
//---- command
#define PM_COMMAND_SHIFT           22
#define PM_COMMAND_MASK            0x3c00000
//---- address
#define PM_REG_OP_ADDR_SHIFT       16
#define PM_REG_OP_ADDR_MASK        0x3f0000
//---- data
#define PM_REG_OP_DATA_SHIFT       0
#define PM_REG_OP_DATA_MASK        0xffff


//================================================================
//---- status of folding
#define   PM_READING               -1

#define   PM_SUCCESS               0
#define   PM_BUSY                  1
#define   PM_INVALID_COMMAND       0xffff

#define   PM_RESULT_SUCCESS        0
#define   PM_RESULT_BUSY           1
#define   PM_RESULT_ERROR          2
//================================================================


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_PM_COMMON_H_ */
#endif
