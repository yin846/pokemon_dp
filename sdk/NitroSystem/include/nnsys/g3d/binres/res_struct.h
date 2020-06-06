/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_struct.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_struct.h,v $
  Revision 1.28  12/21/2004 08:24:38  nishida_kenji
  supports env map & proj map.

  Revision 1.27  10/28/2004 00:37:23  nishida_kenji
  remove unused codes.

  Revision 1.26  10/06/2004 04:49:47  nishida_kenji
  add boxPosScale and boxInvPosScale to NNSG3dResMdlInfo.

  Revision 1.25  09/28/2004 07:34:27  nishida_kenji
  fix bitmask(NNSG3dTexImageParamEx).

  Revision 1.24  08/03/2004 04:25:44  nishida_kenji
  Add NNSG3dResAnmCommon

  Revision 1.23  08/03/2004 01:44:44  nishida_kenji
  NNSG3dResXXXAnmSet, NNSG3dResDictAnmSetData became soft types.

  Revision 1.22  07/27/2004 13:46:06  nishida_kenji
  add
  NNS_G3D_TEXMTXMODE_3DSMAX
  NNS_G3D_TEXMTXMODE_XSI

  Revision 1.21  07/22/2004 07:25:47  kitani_toshikazu
  Added comments

  Revision 1.20  07/21/2004 11:59:20  kitani_toshikazu
  Added NNSG3dResJntSRTAnmResult

  Revision 1.19  07/21/2004 09:08:11  kitani_toshikazu
  Changed the element type of the matrix for the animation test from fx16 to fx32

  Revision 1.18  07/20/2004 01:37:14  nishida_kenji
  fixed comments

  Revision 1.17  07/17/2004 12:58:49  nishida_kenji
  NODEDESC_BB -> BB
  NODEDESC_BBY -> BBY

  Revision 1.16  07/15/2004 02:43:20  nishida_kenji
  add magW, magH field on resMat.

  Revision 1.15  07/14/2004 11:30:19  nishida_kenji
  add tex4pltt flag on a pltt dictionary.

  Revision 1.12  07/07/2004 12:40:56  nishida_kenji
  revise MAT/SHP itemTag constant.

  Revision 1.11  07/02/2004 06:03:04  nishida_kenji
  inline -> static inline

  Revision 1.10  06/30/2004 11:14:39  nishida_kenji
  Add enum values for NODEDESC's flag

  Revision 1.9  06/30/2004 05:30:21  nishida_kenji
  Workaround to the case that pos_scale is not 0.

  Revision 1.8  06/29/2004 02:09:02  nishida_kenji
  NNS_G3D_SBCFLG_??? becomes 3-bit field.

  Revision 1.7  06/28/2004 08:53:35  kitani_toshikazu
  Described the animation type related to the accessor

  Revision 1.6  06/28/2004 00:21:45  nishida_kenji
  (none)

  Revision 1.5  2004/06/23 09:13:28  nishida_kenji
  u16 VRamBase --> u32 vramKey at NNSG3dResTexInfo, NNSG3dResTex4x4Info, NNSG3dResPlttInfo

  Revision 1.4  06/22/2004 01:31:52  nishida_kenji
  (none)

  Revision 1.3  06/16/2004 23:59:16  nishida_kenji
  NitroSystem -> NITRO-System

  Revision 1.2  06/15/2004 00:50:11  nishida_kenji
  (none)

  Revision 1.1  06/11/2004 04:11:12  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_BINRES_RES_STRUCT_H_
#define NNSG3D_BINRES_RES_STRUCT_H_

#ifdef _CVTR
// VC++
#define NNS_G3D_RES_INLINE __inline
#else
#define NNS_G3D_RES_INLINE static inline
#endif


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
    An overview of relation between NNS_G3D_DATABLK_MDLSET structs
    (NNSG3dResMdlSet as a root)
    
    .... -- 1:N relation(referenced by name/index, using NNSG3dResDict)
    ---> -- 1:1 relation(own/refer by offset)

    NNSG3dResMdlSet .... NNSG3dResMdl ---> NNSG3dResMdlInfo
                                      ---> NNSG3dResNodeInfo
                                      ---> Structure Byte Code
                                      ---> NNSG3dResMat
                                      ---> NNSG3dResShp

    NNSG3dResMdlInfo ---> NNSG3dSbcType/NNSG3dScalingRule/NNSG3dTexMtxMode etc.
    
    NNSG3dResNodeInfo .... NNSG3dResNodeData ---> NNSG3dSRTFlag etc.

    NNSG3dResMat .... NNSG3dResMatData

    NNSG3dResShp .... NNSG3dResShpData ---> Display List
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    Signatures for G3D's binary files

 *---------------------------------------------------------------------------*/
#define NNS_G3D_SIGNATURE_NSBMD  '0DMB'
#define NNS_G3D_SIGNATURE_NSBTX  '0XTB'
#define NNS_G3D_SIGNATURE_NSBCA  '0ACB'
#define NNS_G3D_SIGNATURE_NSBVA  '0AVB'
#define NNS_G3D_SIGNATURE_NSBMA  '0AMB'
#define NNS_G3D_SIGNATURE_NSBTP  '0PTB'
#define NNS_G3D_SIGNATURE_NSBTA  '0ATB'


/*---------------------------------------------------------------------------*
    Signatures for G3D data blocks

 *---------------------------------------------------------------------------*/
#define NNS_G3D_DATABLK_MDLSET     '0LDM'
#define NNS_G3D_DATABLK_TEX        '0XET'
#define NNS_G3D_DATABLK_JNT_ANM    '0TNJ'
#define NNS_G3D_DATABLK_VIS_ANM    '0SIV'
#define NNS_G3D_DATABLK_MATC_ANM   '0TAM'
#define NNS_G3D_DATABLK_TEXPAT_ANM '0TAP'
#define NNS_G3D_DATABLK_TEXSRT_ANM '0TRS'


/*---------------------------------------------------------------------------*
    The material shape classification index

    It is possible to have multiple data formats for material shapes.
      Expansion is assumed when a type of shape is needed that holds
    the index of the vertex data used to perform the vertex animation.

    NOTICE:
        This has become the index of the function table, so
        the usable values are dependent on the size of the function table.
 *---------------------------------------------------------------------------*/
typedef u16 NNSG3dItemTag;
#define NNS_G3D_ITEMTAG_MAT_STANDARD  0
#define NNS_G3D_ITEMTAG_SHP_STANDARD  0

/////////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------*
    Name:        NNSG3dResFileHeader

    Description: A header of a resource file

                 signature  = (sigVal = NNS_G3D_SIGNATURE_XXXX)
                 byteOrder  = 0xfeff
                 version    = described in u8.u8 style
                 fileSize   = size of a data file
                 headerSize = always 16
                 dataBlocks = the number of blocks
                              (the number of datablock header)
                 Following 'dataBlocks' words are the indices to the blocks.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResFileHeader_
{
    union 
    {
        char signature[4];
        u32  sigVal;
    };
    u16  byteOrder;
    u16  version;
    u32  fileSize;
    u16  headerSize;
    u16  dataBlocks;
}
NNSG3dResFileHeader;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResDataBlockHeader:

   Description: A header of a data block
                
                kind = the kind of a data block
                size = size of a data block
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDataBlockHeader_
{
    union
    {
        u32 kind;
        char chr[4];
    };
    u32 size;
}
NNSG3dResDataBlockHeader;


