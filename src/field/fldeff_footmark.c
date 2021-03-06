//******************************************************************************
/**
 * 
 * @file	fldeff_footmark.c
 * @brief	tB[hOBJ«Õ
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_footmark.h"

//==============================================================================
//	define
//==============================================================================
#define FOOTMARK_VANISH_START_FRAME (16)						///<_ÅJnt[(ÔÎ40)
//#define FOOTMARK_VANISH_START_FRAME (20)						///<_ÅJnt[(ÔÎ40)
#define FOOTMARK_VANISH_END_FRAME (28)							///<_ÅI¹t[(ÔÎ56)

//--------------------------------------------------------------
///	«Õ¯Ê
//--------------------------------------------------------------
enum
{
	FOOTMARK_WALK_UP = 0,										///<«Õ@ã
	FOOTMARK_WALK_DOWN,											///<«Õ@º
	FOOTMARK_WALK_LEFT,											///<«Õ@¶
	FOOTMARK_WALK_RIGHT,										///<«Õ@E
	FOOTMARK_CYCLE_UD,											///<©]Ô«Õ ãº
	FOOTMARK_CYCLE_LR,											///<©]Ô«Õ ¶E
	FOOTMARK_CYCLE_UL,											///<©]Ô«Õ ã¶
	FOOTMARK_CYCLE_UR,											///<©]Ô«Õ ãE
	FOOTMARK_CYCLE_DL,											///<©]Ô«Õ º¶
	FOOTMARK_CYCLE_DR,											///<©]Ô«Õ ºE
	FOOTMARK_MAX,												///<«ÕÅå
};

//--------------------------------------------------------------
///	«Õ^Cv
//--------------------------------------------------------------
typedef enum
{
	FOOTMARKTYPE_NORMAL = 0,		///<«Õ^Cv@Q«
	FOOTMARKTYPE_CYCLE,				///<«Õ^Cv@©]Ô
	FOOTMARKTYPE_SNOW,				///<«Õ^Cv@á
	FOOTMARKTYPE_SNOWDEEP,			///<«Õ^Cv@[¢á
}FOOTMARKTYPE;

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_FOOTMARK^
//--------------------------------------------------------------
typedef struct _TAG_FE_FOOTMARK * FE_FOOTMARK_PTR;

//--------------------------------------------------------------
///	FE_FOOTMARK\¢Ì
//--------------------------------------------------------------
typedef struct _TAG_FE_FOOTMARK
{
	FE_SYS *fes;
	NNSG3dRenderObj render[FOOTMARK_MAX];
	NNSG3dRenderObj render_snow[FOOTMARK_MAX];
	NNSG3dResMdl *model[FOOTMARK_MAX];
	NNSG3dResMdl *model_snow[FOOTMARK_MAX];
	NNSG3dResFileHeader *res_file[FOOTMARK_MAX];
	NNSG3dResFileHeader *res_file_snow[FOOTMARK_MAX];
}FE_FOOTMARK;

#define FE_FOOTMARK_SIZE (sizeof(FE_FOOTMARK)) ///<FE_FOOTMARKTCY

//--------------------------------------------------------------
///	FOOTMARK_ADD_H\¢Ì
//--------------------------------------------------------------
typedef struct
{
	FOOTMARKTYPE type;
	NNSG3dRenderObj *render;
	NNSG3dResMdl *model;
}FOOTMARK_ADD_H;

//--------------------------------------------------------------
///	FOOTMARK_WORK\¢Ì
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int frame;
	int vanish_sw;
	int alpha;
	FOOTMARKTYPE type;
	NNSG3dRenderObj *render;
	NNSG3dResMdl *model;
}FOOTMARK_WORK;

#define FOOTMARK_WORK_SIZE (sizeof(FOOTMARK_WORK))

//==============================================================================
//	vg^Cv
//==============================================================================
static void FootMark_GraphicInit( FE_FOOTMARK_PTR fm );
static void FootMark_GraphicDelete( FE_FOOTMARK_PTR fm );

static NNSG3dRenderObj * FootMark_TypeRenderGet(
		FE_FOOTMARK_PTR fm, FOOTMARKTYPE type, int now_dir, int old_dir );
static NNSG3dResMdl * FootMark_TypeMdlGet(
		FE_FOOTMARK_PTR fm, FOOTMARKTYPE type, int now_dir, int old_dir );

static EOA_PTR FldOBJFootMark_AddMain( CONST_FIELD_OBJ_PTR fldobj, FOOTMARKTYPE type );

static const u32 DATA_3DModelArcID_FootMark[FOOTMARK_MAX];
static const u32 DATA_3DModelArcID_FootMarkSnow[FOOTMARK_MAX];
static const int DATA_FootMarkCycleDirTbl[DIR_4_MAX][DIR_4_MAX];
static const EOA_H_NPP DATA_EoaH_FootMark;

//==============================================================================
//	«Õ@VXe
//==============================================================================
//--------------------------------------------------------------
/**
 * «Õú»
 * @param	fes		FE_SYS_PTR
 * @retval	FE_FOOTMARK_PTR		FE_FOOTMARK_PTR
 */
