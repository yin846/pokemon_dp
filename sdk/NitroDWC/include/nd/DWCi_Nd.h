/*--------------------------------------------------------------------------
  Project:  DWCi_Nd
  File:     DWCi_Nd.h

  Copyright 2006 Nintendo.  All rights reserved.

  $Log: DWCi_Nd.h,v $
  Revision 1.13  07/19/2006 00:48:27  nakata
  Synchronized the source code with the Denyu-sha CVS server

  Revision 1.12  06/22/2006 04:27:00  nakata
  Set the minimum for a download buffer to 128 bytes (which should allow for an excess over the value calculated from the server specifications)

  Revision 1.11  06/22/2006 00:48:56  nakata
  Changes to allocate the minimum capacity for the download buffer

  Revision 1.10  06/05/2006 01:25:38  nakata
  Corrected an operational error caused by a the linking of a conditional expression due to an arithmetic operator

  Revision 1.23  06/02/2006 01:05:01  aida
  modified conditional operator

  Revision 1.22  05/17/2006 05:46:09  aida
  modified macro name

  Revision 1.21  05/12/2006 05:47:13  aida
  modified macro name
  modified structure name

  Revision 1.20  05/11/2006 07:57:38  aida
  modified DWC_ND_COUNTBUFFER_LENGTH

  Revision 1.19  05/11/2006 04:40:24  aida
  added DWC_ND_COUNTBUFFER_LENGTH
  added DWC_ND_LISTBUFFER_HEAD
  added DWC_ND_LISTBUFFER_NUM

  Revision 1.18  05/11/2006 04:38:39  aida
  deleted DWC_ND_RECEIVEBUFFER_LENGTH

  Revision 1.17  05/08/2006 10:33:30  aida
  added gamecd, passwd on DWCi_Nd_Startup()
  fixed macro names

  Revision 1.16  05/01/2006 04:34:02  aida
  deleted DWC_ND_USERAGENT

  Revision 1.15  05/01/2006 03:24:49  aida
  modified enum macro names
  modified behaviour of DWCi_NdGetFileAsync()

  Revision 1.14  04/26/2006 02:52:40  aida
  deleted "#include <dwc.h>"

  Revision 1.13  04/25/2006 05:05:55  aida
  deleted global definition #include <dwc_backup.h>

  Revision 1.12  04/25/2006 05:05:02  aida
  changed copyright description

  Revision 1.11  04/24/2006 07:58:53  aida
  changed DWCNdFileInfo::explain[] char to short

  Revision 1.10  04/17/2006 07:09:36  aida
  changed k-option to -kkv

  Revision 1.9  04/17/2006 07:00:20  aida
  fixed log tag

  Revision 1.5  03/28/2006 09:38:51  aida
  no message

  $NoKeywords: $
 *-------------------------------------------------------------------------*/
#ifndef DWC_ND_DWC_ND_H
#define DWC_ND_DWC_ND_H

#if ((defined WIN32) || (defined _WIN32))

#pragma once
#define BOOL int
typedef unsigned long u32;
typedef unsigned long long u64;
#include <stdlib.h>

#else

#ifndef _NITRO
#define _NITRO
#endif
#include <nitro.h>
#include <nitroWiFi.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////

// Argument provided to the NHTTP library     HTTP networking thread priority
#define DWC_ND_NHTTP_THREAD_PRIORITY		10

// receive buffer minimum value
#define DWC_ND_RECVBUF_MIN					128  // [byte]

// Receive buffer for file lists
// Use with DWCi_NdGetFileListAsync()
#define DWC_ND_LISTBUFFER_PERNUM			256  // [byte]

// DL game code (gamecd) byte length passed to DWCi_NdStartup()
#define DWC_ND_LENGTH_GAMECODE				4

// Private key (passwd) byte length passed to DWCi_NdStartup()
#define DWC_ND_LENGTH_PASSWORD				16


typedef enum {
	DWC_ND_ERROR_NONE,     // No error
	DWC_ND_ERROR_ALLOC,    // Failure in heap memory allocation
	DWC_ND_ERROR_BUSY,     // A new request was made while downloading
	DWC_ND_ERROR_HTTP,     // HTTP communications fail
	DWC_ND_ERROR_BUFFULL,  // Insufficient receive buffer
	DWC_ND_ERROR_DLSERVER, // The server has returned an error code
	DWC_ND_ERROR_CANCELED, // The download was cancelled
	DWC_ND_ERROR_PARAM,		 // Illegal parameter
	DWC_ND_ERROR_FATAL,		 // Undefined fatal error
	DWC_ND_ERROR_MAX
} DWCNdError;

typedef enum {
	DWC_ND_CBREASON_INITIALIZE,			// End initialization process (Not used by DWCi_Nd)
	DWC_ND_CBREASON_GETFILELISTNUM, // End of acquisition process for number of file list entries
	DWC_ND_CBREASON_GETFILELIST,    // End of file list acquisition process
	DWC_ND_CBREASON_GETFILE,        // End file acquisition process
	DWC_ND_CBREASON_MAX
} DWCNdCallbackReason;

#define DWC_ND_FILEATTR_LEN	10

typedef struct {
	char name[32+1];			// Contents name (NULL terminator ASCII character string)
	short explain[50+1];	// Explanatory note (UTF16 LE character string)
	char param1[DWC_ND_FILEATTR_LEN+1];	// Attribute 1
	char param2[DWC_ND_FILEATTR_LEN+1];	// Attribute 2
	char param3[DWC_ND_FILEATTR_LEN+1];	// Attribute 3
	unsigned int size;	// File size
} DWCNdFileInfo;

typedef void* (*DWCNdAlloc)( u32 size, int align );
typedef void (*DWCNdFree)( void* ptr );
typedef void (*DWCNdCallback)( DWCNdCallbackReason reason, DWCNdError errpr, int servererror );
typedef void (*DWCNdCleanupCallback)( void );

BOOL DWCi_NdStartup( DWCNdAlloc alloc, DWCNdFree free, char* url, char* token, char* gamecd, char* passwd, DWCNdCallback callback );
void DWCi_NdCleanupAsync( DWCNdCleanupCallback callback );
DWCNdError DWCi_NdGetError( void );

BOOL DWCi_NdSetAttr( char* attr1, char* attr2, char* attr3 );
BOOL DWCi_NdGetFileListNumAsync( int* entrynum );
BOOL DWCi_NdGetFileListAsync( DWCNdFileInfo* filelist, int offset, int num );
BOOL DWCi_NdGetFileAsync( DWCNdFileInfo* filelist, char* buf, unsigned int bufsize );
BOOL DWCi_NdCancelAsync( void );
BOOL DWCi_NdGetProgress( u32* received, u32* contentlen );


#ifdef __cplusplus
}
#endif

#endif
