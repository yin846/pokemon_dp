/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Foundation
  File:     list.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: list.h,v $
  Revision 1.5  01/21/2005 04:59:49  kitamurs
  Changed year

  Revision 1.4  10/15/2004 06:13:25  nishida
  Revised comments.

  Revision 1.3  02/12/2004 10:42:27  nishida
  added include for nitro/types.h

  Revision 1.2  02/02/2004 01:02:18  nishida
  added include for stddef.h

  Revision 1.1  01/30/2004 03:38:15  nishida
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_LIST_H_
#define NNS_FND_LIST_H_

#include <stddef.h>
#include <nitro/types.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         NNSFndLink

  Description:  Node structure for two-way link list. This structure is stored as a member of the structure to be linked.
                 
 *---------------------------------------------------------------------------*/
typedef struct
{
    void*       prevObject;     // Pointer to the previous linked object
    void*       nextObject;     // Pointer to the next linked object

} NNSFndLink;


/*---------------------------------------------------------------------------*
  Name:         NNSFndList

  Description:  Two-way link list structure
 *---------------------------------------------------------------------------*/
typedef struct 
{
    void*       headObject;     // Pointer for the object linked to the head
    void*       tailObject;     // Pointer for the object linked to the end
    u16         numObjects;     // Number of objects linked to the list
    u16         offset;         // Offset for NNSFndLink type structure member

} NNSFndList;


/*---------------------------------------------------------------------------*
  Name:         NNS_FND_INIT_LIST

  Description:  Macro to initialize list structure. Actual initialization is performed by the function NNS_FndInitList().
                 

                This macro finds the offset using the offsetof macro based on the name of the specified structure and the NNSFndLink type member variable name and then passes that offset to the function NNSFndInitList.
                 
                 

  Arguments:    list:       Pointer for link structure
                structName: Structure name of object you wish to link to the list
                linkName:   The NNSFndLink type member variable name used to link this object.
                             

  Returns:      None.
 *---------------------------------------------------------------------------*/

#define NNS_FND_INIT_LIST(list, structName, linkName) \
            NNS_FndInitList(list, offsetof(structName, linkName))


/*---------------------------------------------------------------------------*
    Function Prototypes

 *---------------------------------------------------------------------------*/

void    NNS_FndInitList(
                NNSFndList*             list,
                u16                     offset);

void    NNS_FndAppendListObject(
                NNSFndList*             list,
                void*                   object);

void    NNS_FndPrependListObject(
                NNSFndList*             list,
                void*                   object);

void    NNS_FndInsertListObject(
                NNSFndList*             list,
                void*                   target,
                void*                   object);

void    NNS_FndRemoveListObject(
                NNSFndList*             list,
                void*                   object);

void*   NNS_FndGetNextListObject(
                NNSFndList*             list,
                void*                   object);

void*   NNS_FndGetPrevListObject(
                NNSFndList*             list,
                void*                   object);

void*   NNS_FndGetNthListObject(
                NNSFndList*             list,
                u16                     index);


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_LIST_H_*/
#endif

