/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CP - include
  File:     context.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: context.h,v $
  Revision 1.7  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.6  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.5  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.4  09/02/2004 07:45:26  yada
  only change comment

  Revision 1.3  2004/06/02 13:11:21  yasu
  Fix ifdef conditions with SDK_CP_NO_SAFE

  Revision 1.2  05/06/2004 10:45:13  takano_makoto
  Modify include file.

  Revision 1.1  05/06/2004 02:55:23  takano_makoto
  Initial Update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CP_CONTEXT_H_
#define NITRO_CP_CONTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/ioreg.h>


//---- cp context thread safe policy
// uncomment the following line and make SDK_CP_NO_SAFE valid,
// if want for divider and sqrt operator to be not thread safe.
//
//#define SDK_CP_NO_SAFE

//----------------------------------------------------------------
typedef struct CPContext
{
    u64     div_numer;
    u64     div_denom;
    u64     sqrt;
    u16     div_mode;
    u16     sqrt_mode;
}
CPContext;

/*---------------------------------------------------------------------------*
  Name:         CP_SaveContext

  Description:  Save current context into specified memory

  Arguments:    context   pointer to the memory to be stored the current context

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CP_SaveContext(CPContext *context);


/*---------------------------------------------------------------------------*
  Name:         CP_RestoreContext

  Description:  Reload specified context as current context

  Arguments:    context   pointer to the memory to switch to the context

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CP_RestoreContext(CPContext *context);


#ifdef __cplusplus
} /* extern "C"*/
#endif


/* NITRO_CP_CONTEXT_H_ */
#endif
