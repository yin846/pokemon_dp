/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     fileIOcommoni.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fileIOcommoni.h,v $
  Revision 1.8  02/03/2005 04:41:15 AM  kitamurs
  Added NNS_McsSeekFile function

  Revision 1.7  01/21/2005 04:51:16 AM  kitamurs
  Changed year

  Revision 1.6  12/22/2004 08:24:33 AM  kitamurs
  Improved error code

  Revision 1.5  09/10/2004 12:04:37 AM  kitamurs
  Improved error handling

  Revision 1.4  06/30/2004 08:26:24 AM  kitamurs
  Changed channel values

  Revision 1.3  06/16/2004 12:23:44 AM  kitamurs
  Started a search for the error during sending

  Revision 1.2  06/14/2004 05:21:12 AM  kitamurs
  Changed type names.

  Revision 1.1  06/09/2004 07:34:11 AM  kitamurs
  Provisional port of the file I/O

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_FILEIOCOMMONI_H_
#define NNS_MCS_FILEIOCOMMONI_H_


#if defined(_MSC_VER)
    #include "mcsStdInt.h"
#else
    #include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    Constant definitions
   ======================================================================== */

#define NNSi_MCS_FILEIO_CHANNEL  (uint16_t)('FI' +0x8000)

enum
{
    NNSi_MCS_FILEIO_CMD_FILEOPEN  ,
    NNSi_MCS_FILEIO_CMD_FILECLOSE ,
    NNSi_MCS_FILEIO_CMD_FILEREAD  ,
    NNSi_MCS_FILEIO_CMD_FILEWRITE ,
    NNSi_MCS_FILEIO_CMD_BROWSEFILE,
    NNSi_MCS_FILEIO_CMD_FINDFIRST ,
    NNSi_MCS_FILEIO_CMD_FINDNEXT  ,
    NNSi_MCS_FILEIO_CMD_FINDCLOSE ,
    NNSi_MCS_FILEIO_CMD_FILESEEK
};

enum
{
    NNSi_MCS_FILEIO_OPEN_DIRECT,
    NNSi_MCS_FILEIO_OPEN_DIALOG
};


/* ========================================================================
    Type Definitions
   ======================================================================== */

#ifdef _WIN32

typedef uint32_t                NNSiMcsVoidPtr;
typedef uint32_t                NNSiMcsFilePtr;
typedef uint32_t                NNSiMcsFileFindDataPtr;

#else

typedef void*                   NNSiMcsVoidPtr;
typedef NNSMcsFile*             NNSiMcsFilePtr;
typedef NNSMcsFileFindData*     NNSiMcsFileFindDataPtr;

/* #ifdef _WIN32*/
#endif

/* ------------------------------------------------------------------------
    Command to send to file I/O server
   ------------------------------------------------------------------------ */

typedef struct NNSiMcsFileIOCmdHeader NNSiMcsFileIOCmdHeader;
struct NNSiMcsFileIOCmdHeader
{
    uint16_t                command;
    uint16_t                type;
};

/*
    File open command
*/
typedef struct NNSiMcsFileIOCmdOpen NNSiMcsFileIOCmdOpen;
struct NNSiMcsFileIOCmdOpen
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                flag;
    char                    filename[NNS_MCS_FILEIO_PATH_MAX];
};

/*
    File close command
*/
typedef struct NNSiMcsFileIOCmdClose NNSiMcsFileIOCmdClose;
struct NNSiMcsFileIOCmdClose
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
};

/*
    File read command
*/
typedef struct NNSiMcsFileIOCmdRead NNSiMcsFileIOCmdRead;
struct NNSiMcsFileIOCmdRead
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
    NNSiMcsVoidPtr          pBuffer;
    uint32_t                size;
};

/*
  File write command
*/
typedef struct NNSiMcsFileIOCmdWrite NNSiMcsFileIOCmdWrite;
struct NNSiMcsFileIOCmdWrite
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
    uint32_t                size;
};

/*
  File seek command
*/
typedef struct NNSiMcsFileIOCmdFileSeek NNSiMcsFileIOCmdFileSeek;
struct NNSiMcsFileIOCmdFileSeek
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
    int32_t                 distanceToMove;
    uint32_t                moveMethod;
};

/*
    FindFirstFile command
*/
typedef struct NNSiMcsFileIOCmdFindFirst NNSiMcsFileIOCmdFindFirst;
struct NNSiMcsFileIOCmdFindFirst
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    NNSiMcsFileFindDataPtr  pFindData;
    char                    pPattern[NNS_MCS_FILEIO_PATH_MAX];
};

/*
    FindNextFile command
*/
typedef struct NNSiMcsFileIOCmdFindNext NNSiMcsFileIOCmdFindNext;
struct NNSiMcsFileIOCmdFindNext
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    NNSiMcsFileFindDataPtr  pFindData;
    uint32_t                handle;
};

/* ------------------------------------------------------------------------
    Result from Windows
   ------------------------------------------------------------------------ */

typedef struct NNSiMcsFileIOResult NNSiMcsFileIOResult;
struct NNSiMcsFileIOResult
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
};

/*
    FileOpen result
*/
typedef struct NNSiMcsFileIOResultOpen NNSiMcsFileIOResultOpen;
struct NNSiMcsFileIOResultOpen
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
    uint32_t                filesize;
};

typedef struct NNSiMcsFileIOResultRead NNSiMcsFileIOResultRead;
struct NNSiMcsFileIOResultRead
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
    NNSiMcsVoidPtr          pBuffer;
    uint32_t                size;
    uint32_t                totalSize;
};

typedef struct NNSiMcsFileIOResultFileSeek NNSiMcsFileIOResultFileSeek;
struct NNSiMcsFileIOResultFileSeek
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
    uint32_t                filePointer;
};

typedef struct NNSiMcsFileIOResultFind NNSiMcsFileIOResultFind;
struct NNSiMcsFileIOResultFind
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
    NNSiMcsFileFindDataPtr  pFindData;
    uint32_t                handle;
    uint32_t                filesize;
    uint32_t                attribute;
    char                    pFilename[NNS_MCS_FILEIO_PATH_MAX];
};


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_MCS_FILEIOCOMMONI_H_*/
#endif

