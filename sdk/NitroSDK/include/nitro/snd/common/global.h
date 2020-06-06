/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - include
  File:     global.h

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: global.h,v $
  Revision 1.7  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.6  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.5  2004/10/15 15:17:55  yasu
  sound patch for NITRO-System 10/12

  Revision 1.5  10/04/2004 02:20:45  ida
  add SND_Shutdown

  Revision 1.4  08/19/2004 06:43:40  ida
  for sleep mode

  Revision 1.3  07/05/2004 06:33:51  ida
  change symbol name

  Revision 1.2  06/28/2004 07:16:43  ida
  fix wrong symbol name

  Revision 1.1  06/23/2004 06:34:03  ida
  hwcore.h -> global.h

  Revision 1.2  04/08/2004 07:29:02  ida
  fix Project name

  Revision 1.1  03/26/2004 06:26:43  ida
  add SND-SDK header files

  Revision 1.1  03/08/2004 05:40:33  ida
  rename files

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_SND_COMMON_GLOBAL_H_
#define NITRO_SND_COMMON_GLOBAL_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	macro definition
 ******************************************************************************/

#define SND_MASTER_VOLUME_MAX        0x7f

/******************************************************************************
	enum definition
 ******************************************************************************/

typedef enum SNDChannelOut
{
    SND_CHANNEL_OUT_MIXER,
    SND_CHANNEL_OUT_BYPASS
}
SNDChannelOut;

typedef enum SNDOutput
{
    SND_OUTPUT_MIXER,
    SND_OUTPUT_CHANNEL1,
    SND_OUTPUT_CHANNEL3,
    SND_OUTPUT_CHANNEL1_3
}
SNDOutput;

/******************************************************************************
	public function declaration
 ******************************************************************************/

#ifdef SDK_ARM7

void    SND_Enable(void);
void    SND_Disable(void);

void    SND_Shutdown(void);

void    SND_BeginSleep(void);
void    SND_EndSleep(void);

void    SND_SetMasterVolume(int volume);

void    SND_SetOutputSelector(SNDOutput left,
                              SNDOutput right, SNDChannelOut channel1, SNDChannelOut channel3);

#endif /* SDK_ARM7*/

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NITRO_SND_COMMON_GLOBAL_H_ */
