/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     exception.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: exception.h,v $
  Revision 1.12  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.11  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.10  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.9  04/07/2004 02:03:17  yada
  fix header comment

  Revision 1.8  02/19/2004 08:44:21  yada
  Added user exception routine type.

  Revision 1.7  02/05/2004 07:27:19  yada
  historic IRIS character string returned from NITRO to IRIS.   

  Revision 1.6  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  01/30/2004 04:31:56  yada
  Built in exception display.   

  Revision 1.4  2004/01/16 01:17:55  yasu
  Support SDK_BB

  Revision 1.3  01/15/2004 02:40:01  yada
  changed IRIS_BB  to -> SDK_BB

  Revision 1.2  01/14/2004 12:06:08  yada
  Minor Revisions

  Revision 1.1  12/08/2003 12:22:34  yada
  Initial release.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_EXCEPTION_H_
#define NITRO_OS_EXCEPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>

//--------------------------------------------------------------------------------
//---- Exception Handler
typedef void (*OSExceptionHandler) (u32, void *);


//--------------------------------------------------------------------------------
/******************************** Exception Vector ********************************/
/*---------------------------------------------------------------------------*
  Name:         OS_SetExceptionVectorUpper

  Description:  set exception vector to hi-address

  Arguments:    none

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_SetExceptionVectorUpper(void);

/*---------------------------------------------------------------------------*
  Name:         OS_SetExceptionVectorLower

  Description:  set exception vector to low-address

  Arguments:    none

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_SetExceptionVectorLower(void);




/******************************** Exception Handler ********************************/
/*---------------------------------------------------------------------------*
  Name:         OS_InitException

  Description:  initialize exception system of sdk os.
                should be called once at first.

  Arguments:    none

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_InitException(void);



/*---------------------------------------------------------------------------*
  Name:         OS_SetUserExceptionHandler

  Description:  set user exception handler

  Arguments:    handler     user routine when exception occurred
                arg         argument in call user handler

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_SetUserExceptionHandler(OSExceptionHandler handler, void *arg);




#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_OS_EXCEPTION_H_ */
#endif
