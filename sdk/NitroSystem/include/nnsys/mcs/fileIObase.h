/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     fileIObase.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fileIObase.h,v $
  Revision 1.11  2005/02/25 04:05:49 AM  kitamurs
  Warning measures

  Revision 1.10  2005/02/25 02:59:41 AM  kitamurs
  Warning measures

  Revision 1.9  02/03/2005 04:42:17 AM  kitamurs
  Added NNS_McsSeekFile function

  Revision 1.8  01/21/2005 04:58:57 AM  kitamurs
  Changed year

  Revision 1.7  12/22/2004 08:23:16 AM  kitamurs
  Improved error code

  Revision 1.6  10/19/2004 01:17:39 AM  kitamurs
  - Changed SDK_... to NNS_...
  - Changed argument names

  Revision 1.5  07/12/2004 02:23:45 AM  kitamurs
  Corrected the compile not going through on the FINALROM build

  Revision 1.4  06/16/2004 08:05:56 AM  kitamurs
  Changed the structure member name

  Revision 1.3  06/16/2004 12:26:40 AM  kitamurs
  Added detection for errors during sending

  Revision 1.2  06/14/2004 05:24:47 AM  kitamurs
  FINAL_ROM provisions

  Revision 1.1  06/09/2004 07:36:00 AM  kitamurs
  Provisional port of the file I/O

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_FILEIOBASE_H_
#define NNS_MCS_FILEIOBASE_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    Type Definitions
   ======================================================================== */

/*
    The file information structure
*/
typedef struct NNSMcsFile NNSMcsFile;
struct NNSMcsFile
{
    u32                 handle;
    u32                 filesize;
    u32                 errCode;
    u32                 srvErrCode;

    u32                 tempData;
    volatile u32        bDone       : 1;
};

typedef struct NNSMcsFileFindData NNSMcsFileFindData;
struct NNSMcsFileFindData
{
    u32         attribute;
    u32         size;
    char        name[NNS_MCS_FILEIO_PATH_MAX];
};


/* ========================================================================
    Macro functions
   ======================================================================== */

#if defined(NNS_FINALROM)

    #define         NNS_McsGetFileSize(pFile)                           ( (void)(pFile), 0U)

    #define         NNS_McsGetServerErrorCode(pFile)                    ( (void)(pFile), 0U)

/* #if defined(NNS_FINALROM) */
#else

    #define         NNS_McsGetFileSize(pFile)                           ((pFile)->filesize)

    #define         NNS_McsGetServerErrorCode(pFile)                    ((pFile)->srvErrCode)

/* #if defined(NNS_FINALROM) */
#endif


/* ========================================================================
    Function Prototypes
   ======================================================================== */

#if defined(NNS_FINALROM)


    #define         NNS_McsInitFileIO()                                 ((void)0)

    #define         NNS_McsOpenFile(pFile, filename, openflag)          ( (void)( (void)( (void)(pFile), (filename) ), (openflag) ), NNS_MCS_FILEIO_ERROR_COMERROR )

    #define         NNS_McsCloseFile(pFile)                             ( (void)(pFile), NNS_MCS_FILEIO_ERROR_COMERROR )

    #define         NNS_McsReadFile(pFile, buffer, length, pReadBytes)  ( (void)( (void)( (void)( (void)(pFile), (buffer) ), (length) ), (pReadBytes) ), NNS_MCS_FILEIO_ERROR_COMERROR )

    #define         NNS_McsWriteFile(pFile, buffer, length)             ( (void)( (void)( (void)(pFile), (buffer) ), (length) ), NNS_MCS_FILEIO_ERROR_COMERROR )

    #define         NNS_McsSeekFile(pFile, distanceToMove, moveMethod, pNewFilePointer) \
                                                                        ( (void)( (void)( (void)( (void)(pFile), (distanceToMove) ), (moveMethod) ), (pNewFilePointer) ), NNS_MCS_FILEIO_ERROR_COMERROR )

    #define         NNS_McsFindFirstFile(pFile, pFindData, pattern)     ( (void)( (void)( (void)(pFile), (pFindData) ), (pattern) ), NNS_MCS_FILEIO_ERROR_COMERROR )

    #define         NNS_McsFindNextFile(pFile, pFindData)               ( (void)( (void)(pFile), (pFindData) ), NNS_MCS_FILEIO_ERROR_COMERROR )

    #define         NNS_McsCloseFind(pFile)                             ( (void)(pFile), NNS_MCS_FILEIO_ERROR_COMERROR )


/* #if defined(NNS_FINALROM) */
#else


    void            NNS_McsInitFileIO(void);

    u32             NNS_McsOpenFile(
                        NNSMcsFile*         pFile,
                        const char*         fileName,
                        u32                 openFlag);

    u32             NNS_McsCloseFile(
                        NNSMcsFile*         pFile);

    u32             NNS_McsReadFile(
                        NNSMcsFile*         pFile,
                        void*               buffer,
                        u32                 length,
                        u32*                pReadBytes);

    u32             NNS_McsWriteFile(
                        NNSMcsFile*         pFile,
                        const void*         buffer,
                        u32                 length);

    u32             NNS_McsSeekFile(
                        NNSMcsFile*         pFile,
                        s32                 distanceToMove,
                        u32                 moveMethod,
                        u32*                pNewFilePointer);

    u32             NNS_McsFindFirstFile(
                        NNSMcsFile*         pFile,
                        NNSMcsFileFindData* pFindData,
                        const char*         pattern);

    u32             NNS_McsFindNextFile(
                        NNSMcsFile*         pFile,
                        NNSMcsFileFindData* pFindData);

    u32             NNS_McsCloseFind(
                        NNSMcsFile*         pFile);


/* #if defined(NNS_FINALROM) */
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_MCS_FILEIOBASE_H_ */
#endif


