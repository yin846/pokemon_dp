/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_uncomp_stream.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_uncomp_stream.c,v $
  Revision 1.10  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.9  2005/02/18 06:40:47  yasu
  Signed/Unsigned conversion warning control

  Revision 1.8  2005/01/19 12:51:31  takano_makoto
  Changed the part using STRB to SWPB

  Revision 1.7  2005/01/18 00:18:28  takano_makoto
  fix comment.

  Revision 1.6  2004/12/21 03:32:06  takano_makoto
  Deleted the part where the pipeline stalls  

  Revision 1.5  2004/12/07 13:18:20  takano_makoto
  Assemblize to speed things up  

  Revision 1.4  2004/12/01 00:02:50  takano_makoto
  Function name change: MI_UncompReadXX -> MI_ReadUncompXX

  Revision 1.3  2004/11/30 10:51:41  takano_makoto
  Prepared a 16bit access function

  Revision 1.2  2004/11/30 08:30:59  takano_makoto
  Changed the return value of MI_InitUncompContextXXX to void

  Revision 1.1  2004/11/30 04:18:56  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/mi/stream.h>
#include <nitro/mi/uncompress.h>
#include <nitro/mi/uncomp_stream.h>

// #define MI_USE_STRB

/*---------------------------------------------------------------------------*
  Name: MI_InitUncompContextRL

  Description: Initializes the streaming uncompression context for run-length
                compressed data

  Arguments: context Pointer to the run-length uncompressed context
                dest        Destination address for uncompressed data
                header      Pointer to the header data of the compressed data   

  Returns: The data size after uncompression.  

 *---------------------------------------------------------------------------*/
void MI_InitUncompContextRL(MIUncompContextRL *context, u8 *dest, const MICompressionHeader *header)
{
    context->destp = dest;
    context->destCount = (s32)header->destSize;
    context->flags = 0;
    context->length = 0;
    context->destTmp = 0;
    context->destTmpCnt = 0;
}


/*---------------------------------------------------------------------------*
  Name: MI_InitUncompContextLZ

  Description: Initializes the streaming uncompression context for LZ compressed data.

  Arguments: context Pointer to the LZ uncompressed context
                dest        Destination address for uncompressed data
                header      Pointer to the header data of the compressed data   

 *---------------------------------------------------------------------------*/
void MI_InitUncompContextLZ(MIUncompContextLZ *context, u8 *dest, const MICompressionHeader *header)
{
    context->destp = dest;
    context->destCount = (s32)header->destSize;
    context->flags = 0;
    context->flagIndex = 0;
    context->length = 0;
    context->lengthFlg = 0;
    context->destTmp = 0;
    context->destTmpCnt = 0;
}


/*---------------------------------------------------------------------------*
  Name: MI_InitUncompContextHuffman

  Description: Initializes the streaming uncompression context for Huffman compressed data.

  Arguments: context Pointer to the Huffman uncompressed context
                dest        Destination address for uncompressed data
                header      Pointer to the header data of the compressed data   

 *---------------------------------------------------------------------------*/
void MI_InitUncompContextHuffman(MIUncompContextHuffman *context, u8 *dest,
                                 const MICompressionHeader *header)
{
    context->destp = dest;
    context->destCount = (s32)header->destSize;
    context->bitSize = (u8)header->compParam;
    context->treeSize = -1;
    context->treep = &context->tree[0];
    context->destTmp = 0;
    context->destTmpCnt = 0;
    context->srcTmp = 0;
    context->srcTmpCnt = 0;
}


//---- This code will be compiled in ARM-Mode
#include <nitro/code32.h>
/*---------------------------------------------------------------------------*
  Name: MI_ReadUncompRL8

  Description: This function performs streaming uncompression of run-length compressed data.
                Data is written in units of 8bits. Data cannot be directly uncompressed to VRAM.

  Arguments: context Pointer to the run-length uncompressed context
                data    Pointer to the next data
                len     Data size

  Returns: Size of remaining uncompressed data.

 *---------------------------------------------------------------------------*/