/////////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------*
   Name:        NNSG3dResDictTreeNode:

   Description: A node of a patricia tree

                refBit   = an index to a referred bit(0-127)
                idxLeft  = go left if the bit is zero
                idxRight = go right if the bit is one
                idxEntry = an index to a data entry
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTreeNode_
{
    u8  refBit;
    u8  idxLeft;
    u8  idxRight;
    u8  idxEntry;
}
NNSG3dResDictTreeNode;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResDict:

   Description: The dictionary header used to search for names.
                The index is the offset from &NNSG3dResDict.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDict_
{
    u8  revision;      // revision of dictionary
    u8  numEntry;      // num of entry(not that of tree node)
    u16 sizeDictBlk;   // size of this block
    u16 dummy_;
    u16 ofsEntry;      // (NNSG3dResDictEntryHeader*)   ((u8*)this + idxEntry)

    // patricia tree and its data entry
    NNSG3dResDictTreeNode node[1];  // variable size
}
NNSG3dResDict;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResName

   Description: A structure for a resource name
                '\0' must be filled if the length of string is less than 16.
 *---------------------------------------------------------------------------*/
#define NNS_G3D_RESNAME_SIZE    (16)
#define NNS_G3D_RESNAME_VALSIZE (NNS_G3D_RESNAME_SIZE / sizeof(u32))

typedef union NNSG3dResName_
{
    char name[NNS_G3D_RESNAME_SIZE];
    u32  val[NNS_G3D_RESNAME_VALSIZE];
}
NNSG3dResName;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResDictEntryHeader

   Description: The dictionary entry header used to search for names.
                There are sizeUnit-16 bytes of data after the names.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictEntryHeader_
{
    u16 sizeUnit;      // The size (in bytes) of one data  entry
    u16 ofsName;       // 
    u8  data[4];       // The NNSG3dResDictEntryItem list (The size of each is sizeUnit)
}
NNSG3dResDictEntryHeader;


/////////////////////////////////////////////////////////////////////////////////
//
//
// Texture Image
//
//


