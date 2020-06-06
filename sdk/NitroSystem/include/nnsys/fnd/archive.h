/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation
  File:     archive.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: archive.h,v $
  Revision 1.3  01/21/2005 04:59:49 AM  kitamurs
  Changed year

  Revision 1.2  10/15/2004 06:14:10 AM  nishida
  Revised NNS_FndOpenArchiveFileByIndex argument name

  Revision 1.1  05/12/2004 11:40:30 AM  nishida
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_ARCHIVE_H_
#define NNS_FND_ARCHIVE_H_

//#include <stddef.h>
#include <nitro/os.h>
#include <nitro/fs.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         NNSFndArchiveFatEntry

  Description:  FAT Entry Structure of Archive
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32						fileTop;			// Offset to start of file image
	u32						fileBottom;			// Offset of end of file image

} NNSiFndArchiveFatEntry;


/*---------------------------------------------------------------------------*
  Name:         NNSiFndArchiveFatData

  Description:  FAT Data Structure of Archive
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32						blockType;			// block type
	u32						blockSize;			// block size
	u16						numFiles;			// number of files
	u16						reserved;			// reserved
	NNSiFndArchiveFatEntry	fatEntries[1];

} NNSiFndArchiveFatData;


/*---------------------------------------------------------------------------*
  Name:         NNSiFndArchiveHeader

  Description:  File Header Structure of Archive
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32						signature;			// signature (NARC)
	u16						byteOrder;			// byte order (0xfeff)
	u16						version;			// version number
	u32						fileSize;			// archive file size
	u16						headerSize;			// archive file header size
	u16						dataBlocks;			// number of data blocks

} NNSiFndArchiveHeader;


/*---------------------------------------------------------------------------*
  Name:         NNSFndArchive

  Description:  NNS Archive Structure. Maintains the archive information.
  				When specifing an archive with the NITRO-System's archive manager, 
  				use this NNS archive structure. 
 *---------------------------------------------------------------------------*/
typedef struct
{
	FSArchive				fsArchive;			// work for file system
	NNSiFndArchiveHeader*	arcBinary;			// start of archive binary data
	NNSiFndArchiveFatData*	fatData;			// start of archive FAT data
	u32						fileImage;			// start of archive file image

} NNSFndArchive;


/*---------------------------------------------------------------------------*
    Function Prototype

 *---------------------------------------------------------------------------*/

BOOL		NNS_FndMountArchive(
					NNSFndArchive*		archive,
					const char*			arcName,
					void*				arcBinary);

BOOL		NNS_FndUnmountArchive(
					NNSFndArchive*		archive);

void*		NNS_FndGetArchiveFileByName(
					const char*			path);

void*		NNS_FndGetArchiveFileByIndex(
					NNSFndArchive*		archive,
					u32					index);

BOOL		NNS_FndOpenArchiveFileByIndex(
					FSFile*				file,
					NNSFndArchive*		archive,
					u32					index);


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_ARCHIVE_H_*/
#endif

