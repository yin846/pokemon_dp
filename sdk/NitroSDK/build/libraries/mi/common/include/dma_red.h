/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI -
  File:     dma_red.h

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dma_red.h,v $
  Revision 1.4  2005/03/01 01:57:00  yosizaki
  Revised copyright year

  Revision 1.3  2005/02/28 05:26:05  yosizaki
  do-indent.

  Revision 1.2  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  2003/12/18 07:20:07  yada
  To leave the definition of red-sdk


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_DMA_RED_H_
#define NITRO_MI_DMA_RED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/os/systemCall.h>
#include <nitro/mi/dma.h>

//----------------------------------------------------------------
//      due to a request from the interface created by   
//      development engineering  
//      Left here and not in nitro-sdk/include/nitro/mi/dma.h   
//

//----------------------------------------------------------------------
//                      DMA Setting
//----------------------------------------------------------------------
//#define SIMULATOR
#ifndef SIMULATOR
#define __MI_DmaSet(dmaNo, srcp, destp, dmaCntData)                       \
{                                                                    \
    vu32 *dmaCntp = &((vu32 *)REG_DMA0SAD_ADDR)[dmaNo * 3];                  \
    dmaCntp[0] = (vu32 )(srcp);                                      \
    dmaCntp[1] = (vu32 )(destp);                                     \
    dmaCntp[2] = (vu32 )(dmaCntData);                                \
    {u32 dummy = dmaCntp[2];}                                        \
    {u32 dummy = dmaCntp[2];}                                        \
}
#define __MI_DmaSetAsync(dmaNo, srcp, destp, dmaCntData)                  \
{                                                                    \
    vu32 *dmaCntp = &((vu32 *)REG_DMA0SAD_ADDR)[dmaNo * 3];                  \
    dmaCntp[0] = (vu32 )(srcp);                                      \
    dmaCntp[1] = (vu32 )(destp);                                     \
    dmaCntp[2] = (vu32 )(dmaCntData);                                \
}
#else
#define __MI_DmaSet(dmaNo, srcp, destp, dmaCntData)                       \
{                                                                    \
    int    i;                                                        \
    for (i=0; i<(dmaCntData & 0x1ffff); i++)                         \
        if ((dmaCntData) & MI_DMA_SRC_FIX) {                            \
            if ((dmaCntData) & MI_DMA_32BIT_BUS)                        \
                    ((vu32 *)(destp))[i] = ((vu32 *)(srcp))[0];      \
            else    ((vu16 *)(destp))[i] = ((vu16 *)(srcp))[0];      \
         } else {                                                    \
            if ((dmaCntData) & MI_DMA_32BIT_BUS)                        \
                ((vu32 *)(destp))[i] = ((vu32 *)(srcp))[i];          \
            else    ((vu16 *)(destp))[i] = ((vu16 *)(srcp))[i];      \
        }                                                            \
}
#define __MI_DmaSetAsync(dmaNo, srcp, destp, dmaCntData)                  \
        __MI_DmaSet(     dmaNo, srcp, destp, dmaCntData)
#endif

//- Set parameter in DMA controller.
//- When SIMULATER is defined, then simulation occurs on the CPU.
//  Enabled when debugging with GDB, etc.  
//- Finally, the "LDR" command is inserted to wait for DMA to start.  
//
//- Arguments:
//  dmaNo       DMA number
//  srcp          source address
//  destp         destination address
//  dmaCntData  Parameter data
//
//Note: When DMA is started with a program in the CPU internal RAM,
//  the  next instruction will be run first.
//  Therefore, when changing the transfer destination in CPU right after DMA,
//  the transfer destination data for DMA sometimes becomes an unintended value
//  because DMA starts up during read/write.
//  In this case, the impact on subsequent code can be avoided by inserting WaitDma()
//  immediately after, and checking if DMA has finished or not.


//----------------------------------------------------------------------
//                      DMA  Clear
//----------------------------------------------------------------------

#define __MI_DmaClear(dmaNo, data, destp, size, bit)             \
{                                                           \
    *(vu##bit *)HW_DMA_CLEAR_DATA_BUF = (vu##bit )(data);           \
    __MI_DmaSet(dmaNo, HW_DMA_CLEAR_DATA_BUF, destp, (                  \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM |              \
        MI_DMA_SRC_FIX        | MI_DMA_DEST_INC    |              \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))));            \
}

