/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     baseCommoni.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: baseCommoni.h,v $
  Revision 1.3  01/21/2005 04:51:16 AM  kitamurs
  Changed year

  Revision 1.2  12/20/2004 05:02:26 AM  kitamurs
  Connection confirmation process on server

  Revision 1.1  12/16/2004 07:25:03 AM  kitamurs
  Newly created the Sleep function

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_BASECOMMONI_H_
#define NNS_MCS_BASECOMMONI_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    Constant definitions
   ======================================================================== */

#define NNSi_MCS_ENS_READ_RB_SIZE       (64 * 1024)
#define NNSi_MCS_ENS_READ_RB_START      0x10000000
#define NNSi_MCS_ENS_READ_RB_END        (NNSi_MCS_ENS_READ_RB_START + NNSi_MCS_ENS_READ_RB_SIZE)

#define NNSi_MCS_ENS_WRITE_RB_SIZE      NNSi_MCS_ENS_READ_RB_SIZE
#define NNSi_MCS_ENS_WRITE_RB_START     NNSi_MCS_ENS_READ_RB_END
#define NNSi_MCS_ENS_WRITE_RB_END       (NNSi_MCS_ENS_WRITE_RB_START + NNSi_MCS_ENS_WRITE_RB_SIZE)

#define NNSi_MCS_MESSAGE_SIZE_MAX       (1024 * 32)

#define NNSi_MCS_MSG_HEAD_SIZE          sizeof(NNSiMcsMsgHead)

#define NNSi_MCS_MSG_DATA_SIZE_MAX      (NNSi_MCS_MESSAGE_SIZE_MAX - NNSi_MCS_MSG_HEAD_SIZE)
#define NNSi_MCS_MSG_DATA_SIZE_MIN      (1024 * 8 - NNSi_MCS_MSG_HEAD_SIZE)

#define NNSi_MCS_MSG_HEAD_VERSION       1

static const uint16_t   NNSi_MCS_SYSMSG_CHANNEL = 0xFFFF;


/* ========================================================================
    Type Definitions
   ======================================================================== */

typedef struct NNSiMcsMsgHead NNSiMcsMsgHead;
struct NNSiMcsMsgHead
{
    uint16_t    version;
    uint16_t    reserved;
    uint32_t    offset;
    uint32_t    totalLen;
};

typedef struct NNSiMcsMsg NNSiMcsMsg;
struct NNSiMcsMsg
{
    NNSiMcsMsgHead  head;
    uint8_t         data[NNSi_MCS_MSG_DATA_SIZE_MAX];
};

typedef struct NNSiMcsEnsMsgBuf NNSiMcsEnsMsgBuf;
struct NNSiMcsEnsMsgBuf
{
    uint32_t    channel;
    uint8_t     buf[NNSi_MCS_MESSAGE_SIZE_MAX];
};


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_MCS_BASECOMMONI_H_*/
#endif

