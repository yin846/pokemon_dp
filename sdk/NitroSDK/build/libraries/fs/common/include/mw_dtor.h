/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     mw_dtor.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_MW_DTOR_H_)
#define NITRO_FS_MW_DTOR_H_

#include <nitro/misc.h>
#include <nitro/types.h>


/* CodeWarrior compiler's global destructor chain operation*/


#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* declaration*/

/*
 * Destructor callback.
 * The first argument is an object pointer equivalent to "this".
 */
typedef void (*MWI_DESTRUCTOR_FUNC) (void *);

/*
 * Destructor chain.
 * Global object provides one for each.
 */
typedef struct MWiDestructorChain
{
    struct MWiDestructorChain *next;   /* next chain*/
    MWI_DESTRUCTOR_FUNC dtor;          /* destructor*/
    void   *obj;                       /* this*/
}
MWiDestructorChain;

/*****************************************************************************/
/* variable*/

/*
 * Global destructor chain.
 * Added at the top, so that they will be called in the reverse order of ctor.
 */
extern MWiDestructorChain *__global_destructor_chain;


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         __register_global_object

  Description:  Processing-related definition function.
                Insert one chain at the start of the global destructor chain

  Arguments:    obj              Pointer to the object to destroy.
                dtor             Destructor routine.
                chain            Pointer to the chain structure.
                                 (Global object provides one for each.)

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    __register_global_object(void *obj, MWI_DESTRUCTOR_FUNC dtor, MWiDestructorChain * chain);
/*
{
	chain->next = __global_destructor_chain;
	chain->dtor = dtor;
	chain->obj = obj;
	__global_destructor_chain = chain;
}
*/


/*****************************************************************************/


#ifdef __cplusplus
} /* extern "C"*/
#endif


#endif /* NITRO_FS_UTIL_H_*/

/*---------------------------------------------------------------------------*
  $Log: mw_dtor.h,v $
  Revision 1.5  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  05/30/2005 04:13:39  yosizaki
  add comments.

  Revision 1.3  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.2  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.1  07/08/2004 13:40:41  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