/*---------------------------------------------------------------------------*
    NNSG3dTexImageParam

    The texture image information inside the texture block
    When associating the material of the model, this information is copied
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_MASK  = 0x0000ffff,
    NNS_G3D_TEXIMAGE_PARAM_S_SIZE_MASK    = 0x00700000,
    NNS_G3D_TEXIMAGE_PARAM_T_SIZE_MASK    = 0x03800000,
    NNS_G3D_TEXIMAGE_PARAM_TEXFMT_MASK    = 0x1c000000,
    NNS_G3D_TEXIMAGE_PARAM_TR_MASK        = 0x20000000,

    NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_SHIFT = 0,
    NNS_G3D_TEXIMAGE_PARAM_S_SIZE_SHIFT   = 20,
    NNS_G3D_TEXIMAGE_PARAM_T_SIZE_SHIFT   = 23,
    NNS_G3D_TEXIMAGE_PARAM_TEXFMT_SHIFT   = 26,
    NNS_G3D_TEXIMAGE_PARAM_TR_SHIFT       = 29
}
NNSG3dTexImageParam;


/*---------------------------------------------------------------------------*
    NNSG3dTexImageParamEx

    ORIGW ORIGH are needed in the calculation of the texture matrix.
    The calculation can be sped up if they are the same as the NNSG3dTexImageParam value,
    so hold on to flag(WHSAME).
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_MASK  = 0x000007ff,
    NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_MASK  = 0x003ff800,
    NNS_G3D_TEXIMAGE_PARAMEX_WHSAME_MASK = 0x80000000,

    NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_SHIFT  = 0,
    NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_SHIFT  = 11,
    NNS_G3D_TEXIMAGE_PARAMEX_WHSAME_SHIFT = 31
}
NNSG3dTexImageParamEx;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexData

    The data entry of the texture name  dictionary.
    Normally, the offset to the variable length data is frequently in the dictionary data,
    but when it is fixed-length, it can be inserted as a dictionary entry.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTexData_
{
    // 31 30 29     28  26 25 23 22 20 19   16 15                       0
    //       Pltt0  TexFmt TSize SSize         The offset that was 3 bit right-shifted
    u32           texImageParam;  // NNSG3dTexImageParam

    // 31      30           22 21     11 10     0
    // SameBit                   OrigH     OrigW
    u32           extraParam;     // NNSG3dTexImageParamEx
}
NNSG3dResDictTexData;


/*---------------------------------------------------------------------------*
    NNSG3dResTexFlag

    When the texture  image (anything other than the 4x4 format) is loaded into the texture image slot,
    NNS_G3D_RESTEX_LOADED is set. It is reset when the texture image is unloaded.
    This is the value for NNSG3dResTexInfo::flag.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RESTEX_LOADED   = 0x0001
}
NNSG3dResTexFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResTexInfo

    The structure that NNSG3dResTex possesses.
    This holds the information related to texture images other than the 4x4COMP format.
    The texture image data is gathered into one data in each nsbmd/nsbtx file.
    It will load / unload with each gathering of data.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTexInfo_
{
    u32 vramKey;         // Stores the VRAM key of the gfd library specifications
    u16 sizeTex;         // A value equivalent to the texture data after a 3-bit right-shift is entered anew
    u16 ofsDict;
    u16 flag;            // NNSG3dResTexFlag
    u16 dummy_;
    u32 ofsTex;          // The offset to the texture data (The offset from &NNSG3dResTexImage)
}
NNSG3dResTexInfo;


/*---------------------------------------------------------------------------*
    NNSG3dResTex4x4Flag

    NNS_G3D_RESTEX4x4_LOADED is set when the texture image of the 4x4COMP format
    is loaded into the texture image slot. It is reset when the texture image is unloaded.
    This is a value for the NNSG3dResTex4x4Info::flag.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RESTEX4x4_LOADED   = 0x0001   // Whether or not it is loaded into VRAM
}
NNSG3dResTex4x4Flag;


/*---------------------------------------------------------------------------*
    NNSG3dResTex4x4Info:

    The structure that NNSG3dResTex possesses.
    It holds the information related to the texture image of the 4x4COMP format.
    The texture image data is gathered into one data in each nsbmd/nsbtx file.
    It will load / unload with each gathering of data.   
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTex4x4Info_
{
    u32 vramKey;         // Stores the VRAM key of the gfd library specifications
    u16 sizeTex;         // The value when then the texture data is 3-bit right-shifted (2-bit for the palette index)
    u16 ofsDict;
    u16 flag;            // The 0 bit is the flag for whether or not the texture is loaded into VRAM
    u16 dummy_;
    u32 ofsTex;          // The offset to the texture data (The bytes from &NNSG3dResTexImage)
    u32 ofsTexPlttIdx;   // The offset to the texture palette index data (The bytes from &NNSG3dResTexImage)
}
NNSG3dResTex4x4Info;


/*---------------------------------------------------------------------------*
    NNSG3dResDictPlttData

    The data entry of the palette name dictionary.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictPlttData_
{
    u16 offset;          // That which 3-bit right-shifted the offset towards the origin of the palette data.
    u16 flag;            // A flag that determines whether or not the 0 bit is the Tex4Pltt
}
NNSG3dResDictPlttData;


/*---------------------------------------------------------------------------*
    NNSG3dResPlttFlag

    NNS_G3D_RESPLTT_LOADED is set when the texture palette is
    loaded into the texture palette slot. It is reset when the texture image is unloaded.
    NNS_G3D_RESPLTT_USEPLTT4 is set when the 4-color palette is included in the palette data.
     This is due to the fact that there are limits on where a 4-color palette can be loaded.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RESPLTT_LOADED   = 0x0001,   // Whether or not it is loaded into VRAM
    NNS_G3D_RESPLTT_USEPLTT4 = 0x8000    // Whether or not the 4-color palette is in the block
}
NNSG3dResPlttFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResPlttInfo

    The structure that NNSG3dResTex possesses.
    This holds the information related to the texture palette.
    For each nsbmd/nsbtx file, the texture palette data are gathered into one,
    It will load / unload with each gathering of data.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResPlttInfo_
{
    u32 vramKey;      // Stores the VRAM key of the gfd library specifications
    u16 sizePltt;     // Takes what 3-bit right-shifted the data size of the texture palette.
    u16 flag;         // NNSG3dResPlttFlag
    u16 ofsDict;
    u16 dummy_;
    u32 ofsPlttData;
}
NNSG3dResPlttInfo;


/*---------------------------------------------------------------------------*
    NNSG3dResTex
   
    Each type of offset being stored, aside from the offset data for the slot in VRAM,
    is using &NNSG3dResTex as its base.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTex_
{
    NNSG3dResDataBlockHeader header;
    NNSG3dResTexInfo         texInfo;
    NNSG3dResTex4x4Info      tex4x4Info;
    NNSG3dResPlttInfo        plttInfo;
    NNSG3dResDict            dict;       // The dictionary for the texture
    // The dictionary for the palette:
    // texture data (anything other than 4x4)
    // texture data (4x4)
    // texture palette index data
    // texture palette data
    //  
}
NNSG3dResTex;


/////////////////////////////////////////////////////////////////////////////////
//
//
// Material
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResDictMatData

    The material name dictionary data.
    The material data is of variable length, so the offset to NNSG3dResMatData (other) is maintained.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictMatData_
{
    u32           offset;       // &The offset from NNSG3dResMat data
}
NNSG3dResDictMatData;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexToMatIdxData

    texture name -> material index string dictionary data
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTexToMatIdxData_
{
    u16           offset;       // The offset from NNSG3dResMat
    u8            numIdx;       // The number of material IDs that correspond to the texture name
    u8            flag;         // The flag for whether or not the texture <-> material binding was performed
}
NNSG3dResDictTexToMatIdxData;


/*---------------------------------------------------------------------------*
    NNSG3dResDictPlttToMatIdxData

    palette name -> material index string dictionary data
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictPlttToMatIdxData_
{
    u16           offset;       // The offset from NNSG3dResMat
    u8            numIdx;       // The number of material IDs that correspond to the palette name
    u8            flag;         // The flag for whether or not the palette <-> material binding was performed
}
NNSG3dResDictPlttToMatIdxData;


/*---------------------------------------------------------------------------*
    NNSG3dMatFlag

    The flag information related to the material.
    If the bit is set, it becomes a flag that uses X.
    At the time of initialization, there is also a flag operated when associating with the texture data.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_MATFLAG_TEXMTX_USE       = 0x0001, // Whether or not to use the texture  matrix
    NNS_G3D_MATFLAG_TEXMTX_SCALEONE  = 0x0002, // ON if the scale is 1.0  (when using the texture matrix)
    NNS_G3D_MATFLAG_TEXMTX_ROTZERO   = 0x0004, // ON if not rotating  (when using the texture matrix)
    NNS_G3D_MATFLAG_TEXMTX_TRANSZERO = 0x0008, // ON if not translating (when using the texture matrix)
    NNS_G3D_MATFLAG_ORIGWH_SAME      = 0x0010, // Set if the width and height of the texture is the same as that of the system
    NNS_G3D_MATFLAG_WIREFRAME        = 0x0020, // ON if displaying wireframe
    NNS_G3D_MATFLAG_DIFFUSE          = 0x0040, // ON if designating the diffuse with the material
    NNS_G3D_MATFLAG_AMBIENT          = 0x0080, // ON if designating the ambient with the material
    NNS_G3D_MATFLAG_VTXCOLOR         = 0x0100, // ON if designating the vtxcolor flag with the material
    NNS_G3D_MATFLAG_SPECULAR         = 0x0200, // ON if designating the specular with the material
    NNS_G3D_MATFLAG_EMISSION         = 0x0400, // ON if designating the emission with the material
    NNS_G3D_MATFLAG_SHININESS        = 0x0800, // ON if designating the shininess flag with the material
    NNS_G3D_MATFLAG_TEXPLTTBASE      = 0x1000, // ON if designating the texture palette base addressN
    NNS_G3D_MATFLAG_EFFECTMTX        = 0x2000  // ON if effect_mtx exists
}
NNSG3dMatFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResMatData

    Individual material data.
    When expanding data, change the value of itemTag and locate the data after the structure.
     
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMatData_
{
    NNSG3dItemTag itemTag;                // The material type recognition tag (For this structure, it must be NNS_G3D_ITEMTAG_MAT_STANDARD)
    u16           size;                   // The size
    u32           diffAmb;                // The parameters for the MaterialColor0 command
    u32           specEmi;                // The parameters for the MaterialColor1 command
    u32           polyAttr;               // The parameters for the PolygonAttr command
    u32           polyAttrMask;           // The bit designated by the material is 1
    u32           texImageParam;          // The parameters for the TexImageParam command
    u32           texImageParamMask;      // The bit designated by the material is 1
    u16           texPlttBase;            // The parameters for the TexPlttBase command
    u16           flag;                   // Cast to NNSG3dMatFlag
    u16           origWidth, origHeight;  // Transmitted from the texture image at the time of initialization
    fx32          magW;                   // The width / origWidth of the bound texture
    fx32          magH;                   // The height / origHeight of the bound texture
//    fx32 scaleS, scaleT;                // exists if NNS_G3D_MATFLAG_TEXMTX_SCALEONE is off
//    fx16 rotSin, rotCos;                // exists if NNS_G3D_MATFLAG_TEXMTX_ROTZERO is off
//    fx32 transS, transT;                // exists if NNS_G3D_MATFLAG_TEXMTX_TRANSZERO is off
//    fx32 effectMtx[16];                 // exists if NNS_G3D_MATFLAG_ENV_EFFECTMTX is on
}
NNSG3dResMatData;


/*---------------------------------------------------------------------------*
    NNSG3dResMat

    This is the structure that gathers the collection of materials held by one model.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMat_
{
    u16              ofsDictTexToMatList;  // The offset to the texture -> material ID string dictionary
    u16              ofsDictPlttToMatList; // The offset to the palette -> material ID string dictionary
    NNSG3dResDict    dict;                 // The dictionary to the NNSG3dResDictMatData
//  NNSG3dResDict    dictTexToMatList;     // (u8*)this + idxDictTexToMatList
//  NNSG3dResDict    dictPlttToMatList;    // (u8*)this + idxDictPlttToMatList
//  u8[]             matIdxData;           // (u8*)this + idxMatIdxData
//  seq of NNSG3dResMatData matData        // (u8*)this + idxMatData
}
NNSG3dResMat;


/////////////////////////////////////////////////////////////////////////////////
//
//
// shape
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResDictShpData

    The shape name dictionary data.
    The shape data is of variable length, so it holds on to the offset to NNSG3dResShpData (other)
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictShpData_
{
    u32           offset;  // (u8*)&NNSG3DResShp + offset -> NNSG3dResShpData&
}
NNSG3dResDictShpData;


/*---------------------------------------------------------------------------*
    NNSG3dShpFlag

    This is a flag that shows the characteristics of the shape display list.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SHPFLAG_USE_NORMAL     = 0x00000001,    // The Normal command exists inside the DL
    NNS_G3D_SHPFLAG_USE_COLOR      = 0x00000002,    // The Color command exists inside the DL
    NNS_G3D_SHPFLAG_USE_TEXCOORD   = 0x00000004,    // The TexCoord command exists inside the DL
    NNS_G3D_SHPFLAG_USE_RESTOREMTX = 0x00000008     // The RestoreMtx command exists inside the DL
}
NNSG3dShpFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResShpData

    Individual shape data.
    Even when describing shapes on different data structures, only the itemTag and size need to be kept.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResShpData_
{
    NNSG3dItemTag itemTag;          // The shape type recognition tag (For this structure, it must be NNS_G3D_ITEMTAG_SHP_STANDARD)
    u16           size;             // The size
    u32           flag;             // NNSG3dShpFlag
    u32           ofsDL;            // The offset to the display list
    u32           sizeDL;           // The size of the display list
}
NNSG3dResShpData;


/*---------------------------------------------------------------------------*
    NNSG3dResShp

    This is the structure that gathers the collection of shapes held by one model.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResShp_
{
    NNSG3dResDict dict;             // The dictionary to the NNSG3dResDictShpData
    // NNSG3dResShpData[] shpData;
    // u32[]              DL;
}
NNSG3dResShp;


#ifndef _CVTR
typedef struct NNSG3dResEvpMtx_
{
    MtxFx43 invM;
    MtxFx33 invN;
}
NNSG3dResEvpMtx;
#endif

/////////////////////////////////////////////////////////////////////////////////
//
//
// ModelInfo
//
//


/*---------------------------------------------------------------------------*
    NNSG3dSbcType

    This designates the type of Structure Byte Code held by the model.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SBCTYPE_NORMAL       = 0        // There is currently only one type
}
NNSG3dSbcType;


/*---------------------------------------------------------------------------*
    NNSG3dScalingRule

    This designates the scaling rules of the model.
    It corresponds with the <model_info>::scaling_rule of the intermediate file format.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SCALINGRULE_STANDARD = 0,   // General matrix calculations are OK
    NNS_G3D_SCALINGRULE_MAYA     = 1,   // A node exists where the Segment Scaling Compensate of Maya must be taken into consideration
    NNS_G3D_SCALINGRULE_SI3D     = 2    // Scaling calculations for Softimage3D must be performed
}
NNSG3dScalingRule;


/*---------------------------------------------------------------------------*
    NNSG3dTexMtxMode

    This designates the calculation method of the texture matrix.
    It corresponds to the <model_info>::tex_mtx_mode of the intermediate file format.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXMTXMODE_MAYA      = 0x00, // Applies the Maya calculation method
    NNS_G3D_TEXMTXMODE_SI3D      = 0x01, // Applies the Softimage3D calculation method
    NNS_G3D_TEXMTXMODE_3DSMAX    = 0x02, // Applies the 3dsMax calculation method
    NNS_G3D_TEXMTXMODE_XSI       = 0x03  // Applies the XSI calculation method
}
NNSG3dTexMtxMode;


/*---------------------------------------------------------------------------*
   Name:        NNSG3dResMdlInfo

   Description: The information attached to the model
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMdlInfo_
{
    u8         sbcType;       // Structure Byte Code type identifier (NNSG3dSbcType)
    u8         scalingRule;   // scaling rule identifier (NNSG3dScalingRule)
    u8         texMtxMode;    // The texture matrix calculation formula identifier (NNSG3dTexMtxMode)
    u8         numNode;

    u8         numMat;
    u8         numShp;
    u8         firstUnusedMtxStackID;
    u8         dummy_;

    // The scale value to multiply by the vertex position coordinates and its reciprocal.
    // When anything other than FX32_ONE, the scale command of the geometry engine is used to
    // process the vertex position coordinates. When the matrix is restored (using the envelope) inside the display list of the shape,
    // scaling is performed by posScale after restoration.
    fx32       posScale;      
    fx32       invPosScale;

    u16        numVertex;    // The value of the IMD file <output_info>::vertex_size
    u16        numPolygon;   // The value of the IMD file <output_info>::polygon_size
    u16        numTriangle;  // The value of the IMD file <output_info>::triangle_size
    u16        numQuad;      // The value of the IMD file <output_info>::quad_size

    fx16       boxX, boxY, boxZ; // These become the actual coordinates if multiplied by posScale
    fx16       boxW, boxH, boxD; // These become the actual coordinates if multiplied by posScale

    fx32       boxPosScale;
    fx32       boxInvPosScale;
}
NNSG3dResMdlInfo;


/////////////////////////////////////////////////////////////////////////////////
//
//
// node
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResDictNodeData

    The node name dictionary data.
    This holds the offset to the NNSG3dResNodeData.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictNodeData_
{
    u32       offset;       // The offset data from &NNSG3dResNodeInfo
}
NNSG3dResDictNodeData;


/*---------------------------------------------------------------------------*
    NNSG3dSRTFlag

    The flag information related to the SRT data of the node.
    The kind of data that exists subsequently depends upon this flag.
    It is intended to reduce the amount of data and abbreviate excess calculations.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SRTFLAG_TRANS_ZERO       = 0x0001, // Trans=(0,0,0), the output of the translation data is abbreviated
    NNS_G3D_SRTFLAG_ROT_ZERO         = 0x0002, // Rot=Identity,  the output of the rotation matrix data is abbreviated
    NNS_G3D_SRTFLAG_SCALE_ONE        = 0x0004, // Scale=(1,1,1), the output of the scale data is abbreviated
    NNS_G3D_SRTFLAG_PIVOT_EXIST      = 0x0008, // A +1 or -1 element (Pivot) exists in the rotation matrix
    NNS_G3D_SRTFLAG_IDXPIVOT_MASK    = 0x00f0, // The location of the +1 / -1 element (Pivot)
    NNS_G3D_SRTFLAG_PIVOT_MINUS      = 0x0100, // If ON, the Pivot is -1, if OFF it is +1
    NNS_G3D_SRTFLAG_SIGN_REVC        = 0x0200, // If set, C is the opposite code of B
    NNS_G3D_SRTFLAG_SIGN_REVD        = 0x0400, // If set, D is the opposite code of A
    NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK = 0xf800, // In the stack if 0-30, not in the stack if 31

    NNS_G3D_SRTFLAG_IDENTITY         = NNS_G3D_SRTFLAG_TRANS_ZERO |
                                       NNS_G3D_SRTFLAG_ROT_ZERO   |
                                       NNS_G3D_SRTFLAG_SCALE_ONE,

    NNS_G3D_SRTFLAG_IDXPIVOT_SHIFT    = 4,
    NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT = 11
}
NNSG3dSRTFlag;


/*---------------------------------------------------------------------------*
    NNSG3dResNodeData

    The type of data that will subsequently appear changes via the value of the flag (the NNSG3dSRTFlag).
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResNodeData_
{
    u16 flag;       // NNSG3dSRTFlag
    fx16 _00;
    // exists if !NNS_G3D_SRT_FLAG_TRANS_ZERO
    //      fx32 Tx, Ty, Tz;
    // exists if !NNS_G3D_SRT_FLAG_ROT_ZERO && !NNS_G3D_SRT_FLAG_PIVOT_EXIST
    //      fx16 _01, _02;
    //      fx16 _10, _11;
    //      fx16 _12, _20;
    //      fx16 _21, _22;
    // exists if !NNS_G3D_SRT_FLAG_ROT_ZERO && NNS_G3D_SRT_FLAG_PIVOT_EXIST
    //      fx16 A, B;
    // exists if !NNS_G3D_SCALE_ONE
    //      fx32 Sx, Sy, Sz;
    //      fx32 InvSx, InvSy, InvSz;
}
NNSG3dResNodeData;


/*---------------------------------------------------------------------------*
    NNSG3dResNodeInfo

    A structure that gathers the set of node SRT information that is kept in one model.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResNodeInfo_
{
    NNSG3dResDict  dict;
    // a seq of NNSG3dResNodeSRTInfo
}
NNSG3dResNodeInfo;


/////////////////////////////////////////////////////////////////////////////////
//
//
// model
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResMdl

    A structure that gathers all the information (other than the texture) about one model.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMdl_
{
    u32                size;        // The size of the model
    u32                ofsSbc;      // The offset of the SBC that used &NNSG3dResMdl as the origin
    u32                ofsMat;      // The offset of the NNSG3dResMat that used &NNSG3dResMdl as the origin
    u32                ofsShp;      // The offset of the NNSG3dResShp that used &NNSG3dResMdl as the origin
    u32                ofsEvpMtx;   // The offset of NNSG3dEvpMtx that uses &NNSG3dResMdl as the origin
    NNSG3dResMdlInfo   info;        // The model information (Fixed length)
    NNSG3dResNodeInfo  nodeInfo;    // The node information
    // SBC
    // Materials
    // Shapes
    // EvpMtxs
}
NNSG3dResMdl;


/////////////////////////////////////////////////////////////////////////////////
//
//
// model  set
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResDictMdlSetData

    The offset (whose origin is &NNSG3dResMdlSet) to the individual models is stored here
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictMdlSetData_
{
    u32 offset;
}
NNSG3dResDictMdlSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResMdlSet

    The model set block structure. Be careful of the fact that this is a structure that includes a data block header.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMdlSet_
{
    NNSG3dResDataBlockHeader header;
    NNSG3dResDict            dict;
    // a seq of NNSG3dResMdl follows
}
NNSG3dResMdlSet;



/////////////////////////////////////////////////////////////////////////////////
//
//
// Structure Byte Code(Sbc)
//
//

/*---------------------------------------------------------------------------*
    The SBC command set

 *---------------------------------------------------------------------------*/
