/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Foundation
  File:     list.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: list.c,v $
  Revision 1.7  01/21/2005 04:55:27  kitamurs
  Changed year

  Revision 1.6  10/15/2004 06:24:04  nishida
  Changed from SDK ASSERT to NNS ASSERT.

  Revision 1.5  05/21/2004 02:08:16  nishida
  Bug was fixed in NNS_FndInsertListObject

  Revision 1.4  02/13/2004 06:07:20  nishida
  Inserted ASSERT in order to check function arguments.

  Revision 1.3  02/12/2004 10:41:19  nishida
  Deleted nitro.h include.

  Revision 1.2  02/09/2004 00:20:10  ida
  IRIS -> NITRO

  Revision 1.1  01/30/2004 03:51:33  nishida
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/misc.h>
#include <nnsys/fnd/list.h>

#define OBJ_TO_LINK(list,obj)   ((NNSFndLink*)(((u32)(obj))+(list)->offset))


/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitList

  Description:  Initializes list structure.

  Arguments:    list:   pointer to list structure.
                offset: Designates the offset internal to the structure of the NNSFndLink-type
                        member variable that is in the structure you want to connect to the list.
                        This is convenient when using the offsetof macro defined in stddef.h.
                         

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndInitList(NNSFndList* list, u16 offset)
{
    NNS_NULL_ASSERT(list);

    list->headObject = NULL;
    list->tailObject = NULL;
    list->numObjects = 0;
    list->offset     = offset;
}

/*---------------------------------------------------------------------------*
  Name:         SetFirstObject                                      [static]

  Description:  Adds the first object to the list.

  Arguments:    list:   pointer to list structure.
                object: pointer to the object you want to add to the list.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
SetFirstObject(NNSFndList* list, void* object)
{
    NNSFndLink* link;

    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    link = OBJ_TO_LINK(list, object);

    link->nextObject = NULL;
    link->prevObject = NULL;
    list->headObject = object;
    list->tailObject = object;
    list->numObjects++;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAppendListObject

  Description:  Adds object to the end of the list.

  Arguments:    list:   pointer to list structure.
                object: pointer to the object you want to add to the list.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndAppendListObject(NNSFndList* list, void* object)
{
    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    if (list->headObject == NULL)
    {
        // When the list is empty.
        SetFirstObject(list, object);
    }
    else
    {
        NNSFndLink* link = OBJ_TO_LINK(list, object);

        link->prevObject = list->tailObject;
        link->nextObject = NULL;

        OBJ_TO_LINK(list, list->tailObject)->nextObject = object;
        list->tailObject = object;
        list->numObjects++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndPrependListObject

  Description:  Inserts object at the head of the list.

  Arguments:    list:   pointer to list structure.
                object: pointer to the object you want to add to the list.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndPrependListObject(NNSFndList* list, void* object)
{
    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    if (list->headObject == NULL)
    {
        // When the list is empty.
        SetFirstObject(list, object);
    }
    else
    {
        NNSFndLink* link = OBJ_TO_LINK(list, object);

        link->prevObject = NULL;
        link->nextObject = list->headObject;

        OBJ_TO_LINK(list, list->headObject)->prevObject = object;
        list->headObject = object;
        list->numObjects++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInsertListObject

  Description:  Inserts object in the specified location. The object is inserted in front of
                the object specified in target. When the insertion point is not specified
                (when the target is NULL), the object is added to the end of the list.
                 

  Arguments:    list:   pointer to list structure.
                target: pointer to object in the position you want to insert.
                object: pointer to the object you want to add to the list.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndInsertListObject(NNSFndList* list, void* target, void* object)
{
    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    if (target == NULL)
    {
        // When the target is not specified, same as NNS_FndAppendListObject().
        NNS_FndAppendListObject(list, object);
    }
    else if (target == list->headObject)
    {
        // When target is the head of the list, same as NNS_FndPrependListObject().
        NNS_FndPrependListObject(list, object);
    }
    else
    {
        NNSFndLink* link    = OBJ_TO_LINK(list, object);
        void*       prevObj = OBJ_TO_LINK(list, target)->prevObject;
        NNSFndLink* prevLnk = OBJ_TO_LINK(list, prevObj);

        link->prevObject    = prevObj;
        link->nextObject    = target;
        prevLnk->nextObject = object;
        OBJ_TO_LINK(list, target)->prevObject = object;
        list->numObjects++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndRemoveListObject

  Description:  Deletes object from the list.

  Arguments:    list:   pointer to list structure.
                object: Pointer to the object you want to delete from the list.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndRemoveListObject(NNSFndList* list, void* object)
{
    NNSFndLink* link;

    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    link = OBJ_TO_LINK(list, object);

    if (link->prevObject == NULL)
    {
        list->headObject = link->nextObject;
    }
    else
    {
        OBJ_TO_LINK(list, link->prevObject)->nextObject = link->nextObject;
    }
    if (link->nextObject == NULL)
    {
        list->tailObject = link->prevObject;
    }
    else
    {
        OBJ_TO_LINK(list, link->nextObject)->prevObject = link->prevObject;
    }
    link->prevObject = NULL;
    link->nextObject = NULL;
    list->numObjects--;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetNextListObject

  Description:  Returns the object to be linked next after the object specified by 'object.'
                 If NULL has been specified for the object, the function returns the object linked to the top of the list.
                 

  Arguments:    list:   pointer to list structure.
                object: Pointer to object in list.

  Returns:      Returns a pointer to the next object after the specified object.
                 If there is no next object, returns NULL.
 *---------------------------------------------------------------------------*/
