/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CP - src
  File:     context.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: cp_context.c,v $
  Revision 1.5  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.4  2005/01/12 11:08:24  takano_makoto
  fix copyright header.

  Revision 1.3  2005/01/12 00:28:41  yada
  consider SDK_TCM_APPLY

  Revision 1.2  2004/05/06 10:46:43  takano_makoto
  Modify include file.

  Revision 1.1  2004/05/06 02:52:40  takano_makoto
  Initial Update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/cp/context.h>

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_begin.h>
#endif
/*---------------------------------------------------------------------------*
  Name:         CP_SaveContext

  Description:  Save current context into specified memory

  Arguments:    context   pointer to the memory to be stored the current context

  Returns:      None
 *---------------------------------------------------------------------------*/
void CP_SaveContext(CPContext *context)
{
    context->div_numer = reg_CP_DIV_NUMER;
    context->div_denom = reg_CP_DIV_DENOM;
    context->div_mode = (u16)(reg_CP_DIVCNT & REG_CP_DIVCNT_MODE_MASK);
    context->sqrt = reg_CP_SQRT_PARAM;
    context->sqrt_mode = (u16)(reg_CP_SQRTCNT & REG_CP_SQRTCNT_MODE_MASK);
}


/*---------------------------------------------------------------------------*
  Name:         CP_RestoreContext

  Description:  Reload specified context as current context

  Arguments:    context   pointer to the memory to switch to the context

  Returns:      None
 *---------------------------------------------------------------------------*/
void CP_RestoreContext(CPContext *context)
{
    reg_CP_DIV_NUMER = context->div_numer;
    reg_CP_DIV_DENOM = context->div_denom;
    reg_CP_DIVCNT = context->div_mode;
    reg_CP_SQRT_PARAM = context->sqrt;
    reg_CP_SQRTCNT = context->sqrt_mode;

}

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_end.h>
#endif
