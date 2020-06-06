/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     heap.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: heap.c,v $
  Revision 1.23  04/27/2005 05:15:58 AM  ida
  Revised so EraseSync is not called when clearing empty heap

  Revision 1.22  02/21/2005 02:37:17 AM  ida
  Warning measures

  Revision 1.21  01/06/2005 02:51:20 AM  ida
  Updated copyright year notation

  Revision 1.20  10/19/2004 01:52:27 AM  ida
  use NNS_SND_HEAP_INVALID_HANDLE

  Revision 1.19  09/07/2004 01:16:56 AM  ida
  use nnsys/misc.h

  Revision 1.18  08/10/2004 12:25:30 AM  ida
  add NNS_SndHeapDestroy

  Revision 1.17  07/16/2004 06:42:49 AM  ida
  fix Project name

  Revision 1.16  07/13/2004 12:52:09 AM  ida
  fix not 4 bytes aligned startAddress in NNS_SndHeapCreate

  Revision 1.15  06/21/2004 01:09:11 AM  ida
  for new snd_drv interface

  Revision 1.14  06/11/2004 01:51:27 AM  ida
  for new snd_drv interface

  Revision 1.13  06/09/2004 08:45:34 AM  ida
  fixed comments

  Revision 1.12  06/09/2004 05:58:10 AM  ida
  memory block 32byte boundary

  Revision 1.11  06/08/2004 12:48:14 AM  ida
  changed interface

  Revision 1.10  05/26/2004 02:26:21 AM  ida
  add assertion

  Revision 1.9  05/25/2004 04:47:50 AM  ida
  removed NNS_SndHeapFindMemoryBlockAll

  Revision 1.8  05/14/2004 01:45:28 AM  ida
  add EraseSync

  Revision 1.7  05/14/2004 12:57:22 AM  ida
  add check heap over
  NNS_SndHeapSetCurrent return oldHeapHandle

  Revision 1.6  05/06/2004 01:40:04 AM  ida
  remove printf

  Revision 1.5  04/30/2004 12:42:19 AM  ida
  added NNS_SndHeapGetSize

  Revision 1.4  04/28/2004 07:47:06 AM  ida
  fixed comments

  Revision 1.3  04/28/2004 02:58:29 AM  ida
  remove DumpHeap

  Revision 1.2  04/28/2004 02:56:07 AM  ida
  add comments

  Revision 1.1  04/26/2004 02:19:28 AM  ida
  add heap.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/heap.h>

#include <nnsys/misc.h>
#include <nnsys/fnd.h>

/******************************************************************************
	macro definition
 ******************************************************************************/

#define HEAP_ALIGN 32

#define ROUNDUP( value, align ) ( ( (u32)(value) + ( (align) - 1 ) ) & ~( (align) - 1 ) )

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndHeap
{
    NNSFndHeapHandle handle;
    NNSFndList sectionList;
} NNSSndHeap;

typedef struct NNSSndHeapBlock
{
    NNSFndLink link;
    u32 size;
    NNSSndHeapDisposeCallback callback;
    u32 data1;
    u32 data2;
    u8 padding[ 0x20 - ( ( sizeof( NNSFndLink ) + sizeof( NNSSndHeapDisposeCallback ) + sizeof( u32 ) * 3 ) & 0x1f ) ];
    u32 buffer[ 0 ];
} NNSSndHeapBlock; // NOTE: must be 32 byte boundary

typedef struct NNSSndHeapSection
{
    NNSFndList blockList;
    NNSFndLink link;
} NNSSndHeapSection;

/******************************************************************************
	static function declarations
 ******************************************************************************/

static void InitHeapSection( NNSSndHeapSection* section );
static BOOL InitHeap( NNSSndHeap* heap, NNSFndHeapHandle handle );
static BOOL NewSection( NNSSndHeap* heap );
static void EraseSync( void );

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapCreate

  Description:  Creates the heap

  Arguments:    startAddress - The starting address
                size         - The memory size

  Returns:      The heap handle
 *---------------------------------------------------------------------------*/