//--------------------------------------------------------------
void * FE_FootMark_Init( FE_SYS *fes )
{
	FE_FOOTMARK_PTR fm;
	
	fm = FE_AllocClearMemory( fes, FE_FOOTMARK_SIZE, ALLOC_FR, 0 );
	fm->fes = fes;
	
	FootMark_GraphicInit( fm );
	return( fm );
}

//--------------------------------------------------------------
/**
 * «Õí
 * @param	fm		FE_FOOTMARK_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FootMark_Delete( void *work )
{
	FE_FOOTMARK_PTR fm = work;
	FootMark_GraphicDelete( fm );
	FE_FreeMemory( fm );
}

//==============================================================================
//	«Õ	p[c
//==============================================================================

//==============================================================================
//	«Õ@OtBbN
//==============================================================================
//--------------------------------------------------------------
/**
 * «Õ OtBbNú»
 * @param	fm		FE_FOOTMARK_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FootMark_GraphicInit( FE_FOOTMARK_PTR fm )
{
	int i;
	u32 id;
	
	for( i = 0; i < FOOTMARK_MAX; i++ ){
		id = DATA_3DModelArcID_FootMark[i];
		FE_simple3DModelInit( fm->fes, id, ALLOC_FR,
			&fm->render[i], &fm->model[i], &fm->res_file[i] );
		
		id = DATA_3DModelArcID_FootMarkSnow[i];
		FE_simple3DModelInit( fm->fes, id, ALLOC_FR,
			&fm->render_snow[i], &fm->model_snow[i], &fm->res_file_snow[i] );
	}
}

//--------------------------------------------------------------
/**
 * «Õ@OtBbNí
 * @param	fm		FE_FOOTMARK_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FootMark_GraphicDelete( FE_FOOTMARK_PTR fm )
{
	int i;
	
	for( i = 0; i < FOOTMARK_MAX; i++ ){
		FE_FreeMemory( fm->res_file[i] );
		FE_FreeMemory( fm->res_file_snow[i] );
	}
}

//--------------------------------------------------------------
/**
 * wè^CvÌ_æ¾
 * @param	fm		FE_FOOTMARK_PTR
 * @param	type	FOOTMARK_TYPE_NORMAL
 * @param	now_dir	»ÝÌü« DIR_UP
 * @param	old_dir	ßÌü« DIR_UP
 * @param	attr	FOOTMARK_ATTR
 * @retval	NNSG3dRenderObj wèÌ_
 */
//--------------------------------------------------------------
static NNSG3dRenderObj * FootMark_TypeRenderGet(
		FE_FOOTMARK_PTR fm, FOOTMARKTYPE type, int now_dir, int old_dir )
{
	int no;
	NNSG3dRenderObj * render = NULL;
	
	switch( type ){
	case FOOTMARKTYPE_NORMAL:
		no = FOOTMARK_WALK_UP + now_dir;
		render = &fm->render[no];
		break;
	case FOOTMARKTYPE_CYCLE:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		render = &fm->render[no];
		break;
	case FOOTMARKTYPE_SNOW:
		no = FOOTMARK_WALK_UP + now_dir;
		render = &fm->render_snow[no];
		break;
	case FOOTMARKTYPE_SNOWDEEP:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		render = &fm->render_snow[no];
		break;
	}
	
	GF_ASSERT( render != NULL && "FootMark_TypeRenderGet()s³\n" );
	return( render );
}