#define NNS_G3D_SBC_NOP           0x00     // NOP
#define NNS_G3D_SBC_RET           0x01     // RET
#define NNS_G3D_SBC_NODE          0x02     // NODE      idxNode, visibility
#define NNS_G3D_SBC_MTX           0x03     // MTX       stackIdx
#define NNS_G3D_SBC_MAT           0x04     // MAT       idxMat
#define NNS_G3D_SBC_SHP           0x05     // SHP       idxShp
#define NNS_G3D_SBC_NODEDESC      0x06     // NODEDESC  idxNode, idxMtxDest, idxMtxParent
#define NNS_G3D_SBC_BB            0x07
#define NNS_G3D_SBC_BBY           0x08
#define NNS_G3D_SBC_NODEMIX       0x09     // NODEMIX   idxNode, idxMtxDest, numMtx, idxMtx1, ratio1, ..., idxMtxN, ratioN
#define NNS_G3D_SBC_CALLDL        0x0a     // CALLDL    u32(relative), u32(size)
#define NNS_G3D_SBC_POSSCALE      0x0b     // POSSCALE
#define NNS_G3D_SBC_ENVMAP        0x0c
#define NNS_G3D_SBC_PRJMAP        0x0d


#define NNS_G3D_SBC_NOCMD         0x1f     // does not exist in compiled structure byte code mnemonic