#define __MI_DmaClearIf(dmaNo, data, destp, size, bit)           \
{                                                           \
    *(vu##bit *)DMA_CLEAR_DATA_BUF = (vu##bit )(data);       \
    __MI_DmaSet(dmaNo, DMA_CLEAR_DATA_BUF, destp, (                \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM |              \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_FIX        | MI_DMA_DEST_INC    |              \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))));            \
}

#define __MI_DmaClearArray(  dmaNo, data, destp, bit)            \
    __MI_DmaClear(       dmaNo, data, destp, sizeof(destp), bit)

#define MI_DmaClearArrayIf(dmaNo, data, destp, bit)            \
    __MI_DmaClearIf(     dmaNo, data, destp, sizeof(destp), bit)

//- Clear RAM in DMA
//- Clear data is put in the stack and copied to the destination.
//- An interrupt request is generated when MI_DmaClearIf/MI_DmaClearArrayIf ends.
//- MI_DmaClearArray/MI_DmaClearArrayIf clears entire destination array.
//
//- Arguments:
//  dmaNo       DMA number
//  data        clear data
//  destp         destination address
//  size        Clear byte cout
//  bit         transfer bit width (16|32)
//
//
//Note: When DMA is started with a program in the CPU internal RAM,
//  the  next instruction will be run first.
//  Therefore, when changing the transfer destination in CPU right after DMA,
//  the transfer destination data for DMA sometimes becomes an unintended value
//  because DMA starts up during read/write.
//  In this case, the impact on subsequent code can be avoided by inserting WaitDma()
//  immediately after, and checking if DMA has finished or not.

//----------------------------------------------------------------------
//                      DMA  Copy
//----------------------------------------------------------------------

#define __MI_DmaCopy(dmaNo, srcp, destp, size, bit)              \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM |              \
        MI_DMA_SRC_INC        | MI_DMA_DEST_INC    |              \
        MI_DMA_##bit##BIT_BUS | ((size)/((bit)/8))))

#define __MI_DmaCopyIf(dmaNo, srcp, destp, size, bit)            \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM |              \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_INC        | MI_DMA_DEST_INC    |              \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))))

#define __MI_DmaCopyArray(  dmaNo, srcp, destp, bit)             \
    __MI_DmaCopy(       dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_DmaCopyArrayIf(dmaNo, srcp, destp, bit)             \
    __MI_DmaCopyIf(     dmaNo, srcp, destp, sizeof(srcp), bit)

//- Copy using DMA.
//- Interrupt request generated when MI_DmaCopyIf/MI_DmaCopyArrayIf ends.
//- MI_DmaCopyArray/MI_DmaCopyArrayIf copies entire source array.
//
//- Arguments:
//  dmaNo       DMA number
//  srcp          source address
//  destp         destination address
//  size        number of transfer bytes
//  bit         transfer bit width (16|32)
//
//
//Note: When DMA is started with a program in the CPU internal RAM,
//  the  next instruction will be run first.
//  Therefore, when changing the transfer destination in CPU right after DMA,
//  the transfer destination data for DMA sometimes becomes an unintended value
//  because DMA starts up during read/write.
//  In this case, the impact on subsequent code can be avoided by inserting WaitDma()
//  immediately after, and checking if DMA has finished or not.

//----------------------------------------------------------------------
//                  H-Blank DMA Copy
//----------------------------------------------------------------------

#define __MI_H_DmaCopy(dmaNo, srcp, destp, size, bit)            \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_H_BLANK |          \
        MI_DMA_SRC_INC        | MI_DMA_DEST_RELOAD     |          \
        MI_DMA_CONTINUOUS_ON  |                                \
        MI_DMA_##bit##BIT_BUS | ((size)/((bit)/8))))

#define __MI_H_DmaCopyIf(dmaNo, srcp, destp, size, bit)          \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_H_BLANK |          \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_INC        | MI_DMA_DEST_RELOAD     |          \
        MI_DMA_CONTINUOUS_ON  |                                \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))))

#define __MI_H_DmaCopyArray(  dmaNo, srcp, destp, bit)           \
    __MI_H_DmaCopy(       dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_H_DmaCopyArrayIf(dmaNo, srcp, destp, bit)           \
    __MI_H_DmaCopyIf(     dmaNo, srcp, destp, sizeof(srcp), bit)

//- Synchronizes with H-Blank and copies using DMA
//- Generates an interrupt request when MI_H_DmaCopyIf/MI_H_DmaCopyArrayIf ends.
//- MI_H_DmaCopyArray/MI_H_DmaCopyArrayIf copies entire source array.
//
//- Arguments:
//  dmaNo       DMA number
//  srcp          source address
//  destp         destination address
//  size        number of transfer bytes
//  bit         transfer bit width (16|32)

//----------------------------------------------------------------------
//                  V-Blank DMA Copy
//----------------------------------------------------------------------

#define __MI_V_DmaCopy(dmaNo, srcp, destp, size, bit)            \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_V_BLANK |          \
        MI_DMA_SRC_INC        | MI_DMA_DEST_INC        |          \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))))