asm s32 MI_ReadUncompRL8( register MIUncompContextRL *context, register const u8* data, register u32 len )
{
                stmfd   sp!, {r4-r8}
                                                                // r0: context, r1: data, r2: len
                ldr     r3, [r0, #MIUncompContextRL.destp]      // r3: destp      = context->destp;
                ldr     r4, [r0, #MIUncompContextRL.destCount]  // r4: destCount  = context->destCount;
                ldrb    r5, [r0, #MIUncompContextRL.flags]      // r5: flags      = context->flags;
                ldrh    r6, [r0, #MIUncompContextRL.length]     // r6: length     = context->length
                
@41:
                cmp     r4, #0          //  while ( destCount > 0 )
                ble     @49
                
                tst     r5, #0x80       //     if ( ! flags & 0x80 ) {
                bne     @43
                
@42:
                cmp     r6, #0          //          while ( length > 0 ) {
                ble     @45
                
                sub     r6, r6, #1      //              length--;
                ldrb    r7, [r1], #1    //              *destp++ = *data++;
                sub     r4, r4, #1      //              destCount--;   // <- avoid pipe stall
#ifdef MI_USE_STRB
                strb    r7, [r3], #1
#else
                swpb    r8, r7,[r3]
                add     r3, r3, #1
#endif
                subs    r2, r2, #1      //              len--;
                beq     @49             //              if ( len == 0 ) { return destCount }
                b       @42             //          }
                                        
@43:                                    //      } else if ( length > 0 ) {
                cmp     r6, #0
                ble     @45
                
                ldrb    r7, [r1], #1    // r7:      srcTmp = *data++;
@44:                                    //          do {
                sub     r4, r4, #1      //              destCount--;    // <- avoid pipe stall
#ifdef MI_USE_STRB
                strb    r7, [r3], #1    //              *destp++ = srcTmp;
#else
                swpb    r8, r7, [r3]
                add     r3, r3, #1
#endif
                subs    r6, r6, #1      //          } while ( --length > 0 )
                bgt     @44
                
                subs    r2, r2, #1      //          len--;
                beq     @49             //          if ( len == 0 ) { return destCount }
                                        //      }
@45:
                ldrb    r5, [r1], #1    //      flags = *data++;
                and     r6, r5, #0x7F   //      length = flags & 0x7F
                tst     r5, #0x80       //      if ( flags & 0x80 ) { length += 3 }
                addne   r6, r6, #2      //      
                add     r6, r6, #1      //      else { length += 1 }
                
                subs    r2, r2, #1      //      len--;
                bne     @41             //      if ( len == 0 ) { return destCount }
                                        //  }
@49:
                str     r3, [r0, #MIUncompContextRL.destp]      // context->destp      = destp;
                str     r4, [r0, #MIUncompContextRL.destCount]  // context->destCount  = destCount;
                strb    r5, [r0, #MIUncompContextRL.flags]      // context->flags      = flags;
                strh    r6, [r0, #MIUncompContextRL.length]     // context->length     = length;
                mov     r0, r4
                
                ldmfd   sp!, {r4-r8}
                bx      lr
}


/*---------------------------------------------------------------------------*
  Name: MI_ReadUncompRL16

  Description: This function performs streaming uncompression of run-length compressed data.
                Data is written in units of 16bits. Data can be directly uncompressed to VRAM,
                but this function is slower than MI_ReadUncompRL8.

  Arguments: context Pointer to the run-length uncompressed context
                data    Pointer to the next data
                len     Data size

  Returns: Size of remaining uncompressed data.

 *---------------------------------------------------------------------------*/
asm s32 MI_ReadUncompRL16( register MIUncompContextRL *context, register const u8* data, register u32 len )
{
                stmfd   sp!, {r4-r9}
                                                                // r0: context, r1: data, r2: len
                ldr     r3, [r0, #MIUncompContextRL.destp]      // r3: destp      = context->destp;
                ldr     r4, [r0, #MIUncompContextRL.destCount]  // r4: destCount  = context->destCount;
                ldrb    r5, [r0, #MIUncompContextRL.flags]      // r5: flags      = context->flags;
                ldrh    r6, [r0, #MIUncompContextRL.length]     // r6: length     = context->length;
                ldrh    r7, [r0, #MIUncompContextRL.destTmp]    // r7: destTmp    = context->destTmp;
                ldrb    r8, [r0, #MIUncompContextRL.destTmpCnt] // r8: destTmpCnt = context->destTmpCnt;
                
@41:
                cmp     r4, #0              //  while ( destCount > 0 )
                ble     @49
                
                tst     r5, #0x80           //     if ( ! flags & 0x80 ) {
                bne     @44
                
@42:
                cmp     r6, #0              //          while ( length > 0 ) {
                ble     @47
                
                ldrb    r9, [r1], #1        //              destTmp |= (*data++) << destTmpCnt;
                sub     r6, r6, #1          //              length--;   // <- avoid pipe stall
                orr     r7, r7, r9, lsl r8
                add     r8, r8, #8          //              destTmpCnt += 8;
                cmp     r8, #16             //              if ( destTmpCnt == 16 ) {
                bne     @43
                strh    r7, [r3], #2        //                  *(u16*)destp = destTmp; destp += 2;
                mov     r7, #0              //                  destTmp = 0;
                mov     r8, #0              //                  destTmpCnt = 0;
                sub     r4, r4, #2          //                  destCount -= 2;
@43:                                        //              }
                subs    r2, r2, #1          //              len--;
                beq     @49                 //              if ( len == 0 ) { return destCount; }
                b       @42                 //          }
                                        
@44:                                        //      } else if ( length > 0 ) {
                cmp     r6, #0
                ble     @47

                ldrb    r9, [r1], #1        // r9:      srcTmp = *data++;
@45:                                        //          do {
                orr     r7, r7, r9, lsl r8  //              destTmp |= srcTmp << destTmpCnt;
                add     r8, r8, #8          //              destTmpCnt += 8;
                cmp     r8, #16             //              if ( destTmpCnt == 16 ) {
                bne     @46
                strh    r7, [r3], #2        //                  *(u16*)destp = destTmp; destp += 2;
                mov     r7, #0              //                  destTmp = 0;
                mov     r8, #0              //                  destTmpCnt = 0;
                sub     r4, r4, #2          //                  destCount -= 2;
                                            //              }
@46:
                subs    r6, r6, #1          //          } while ( --length > 0 );
                bgt     @45
                
                subs    r2, r2, #1          //          len--;
                beq     @49                 //          if ( len == 0 ) { return destCount; }
                                            //      }
@47:
                ldrb    r5, [r1], #1        //      flags = *data++;
                and     r6, r5, #0x7F       //      length = flags & 0x7F
                tst     r5, #0x80           //      if ( flags & 0x80 ) { length += 3 }
                addne   r6, r6, #2          //      
                add     r6, r6, #1          //      else { length += 1 }
                
                subs    r2, r2, #1          //      len--;
                bne     @41                 //      if ( len == 0 ) { return destCount }
                                            //  }
@49:
                str     r3, [r0, #MIUncompContextRL.destp]      // context->destp      = destp;
                str     r4, [r0, #MIUncompContextRL.destCount]  // context->destCount  = destCount;
                strb    r5, [r0, #MIUncompContextRL.flags]      // context->flags      = flags;
                strh    r6, [r0, #MIUncompContextRL.length]     // context->length     = length;
                strh    r7, [r0, #MIUncompContextRL.destTmp]    // context->destTmp    = destTmp;
                strb    r8, [r0, #MIUncompContextRL.destTmpCnt] // context->destTmpCnt = destTmpCnt;
                mov     r0, r4
                
                ldmfd   sp!, {r4-r9}
                bx      lr
}

/*---------------------------------------------------------------------------*
  Name: MI_ReadUncompLZ8

  Description: This function performs streaming uncompression of LZ compressed data.
                Data is written in units of 8bits. Data cannot be directly uncompressed to VRAM.

  Arguments: context Pointer to the LZ uncompressed context
                data    Pointer to the next data
                len     Data size

  Returns: Size of remaining uncompressed data.

 *---------------------------------------------------------------------------*/
asm s32 MI_ReadUncompLZ8( register MIUncompContextLZ *context, register const u8* data, register u32 len )
{
                stmfd   sp!, {r4-r10}
                                                                // r0: context, r1: data, r2: len
                ldr     r3, [r0, #MIUncompContextLZ.destp]      // r3: destp      = context->destp;
                ldr     r4, [r0, #MIUncompContextLZ.destCount]  // r4: destCount  = context->destCount;
                ldrb    r5, [r0, #MIUncompContextLZ.flags]      // r5: flags      = context->flags;
                ldrb    r6, [r0, #MIUncompContextLZ.flagIndex]  // r6: flagIndex  = context->flagIndex;
                ldrb    r7, [r0, #MIUncompContextLZ.length]     // r7: length     = context->length
                ldrb    r8, [r0, #MIUncompContextLZ.lengthFlg]  // r8: lengthFlg  = context->lengthFlg;
                
@21:
                cmp     r4, #0                  //  while ( destCount > 0 ) {
                ble     @29
                
                cmp     r6, #0                  //      wihle ( flagIndex > 0 ) {
                beq     @28
@22
                cmp     r2, #0                  //          if ( len == 0 ) { return destCount }
                beq     @29
                
                tst     r5, #0x80               //          if ( ! flags & 0x80 ) {
                bne     @23
                ldrb    r9, [r1], #1            //              *destp++ = *srcp++;
                sub     r4, r4, #1              //              destCount--;    // <- avoid pipe stall
                sub     r2, r2, #1              //              len--;
#ifdef MI_USE_STRB
                strb    r9, [r3], #1
#else
                swpb    r9, r9, [r3]
                add     r3, r3, #1
#endif
                b       @26
@23:                                            //          } else {
                cmp     r8, #0                  //              if ( lengthFlg == 0 ) {
                bne     @24
                ldrb    r7, [r1], #1            //                  length = *data++;
                mov     r8, #1                  //                  lengthFlg = 1;
                sub     r2, r2, #1              //                  len--
                                                //              }
@24:
                cmp     r2, #0                  //              if ( len == 0 ) { return destCount }
                beq     @29
                
                and     r9, r7, #0xF            //  r9:         offset = ( length & 0xF ) << 8;
                mov     r10, r9, lsl #8         
                ldrb    r9, [r1], #1            //              offset = ( offset | *data++ ) + 1;
                mov     r8, #0                  //              lengthFlg = 0;  // <- avoid pipe stall
                sub     r2, r2, #1              //              len--;
                orr     r9, r9, r10
                add     r9, r9, #1
                mov     r10, #3                 //              length = ( length >> 4 ) + 3;
                adds    r7, r10, r7, asr #4
                beq     @26                     //              while ( length > 0 ) {
@25:
                ldrb    r10, [r3, -r9]          //                  *destp++ = *(destp - offset);
                sub     r4, r4, #1              //                  destCount--;    // <- avoid pipe stall
#ifdef MI_USE_STRB
                strb    r10, [r3], #1
#else
                swpb    r10, r10, [r3]
                add     r3, r3, #1
#endif
                subs    r7, r7, #1              //                  length--;
                bgt     @25                     //              }
@26:                                            //          }
                cmp     r4, #0                  //          if ( destCount == 0 ) { return destCount }
                beq     @29
                mov     r5, r5, lsl #1          //          flags <<= 1;
                subs    r6, r6, #1              //          flagIndex--;
                bne     @22                     //      }
                
@28:
                cmp     r2, #0                  //      if ( len == 0 ) { return destCount }
                beq     @29
                ldrb    r5, [r1], #1            //      flags = *data++;
                mov     r6, #8                  //      flagIndex = 8;
                sub     r2, r2, #1              //      len--
                b       @21                     //  }
                
@29:
                str     r3, [r0, #MIUncompContextLZ.destp]      // context->destp      = destp;
                str     r4, [r0, #MIUncompContextLZ.destCount]  // context->destCount  = destCount;
                strb    r5, [r0, #MIUncompContextLZ.flags]      // context->flags      = flags;
                strb    r6, [r0, #MIUncompContextLZ.flagIndex]  // context->flagIndex  = flagIndex;
                strb    r7, [r0, #MIUncompContextLZ.length]     // context->length     = length;
                strb    r8, [r0, #MIUncompContextLZ.lengthFlg]  // context->lengthFlg  = lengthFlg;
                mov     r0, r4                  // return destCount
                
                ldmfd   sp!, {r4-r10}
                bx      lr
                
}


/*---------------------------------------------------------------------------*
  Name: MI_ReadUncompLZ16

  Description: This function performs streaming uncompression of LZ compressed data.
                Data is written in units of 16bits. Data can be directly uncompressed to VRAM,
                but this function is slower than MI_ReadUncompLZ8

  Arguments: context Pointer to the LZ uncompressed context
                data    Pointer to the next data
                len     Data size

  Returns: Size of remaining uncompressed data.

 *---------------------------------------------------------------------------*/
asm s32 MI_ReadUncompLZ16( register MIUncompContextLZ *context, register const u8* data, register u32 len )
{
                stmfd   sp!, {r4-r12,lr}
                                                                 // r0: context, r1: data, r2: len
                ldr     r3, [r0, #MIUncompContextLZ.destp]       // r3: destp      = context->destp;
                ldr     r4, [r0, #MIUncompContextLZ.destCount]   // r4: destCount  = context->destCount;
                ldrb    r5, [r0, #MIUncompContextLZ.flags]       // r5: flags      = context->flags;
                ldrb    r6, [r0, #MIUncompContextLZ.flagIndex]   // r6: flagIndex  = context->flagIndex;
                ldrb    r7, [r0, #MIUncompContextLZ.length]      // r7: length     = context->length
                ldrb    r8, [r0, #MIUncompContextLZ.lengthFlg]   // r8: lengthFlg  = context->lengthFlg;
                ldrh    r9, [r0, #MIUncompContextLZ.destTmp]     // r9: destTmp    = context->destTmp;
                ldrb    r10, [r0, #MIUncompContextLZ.destTmpCnt] // r10: destTmpCnt = context->destTmpCnt;
                
@21:
                cmp     r4, #0                  //  while ( destCount > 0 ) {
                ble     @29
                
                cmp     r6, #0                  //      wihle ( flagIndex > 0 ) {
                beq     @28
@22
                cmp     r2, #0                  //          if ( len == 0 ) { return destCount }
                beq     @29
                
                tst     r5, #0x80               //          if ( ! flags & 0x80 ) {
                bne     @23
                ldrb    r11, [r1], #1           //              destTmp |= (*data++) << destTmpCnt;
                sub     r2, r2, #1              //              len--;  // <- avoid pipe stall
                orr     r9, r9, r11, lsl r10
                add     r10, r10, #8            //              destTmpCnt += 8;
                cmp     r10, #16                //              if ( destTmpCnt == 16 ) {
                bne     @26
                strh    r9, [r3], #2            //                  *(u16*)destp = destTmp; destp += 2;
                sub     r4, r4, #2              //                  destCount -= 2;
                mov     r9, #0                  //                  destTmp = 0;
                mov     r10, #0                 //                  destTmpCount = 0;
                                                //              }
                b       @26
@23:                                            //          } else {
                cmp     r8, #0                  //              if ( lengthFlg == 0 ) {
                bne     @24
                ldrb    r7, [r1], #1            //                  length = *data++;
                mov     r8, #1                  //                  lengthFlg = 1;
                sub     r2, r2, #1              //                  len--
                                                //              }
@24:
                cmp     r2, #0                  //              if ( len == 0 ) { return destCount }
                beq     @29
                
                and     r11, r7, #0xF           //  r11:        offset = ( length & 0xF ) << 8;
                mov     r12, r11, lsl #8         
                ldrb    r11, [r1], #1           //              offset = ( offset | *data++ ) + 1;
                mov     r8, #0                  //              lengthFlg = 0;  // <- avoid pipe stall
                sub     r2, r2, #1              //              len--
                orr     r11, r11, r12
                add     r11, r11, #1
                mov     r12, #3                 //              length = ( length >> 4 ) + 3;
                adds    r7, r12, r7, asr #4
                beq     @26                     //              while ( length > 0 ) {
@25:
                subs    r12, r11, r10, lsr #3   //  r12:            offsetTmp = offset - ( destTmpCnt / 8 );
                bne     @25_1                   //                  if ( offsetTmp == 0 ) {
                and     r14, r9, #0xF           //                      destTmp |= ( destTmp & 0xF ) << 8;
                orr     r9, r9, r14, lsl #8
                b       @25_2
@25_1:                                          //                  } else {
                add     r14, r12, #1            //  r14:                tmpData = *(u16*)( destp - ( (offsetTmp + 1) & ~0x1 ) )
                mov     r14, r14, lsr #1
                sub     r14, r3, r14, lsl #1
                ldrh    r14, [r14, #0]
                tst     r12, #1                 //                      if ( offsetTmp & 1 ) {
                movne   r14, r14, lsr #8        //                          tmpData >>= 8;
                                                //                      } else {
                andeq   r14, r14, #0xFF         //                          tmpData &= 0xFF;
                                                //                      }
                orr     r9, r9, r14, lsl r10    //                      destTmp |= tmpData << destTmpCnt;
@25_2:                                          //                  }
                add     r10, r10, #8            //                  destTmpCnt += 8;
                cmp     r10, #16                //                  if ( destTmpCnt == 16 ) {
                bne     @25_3
                strh    r9, [r3], #2            //                      *(u16*)destp = destTmp; destp += 2;
                sub     r4, r4, #2              //                      destCount -= 2;
                mov     r9, #0                  //                      destTmp = 0;
                mov     r10, #0                 //                      destTmpCnt = 0;
                                                //                  }
@25_3:
                subs    r7, r7, #1              //                  length--;
                bgt     @25                     //              }
@26:                                            //          }
                cmp     r4, #0                  //          if ( destCount == 0 ) { return destCount }
                beq     @29
                mov     r5, r5, lsl #1          //          flags <<= 1;
                subs    r6, r6, #1              //          flagIndex--;
                bne     @22                     //      }
                
@28:
                cmp     r2, #0                  //      if ( len == 0 ) { return destCount }
                beq     @29
                ldrb    r5, [r1], #1            //      flags = *data++;
                mov     r6, #8                  //      flagIndex = 8;
                sub     r2, r2, #1              //      len--
                b       @21                     //  }
                
@29:
                str     r3, [r0, #MIUncompContextLZ.destp]       // context->destp      = destp;
                str     r4, [r0, #MIUncompContextLZ.destCount]   // context->destCount  = destCount;
                strb    r5, [r0, #MIUncompContextLZ.flags]       // context->flags      = flags;
                strb    r6, [r0, #MIUncompContextLZ.flagIndex]   // context->flagIndex  = flagIndex;
                strb    r7, [r0, #MIUncompContextLZ.length]      // context->length     = length;
                strb    r8, [r0, #MIUncompContextLZ.lengthFlg]   // context->lengthFlg  = lengthFlg;
                strh    r9, [r0, #MIUncompContextLZ.destTmp]     // context->destTmp    = destTmp;
                strb    r10, [r0, #MIUncompContextLZ.destTmpCnt] // context->destTmpCnt = destTmpCnt;
                mov     r0, r4                  // return destCount
                
                ldmfd   sp!, {r4-r12,lr}
                bx      lr
}


/*---------------------------------------------------------------------------*
  Name: MI_ReadUncompHuffman

  Description: This function performs streaming uncompression of Huffman compressed data.

  Arguments: context Pointer to the Huffman uncompressed context
                data    Pointer to the next data
                len     Data size

  Returns: Size of remaining uncompressed data.

 *---------------------------------------------------------------------------*/
#define TREE_END_MASK   0x80

asm s32 MI_ReadUncompHuffman( register MIUncompContextHuffman *context, register const u8* data, register u32 len )
{
                stmfd   sp!, {r4-r12, lr}
                                                                      // r0: context, r1: data, r2: len
                ldr     r3, [r0, #MIUncompContextHuffman.destp]       // r3: destp      = context->destp;
                ldr     r4, [r0, #MIUncompContextHuffman.destCount]   // r4: destCount  = context->destCount;
                ldr     r5, [r0, #MIUncompContextHuffman.treep]       // r5: treep      = context->treep;
                ldr     r6, [r0, #MIUncompContextHuffman.srcTmp]      // r6: srcTmp     = context->srcTmp;
                ldr     r7, [r0, #MIUncompContextHuffman.destTmp]     // r7: destTmp    = context->destTmp;
                ldrsh   r8, [r0, #MIUncompContextHuffman.treeSize]    // r8: treeSize   = context->treeSize;
                ldrb    r9, [r0, #MIUncompContextHuffman.srcTmpCnt]   // r9: srcTmpCnt  = context->srcTmpCnt;
                ldrb    r10, [r0, #MIUncompContextHuffman.destTmpCnt] // r10: destTmpCnt = context->destTmpCnt;
                ldrb    r11, [r0, #MIUncompContextHuffman.bitSize]    // r11: bitSize   = context->bitSize;
                
                cmp     r8, #0                                  //  if ( treeSize < 0 ) {
                beq     @12
                bgt     @11
                ldrb    r8, [r1], #1                            //      treeSize = ( *data + 1 ) * 2 - 1;
                sub     r2, r2, #1                              //      len--;  // <- avoid pipe stall
                strb    r8, [r5], #1                            //      *treep++ = *data++;
                add     r8, r8, #1
                mov     r8, r8, lsl #1
                sub     r8, r8, #1
@11:                                                            //  }
                                                                //  while ( treeSize > 0 ) {
                cmp     r2, #0                                  //      if ( len == 0 ) { return destCount; }
                beq     @19
                ldrb    r12, [r1], #1                           //      *treep++ = *data++;
                sub     r2, r2, #1                              //      len--;  // <- avoid pipe stall
                strb    r12, [r5], #1
                subs    r8, r8, #1                              //      treeSize--;
                addeq   r5, r0, #MIUncompContextHuffman.tree[1] //      if ( treeSize == 0 ) { treep = &context->tree[ 1 ]; }
                bgt     @11                                     //  }
@12:
                cmp     r4, #0                                  //  while ( destCount > 0 ) {
                ble     @19
@13:
                cmp     r9, #32                                 //      while ( srcTmpCnt < 32 ) {
                bge     @15
@14:
                cmp     r2, #0                                  //          if ( len == 0 ) { return destCount; }
                beq     @19
                ldr     r12, [r1], #1                           //          srcTmp |= (*data++) << srcTmpCnt;
                sub     r2, r2, #1                              //          len--;  // <- avoid pipe stall
                orr     r6, r6, r12, lsl r9
                add     r9, r9, #8                              //          srcTmpCnt += 8;
                cmp     r9, #32                                 //      }
                blt     @14
@15:                                                            //      do {
                mov     r12, r6, lsr #31                        // r12:     select = srcTmp >> 31;
                ldrb    r14, [r5, #0]                           // r14:     endFlag = ( *treep << select ) & TREE_END_MASK;
                mov     r5, r5, lsr #1                          //          treep = ( treep & ~0x1 ) + ( ( (*treep & 0x3F) + 1 ) * 2 ) + select;
                mov     r5, r5, lsl #1
                and     r8, r14, #0x3F
                add     r8, r8, #1
                add     r5, r5, r8, lsl #1
                add     r5, r5, r12
                mov     r8, #0
                mov     r14, r14, lsl r12
                ands    r14, r14, #TREE_END_MASK
                mov     r6, r6, lsl #1                          //          srcTmp <<= 1;
                sub     r9, r9, #1                              //          srcTmpCnt--;
                beq     @17                                     //          if ( ! endFlag ) { continue; }
                
                mov     r7, r7, lsr r11                         //          destTmp >>= bitSize;
                ldrb    r12, [r5, #0]                           //          destTmp |= *treep << ( 32 - bitSize );
                rsb     r14, r11, #32
                orr     r7, r7, r12, lsl r14
                add     r5, r0, #MIUncompContextHuffman.tree[1] //          treep = &context->tree[ 1 ];
                add     r10, r10, r11                           //          destTmpCnt += bitSize;
                cmp     r10, #32                                //          if ( destTmpCnt == 32 ) {
                bne     @17
                str     r7, [r3], #4                            //              *(u32*)destp = destTmp; destp += 4;
                mov     r10, #0                                 //              destTmpCnt = 0;
                subs    r4, r4, #4                              //              destCount -= 4;
                movle   r4, #0                                  //              if ( destCount <= 0 ) { return 0 };
                ble     @19
                                                                //          }
@17:
                cmp     r9, #0                                  //      } while ( srcTmpCnt > 0 );
                bgt     @15
                cmp     r4, #0                                  //  }
                bgt     @13
                
@19:             
                str     r3, [r0, #MIUncompContextHuffman.destp]       // r3: context->destp       = destp;
                str     r4, [r0, #MIUncompContextHuffman.destCount]   // r4: context->destCount   = destCount;
                str     r5, [r0, #MIUncompContextHuffman.treep]       // r5: context->treep       = treep;
                str     r6, [r0, #MIUncompContextHuffman.srcTmp]      // r6: context->srcTmp      = srcTmp;
                str     r7, [r0, #MIUncompContextHuffman.destTmp]     // r7: context->destTmp     = destTmp;
                strh    r8, [r0, #MIUncompContextHuffman.treeSize]    // r8: context->treeSize    = treeSize;
                strb    r9, [r0, #MIUncompContextHuffman.srcTmpCnt]   // r9: context->srcTmpCnt   = srcTmpCnt;
                strb    r10, [r0, #MIUncompContextHuffman.destTmpCnt] // r10: context->destTmpCnt = destTmpCnt;
                strb    r11, [r0, #MIUncompContextHuffman.bitSize]    // r11: context->bitSize    = bitSize;
                mov     r0, r4                                  //  return destCount
                
                ldmfd   sp!, {r4-r12, lr}
                bx      lr
    
}

//---- end limitation of processer mode
#include <nitro/codereset.h>