#define NNS_G3D_SBC_COMMAND_NUM   0x20

#define NNS_G3D_SBCFLG_000        0x00
#define NNS_G3D_SBCFLG_001        0x20
#define NNS_G3D_SBCFLG_010        0x40
#define NNS_G3D_SBCFLG_011        0x60
#define NNS_G3D_SBCFLG_100        0x80
#define NNS_G3D_SBCFLG_101        0xa0
#define NNS_G3D_SBCFLG_110        0xc0
#define NNS_G3D_SBCFLG_111        0xe0

#define NNS_G3D_SBCCMD_MASK       0x1f
#define NNS_G3D_SBCFLG_MASK       0xe0


typedef enum
{
    NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_APPLY  = 0x01,
    NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_PARENT = 0x02
}
NNSG3dSbcNodeDescFlag;




/////////////////////////////////////////////////////////////////////////////////
//
//
// animation
//
//

//
// REQUIREMENT:
// In one block of the animation file, each animation structure
// held at the start of NNSG3dResAnmHeader must be able to be reached
// from the dictionary from the NNSG3dResAnmSet, via the NNSG3dResDictAnmSetData.
//

/*---------------------------------------------------------------------------*
    NNSG3dResAnmHeader
    
    The header information that comes at the start of each animation resource
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResAnmHeader_
{
    // 'M' for material animation
    // 'J' for joint animation
    // 'V' for visibility animation
    u8  category0;
    u8  revision;
    // 'CA' for character(joint) animation
    // 'VA' for visibility animation
    // 'MA' for material color animation
    // 'TP' for texture pattern animation
    // 'TA' for texture SRT animation
    u16  category1; 
}
NNSG3dResAnmHeader;

/*---------------------------------------------------------------------------*
    NNSG3dResAnmCommon

    All animations must have a header and a numFrame.
 *---------------------------------------------------------------------------*/
typedef struct
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u16                dummy_;
}
NNSG3dResAnmCommon;


/*---------------------------------------------------------------------------*
    NNSG3dResDictAnmSetData

    Individual animations can be accomplished with the (u8*)&NNSG3dResAnmSet+offset
 *---------------------------------------------------------------------------*/
typedef struct
{
    u32 offset;                      // The offset for accomplishing individual animations is stored here.
}
NNSG3dResDictAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResAnmSet

    This is a structure for gathering animation sets.
 *---------------------------------------------------------------------------*/
typedef struct
{
    NNSG3dResDataBlockHeader header;
    NNSG3dResDict            dict;   // file name -> each AnmSetData
}
NNSG3dResAnmSet;



