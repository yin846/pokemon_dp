/*--------------------------------------------------------------------------
  Project:  NHTTP
  File:     NHTTP.h

  Copyright 2006 Nintendo.  All rights reserved.

  $Log: NHTTP.h,v $
  Revision 1.11  07/19/2006 00:48:36  nakata
  Synchronized the source code with the Denyu-sha CVS server

  Revision 1.10  06/05/2006 01:25:42  nakata
  Corrected an operational error caused by a the linking of a conditional expression due to an arithmetic operator

  Revision 1.10  06/02/2006 01:05:01  aida
  modified conditional operator

  Revision 1.9  05/17/2006 05:45:00  aida
  modified macro name

  Revision 1.8  05/12/2006 05:49:03  aida
  modified macro name

  Revision 1.7  04/25/2006 05:06:20  aida
  changed copyright description

  Revision 1.6  04/17/2006 06:53:54  aida
  changed k-option to -kkv

  Revision 1.5  03/28/2006 09:38:51  aida
  no message

  $NoKeywords: $
 *-------------------------------------------------------------------------*/
#ifndef NHTTP_NHTTP_H
#define NHTTP_NHTTP_H

#if ((defined WIN32) || (defined _WIN32))

#pragma once
#define BOOL int
#define TRUE 1
#define FALSE 0
#define SDK_ASSERT(XX)	_ASSERT(XX)
typedef unsigned long u32;
typedef struct _SOCCaInfo {
 	char* caname;
 	int casize;
 	unsigned char* cabody;
 	int caexpsize;
 	unsigned char* caexp;
} SOCCaInfo, CPSCaInfo;
typedef struct _SOCSslConnection {
	char *server_name;				// server name to match
	int (*auth_callback)(int, struct _CPSSslConnection *, int);
	SOCCaInfo **ca_info;
	int ca_builtins;
} SOCSslConnection;
#include <stdlib.h>
#include <crtdbg.h>

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

// Stack size of networking thread
#define NHTTP_COMMTHREAD_STACK	(8*1024)	// [byte]

// TCP port number for HTTP and HTTPS
#define NHTTP_HTTP_PORT			80
#define NHTTP_HTTPS_PORT		443

// Character string attached to the HTTP request header   such as GET / HTTP/1.1
#define NHTTP_STR_HTTPVER		"1.1"	

// Size of the initial reception buffer of the entity header (defined in the specification)
#define NHTTP_HDRRECVBUF_INILEN			1024	// [byte]

// Block length when expanding the reception buffer of the entity header (defined in the specification)
#define NHTTP_HDRRECVBUF_BLOCKSHIFT	9
#define NHTTP_HDRRECVBUF_BLOCKLEN		(1<<NHTTP_HDRRECVBUF_BLOCKSHIFT)	// [byte]
#define NHTTP_HDRRECVBUF_BLOCKMASK	(NHTTP_HDRRECVBUF_BLOCKLEN-1)


typedef enum {
	NHTTP_ERROR_NONE,          // 0
	NHTTP_ERROR_ALLOC,         // 1
	NHTTP_ERROR_TOOMANYREQ,    // 2
	NHTTP_ERROR_SOCKET,        // 3
	NHTTP_ERROR_DNS,           // 4
	NHTTP_ERROR_CONNECT,       // 5
	NHTTP_ERROR_BUFFULL,       // 6
	NHTTP_ERROR_HTTPPARSE,     // 7
	NHTTP_ERROR_CANCELED,      // 8
	NHTTP_ERROR_NITROSDK,      // 9
	NHTTP_ERROR_NITROWIFI,     // 10
	NHTTP_ERROR_UNKNOWN,       // 11
	NHTTP_ERROR_MAX
} NHTTPError;

typedef enum {
	NHTTP_REQMETHOD_GET,
	NHTTP_REQMETHOD_POST,
	NHTTP_REQMETHOD_HEAD,
	NHTTP_REQMETHOD_MAX
} NHTTPReqMethod;

typedef struct _NHTTPRes  NHTTPRes;
typedef struct _NHTTPReq  NHTTPReq;
typedef void* (*NHTTPAlloc)( u32 size, int align );
typedef void (*NHTTPFree)( void* ptr );
typedef void (*NHTTPCleanupCallback)( void );
typedef void (*NHTTPReqCallback)( NHTTPError error, NHTTPRes* res, void* param );


BOOL NHTTP_Startup( NHTTPAlloc alloc, NHTTPFree free, u32 threadprio );
void NHTTP_CleanupAsync( NHTTPCleanupCallback callback );
NHTTPError NHTTP_GetError(void);

NHTTPReq* NHTTP_CreateRequest( char* url, NHTTPReqMethod method, char* buf, u32 len, NHTTPReqCallback callback, void* param );
void NHTTP_DestroyRequest( NHTTPReq* req );
BOOL NHTTP_AddHeaderField( NHTTPReq* req, char* label, char* value );
BOOL NHTTP_AddPostDataAscii( NHTTPReq* req, char* label, char* value );
BOOL NHTTP_AddPostDataBinary( NHTTPReq* req, char* label, char* value, u32 length );
BOOL NHTTP_SetCAChain( NHTTPReq* req, SOCCaInfo** cainfo, int calength );

int NHTTP_SendRequestAsync( NHTTPReq* req );
BOOL NHTTP_CancelRequestAsync( int id );
BOOL NHTTP_GetProgress( u32* received, u32* contentlen );
void NHTTP_DestroyResponse( NHTTPRes* res );
int NHTTP_GetHeaderField( NHTTPRes* res, char* label, char** value );
int NHTTP_GetHeaderAll( NHTTPRes* res, char** value );
int NHTTP_GetBodyAll( NHTTPRes* res, char** value );


#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////

#endif
