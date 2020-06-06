/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makerom
  File:     defval.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: defval.h,v $
  Revision 1.6  03/29/2006 13:13:22 AM  yasu
  IF-ELSE-ENDIF support

  Revision 1.5  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.3  08/05/2004 13:38:44 AM  yasu
  Support -M option

  Revision 1.2  07/10/2004 04:06:17 AM  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.1  03/26/2004 05:07:33 AM  yasu
  support variables like as -DNAME=VALUE

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef	 DEFVAL_H_
#define	 DEFVAL_H_

#define	DEFVAL_DEFAULT_BUFFER_SIZE	(1024)

BOOL AddDefValFromFile(char *filename);
void AddDefVal(char *opt);
const char *SearchDefVal(const char *name);
const char *SearchDefValCleaned(const char *name);
char *ResolveDefVal(char *str);
char *ResolveStringModifier(const char *in_value, char modifier);

#endif //DEFVAL_H_