/////////////////////////////////////////////////////////////////////////////////
//
//
// visibility  animation
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResVisAnm

    This is a structure that gathers data on one part of the visibility animation
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResVisAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u16                numNode;
    u16                size;
    u16                dummy_;
    // variable length
    u32                visData[1];
}
NNSG3dResVisAnm;

/*---------------------------------------------------------------------------*
    NNSG3dResDictVisAnmSetData

    The offset to the data for individual visibility animations (NNSG3dResVisAnm) is stored here.
    It uses &NNSG3dResVisAnmSet as the origin.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictVisAnmSetData;

/*---------------------------------------------------------------------------*
    NNSG3dResVisAnmSet

    This is a structure that gathers visibility animation sets.
    NNSG3dResVisAnm is listed in the back.
    The dictionary references NNSG3dResVisAnm via the NNSG3dResDictVisAnmSetData.
    The kind must be equal to NNS_G3D_SIGNATURE_NSBVA.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResVisAnmSet;


/////////////////////////////////////////////////////////////////////////////////
//
//
// material color animation
//
//

/*---------------------------------------------------------------------------*
    NNSG3dMatCElem

    The value of each data member of NNSG3dResDictMatCAnmData.
    If NNS_G3D_MATCANM_ELEM_CONST is set, the lower 16 bits are a constant color value.
    If not set, they are the offset to the color data string.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_MATCANM_ELEM_CONST                 = 0x20000000, // ON when the data is not animating
    NNS_G3D_MATCANM_ELEM_STEP_1                = 0x00000000, // When there is data in every frame
    NNS_G3D_MATCANM_ELEM_STEP_2                = 0x40000000, // When there is data in every 2nd frame
    NNS_G3D_MATCANM_ELEM_STEP_4                = 0x80000000, // When there is data in every 4th frame

    NNS_G3D_MATCANM_ELEM_STEP_MASK             = 0xc0000000,
    NNS_G3D_MATCANM_ELEM_LAST_INTERP_MASK      = 0x1fff0000,
    NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_MASK  = 0x0000ffff,

    NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_SHIFT = 0,
    NNS_G3D_MATCANM_ELEM_LAST_INTERP_SHIFT     = 16
}
NNSG3dMatCElem;


/*---------------------------------------------------------------------------*
    NNSG3dResDictMatCAnmData

    Information about each element of the material color animation
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictMatCAnmData_
{
    // NNSG3dMatCElem:
    // 31  30 29      28         16 15               0
    // frmStp isConst iFrame_interp offset/constantCol
    u32 diffuse;
    u32 ambient;
    u32 specular;
    u32 emission;
    u32 polygon_alpha;
}
NNSG3dResDictMatCAnmData;


/*---------------------------------------------------------------------------*
    NNSG3dMatCAnmOption

    The options for the material color animation (designate with the intermediate file)
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_MATCANM_OPTION_INTERPOLATION              = 0x0001, // Corresponds to the <mat_color_info>::interpolation
    NNS_G3D_MATCANM_OPTION_END_TO_START_INTERPOLATION = 0x0002  // Corresponds to the <mat_color_info>::interp_end_to_start
}
NNSG3dMatCAnmOption;


/*---------------------------------------------------------------------------*
    NNSG3dResMatCAnm

    A structure that gathers data about one part of the material color animation
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResMatCAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u16                flag;     // NNSG3dMatCAnmOption
    NNSG3dResDict      dict;
}
NNSG3dResMatCAnm;


/*---------------------------------------------------------------------------*
    NNSG3dResDictMatCAnmSetData

    The offset to the data for individual material color animations is stored here.
    It uses &NNSG3dResMatCAnmSet as the origin.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictMatCAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResMatCAnmSet

    A structure that gathers the sets of the material color animation.
    NNSG3dResMatCAnm is listed in the back.
    The dictionary references NNSG3dResMatCAnm via the NNSG3dResDictMatCAnmSetData.
    The kind must be equal to NNS_G3D_DATABLK_MATC_ANM.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResMatCAnmSet;


/////////////////////////////////////////////////////////////////////////////////
//
//
// texture pattern animation
//
//


/*---------------------------------------------------------------------------*
    NNSG3dResTexPatAnmFV

    This is the  frame ->(texture and palette) data.
    This data list makes up the texture pattern animation.
    This means that the texture is swapped into idTex and the palette is swapped from idxFrame
    into idPltt from idxFrame
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTexPatAnmFV_
{
    u16 idxFrame;
    u8  idTex;
    u8  idPltt;
}
NNSG3dResTexPatAnmFV;


/*---------------------------------------------------------------------------*
    NNSG3dTexPatAnmOption

    This is the option for the texture pattern animation.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXPATANM_OPTION_NOPLTT = 0x0001    // ON when the palette is not animating
}
NNSG3dTexPatAnmOption;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexPatAnmData

    This is the data of the texture pattern animation held in each material.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTexPatAnmData_
{
    u16  numFV;           // The number of NNSG3dResTexPatAnmFV
    u16  flag;            // NNSG3dTexPatAnmOption (Not currently used)
    fx16 ratioDataFrame;  // This is the numFV / numFrame. It is a hint for data searching.
    u16  offset;          // The offset to the FV data that used &NNSG3dResTexPatAnm as the origin
}
NNSG3dResDictTexPatAnmData;


/*---------------------------------------------------------------------------*
    NNSG3dResTexPatAnm

    The structure that gathers one part of the texture pattern animation data
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTexPatAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u8                 numTex;       // The number of textures used
    u8                 numPltt;      // The number of palettes used
    u16                ofsTexName;   // The offset to the texture name array (numTex NNSG3dResNames are listed)
    u16                ofsPlttName;  // The offset to the palette name array (numPltt NNSG3dResNames are listed)
    NNSG3dResDict      dict;         // The dictionary that references NNSG3dResDictTexPatAnmData from the material name
    // Frame-Value data
    // texture name array
    // palette name array
}
NNSG3dResTexPatAnm;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexPatAnmSetData

    The offset to the data of individual texture pattern animations is stored here.
    It uses &NNSG3dResTexPatAnmSet as the origin.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictTexPatAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResTexPatAnmSet

    The structure that gathers the sets of the texture pattern animations.
    The NNSG3dResTexPatAnm are arranged at the back.
    The dictionary references the NNSG3dResTexPatAnm via the NNSG3dResDictTexPatAnmSetData
    The kind must be NNS_G3D_DATABLK_TEXPAT_ANM.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResTexPatAnmSet;


/////////////////////////////////////////////////////////////////////////////////
//
//
// Texture SRT Animation
//
//

/*---------------------------------------------------------------------------*
    NNSG3dTexSRTElem

    Each kind of texture SRT animation flag.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_TEXSRTANM_ELEM_FX16   = 0x10000000, // ON when the data is held by the fx16 vector (Always OFF when rot)
    NNS_G3D_TEXSRTANM_ELEM_CONST  = 0x20000000, // ON when the data is not animating
    NNS_G3D_TEXSRTANM_ELEM_STEP_1 = 0x00000000, // When there is data in every frame
    NNS_G3D_TEXSRTANM_ELEM_STEP_2 = 0x40000000, // When there is data in every 2nd frame
    NNS_G3D_TEXSRTANM_ELEM_STEP_4 = 0x80000000, // When there is data in every 4th frame

    NNS_G3D_TEXSRTANM_ELEM_STEP_MASK         = 0xc0000000,
    NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_MASK  = 0x0000ffff,

    NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_SHIFT = 0
}
NNSG3dTexSRTElem;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexSRTAnmData

    The information about the texture SRT animation of each material.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResDictTexSRTAnmData_
{
    // word #0(NNSG3dTexSRTElem)
    // 31  30 29      28     15          0
    // frmStp isConst isFx16 iFrame_interp
    // word #1
    // 31               0
    // offset/constantVal
    u32 scaleS;             // NNSG3dTexSRTElem
    u32 scaleSEx;           // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST, otherwise, the offset to the data string

    u32 scaleT;             // NNSG3dTexSRTElem
    u32 scaleTEx;           // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST, otherwise, the offset to the data string

    u32 rot;                // NNSG3dTexSRTElem
    u32 rotEx;              // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST, otherwise, the offset to the data string
                            //
                            // The rot is stored as a pair of cos values and sin values
                            // 31          15          0
                            // cos in fx16   sin in fx16
                            
                            
    u32 transS;             // NNSG3dTexSRTElem
    u32 transSEx;           // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST, otherwise, the offset to the data string

    u32 transT;             // NNSG3dTexSRTElem
    u32 transTEx;           // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST, otherwise, the offset to the data string
}
NNSG3dResDictTexSRTAnmData;


/*---------------------------------------------------------------------------*
    NNSG3dTexSRTAnmOption

    The texture SRT animation option (designate with the intermediate file)
 *---------------------------------------------------------------------------*/