#define __MI_V_DmaCopyIf(dmaNo, srcp, destp, size, bit)          \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_V_BLANK |          \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_INC        | MI_DMA_DEST_INC        |          \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))))

#define __MI_V_DmaCopyArray(  dmaNo, srcp, destp, bit)           \
    __MI_V_DmaCopy(       dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_V_DmaCopyArrayIf(dmaNo, srcp, destp, bit)           \
    __MI_V_DmaCopyIf(     dmaNo, srcp, destp, sizeof(srcp), bit)

//- Synchronizes with V-Blank and copies using DMA
//- Generates interrupt request when MI_V_DmaCopyIf/MI_V_DmaCopyArrayIf ends.
//- MI_V_DmaCopyArray/MI_V_DmaCopyArrayIf copies entire source array.
//
//- Arguments:
//  dmaNo       DMA number
//  srcp          source address
//  destp         destination address
//  size        number of transfer bytes
//  bit         transfer bit width (16|32)

//----------------------------------------------------------------------
//                  Main Memory Display DMA
//----------------------------------------------------------------------

#define __MI_DmaDispMainmem(dmaNo, srcp)                         \
                                                            \
    __MI_DmaSet(dmaNo, srcp, REG_DISP_MMEM_FIFO_ADDR,  (              \
        MI_DMA_ENABLE         | MI_DMA_TIMING_DISP_MMEM |        \
        MI_DMA_SRC_INC        | MI_DMA_DEST_FIX          |        \
		MI_DMA_CONTINUOUS_ON  |								\
        MI_DMA_32BIT_BUS      | (4)))

//- DMA transfer to display image on the main memory
//
//- Arguments:
//  dmaNo       DMA number
//  srcp          source address

//----------------------------------------------------------------------
//                  Geometry FIFO-DMA
//----------------------------------------------------------------------

#define __MI_GX_Dma(dmaNo, srcp, length)                         \
                                                            \
    __MI_DmaSetAsync(dmaNo, srcp, REG_GXFIFO_ADDR,  (                 \
        MI_DMA_ENABLE         | MI_DMA_TIMING_GXFIFO  |          \
        MI_DMA_SRC_INC        | MI_DMA_DEST_FIX        |          \
        MI_DMA_32BIT_BUS      | (length)))

#define __MI_GX_DmaIf(dmaNo, srcp, length)                       \
                                                            \
    __MI_DmaSetAsync(dmaNo, srcp, REG_GXFIFO_ADDR  (                 \
        MI_DMA_ENABLE         | MI_DMA_TIMING_GXFIFO  |          \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_INC        | MI_DMA_DEST_FIX        |          \
        MI_DMA_32BIT_BUS      | (length)))

#define __MI_GX_DmaFast(dmaNo, srcp, length)                     \
                                                            \
    __MI_DmaSetAsync(dmaNo, srcp, REG_GXFIFO_ADDR,  (                 \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM     |          \
        MI_DMA_SRC_INC        | MI_DMA_DEST_FIX        |          \
        MI_DMA_32BIT_BUS      | (length)))

#define __MI_GX_DmaFastIf(dmaNo, srcp, length)                   \
                                                            \
    __MI_DmaSetAsync(dmaNo, srcp, REG_GXFIFO_ADDR,  (                 \
        DMA_ENABLE         | DMA_TIMING_IMM     |          \
        DMA_IF_ENABLE      |                                \
        DMA_SRC_INC        | DMA_DEST_FIX        |          \
        DMA_32BIT_BUS      | (length)))

#define __MI_GX_DmaArray(  dmaNo, srcp, destp, bit)              \
    __MI_GX_Dma(  dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_GX_DmaArrayIf(dmaNo, srcp, destp, bit)              \
    __MI_GX_DmaIf(dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_GX_DmaArrayFast(  dmaNo, srcp, destp, bit)          \
    __MI_GX_DmaFast(  dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_GX_DmaArrayFastIf(dmaNo, srcp, destp, bit)          \
    __MI_GX_DmaFastIf(dmaNo, srcp, destp, sizeof(srcp), bit)

//- Copies using DMA by request from geometry FIFO
//- MI_GX_DmaIf/MI_GX_DmaArrayIf/MI_GX_DmaFastIf/MI_GX_DmaArrayFastIf
//  generates interrupt request when ending
//- MI_GX_DmaArray/MI_GX_DmaArrayIf/MI_GX_DmaArrayFast/MI_GX_DmaArrayFastIf
//  copies entire source array.
//
//- Arguments:
//  dmaNo       DMA number
//  srcp          source address
//  size        number of transfer bytes


//----------------------------------------------------------------------
//                      DMA End Wait
//----------------------------------------------------------------------

#define __MI_WaitDma(dmaNo)                                      \
{                                                           \
    vu32 *(dmaCntp) = &((vu32 *)REG_DMA0SAD_ADDR)[dmaNo * 3];       \
    while (dmaCntp[2] & MI_DMA_ENABLE) ;                       \
}

//- Wait for end of DMA
//
//- Arguments:
//  dmaNo       DMA number


//----------------------------------------------------------------------
//                      DMA  Stop
//----------------------------------------------------------------------

#define __MI_StopDma(dmaNo)                                      \
{                                                           \
    vu16* dmaCntp = &((vu16 *)REG_DMA0SAD_ADDR)[dmaNo * 6];       \
    dmaCntp[5] &= ~((MI_DMA_TIMING_MASK | MI_DMA_CONTINUOUS_ON)  \
                   >> 16);                                  \
    dmaCntp[5] &= ~( MI_DMA_ENABLE   >> 16);                   \
    {u32 dummy = dmaCntp[5];}                               \
    {u32 dummy = dmaCntp[5];}                               \
}

//- Stops DMA.
//- However, DMA runs only once if automatically started during processing.
//
//- Arguments:
//  dmaNo       DMA number


//----------------------------------------------------------------------
//                      CPU Clear
//----------------------------------------------------------------------

#define __MI_CpuClear(data, destp, size, bit)    UTL_CpuClear##bit(data, (void *)(destp), size)

#define __MI_CpuClearArray(data, destp, bit)                     \
    __MI_CpuClear(     data, destp, sizeof(destp), bit)

//- Calls system call that does RAM clear on CPU.
//- Clear data is put in the stack and copied to the destination.
//- CpuClearArray clears entire destination array.
//
//- Arguments:
//  data        clear data
//  destp         destination address
//  size        Clear byte cout
//  bit         transfer bit width (16|32)

//----------------------------------------------------------------------
//                      CPU Copy
//----------------------------------------------------------------------

#define __MI_CpuCopy(srcp, destp, size, bit)    UTL_CpuCopy##bit((void *)(srcp), (void *)(destp), size)

#define __MI_CpuCopyArray(srcp, destp, bit)                      \
    __MI_CpuCopy(     srcp, destp, sizeof(srcp), bit)

//- Calls system call that does copy on CPU.
//- CpuCopyArray copies entire source array.
//
//- Arguments:
//  srcp          source address
//  destp         destination address
//  size        number of transfer bytes
//  bit         transfer bit width (16|32)

//----------------------------------------------------------------------
//                  CPU High-speed Clear (32Byte units)
//----------------------------------------------------------------------

#define __MI_CpuClearFast(data, destp, size)  UTL_CpuClearFast(data, (void *)(destp), size)

#define MI_CpuClearArrayFast(data, destp)                      \
    __MI_CpuClearFast(     data, destp, sizeof(destp))

//- Calls the system call that rapidly clears with CPU.
//- Clear data is put in the stack and copied to the destination.
//- CpuClearArrayFast clears entire destination array.
//
//- Arguments:
//  data        clear data
//  destp         destination address
//  size        Clear byte cout

//----------------------------------------------------------------------
//                  CPU High-speed Copy (32Byte unit)
//----------------------------------------------------------------------

#define __MI_CpuCopyFast(srcp, destp, size)   UTL_CpuCopyFast((void *)(srcp), (void *)(destp), size)


#define MI_CpuCopyArrayFast(srcp, destp)                       \
    __MI_CpuCopyFast(     srcp, destp, sizeof(srcp))

//- Calls the system call that rapidly copies in the CPU.
//- CpuCopyArrayFast copies entire source array.
//
//- Arguments:
//  srcp          source address
//  destp         destination address
//  size        number of transfer bytes





#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_MI_DMA_RED_H_ */
#endif

