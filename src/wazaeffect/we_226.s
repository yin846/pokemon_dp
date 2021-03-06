//==============================================================================
/**
 * @file	we_226.s
 * @brief	バトンタッチ			226
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_BATONTATTI
	
// =============================================================================
//
//
//	■バトンタッチ			226
//
//
// =============================================================================
#define BATONTATTI_RES_NO			(0)
#define BATONTATTI_RES_NUM		(1)
#define BATONTATTI_ACT_NUM		(1)
WEST_BATONTATTI:

	LOAD_PARTICLE_DROP	0,W_226_SPA
	
	///<初期化処理　読み込み処理
	CATS_RES_INIT		BATONTATTI_RES_NO, BATONTATTI_ACT_NUM, BATONTATTI_RES_NUM, BATONTATTI_RES_NUM, BATONTATTI_RES_NUM, BATONTATTI_RES_NUM, 0, 0
	CATS_CAHR_RES_LOAD	BATONTATTI_RES_NO, EFFECT_226_NCGR_BIN
	CATS_PLTT_RES_LOAD	BATONTATTI_RES_NO, EFFECT_226_NCLR, 1
	CATS_CELL_RES_LOAD	BATONTATTI_RES_NO, EFFECT_226_NCER_BIN
	CATS_CELL_ANM_RES_LOAD	BATONTATTI_RES_NO, EFFECT_226_NANR_BIN
	
	///<動作
	CATS_ACT_ADD		BATONTATTI_RES_NO, WEST_CSP_WE_226, EFFECT_226_NCGR_BIN, EFFECT_226_NCLR, EFFECT_226_NCER_BIN, EFFECT_226_NANR_BIN, 0, 0, 0,0,
	
	SE_L				SEQ_SE_DP_SUIKOMU

	WAIT 32
	
	SE_L				SEQ_SE_DP_BOWA2
	SE_WAITPLAY_L		SEQ_SE_DP_W100,40

	ADD_PARTICLE 	0,W_226_BALL_CLOSE1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_NONE, OPERATOR_POS_226, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_226_BALL_CLOSE2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_NONE, OPERATOR_POS_226, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	WAIT_FLAG

	///<破棄処理
	CATS_RES_FREE		BATONTATTI_RES_NO
	
	SEQEND
