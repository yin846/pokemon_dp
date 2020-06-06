/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     fileIOcommon.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fileIOcommon.h,v $
  Revision 1.9  02/03/2005 04:42:17 AM  kitamurs
  Added NNS_McsSeekFile function

  Revision 1.8  01/21/2005 04:58:57 AM  kitamurs
  Changed year

  Revision 1.7  01/14/2005 05:19:15 AM  kitamurs
  Added comment

  Revision 1.6  12/24/2004 06:09:21 AM  kitamurs
  Changed value

  Revision 1.5  12/22/2004 08:23:16 AM  kitamurs
  Improved error code

  Revision 1.4  11/18/2004 01:26:30 AM  kitamurs
  Added a feature to create directory when opening a file.

  Revision 1.3  10/24/2004 11:42:58 PM  kitamurs
  Added support for environmental variable

  Revision 1.2  06/24/2004 08:20:52 AM  kitamurs
  Set the maximum length of the path to the MAX_PATH of Windows

  Revision 1.1  06/09/2004 07:36:00 AM  kitamurs
  Provisional port of the file I/O

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_FILEIOCOMMON_H_
#define NNS_MCS_FILEIOCOMMON_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    Constant definitions
   ======================================================================== */

enum
{
    NNS_MCS_FILEIO_ERROR_SUCCESS        = 0,
    NNS_MCS_FILEIO_ERROR_COMERROR,
    NNS_MCS_FILEIO_ERROR_NOTCONNECT,
    NNS_MCS_FILEIO_ERROR_SERVERERROR,
    NNS_MCS_FILEIO_ERROR_NOMOREFILES,

    NNSi_MCS_FILEIO_RESULT_READING      = 0x80
};


enum
{
    NNS_MCS_FILEIO_FLAG_READ      = 1 << 0,
    NNS_MCS_FILEIO_FLAG_WRITE     = 1 << 1,
    // NNS_MCS_FILEIO_FLAG_OVERWRITE = 1 << 2,
    NNS_MCS_FILEIO_FLAG_FORCE     = 1 << 3,
    NNS_MCS_FILEIO_FLAG_INCENVVAR = 1 << 4,
    NNS_MCS_FILEIO_FLAG_CREATEDIR = 1 << 5
};

//  Same as MAX_PATH in windef.h
#define NNS_MCS_FILEIO_PATH_MAX 260

enum
{
    NNS_MCS_FILEIO_SEEK_BEGIN,
    NNS_MCS_FILEIO_SEEK_CURRENT,
    NNS_MCS_FILEIO_SEEK_END
};

// Same as the value of the file attribute defined in winnt.h
enum
{
    NNS_MCS_FILEIO_ATTRIBUTE_READONLY             = 0x00000001,
    NNS_MCS_FILEIO_ATTRIBUTE_HIDDEN               = 0x00000002,
    NNS_MCS_FILEIO_ATTRIBUTE_SYSTEM               = 0x00000004,
    NNS_MCS_FILEIO_ATTRIBUTE_DIRECTORY            = 0x00000010,
    NNS_MCS_FILEIO_ATTRIBUTE_ARCHIVE              = 0x00000020,
    NNS_MCS_FILEIO_ATTRIBUTE_DEVICE               = 0x00000040,
    NNS_MCS_FILEIO_ATTRIBUTE_NORMAL               = 0x00000080,
    NNS_MCS_FILEIO_ATTRIBUTE_TEMPORARY            = 0x00000100,
    NNS_MCS_FILEIO_ATTRIBUTE_SPARSE_FILE          = 0x00000200,
    NNS_MCS_FILEIO_ATTRIBUTE_REPARSE_POINT        = 0x00000400,
    NNS_MCS_FILEIO_ATTRIBUTE_COMPRESSED           = 0x00000800,
    NNS_MCS_FILEIO_ATTRIBUTE_OFFLINE              = 0x00001000,
    NNS_MCS_FILEIO_ATTRIBUTE_NOT_CONTENT_INDEXED  = 0x00002000,
    NNS_MCS_FILEIO_ATTRIBUTE_ENCRYPTED            = 0x00004000,

    NNS_MCS_FILEIO_ATTRIBUTE_LAST
};


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_MCS_FILEIOCOMMON_H_*/
#endif