//--------------------------------------------------------------
/**
 * wè^CvÌfæ¾
 * @param	fm		FE_FOOTMARK_PTR
 * @param	type	FOOTMARK_TYPE_NORMAL
 * @param	now_dir	»ÝÌü« DIR_UP
 * @param	old_dir	ßÌü« DIR_UP
 * @param	attr	FOOTMARK_ATTR
 * @retval	NNSG3dMdl wèÌ_
 */
//--------------------------------------------------------------
static NNSG3dResMdl * FootMark_TypeMdlGet(
		FE_FOOTMARK_PTR fm, FOOTMARKTYPE type, int now_dir, int old_dir )
{
	int no = 0;
	NNSG3dResMdl *model = NULL;
	
	switch( type ){
	case FOOTMARKTYPE_NORMAL:
		no = FOOTMARK_WALK_UP + now_dir;
		model = fm->model[no];
		break;
	case FOOTMARKTYPE_CYCLE:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		model = fm->model[no];
		break;
	case FOOTMARKTYPE_SNOW:
		no = FOOTMARK_WALK_UP + now_dir;
		model = fm->model_snow[no];
		break;
	case FOOTMARKTYPE_SNOWDEEP:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		model = fm->model_snow[no];
		break;
	}
	
	GF_ASSERT( model != NULL && "FootMark_TypeMdlGet()s³\n" );
	return( model );
}

//==============================================================================
//	«Õ@EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * tB[hOBJ Q¬«ÕÇÁ
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkNormal_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_NORMAL) );
}

//--------------------------------------------------------------
/**
 * tB[hOBJ ©]Ô«ÕÇÁ
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkCycle_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_CYCLE) );
}

//--------------------------------------------------------------
/**
 * tB[hOBJ á«ÕÇÁ
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkSnow_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_SNOW) );
}

//--------------------------------------------------------------
/**
 * tB[hOBJ [¢á«ÕÇÁ
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkSnowDeep_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_SNOWDEEP) );
}

//--------------------------------------------------------------
/**
 * tB[hOBJ«ÕÇÁ@C
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	attr	FOOTMARK_ATTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
static EOA_PTR FldOBJFootMark_AddMain( CONST_FIELD_OBJ_PTR fldobj, FOOTMARKTYPE type )
{
	VecFx32 mtx;
	EOA_PTR eoa;
	FOOTMARK_ADD_H head;
	int gx = FieldOBJ_OldPosGX_Get( fldobj );
	int gz = FieldOBJ_OldPosGZ_Get( fldobj );
	fx32 y = FieldOBJ_VecPosYGet( fldobj );
	int now_dir = FieldOBJ_DirDispGet( fldobj );
	int old_dir = FieldOBJ_DirDispOldGet( fldobj );
	int pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
	FE_SYS *fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	FE_FOOTMARK_PTR fm = FE_EffectWorkGet( fes, FE_FLD_FOOTMARK );
	
	head.render = FootMark_TypeRenderGet( fm, type, now_dir, old_dir );
	head.model = FootMark_TypeMdlGet( fm, type, now_dir, old_dir );
	head.type = type;
	
	FieldOBJTool_GridCenterPosGet( gx, gz, &mtx );
	mtx.y = y;
	
	switch( type ){
	case FOOTMARKTYPE_NORMAL:
	case FOOTMARKTYPE_CYCLE:
		mtx.y += NUM_FX32( -8 );
			
		if( type == FOOTMARK_WALK_LEFT || type == FOOTMARK_WALK_RIGHT ){
			mtx.z += NUM_FX32( -2 );
		}else{
			mtx.z += NUM_FX32( 1 );
		}
		
		break;
	case FOOTMARKTYPE_SNOW:
	case FOOTMARKTYPE_SNOWDEEP:
		mtx.y += NUM_FX32( -10 );
			
		if( type == FOOTMARK_WALK_LEFT || type == FOOTMARK_WALK_RIGHT ){
			mtx.z += NUM_FX32( -2 );
		}else{
			mtx.z += NUM_FX32( 2 );
		}
	}
	
	eoa = FE_EoaAddNpp( fes, &DATA_EoaH_FootMark, &mtx, 0, &head, pri );
	return( eoa );
}

//--------------------------------------------------------------
/**
 * EOA «Õ@ú»
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	int TRUE=³íI¹BFALSE=ÙíI¹
 */
