/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation
  File:     archive_block.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: archive_block.h,v $
  Revision 1.2  01/21/2005 04:54:12 AM  kitamurs
  Changed year

  Revision 1.1  05/12/2004 11:46:12 AM  nishida
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_ARCHIVE_BLOCK_H_
#define NNS_FND_ARCHIVE_BLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         NNSiFndArchiveBlockHeader

  Description:  The data block header of the archive file
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32			blockType;				// block type
	u32			blockSize;				// block size

} NNSiFndArchiveBlockHeader;


/*---------------------------------------------------------------------------*
  Name:         NNSiFndArchiveDirBlock
  				NNSiFndArchiveImgBlock

  Description:  The directory information block header structure of the archive file
  				The file image block header structure of the archive file
 *---------------------------------------------------------------------------*/

typedef	NNSiFndArchiveBlockHeader	NNSiFndArchiveDirBlockHeader;
typedef	NNSiFndArchiveBlockHeader	NNSiFndArchiveImgBlockHeader;


/*---------------------------------------------------------------------------*
  Name:         NNSFndArchiveFatBlock

  Description:  The FAT block header structure of the archive file
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32			blockType;				// block type
	u32			blockSize;				// block size
	u16			numFiles;				// number of files
	u16			reserved;				// reserved

} NNSiFndArchiveFatBlockHeader;


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_ARCHIVE_BLOCK_H_*/
#endif