void*
NNS_FndGetNextListObject(NNSFndList* list, void* object)
{
    NNS_NULL_ASSERT(list);

    if (object == NULL)
    {
        return list->headObject;
    }
    return OBJ_TO_LINK(list, object)->nextObject;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetPrevListObject

  Description:  Returns the object linked in front of the object specified by 'object.'
                  If NULL has been specified for the object, the function returns the object linked to the bottom of the list.
                  

  Arguments:    list:   pointer to list structure.
                object: Pointer to object in list.

  Returns:      Returns a pointer to the previous object before the specified object.
                  If there is no object in front, returns NULL.
 *---------------------------------------------------------------------------*/
void*
NNS_FndGetPrevListObject(NNSFndList* list, void* object)
{
    NNS_NULL_ASSERT(list);

    if (object == NULL)
    {
        return list->tailObject;
    }
    return OBJ_TO_LINK(list, object)->prevObject;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetNthListObject

  Description:  Returns a pointer to the object linked Nth on the list.
                   Note that the processing time required is longer the deeper in the list the object being linked appears due to the fact that the list is traversed in order beginning from the top.
                 

  Arguments:    index    :  Object index.

  Returns:      Returns pointer to object. NULL is returned if there is no object corresponding to the specified index.
                 
 *---------------------------------------------------------------------------*/
void*
NNS_FndGetNthListObject(NNSFndList* list, u16 index)
{
    int         count  = 0;
    NNSFndLink* object = NULL;

    NNS_NULL_ASSERT(list);

    while ((object = NNS_FndGetNextListObject(list, object)) != NULL)
    {
        if (index == count)
        {
            return object;
        }
        count++;
    }
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_UTCountListNode

  Description:  Counts the number of objects on the list. Because it counts the list in order from the beginning,
                it will take time if the list is long.
                   

  Arguments:    list:   pointer to list structure.
 
  Returns:      Number of objects on the list.
 *---------------------------------------------------------------------------*/
#if 0
int
NNS_UTCountListNode(NNS_UTList* list)
{
    int             count  = 0;
    NNS_UTListNode* object = NULL;

    NNS_NULL_ASSERT(list);

    while ((object = NNS_UTGetNextListObject(list, object)) != NULL)
    {
        count++;
    }
    return count;
}
#endif