//--------------------------------------------------------------
static int EoaFootMark_Init( EOA_PTR eoa, void *wk )
{
	VecFx32 mtx;
	FOOTMARK_WORK *work;
	const FOOTMARK_ADD_H *head;
	
	work = wk;
	work->type = EOA_AddParamGet( eoa );
	
	head = EOA_AddPtrGet( eoa );
	work->render = head->render;
	work->model = head->model;
	work->alpha = 31;
	work->type = head->type;
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA «Õ@í
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFootMark_Delete( EOA_PTR eoa, void *wk )
{
}

//--------------------------------------------------------------
/**
 * EOA «Õ@®ì
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFootMark_Move( EOA_PTR eoa, void *wk )
{
	FOOTMARK_WORK *work;
	
	work = wk;
	
	switch( work->seq_no ){
	case 0:
		work->frame++;
		
		if( work->frame >= FOOTMARK_VANISH_START_FRAME ){
			work->seq_no++;
		}
		
		break;
	case 1:
		work->alpha -= 2;
		
		if( work->alpha < 0 ){
			FE_EoaDelete( eoa );
			return;
		}
	}
}

//--------------------------------------------------------------
/**
 * EOA «Õ@`æ
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFootMark_Draw( EOA_PTR eoa, void *wk )
{
	FOOTMARK_WORK *work;
	
	work = wk;
	
	if( work->vanish_sw == TRUE ){
		return;
	}
	
	{
		VecFx32 vec;
		VecFx32 scale = { FX32_ONE, FX32_ONE, FX32_ONE };
		MtxFx33 rot	  = { FX32_ONE, 0,0,0, FX32_ONE, 0,0,0,FX32_ONE};
		
		EOA_MatrixGet( eoa, &vec );
		
		NNS_G3dMdlUseMdlAlpha( work->model );
		NNS_G3dMdlSetMdlAlphaAll( work->model, work->alpha );
		simple_3DModelDraw( work->render, &vec, &rot, &scale );
	}
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	«Õimde[u@ÀÑÍFOOTMARK_WALK_UPÌlÉêv
//--------------------------------------------------------------
static const u32 DATA_3DModelArcID_FootMark[FOOTMARK_MAX] =
{
	NARC_fldeff_f_mark_u_nsbmd,
	NARC_fldeff_f_mark_d_nsbmd,
	NARC_fldeff_f_mark_l_nsbmd,
	NARC_fldeff_f_mark_r_nsbmd,
	NARC_fldeff_c_mark_u_nsbmd,
	NARC_fldeff_c_mark_l_nsbmd,
	NARC_fldeff_c_mark_ul_nsbmd,
	NARC_fldeff_c_mark_ur_nsbmd,
	NARC_fldeff_c_mark_dl_nsbmd,
	NARC_fldeff_c_mark_dr_nsbmd,
};

//--------------------------------------------------------------
///	«Õimde[u@ÀÑÍFOOTMARK_WALK_UPÌlÉêv@á
//--------------------------------------------------------------
static const u32 DATA_3DModelArcID_FootMarkSnow[FOOTMARK_MAX] =
{
	NARC_fldeff_nf_mark_u_nsbmd,
	NARC_fldeff_nf_mark_d_nsbmd,
	NARC_fldeff_nf_mark_l_nsbmd,
	NARC_fldeff_nf_mark_r_nsbmd,
	NARC_fldeff_nc_mark_u_nsbmd,
	NARC_fldeff_nc_mark_l_nsbmd,
	NARC_fldeff_nc_mark_ul_nsbmd,
	NARC_fldeff_nc_mark_ur_nsbmd,
	NARC_fldeff_nc_mark_dl_nsbmd,
	NARC_fldeff_nc_mark_dr_nsbmd,
};

//--------------------------------------------------------------
///	©]Ô«Õe[u [ßûü][»Ýûü]
//--------------------------------------------------------------
static const int DATA_FootMarkCycleDirTbl[DIR_4_MAX][DIR_4_MAX] =
{
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_UL},
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_DL},
	{FOOTMARK_CYCLE_DL,FOOTMARK_CYCLE_UL,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
	{FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
};

//--------------------------------------------------------------
///	«ÕEOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_FootMark =
{
	FOOTMARK_WORK_SIZE,
	EoaFootMark_Init,
	EoaFootMark_Delete,
	EoaFootMark_Move,
	EoaFootMark_Draw,
};

