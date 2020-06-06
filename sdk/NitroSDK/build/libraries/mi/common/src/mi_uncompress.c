/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI -
  File:     mi_uncompress.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_uncompress.c,v $
  Revision 1.13  2005/01/30 23:58:00  takano_makoto
  fix copyright header.

  Revision 1.12  2005/01/28 13:10:48  takano_makoto
  fixed comments.

  Revision 1.11  2004/11/15 08:43:47  takano_makoto
  Optimized the MI_UnfilterDiff function code.

  Revision 1.10  2004/11/15 08:34:27  takano_makoto
  Revised to eliminate pipeline stall.

  Revision 1.8  2004/07/20 07:31:34  yada
  MI_UnpackBitsParam->MIUnpackBitsParam

  Revision 1.7  2004/05/27 11:06:58  yada
  fix bug in MI_UncompressRL8().
  bug was that swpb destroys r5.

  Revision 1.6  2004/05/27 05:13:32  yada
  fix bug in MI_UncompressRL8().  bcs -> bne

  Revision 1.5  2004/02/13 06:28:35  yada
  only change comment

  Revision 1.4  2004/02/10 06:39:14  yada
  Changed function name. UnComp -> UnCompress, etc.

  Revision 1.3  2004/02/10 05:10:07  yada
  Change function names. -Byte -> -8. -short -> -16

  Revision 1.2  2004/02/10 02:28:25  yada
  Store in code32.

  Revision 1.1  2004/02/10 01:22:22  yada
  Move from UTL_.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/types.h>
#include <nitro/mi/uncompress.h>

// ****Bug fix****
//  Because of CW bug, the half word access instructions ldrh or strh
//  do not pass through the inline assembler.  To get around this problem
//  an instruction value is written to the value with dcd.
//  I will remove the following define.
//#define CW_BUG_FOR_LDRH_AND_STRH

//---- This code will be compiled in ARM-Mode
#include <nitro/code32.h>

//======================================================================
//          Expanding compressed data
//======================================================================
//----------------------------------------------------------------------
//          Expanding bit compressed data
//
//- Expands 0 fixed bit packed data.
//- Align the destination address to a 4-byte boundary.
//
//- Arguments:
//             void *srcp              source address
//             void *destp             destination address
//  MIUnpackBitsParam *paramp   Address of MIUnpackBitsParam structure
//
//- MIUnpackBitsParam structure
//    u16 srcNum              source data byte count
//    u8  srcBitNum           1 source data bit count
//    u8  destBitNum          1 destination data bit count
//    u32 destOffset:31       offset number added to source data
//        destOffset0_On:1    flag for whether to add offset to 0 data
//
//- Return values: None
//----------------------------------------------------------------------

