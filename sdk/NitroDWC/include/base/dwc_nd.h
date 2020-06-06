/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  -
  File:     dwc_nd.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_nd.h,v $
  Revision 1.7  05/15/2006 01:18:19  nakata
  Added support for the 5/12 vserion of the DWC_Nd library

  Revision 1.6  05/12/2006 11:25:10  nakata
  Added support for Ooki's remarks

  Revision 1.5  05/10/2006 11:59:21  nakata
  Made changes to the initialization functions in accordance with the changes to the Nd library specifications

  Revision 1.4  05/02/2006 00:27:15  nakata
  Updated DWC_nd to the 5/2 version

  Revision 1.3  04/28/2006 12:26:58  nakata
  Suppressed the warning when compiling DWC_ND_PARAMERR

  Revision 1.2  04/28/2006 10:09:02  nakata
  Added a password to DWC_NdInitAsync
  Changed functions in accordance with the changes to the DWC_NdGetFileAsync specification
  Added revised headers that go along with each of the constants in the guidelines

  Revision 1.1  04/25/2006 11:46:56  nakata
  Added the DWC_Nd wrapper



  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_ND_H_
#define DWC_ND_H_

#include <nd/DWCi_Nd.h>

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        enum
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        typedef - function
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:			DWC_NdInitAsync

  Description:	The Nd library initialization function

  Arguments:	callback - The pointer to the notification callback function that the Nd library calls.
                gamecd - The game code needed to access the download server
                passwd - The password needed to access the download server
  Returns:		TRUE  - Initialization successful
				FALSE - Initialization failed
 *---------------------------------------------------------------------------*/
BOOL DWC_NdInitAsync(DWCNdCallback callback, char *gamecd, char *passwd);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdCleanupAsync

  Description:	The Nd library deallocation function

  Arguments:	callback - The pointer to the deallocation complete notification callback function that the Nd library calls.
  Returns:		None
 *---------------------------------------------------------------------------*/
void DWC_NdCleanupAsync(DWCNdCleanupCallback callback);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdSetAttr

  Description:	The function for configuring attributes at download

  Arguments:	attr1 - Attribute character string 1
				attr2 - Attribute character string 2
				attr3 - Attribute character string 3
  Returns:		TRUE  - Attribute configuration successful
				FALSE - Attribute configuration failed
 *---------------------------------------------------------------------------*/
BOOL DWC_NdSetAttr(const char* attr1, const char* attr2, const char* attr3);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetFileListNumAsync

  Description:	A function for starting the process for getting the total number of file list entries.

  Arguments:	entrynum - The pointer to the region that stores the total number of file list entries when networking is complete
  Returns:		TRUE  - Waiting for the process that started networking to complete
				FALSE - Process failure
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileListNumAsync(int* entrynum);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdFileListAsync

  Description:	A function that starts the process for obtaining the file list.

  Arguments:	filelist - The pointer to the array where the obtained file list is stored
				offset   - The starting position of the offset of the obtained file list
				num      - The maximum length of the file list to obtain
  Returns:		TRUE  - Waiting for the process that started networking to complete
				FALSE - Process failure
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileListAsync(DWCNdFileInfo* filelist, unsigned int offset, unsigned int num);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetFileAsync

  Description:	A function that starts the process for obtaining the file.

  Arguments:	filename - The file name to be obtained
				buf      - The pointer to the region where the obtained file data is stored
				size     - The size of the region where the obtained file data is stored
  Returns:		TRUE  - Waiting for the process that started networking to complete
				FALSE - Process failure
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileAsync(DWCNdFileInfo *fileinfo, char* buf, unsigned int size);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdCancelAsync

  Description:	A function that notifies of a disconnect in a running process.

  Arguments:	None
  Returns:		TRUE  - Process successfully interrupted, and waiting for process to complete
				FALSE - Process failure
 *---------------------------------------------------------------------------*/
BOOL DWC_NdCancelAsync(void);

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetProgress

  Description:	A function that gets the progress of a running process.

  Arguments:	received   - The pointer to the region where the amount of bytes of received data are stored
				contentlen - The pointer to the region where the received data content length is stored
  Returns:		TRUE  - Progress successfully obtained
				FALSE - Failed to obtain progress, or process is not running
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetProgress(u32* received, u32* contentlen);

#ifdef __cplusplus
}
#endif

#endif // DWC_ND_H_