NNSSndHeapHandle NNS_SndHeapCreate( void* startAddress, u32 size )
{
    NNSSndHeap* heap;
    void* endAddress;
    NNSFndHeapHandle handle;
    
    NNS_NULL_ASSERT( startAddress );
    
    endAddress   = (u8*)startAddress + size;
    startAddress = (void*)ROUNDUP( startAddress, 4 ); // NNSSndHeap align
    
    if ( startAddress > endAddress ) return NNS_SND_HEAP_INVALID_HANDLE;
    
    size = (u32)( (u8*)endAddress - (u8*)startAddress );
    if ( size < sizeof( NNSSndHeap ) ) {
        return NNS_SND_HEAP_INVALID_HANDLE;
    }
    
    size -= sizeof( NNSSndHeap );
    
    heap = (NNSSndHeap*)startAddress;
    startAddress = heap + 1;
    
    handle = NNS_FndCreateFrmHeap( startAddress, size );
    if ( handle == NNS_FND_HEAP_INVALID_HANDLE ) {
        return NULL;
    }
    
    if ( ! InitHeap( heap, handle ) ) {
        NNS_FndDestroyFrmHeap( handle );
        return NNS_SND_HEAP_INVALID_HANDLE;
    }
    
    return heap;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapDestroy

  Description:  Disposal Heap

  Arguments:    heap - Sound Heap

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHeapDestroy( NNSSndHeapHandle heap )
{
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    NNS_SndHeapClear( heap );
    
    NNS_FndDestroyFrmHeap( heap->handle );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapClear

  Description:  Returns the state at the time of heap creation

  Arguments:    heap - The sound heap

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHeapClear( NNSSndHeapHandle heap )
{
    NNSSndHeapSection* section=NULL;
    void* object;
    BOOL result;
    BOOL doCallback = FALSE;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    // Section disposal
    while ( ( section = (NNSSndHeapSection*)NNS_FndGetPrevListObject( & heap->sectionList, NULL ) ) != NULL )
    {
        // Call the callback
        object = NULL;
        while ( ( object = NNS_FndGetPrevListObject( & section->blockList, object ) ) != NULL )
        {
            NNSSndHeapBlock* block = (NNSSndHeapBlock*)object;
            if ( block->callback != NULL ) {
                block->callback( block->buffer, block->size, block->data1, block->data2 );
                doCallback = TRUE;
            }
        }
        
        // Delete from the section list
        NNS_FndRemoveListObject( & heap->sectionList, section );
    }
    
    // Clear the heap
    NNS_FndFreeToFrmHeap( heap->handle, NNS_FND_FRMHEAP_FREE_ALL );
    
    // Synchronizes discontinuation of sound data usage
    if ( doCallback ) EraseSync();
    
    // Create the base section
    result = NewSection( heap );
    NNS_ASSERTMSG( result, "NNS_SndHeapClear(): NewSection is Failed");
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapAlloc

  Description:  Allocates memory from the heap

  Arguments:    heap - Sound Heap
                size         - The memory size
                callback - The callback function called when getting rid of the memory
                data1    - Callback data 1
                data2    - Callback data 2

  Returns:      The pointer for the allocated memory
 *---------------------------------------------------------------------------*/
void* NNS_SndHeapAlloc( NNSSndHeapHandle heap, u32 size, NNSSndHeapDisposeCallback callback, u32 data1, u32 data2 )
{
    NNSSndHeapSection* section;
    NNSSndHeapBlock* block;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    block = (NNSSndHeapBlock*)NNS_FndAllocFromFrmHeapEx(
        heap->handle, sizeof( NNSSndHeapBlock ) + ROUNDUP( size, HEAP_ALIGN ), HEAP_ALIGN );
    if ( block == NULL ) return NULL;
    
    section = (NNSSndHeapSection*)NNS_FndGetPrevListObject( & heap->sectionList, NULL );
    
    block->size = size;
    block->callback = callback;
    block->data1 = data1;
    block->data2 = data2;
    NNS_FndAppendListObject( & section->blockList, block );
    
    NNS_ASSERTMSG( ( (u32)( block->buffer ) & 0x1f ) == 0, "NNS_SndHeapAlloc: Internal Error" );
    
    return block->buffer;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapSaveState

  Description:  Saves the heap state

  Arguments:    heap - Sound heap

  Returns:      Returns the saved hierarchy level.
                Returns a -1 if failed.
 *---------------------------------------------------------------------------*/
int NNS_SndHeapSaveState( NNSSndHeapHandle heap )
{
    BOOL result;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    if ( ! NNS_FndRecordStateForFrmHeap( heap->handle, heap->sectionList.numObjects ) ) {
        return -1;
    }
    
    if ( ! NewSection( heap ) ) {
        result = NNS_FndFreeByStateToFrmHeap( heap->handle, 0 );
        NNS_ASSERTMSG( result, "NNS_SndHeapSaveState(): NNS_FndFreeByStateToFrmHeap is Failed");
        return -1;
    }
    
    return heap->sectionList.numObjects - 1;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapLoadState

  Description:  Returns the heap state

  Arguments:    heap - Sound Heap
                level - The hierarchy level

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHeapLoadState( NNSSndHeapHandle heap, int level )
{
    NNSSndHeapSection* section;
    void* object = NULL;
    BOOL result;
    BOOL doCallback = FALSE;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    NNS_MINMAX_ASSERT( level, 0, heap->sectionList.numObjects-1 );
    
    if ( level == 0 ) {
        NNS_SndHeapClear( heap );
        return;
    }
    while( level < heap->sectionList.numObjects )
    {
        // get latest section
        section = (NNSSndHeapSection*)NNS_FndGetPrevListObject( & heap->sectionList, NULL );
        
        // call dispose callback
        while ( ( object = NNS_FndGetPrevListObject( & section->blockList, object ) ) != NULL )
        {
            NNSSndHeapBlock* block = (NNSSndHeapBlock*)object;
            if ( block->callback != NULL ) {
                block->callback( block->buffer, block->size, block->data1, block->data2 );
                doCallback = TRUE;
            }
        }
        
        // Delete from the section list
        NNS_FndRemoveListObject( & heap->sectionList, section );
    }
    
    // Restore the heap state
    result = NNS_FndFreeByStateToFrmHeap( heap->handle, (u32)level );
    NNS_ASSERTMSG( result, "NNS_SndHeapLoadState(): NNS_FndFreeByStateToFrmHeap is Failed");   

    // Synchronizes discontinuation of sound data usage
    if ( doCallback ) EraseSync();
    
    // Resave
    result =  NNS_FndRecordStateForFrmHeap( heap->handle, heap->sectionList.numObjects );
    NNS_ASSERTMSG( result, "NNS_SndHeapLoadState(): NNS_FndRecordStateForFrmHeap is Failed");
    
    // Create section
    result = NewSection( heap );
    NNS_ASSERTMSG( result, "NNS_SndHeapLoadState(): NewSection is Failed");
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapGetCurrentLevel

  Description:  Gets the current hierarchy level of the heap

  Arguments:    heap - Sound Heap

  Returns:      The current hierarchy level
 *---------------------------------------------------------------------------*/
int NNS_SndHeapGetCurrentLevel( NNSSndHeapHandle heap )
{
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    return heap->sectionList.numObjects - 1;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapGetSize

  Description:  Gets the capacity of the heap

  Arguments:    heap - Sound Heap

  Returns:      The heap capacity
 *---------------------------------------------------------------------------*/
u32 NNS_SndHeapGetSize( NNSSndHeapHandle heap )
{
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    return
        (u32)( (u8*)NNS_FndGetHeapEndAddress( heap->handle ) -
               (u8*)NNS_FndGetHeapStartAddress( heap->handle ) )
        ;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHeapGetFreeSize

  Description:  Gets the amount of free space in the heap

  Arguments:    heap - Sound Heap

  Returns:      The amount of free space
 *---------------------------------------------------------------------------*/
u32 NNS_SndHeapGetFreeSize( NNSSndHeapHandle heap )
{
    u32 size;
    
    NNS_ASSERT( heap != NNS_SND_HEAP_INVALID_HANDLE );
    
    size = NNS_FndGetAllocatableSizeForFrmHeapEx( heap->handle, HEAP_ALIGN );
    
    if ( size < sizeof( NNSSndHeapBlock ) ) return 0;
    size -= sizeof( NNSSndHeapBlock );
    
    size &= ~0x1f;
    
    return size;
}

/******************************************************************************
	static functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         InitHeapSection

  Description:  Initializes the NNSSndHeapSection structure

  Arguments:    section - The heap section

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitHeapSection( NNSSndHeapSection* section )
{
    NNS_FND_INIT_LIST( & section->blockList, NNSSndHeapBlock, link );    
}

/*---------------------------------------------------------------------------*
  Name:         InitHeap

  Description:  Initializes the NNSSndHeap structure

  Arguments:    heap - Sound Heap
                handle - The frame heap handle

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
static BOOL InitHeap( NNSSndHeap* heap, NNSFndHeapHandle handle )
{
    NNS_FND_INIT_LIST( & heap->sectionList, NNSSndHeapSection, link );
    heap->handle = handle;
    
    // Create the base section
    if ( ! NewSection( heap ) ) {
        return FALSE;
    }
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NewSection

  Description:  Creates a new section

  Arguments:    heap - Sound Heap

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
static BOOL NewSection( NNSSndHeap* heap )
{
    NNSSndHeapSection* section;
    
    // new NNSSndHeapSection
    section = (NNSSndHeapSection*)NNS_FndAllocFromFrmHeap( heap->handle, sizeof( NNSSndHeapSection ) );
    if ( section == NULL ) {
        return FALSE;
    }
    InitHeapSection( section );
    
    NNS_FndAppendListObject( & heap->sectionList, section );
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         EraseSync

  Description:  Performs synchronization to stop the use of data completely when the heap is deleted.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void EraseSync( void )
{
    u32 commandTag;
    
    commandTag = SND_GetCurrentCommandTag();
    (void)SND_FlushCommand( SND_COMMAND_BLOCK );
    SND_WaitForCommandProc( commandTag );
}

/*====== End of heap.c ======*/