typedef enum NNSG3dTexSRTAnmOption_
{
    NNS_G3D_TEXSRTANM_OPTION_INTERPOLATION              = 0x01, // Corresponds to <tex_srt_info>::interpolation
    NNS_G3D_TEXSRTANM_OPTION_END_TO_START_INTERPOLATION = 0x02  // Corresponds to <tex_srt_info>::interp_end_to_start
}
NNSG3dTexSRTAnmOption;


/*---------------------------------------------------------------------------*
    NNSG3dResTexSRTAnm

    The structure that gathers one part of the texture SRT animation data.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResTexSRTAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u8                 flag;             // NNSG3dTexSRTAnmOption
    u8                 texMtxMode;       // NNSG3dTexMtxMode
    NNSG3dResDict      dict;             // The material name -> the NNSG3dResDictTexSRTAnmData dictionary
}
NNSG3dResTexSRTAnm;


/*---------------------------------------------------------------------------*
    NNSG3dResDictTexSRTAnmSetData

    The offset to the data of individual texture SRT animations is stored here.
    It uses &NNSG3dResTexSRTAnmSet as the origin.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictTexSRTAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResTexSRTAnmSet

    The structure that gathers the sets of the texture SRT animations.
    NNSG3dResTexSRTAnm is listed in the back.
    The dictionary that references NNSG3dResTexSRTAnm via the NNSG3dResDictTexSRTAnmSetData
    The kind must be equal to NNS_G3D_SIGNATURE_NSBTA.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResTexSRTAnmSet;


/////////////////////////////////////////////////////////////////////////////////
//
//
// joint  animation
//
//

/*---------------------------------------------------------------------------*
    NNSG3dResJntAnmSRT and friends

    Describes the SRT animation information of one joint.
    The memory layout after this is determined by which bit of the info is ON.
    Successive data is arranged in TRS order.
    
    NNS_G3D_JNTANM_SRTINFO_IDENTITY
        --> Subsequent data does not exist
        NNS_G3D_JNTANM_SRTINFO_IDENTITY_T -- Trans = (0, 0, 0)
        NNS_G3D_JNTANM_SRTINFO_BASE_T     --  References the Trans of the model
            --> The Translation data does not exist
            NNS_G3D_JNTANM_SRTINFO_CONST_TX -- The Trans X component is const
                --> Gets a 1 word portion of the Tx data / otherwise NNSG3dJntAnmTInfo(2wd)
            NNS_G3D_JNTANM_SRTINFO_CONST_TY -- The Trans Y component is const
                --> Gets a 1 word portion of the Ty data / otherwise NNSG3dJntAnmTInfo(2wd)
            NNS_G3D_JNTANM_SRTINFO_CONST_TZ -- The Trans Z component is const
                --> Gets a 1 word portion of the Tz data / otherwise NNSG3dJntAnmTInfo(2wd)
        NNS_G3D_JNTANM_SRTINFO_IDENTITY_R -- Rot = I
        NNS_G3D_JNTANM_SRTINFO_BASE_R     --  References the rot of the model
            --> The rotation data does not exist
            NNS_G3D_JNTANM_SRTINFO_CONST_R -- Rot is const
                --> The index data to a 1 word portion of RIdx  / otherwise NNSG3dJntAnmRInfo(2wd)
        NNS_G3D_JNTANM_SRTINFO_IDENTITY_S -- Scale = (1, 1, 1)
        NNS_G3D_JNTANM_SRTINFO_BASE_S     --  References the Scale of the model
            NNS_G3D_JNTANM_SRTINFO_CONST_SX -- The Scale X component is const
                --> Gets a 2 word portion of the Sx and InvSx data / otherwise NNSG3dJntAnmSInfo(2wd)
            NNS_G3D_JNTANM_SRTINFO_CONST_SY -- The Scale Y component is const
                --> Gets a 2 word portion of the Sy and InvSy data / otherwise NNSG3dJntAnmSInfo(2wd)
            NNS_G3D_JNTANM_SRTINFO_CONST_SZ -- The Scale Z component is const
                --> Gets a 2 word portion of the Sz and InvSz data / otherwise NNSG3dJntAnmSInfo(2wd)

    The offset below is the offset from the SRT data region
    NNSG3dJntAnmTInfo
        When NNS_G3D_JNTANM_TINFO_FX16ARRAY,
        the fx16 type array is referenced by the offset.

    NNSG3dJntAnmRInfo
        The RotIdx array is referenced by the offset.
        Actual data will be accessed from RotIdx.

    NNSG3dJntAnmSInfo

    RotIdx array
        A list of 16-bit values, the index to the Rot3 array (the compression 3x3 matrix) if 0x8000 is ON.
        If OFF, the index to the Rot9 array.
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    NNSG3dJntAnmSRTTag

    The behavior of the SRT data is described in this.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_SRTINFO_IDENTITY    = 0x00000001,    // ON when there are no changes to the SRT
    NNS_G3D_JNTANM_SRTINFO_IDENTITY_T  = 0x00000002,    // ON when Trans remains (0,0,0)
    NNS_G3D_JNTANM_SRTINFO_BASE_T      = 0x00000004,    // ON when the value of the model is used in Trans
    NNS_G3D_JNTANM_SRTINFO_CONST_TX    = 0x00000008,    // ON when Tx is a constant
    NNS_G3D_JNTANM_SRTINFO_CONST_TY    = 0x00000010,    // ON when Ty is a constant
    NNS_G3D_JNTANM_SRTINFO_CONST_TZ    = 0x00000020,    // ON when Tz is a constant
    NNS_G3D_JNTANM_SRTINFO_IDENTITY_R  = 0x00000040,    // ON when there is no Rot
    NNS_G3D_JNTANM_SRTINFO_BASE_R      = 0x00000080,    // ON when the value of the model is used in Rot
    NNS_G3D_JNTANM_SRTINFO_CONST_R     = 0x00000100,    // ON when Rot is a constant
    NNS_G3D_JNTANM_SRTINFO_IDENTITY_S  = 0x00000200,    // ON when the Scale is not used
    NNS_G3D_JNTANM_SRTINFO_BASE_S      = 0x00000400,    // ON when the value of the model is used in Scale
    NNS_G3D_JNTANM_SRTINFO_CONST_SX    = 0x00000800,    // ON when Sx is a constant
    NNS_G3D_JNTANM_SRTINFO_CONST_SY    = 0x00001000,    // ON when Sy is a constant
    NNS_G3D_JNTANM_SRTINFO_CONST_SZ    = 0x00002000,    // ON when Sz is a constant
    
    NNS_G3D_JNTANM_SRTINFO_NODE_MASK   = 0xff000000,    // Takes the node ID. Set so that it is OK if not defined by all nodes.
    NNS_G3D_JNTANM_SRTINFO_NODE_SHIFT  = 24
}
NNSG3dJntAnmSRTTag;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmTInfo

    Information about the Trans data.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_TINFO_STEP_1            = 0x00000000,    // When there is data in every frame
    NNS_G3D_JNTANM_TINFO_STEP_2            = 0x40000000,    // When there is data in every 2nd frame
    NNS_G3D_JNTANM_TINFO_STEP_4            = 0x80000000,    // When there is data in every 4th frame
    NNS_G3D_JNTANM_TINFO_FX16ARRAY         = 0x20000000,    // ON when the animation  data is an fx16 array

    NNS_G3D_JNTANM_TINFO_LAST_INTERP_MASK  = 0x1fff0000,
    NNS_G3D_JNTANM_TINFO_STEP_MASK         = 0xc0000000,

    NNS_G3D_JNTANM_TINFO_LAST_INTERP_SHIFT = 16,
    NNS_G3D_JNTANM_TINFO_STEP_SHIFT        = 30
}
NNSG3dJntAnmTInfo;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmRInfo

    Information about the rotation data
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_RINFO_STEP_1            = 0x00000000,    // When there is data in every frame
    NNS_G3D_JNTANM_RINFO_STEP_2            = 0x40000000,    // When there is data in every 2nd frame
    NNS_G3D_JNTANM_RINFO_STEP_4            = 0x80000000,    // When there is data in every 4th frame

    NNS_G3D_JNTANM_RINFO_LAST_INTERP_MASK  = 0x1fff0000,
    NNS_G3D_JNTANM_RINFO_STEP_MASK         = 0xc0000000,

    NNS_G3D_JNTANM_RINFO_LAST_INTERP_SHIFT = 16,
    NNS_G3D_JNTANM_RINFO_STEP_SHIFT        = 30
}
NNSG3dJntAnmRInfo;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmPivotInfo

    The information about the compressed rotation matrix.
    If the rows and columns of the +1/-1 elements are removed, it becomes a 2x2 matrix and is named such as
    A B
    C D
     Since C=+B/-B and D=+A/-A, signed data is placed in:
    NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVC
    NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVD
      
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_PIVOTINFO_IDXPIVOT_MASK = 0x000f,    // The location of the element that is +1/-1
    NNS_G3D_JNTANM_PIVOTINFO_MINUS         = 0x0010,    // If ON -1/ If OFF +1
    NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVC     = 0x0020,    // ON if C=-B
    NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVD     = 0x0040,    // ON if D=-A

    NNS_G3D_JNTANM_PIVOT_INFO_IDXPIVOT_SHIFT = 0
}
NNSG3dJntAnmPivotInfo;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmRIdx

    The index to the rotation matrix data.
    When NNS_G3D_JNTANM_RIDX_PIVOT is ON, the index to the pivot compression matrix.
    When OFF, the index to the normal rotation matrix data.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_RIDX_PIVOT         = 0x8000,
    NNS_G3D_JNTANM_RIDX_IDXDATA_MASK  = 0x7fff,
    NNS_G3D_JNTANM_RIDX_IDXDATA_SHIFT = 0
}
NNSG3dJntAnmRIdx;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmSInfo


 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_SINFO_STEP_1            = 0x00000000,    // When there is data in every frame
    NNS_G3D_JNTANM_SINFO_STEP_2            = 0x40000000,    // When there is data in every 2nd frame
    NNS_G3D_JNTANM_SINFO_STEP_4            = 0x80000000,    // When there is data in every 4th frame
    NNS_G3D_JNTANM_SINFO_FX16ARRAY         = 0x20000000,    // When the data value is an fx16 type

    NNS_G3D_JNTANM_SINFO_LAST_INTERP_MASK  = 0x1fff0000,
    NNS_G3D_JNTANM_SINFO_STEP_MASK         = 0xc0000000,

    NNS_G3D_JNTANM_SINFO_LAST_INTERP_SHIFT = 16,
    NNS_G3D_JNTANM_SINFO_STEP_SHIFT        = 30
}
NNSG3dJntAnmSInfo;


/*---------------------------------------------------------------------------*
    NNSG3dResJntAnmSRTInfo


 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResJntAnmSRTInfo_
{
    u32 info;    // NNSG3dJntAnmTInfo or NNSG3dJntAnmRInfo or NNSG3dJntAnmSInfo
    u32 offset;
}
NNSG3dResJntAnmSRTInfo;


/*---------------------------------------------------------------------------*
    NNSG3dResJntAnmSRTTag


 *---------------------------------------------------------------------------*/
