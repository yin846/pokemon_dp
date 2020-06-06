/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - include
  File:     uncomp_stream.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: uncomp_stream.h,v $
  Revision 1.7  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.6  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.5  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.4  12/01/2004 00:03:12  takano_makoto
  Function name change: MI_UncompReadXX -> MI_ReadUncompXX

  Revision 1.3  11/30/2004 10:51:52  takano_makoto
  Prepared a 16bit access function

  Revision 1.1  11/30/2004 04:18:26  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_UNCOMP_STREAM_H_
#define NITRO_MI_UNCOMP_STREAM_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    u8     *destp;                     // Write-destination pointer               4B
    s32     destCount;                 // Remaining size to write                 4B
    u16     destTmp;                   // Data being written                     2B
    u8      destTmpCnt;                // Size of data being written             1B
    u8      flags;                     // Compression flag                       1B
    u16     length;                    // Remaining size of continuous write  2B
    u8      _padding[2];               //                             2B
    //                         Total is 16B
}
MIUncompContextRL;


typedef struct
{
    u8     *destp;                     // Write-destination pointer               4B
    s32     destCount;                 // Remaining size to write                 4B
    u16     destTmp;                   // Data being written                     2B
    u8      destTmpCnt;                // Size of data being written             1B
    u8      flags;                     // Compression flag                       1B
    u8      flagIndex;                 // Current compression flag index         1B
    u8      length;                    // Remaining length of continuous write   1B
    u8      lengthFlg;                 // Length-acquired flag                   1B
    u8      _padding[1];               //                                 1B
    //                             Total is 16B
}
MIUncompContextLZ;


typedef struct
{
    u8     *destp;                     // Write-destination pointer               4B
    s32     destCount;                 // Remaining size to write                 4B
    u8     *treep;                     // Huffman coding table, current pointer   4B    
    u32     srcTmp;                    // Data being read                         4B
    u32     destTmp;                   // Data being decoded                      4B
    s16     treeSize;                  // Size of Huffman encoding table          2B    
    u8      srcTmpCnt;                 // Size of data being read                 1B
    u8      destTmpCnt;                // Number of bits that have been decoded   1B
    u8      bitSize;                   // Size of encoded bits                    1B
    u8      _padding2[3];              //                                        3B
    u8      tree[0x200];               // Huffman encoding table                512B (32B OK if 4bit encoding, but enough secured for 8bit encoding)    
    //                                   Total = 540B   (60B sufficient if 4bit encoding)    
}
MIUncompContextHuffman;


void    MI_InitUncompContextRL(MIUncompContextRL *context, u8 *dest,
                               const MICompressionHeader *header);
void    MI_InitUncompContextLZ(MIUncompContextLZ *context, u8 *dest,
                               const MICompressionHeader *header);
void    MI_InitUncompContextHuffman(MIUncompContextHuffman *context, u8 *dest,
                                    const MICompressionHeader *header);

s32     MI_ReadUncompRL8(MIUncompContextRL *context, const u8 *data, u32 len);
s32     MI_ReadUncompRL16(MIUncompContextRL *context, const u8 *data, u32 len);
s32     MI_ReadUncompLZ8(MIUncompContextLZ *context, const u8 *data, u32 len);
s32     MI_ReadUncompLZ16(MIUncompContextLZ *context, const u8 *data, u32 len);
s32     MI_ReadUncompHuffman(MIUncompContextHuffman *context, const u8 *data, u32 len);

#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_MI_UNCOMP_STREAM_H_ */
#endif
