//==============================================================================
/**
 * @file	we_056.s
 * @brief	ハイドロポンプ			56
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_HAIDOROPONPU
	
// =============================================================================
//
//
//	■ハイドロポンプ			56
//
//
// =============================================================================
#define WE_056_SHAKE_NUM	(SHAKE_M_NUM * 7)
WEST_HAIDOROPONPU:

	LOAD_PARTICLE_DROP	0,W_056_SPA

	WORK_CLEAR
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 	1
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  	WEDEF_HAIKEI_SPEED_X
	HAIKEI_CHG		BG_ID_056, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	HAIKEI_CHG_WAIT

	ADD_PARTICLE 	0,W_056_SPIN_RING01, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_RSP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	FUNC_CALL		WEST_SP_BG_SHAKE, 5, 0, 3, 0, 20, 0
	WAIT			2

	ADD_PARTICLE 	0,W_056_WATER_BREATH01, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_L2SP, OPERATOR_AXIS_AT_OLD, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_056_056_RING_BEAM,	EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_L2SP, OPERATOR_AXIS_AT_OLD, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	SE_FLOW_LR		SEQ_SE_DP_SUIRYU
	
	WAIT			8
	
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, WE_056_SHAKE_NUM,  WE_TOOL_M1 | WE_TOOL_SSP,
	ADD_PARTICLE 	0,W_056_KEMURI_BLU,		EMTFUNC_DEFENCE_POS	
	ADD_PARTICLE 	0,W_056_056_BALL_BURN,	EMTFUNC_DEFENCE_POS	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_BLUE, 10, 25
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, WE_056_SHAKE_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,


	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WORK_CLEAR
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 	1
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  	WEDEF_HAIKEI_SPEED_X
	HAIKEI_RECOVER	BG_ID_056, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT
	
	SEQEND