typedef struct NNSG3dResJntAnmSRTTag_
{
    u32 tag;    // NNSG3dJntAnmSRTTag;
}
NNSG3dResJntAnmSRTTag;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmOption

    The options for the joint animation (designate with the intermediate file)
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_OPTION_INTERPOLATION              = 0x01, // <node_anm_info>::interpolation
    NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION = 0x02  // <node_anm_info>::interp_end_to_start
}
NNSG3dJntAnmOption;


typedef struct NNSG3dResJntAnm_
{
    NNSG3dResAnmHeader anmHeader;
    u16                numFrame;
    u16                numNode;
    u32                flag;
    u32                ofsRot3;
    u32                ofsRot5;
    // u16[numNode] ofsTag
    // vector of NNSG3dResJntAnmSRTTag
}
NNSG3dResJntAnm;


/*---------------------------------------------------------------------------*
    NNSG3dResDictJntAnmSetData

    The offset to the data for the individual joint animations (NNSG3dResJntAnm) is stored here.
    It uses &NNSG3dResJntAnmSet as the origin.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResDictAnmSetData NNSG3dResDictJntAnmSetData;


/*---------------------------------------------------------------------------*
    NNSG3dResJntAnmSet

    The structure that gathers the sets of the joint animations
    NNSG3dResJntAnm is listed in the back.
    The dictionary references the NNSG3dResJntAnm via the NNSG3dResDictJntAnmSetData.
    The kind must be equal to NNS_G3D_SIGNATURE_NSBCA.
 *---------------------------------------------------------------------------*/
typedef NNSG3dResAnmSet NNSG3dResJntAnmSet;


#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // NNSG3D_BINRES_RES_STRUCT_H_