asm void MI_UnpackBits( register const void *srcp, register void *destp, register MIUnpackBitsParam *paramp )
{
                stmfd   sp!, {r4-r11, lr}
                sub     sp, sp, #4

                ldrh    r7, [r2, #0]            // r7:  srcNum    = unPackBitsParamp->srcNum

@00:            ldrb    r6, [r2, #2]            // r6:  srcBitNum = unPackBitsParamp->srcBitNum
                rsb     r10, r6, #8             // r10: srcBitNumInv = 8 - srcBitNum
                mov     r14, #0                 // r14: destBak = 0
                ldr     r11, [r2, #4]           // r8:  destOffset0_On
                mov     r8, r11, lsr #31        //          = unPackBitsParamp->destOffset0_On
                ldr     r11,[r2, #4]            //      destOffset = unPackBitsParamp->destOffset
                mov     r11,r11, lsl #1
                mov     r11,r11, lsr #1
                str     r11,[sp, #0]
                ldrb    r2, [r2, #3]            // r2:  destBitNum = unPackBitsParamp->destBitNum
                mov     r3, #0                  // r3:  destBitCount = 0

@01:            subs    r7, r7, #1              //  while (--srcNum >= 0) {
                blt     @06

                mov     r11, #0xff              // r5:  srcMask = 0xff >> srcBitNumInv;
                mov     r5, r11, asr r10
                ldrb    r9, [r0], #1            // r9:  srcTmp = *srcp++;
                mov     r4, #0                  // r4:  srcBitCount = 0;

@02:            cmp     r4, #8                  //      while (srcBitCount < 8) {
                bge     @01

                and     r11, r9, r5             // r12:     destTmp = ((srcTmp & srcMask) >>srcBitCount);
                movs    r12, r11, lsr r4
                cmpeq   r8,  #0
                beq     @04

@03:            ldr     r11, [sp, #0]           //          destTmp += destOffset;
                add     r12, r12, r11
@04:            orr     r14, r14, r12, lsl r3   //          destBak |= destTmp << destBitCount;
                add     r3, r3, r2              //          destBitCount += destBitNum;

                cmp     r3, #0x20               //          if (destBitCount >= 32) {
                blt     @05

                str     r14, [r1], #4           //              *destp++ = destBak;
                mov     r14, #0                 //              destBak = 0;
                mov     r3,  #0                 //              destBitCount = 0;
                                                //          }
@05:            mov     r5, r5, lsl r6          //          srcMask    <<= srcBitNum;
                add     r4, r4, r6              //          srcBitCount += srcBitNum;
                b       @02                     //      }
                                                //  }
@06:            add     sp, sp, #4
                ldmfd   sp!, {r4-r11, lr}
                bx      lr
}

//----------------------------------------------------------------------
//          8-bit expanding LZ77 compressed data
//
//- Expand LZ77 compressed data and write in 8-bit units.
//- You cannot uncompress directly in VRAM.
//- If the compressed data size is not a multiple of 4,
//  adjust as much as possible by packing with 0s.
//- Align the source address to a four byte boundary.
//
//- Arguments:
//    void *srcp              source address
//    void *destp             destination address
//
//- Data Header
//    u32 :4                  reserved
//        compType:4          compression type  ( = 1)
//        destSize:24         Data size after expansion
//
//- flag data format
//    u8  flags               compression/no compression flag
//                            (0, 1) = (uncompressed data, compressed data)
//- code data format  (Big Endian)
//    u16 length:4            uncompressed data length - 3 (compress only when match length over 3-Byte)
//        offset:12           match data offset - 1
//
//- Return values: None
//----------------------------------------------------------------------

asm void MI_UncompressLZ8( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4-r6, lr}

                ldr     r5, [r0], #4            // r2:  destCount = *(u32 *)srcp >> 8
                mov     r2, r5, lsr #8          // r0:  srcp += 4

@21:            cmp     r2, #0                  //  while (destCount > 0) {
                ble     @26

                ldrb    r14, [r0], #1           // r14: flags = *srcp++
                mov     r4, #8                  //
@22:            subs    r4, r4, #1              //      for (i = 8; --i >= 0; ) {
                blt     @21

                tst     r14, #0x80              //          if (!(flags & 0x80)) {
                bne     @23

                ldrb    r6, [r0], #1            //              *srcp++;
                swpb    r6, r6, [r1]            // r1:          *destp++; (Byte-writing measure)
                add     r1, r1, #1
                sub     r2, r2, #1              //              destCount--;
                b       @25
                                                //          } else {
@23:            ldrb    r5, [r0, #0]            // r3:          length = (*srcp >> 4) + 3;
                mov     r6, #3
                add     r3, r6, r5, asr #4
                ldrb    r6, [r0], #1            // r12:         offset = (*srcp++ & 0x0f) << 8;
                and     r5, r6, #0xf
                mov     r12,r5, lsl #8
                ldrb    r6, [r0], #1            //              offset = (offset | *srcp++) + 1;
                orr     r5, r6, r12
                add     r12, r5, #1
                sub     r2, r2, r3              //              destCount -= length;
                                                //              do {
@24:            ldrb    r5, [r1, -r12]          //                  *destp++ = destp[-offset]
                swpb    r5, r5, [r1]            //    (For byte write)
                add     r1, r1, #1
                subs    r3, r3, #1              //              } while (--length > 0);
                bgt     @24
                                                //          }
@25:            cmp     r2, #0                  //          if (destCount <= 0)   break;
                movgt   r14, r14, lsl #1        //          flags <<= 1
                bgt     @22                     //      }
                b       @21                     //  }

@26:            ldmfd   sp!, {r4-r6, lr}
                bx      lr
}


//----------------------------------------------------------------------
//          16-bit expanding LZ77 compressed data
//
//- Expand LZ77 compressed data and write in 16-bit units.
//You can also expand in data TCM and main memory.
//  it is slower than MI_UncompressLZ778().
//- Make the compressed data such that it looks up matching strings with less than 2-bytes.
//- If the compressed data size is not a multiple of 4,
//  adjust as much as possible by packing with 0s.
//- Align the source address to a four byte boundary.
//
//- Arguments:
//    void *srcp              source address
//    void *destp             destination address
//
//- Data Header
//    u32 :4                  reserved
//        compType:4          compression type  ( = 1)
//        destSize:24         Data size after expansion
//
//- flag data format
//    u8  flags               compression/no compression flag
//                            (0, 1) = (uncompressed data, compressed data)
//- code data format  (Big Endian)
//    u16 length:4            uncompressed data length - 3 (compress only when match length over 3-Byte)
//        offset:12           Match data offset ( >= 2)  - 1
//
//- Return values: None
//----------------------------------------------------------------------

asm void MI_UncompressLZ16( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4-r10, lr}

                mov     r3, #0                  // r3:  destTmp = 0
                ldr     r8, [r0], #4            // r10: destCount = *(u32 *)srcp >> 8
                mov     r10, r8, lsr #8         // r0:  srcp += 4
                mov     r2, #0                  // r2:  shift = 0

@31:            cmp     r10, #0                 //  while (destCount > 0) {
                ble     @36

                ldrb    r6, [r0], #1            // r6:  flags = *srcp++;
                mov     r7, #8                  //      for ( i = 8; --i >= 0; ) {
@32:            subs    r7, r7, #1
                blt     @31

                tst     r6, #0x80               //          if (!(flags & 0x80)) {
                bne     @33

                ldrb    r9, [r0], #1            //              destTmp |= *srcp++ << shift;
                orr     r3, r3, r9, lsl r2
                sub     r10, r10, #1            //              destCount--;

                eors    r2, r2, #8              //              if (!(shift ^= 8)) {
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                streqh  r3, [r1], #2            //              *destp++ = destTmp;
#else
                dcd     0x00c130b2
#endif
                moveq   r3, #0                  //              destTmp = 0;
                b       @35                     //          } else {

@33:            ldrb    r9, [r0, #0]            // r5:          length = (*srcp >> 4) + 3;
                mov     r8, #3
                add     r5, r8, r9, asr #4
                ldrb    r9, [r0], #1            // r4:          offset = (*srcp++ & 0x0f) << 8;
                and     r8, r9, #0xf
                mov     r4, r8, lsl #8
                ldrb    r9, [r0], #1            //              offset = (offset | *srcp++) + 1;
                orr     r8, r9, r4
                add     r4, r8, #1
                rsb     r8, r2, #8              // r14:         offset0_8 = (8 - shift)
                and     r9, r4, #1              //                          ^ ((offset & 1) << 3);
                eor     r14, r8, r9, lsl #3
                sub     r10, r10, r5            //              destCount -= length;
                                                //              do {
@34:            eor     r14, r14, #8            //                  offset0_8 ^= 8;
                rsb     r8, r2, #8              //                  destTmp |= (destp[-((offset
                add     r8, r4, r8, lsr #3      //                              + ((8 - shift) >> 3)) >> 1)];
                mov     r8, r8, lsr #1
                mov     r8, r8, lsl #1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                ldrh    r9, [r1, -r8]
#else
                dcd     0xe11190b8
#endif
                mov     r8, #0xff
                and     r8, r9, r8, lsl r14
                mov     r8, r8, asr r14
                orr     r3, r3, r8, lsl r2
                eors    r2, r2, #8              //                  if (!(shift ^= 8)) {
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                streqh  r3, [r1], #2            //                      *destp++ = destTmp;
#else
                dcd     0x00c130b2
#endif
                moveq   r3, #0                  //                      destTmp = 0;
                                                //                  }
                subs    r5, r5, #1              //              } while (--length > 0);
                bgt     @34                     //          }

@35:            cmp     r10, #0                 //          if (destCount <= 0)   break;
                movgt   r6, r6, lsl #1          //          flags <<= 1
                bgt     @32                     //      }
                b       @31                     //  }

@36:            ldmfd   sp!, {r4-r10, lr}
                bx      lr
}

//----------------------------------------------------------------------
//          Expanding Huffman compressed data
//
//- Uncompresses Huffman compressed data and writes in units of 32 bits.
//- If the compressed data size is not a multiple of 4,
//  adjust as much as possible by packing with 0s.
//- Align the source address to a four byte boundary.
//
//- Arguments:
//    void *srcp              source address
//    void *destp             destination address
//
//- Data Header
//    u32 bitSize:4           1 data/bit size  (normal 4|8)
//        compType:4          compression type  ( = 2)
//        destSize:24         Data size after expansion
//
//- tree table
//    u8           treeSize        tree table size/2 - 1
//    TreeNodeData nodeRoot        root node
//
//    TreeNodeData nodeLeft        root left node
//    TreeNodeData nodeRight       root right node
//
//    TreeNodeData nodeLeftLeft    left left node
//    TreeNodeData nodeLeftRight   left ride node
//
//    TreeNodeData nodeRightLeft   right left node
//    TreeNodeData nodeRightRight  right right node
//
//            -
//            -
//
//  compressed data after this
//
//- TreeNodeData structure
//    u8  nodeNextOffset:6    offset to next node - 1 (2-Byte units)
//        rightEndFlag:1      right node end flag
//        leftEndzflag:1       Left node end flag
//                            when end flag is set
//                            there is data in next node
//
//- Return values: None
//----------------------------------------------------------------------

asm void MI_UncompressHuffman( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4-r11, lr}
                sub     sp, sp, #4

                add     r2, r0, #4              // r2:  *treep = (u8 *)srcp + 4
                add     r7, r2, #1              // r7:  *treeStartp = treep + 1
                ldrb    r10,[r0, #0]            // r4:  DataBit = *(u8 *)srcp & 0x0f
                and     r4, r10, #0xf
                mov     r3, #0                  // r3:  destTmp = 0
                mov     r14,#0                  // r14: destTmpCount = 0
                and     r10,r4,  #7             //      destTmpDataNum = 4 + (DataBit & 0x7)
                add     r11,r10, #4
                str     r11,[sp, #0]
                ldr     r10,[r0, #0]            // r12: destCount = *srcp >> 8
                mov     r12,r10, lsr #8
                ldrb    r10,[r2, #0]            // r0:  srcp  = (u32 *)(treep + ((*treep + 1) << 1))
                add     r10,r10, #1
                add     r0, r2, r10, lsl #1
                mov     r2, r7                  //      treep = treeStartp

@11:            cmp     r12, #0                 //  while (destCount > 0) {
                ble     @14

                mov     r8,  #32                // r8:  srcCount = 32;
                ldr     r5, [r0], #4            // r5:  srcTmp = *srcp++;

@12:            subs    r8, r8,  #1             //      while (--srcCount >= 0) {
                blt     @11

                mov     r10,#1                  // r9:      treeShift = (srcTmp >> 31) & 0x1
                and     r9, r10, r5, lsr #31
                ldrb    r6, [r2, #0]            // r6:      treeCheck = *treep
                mov     r6, r6, lsl r9          //          treeCheck <<= treeShift
                mov     r10,r2, lsr #1          //          treep = (u8 *)((((u32 )treep>>1) <<1)
                mov     r10,r10,lsl #1          //                          + (((*treep & 0x3f)+1) <<1)+treeShift)
                ldrb    r11,[r2,#0]
                and     r11,r11,#0x3f
                add     r11,r11,#1
                add     r10,r10,r11,lsl #1
                add     r2, r10,r9

                tst     r6, #0x80               //          if (treeCheck & TREE_END) {
                beq     @13

                mov     r3, r3, lsr r4          //              destTmp >>= DataBit;
                ldrb    r10,[r2, #0]            //              destTmp |= *treep << (32 - DataBit);
                rsb     r11, r4,  #32
                orr     r3, r3, r10, lsl r11
                mov     r2, r7                  //              treep = treeStartp;
                add     r14,r14,#1              //              if (++destTmpCount == destTmpDataNum) {
                ldr     r11,[sp, #0]
                cmp     r14,r11

                streq   r3, [r1], #4            //                  *destp++ = destTmp;
                subeq   r12,r12,  #4            //                  destCount -= 4;
                moveq   r14,#0                  //                  destTmpCount = 0;
                                                //              }
                                                //          }
@13:            cmp     r12,#0                  //          if (destCount <= 0)   break;
                movgt   r5, r5, lsl #1          //          srcTmp <<= 1;
                bgt     @12                     //      }
                b       @11                     //  }

@14:            add     sp, sp, #4
                ldmfd   sp!, {r4-r11, lr}
                bx      lr
}

//----------------------------------------------------------------------
//          8-bit expanding runlength compressed data
//
//- Uncompresses run length compressed data, and writes in 8-bit units.
//- You cannot uncompress directly in VRAM.
//- If the compressed data size is not a multiple of 4,
//  adjust as much as possible by packing with 0s.
//- Align the source address to a four byte boundary.
//
//- Arguments:
//    void *srcp              source address
//    void *destp             destination address
//
//- Data Header
//    u32 :4                  reserved
//        compType:4          compression type  ( = 3)
//        destSize:24         Data size after expansion
//
//- flag data format
//    u8  length:7            uncompressed data length - 1 (when no compression)
//                            uncompressed data length - 3 (compress only when consecutive length over 3 Byte)
//        flag:1               (0, 1)  =  (no compression data, compression data)
//
//- Return values: None
//----------------------------------------------------------------------

asm void MI_UncompressRL8( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4, r5, r7}

                ldmia   r0!, {r3}               // r7:  destCount = *(u32 *)srcp >> 8;
                mov     r7, r3, lsr #8          // r0:  srcp += 4;

@41:            cmp     r7, #0                  //  while (destCount > 0) {
                ble     @45

                ldrb    r4, [r0], #1            // r4:  flags = *srcp++;
                ands    r2, r4, #0x7f           // r2:  length = flags & 0x7f;
                tst     r4, #0x80               //      if (!(flags & 0x80)) {
                bne     @43

                add     r2, r2, #1              //          length++;
                sub     r7, r7, r2              //          destCount -= length;
@42:            ldrb    r3, [r0], #1            //          do{
                swpb    r3, r3, [r1]            //              *destp++ = *srcp++; (Byte-writing measure)
                add     r1, r1, #1
                subs    r2, r2, #1              //          } while (--length > 0);
                bgt     @42                     //      } else {
                b       @41

@43:            add     r2, r2, #3              //          length += 3;
                sub     r7, r7, r2              //          destCount -= length;
                ldrb    r5, [r0], #1            //          srcTmp  = *srcp++;
@44:            swpb    r4, r5, [r1]            //          do { (Byte-writing measure)
                add     r1, r1, #1              //              *destp++ =  srcTmp;
                subs    r2, r2, #1              //          } while (--length > 0);
                bgt     @44                     //      }
                b       @41                     //  }

@45:            ldmfd   sp!, {r4, r5, r7}
                bx      lr
}

//----------------------------------------------------------------------
//          16-bit expanding runlength compressed data
//
//- Uncompresses run length compressed data, and writes in 16-bit units.
//- Although it can expand in data TCM or main memory,
//  it is slower than MI_UncompressLZ778().//----
//- If the compressed data size is not a multiple of 4,
//  adjust as much as possible by packing with 0s.
//- Align the source address to a four byte boundary.
//
//- Arguments:
//    void *srcp              source address
//    void *destp             destination address
//
//- Data Header
//    u32 :4                  reserved
//        compType:4          compression type  ( = 3)
//        destSize:24         Data size after expansion
//
//- flag data format
//    u8  length:7            uncompressed data length - 1 (when no compression)
//                            uncompressed data length - 3 (compress only when consecutive length over 3 Byte)
//        flag:1               (0, 1)  =  (no compression data, compression data)
//
//- Return values: None
//----------------------------------------------------------------------

//---- This code will be compiled in Thumb-Mode
#include <nitro/code16.h>

asm void MI_UncompressRL16( register const void *srcp, register void *destp )
{
                push    {r4-r7}
                sub     sp, #0xc

                mov     r7, #0                  // r7:  destTmp = 0
                ldmia   r0!, {r3}               // r5:  destCount = *(u32 *)srcp >> 8
                lsr     r5, r3, #8              // r0:  srcp += 4

                mov     r4, #0                  // r4:  shift = 0
@51:            cmp     r5, #0                  //  while (destCount > 0) {
                ble     @57

                ldrb    r3, [r0,  #0]           //      flags = *srcp++;
                str     r3, [sp, #4]
                add     r0, #1
                ldr     r3, [sp, #4]            // r2:  length = flags & 0x7f;
                lsl     r2, r3, #25
                lsr     r2, r2, #25

                ldr     r6, [sp, #4]            //      if (!(flags & 0x80)) {
                lsr     r3, r6, #8
                bcs     @54

                add     r2, #1                  //          length++;
                sub     r5, r5, r2              //          destCount -= length;
                                                //          do {
@52:            ldrb    r6, [r0, #0]            //              destTmp |= *srcp++ << shift;
                lsl     r6, r4
                orr     r7, r6
                add     r0, #1
                mov     r3, #8                  //              if (!(shift ^= 8)) {
                eor     r4, r3
                bne     @53
                strh    r7, [r1, #0]            //                  *destp++ = destTmp;
                add     r1, #2
                mov     r7, #0                  //                  destTmp = 0;
                                                //              }
@53:            sub     r2, r2, #1              //          } while (--length > 0);
                bgt     @52
                b       @51                     //      } else {

@54:            add     r2, #3                  //          length += 3;
                sub     r5, r5, r2              //          destCount -= length;
                ldrb    r6, [r0, #0]            //          srcTmp  = *srcp++;
                str     r6, [sp, #8]
                add     r0, #1
                                                //          do {
@55:            ldr     r6, [sp, #8]            //              destTmp |= srcTmp  << shift;
                lsl     r6, r4
                orr     r7, r6
                mov     r3, #8                  //              if (!(shift ^= 8)) {
                eor     r4, r3
                bne     @56
                strh    r7, [r1, #0]            //                  *destp++ = destTmp;
                add     r1, #2
                mov     r7, #0                  //                  destTmp = 0;
                                                //              }
@56:            sub     r2, r2, #1              //          } while (--length > 0);
                bgt     @55                     //      }
                b       @51                     //  }

@57:            add     sp, #0xc
                pop     {r4-r7}
                bx      lr
}

//---- This code will be compiled in Thumb-Mode
#include <nitro/code32.h>

//----------------------------------------------------------------------
//          Decode differential filter conversion. Use 8-bit decoding.
//
//- Restores diff files and writes in 8 bit units.
//- You cannot uncompress directly in VRAM.
//- If the compressed data size is not a multiple of 4,
//  adjust as much as possible by packing with 0s.
//- Align the source address to a four byte boundary.
//
//- Arguments:
//    void *srcp              source address
//    void *destp             destination address
//
//- Data Header
//    u32 :4                  Bit size of unit
//        compType:4          compression type  ( = 3)
//        destSize:24         Data size after expansion
//
//- Return values: None
//----------------------------------------------------------------------

asm void MI_UnfilterDiff8( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4}

                ldmia   r0, {r2}                // r2:  destCount = (u32 *)srcp;
                mov     r3, #0                  // r3:  sum = 0;
                and     r4, r2, #0xF            // r4:  bitSize   = (u32 *)srcp & 0xF;
                mov     r2, r2, lsr #8          //      destCount = (u32 *)srcp >> 8;
                cmp     r4, #1                  //      if (bitSize != 1) {
                bne     @63

@61             // diff calculation in units of 8 bits
                add     r0, r0, #3              //          srcp += 4;
                sub     r1, r1, #1
@62                                             //          do {
                ldrb    r4, [r0, #1]!           //              tmp = *(srcp++);
                subs    r2, r2, #1              //              destCount--; implemented here for elimination of pipeline stall
                add     r3, r3, r4              //              sum += tmp
                strb    r3, [r1, #1]!           //              *(destp++) = sum;
                bgt     @62                     //          } while ( destCount > 0 );
                b       @65                     //      } else {

@63             // diff calculation in units of 16 bits
                add     r0, r0, #2              //
                sub     r1, r1, #2              //
@64                                             //          do {
                ldrh    r4, [r0, #2]!           //              tmp = *(u16*)srcp; srcp += 2;
                subs    r2, r2, #2              //              destCount -= 2; implemented here for elimination of pipeline stall
                add     r3, r3, r4              //              sum += tmp;
                strh    r3, [r1, #2]!           //              *(u16*)destp = sum; destp += 2;
                bgt     @64                     //          } while ( destCount > 0 );
                                                //      }
@65
                ldmfd   sp!, {r4}
                bx      lr
}

//----------------------------------------------------------------------
//          Decode differential filter conversion. Use 16-bit decoding.
//
//- Restores diff files and writes in 16 bit units.
//- You can decode to data TCM or VRAM. However,
//  it is slower than MI_UncompressLZ778().//----
//- If the compressed data size is not a multiple of 4,
//  adjust as much as possible by packing with 0s.
//- Align the source address to a four byte boundary.
//
//- Arguments:
//    void *srcp              source address
//    void *destp             destination address
//
//- Data Header
//    u32 :4                  Bit size of unit
//        compType:4          compression type  ( = 3)
//        destSize:24         Data size after expansion
//
//- Return values: None
//----------------------------------------------------------------------

asm void MI_UnfilterDiff16( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4, r5}

                ldmia   r0, {r2}                // r2:  destCount = (u32 *)srcp;
                mov     r3, #0                  // r3:  sum = 0;
                and     r4, r2, #0xF            // r4:  bitSize   = (u32 *)srcp & 0xF;
                mov     r2, r2, lsr #8          //      destCount = (u32 *)srcp >> 8;
                cmp     r4, #1                  //      if (bitSize != 1) {
                bne     @63

@61             // diff calculation in units of 8 bits
                add     r0, r0, #2              //          srcp += 4;
                sub     r1, r1, #2
@62                                             //          do {
                ldrh    r4, [r0, #2]!           //          tmp = *(u16*)srcp; srcp += 2;
                sub     r2, r2, #2              //              destCount -= 2; implemented here for elimination of pipeline stall
                add     r3, r3, r4              //              sum += tmp
                and     r5, r3, #0xFF           // r5:          tmp2 = sum & 0xFF;
                add     r3, r3, r4, lsr #8      //              sum += (tmp >> 8);
                add     r5, r5, r3, lsl #8      //              tmp2 += (sum << 8);
                strh    r5, [r1, #2]!           //              *(u16*)destp = tmp2; destp += 2;
                cmp     r2, #1                  //
                bgt     @62                     //          } while ( destCount > 1 );
                bne     @65                     //          if ( destCount < 1 ) return;
                                                //          else // if (destCount == 1) {
                ldrh    r4, [r0, #2]!           //              tmp = *(u16*)srcp; srcp += 2;
                add     r3, r3, r4              //              sum += tmp;
                and     r5, r3, #0xFF           //              tmp2 = sum & 0xFF
                strh    r5, [r1, #2]!           //              *(u16*)destp = tmp2; destp += 2;
                b       @65                     //          }
                                                //      } else {
@63             // diff calculation in units of 16 bits
                add     r0, r0, #2
                sub     r1, r1, #2
@64                                             //          do {
                ldrh    r4, [r0, #2]!           //              tmp = *(u16*)srcp; srcp += 2;
                subs    r2, r2, #2              //              destCount -= 2; implemented here for elimination of pipeline stall
                add     r3, r3, r4              //              sum += tmp;
                strh    r3, [r1, #2]!           //              *(u16*)destp = sum; destp += 2;
                bgt     @64                     //          } while ( destCount > 0 );
                                                //      }
@65
                ldmfd   sp!, {r4,r5}
                bx      lr
}

//---- end limitation of processer mode
#include <nitro/codereset.h>
