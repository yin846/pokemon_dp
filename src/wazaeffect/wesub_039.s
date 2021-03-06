//==============================================================================
/**
 * @file	we_328.s
 * @brief	すなじごく			328
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_SUNAZIGOKU
	
// =============================================================================
//
//
//	■すなじごく			328
//
//
// =============================================================================
WEST_SUNAZIGOKU:
	
	LOAD_PARTICLE_DROP	0,W_328_SPA
	
	ADD_PARTICLE 	0,W_328_328_SAND_TORNADO1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_328_328_SAND_TORNADO2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	SE_REPEAT_L		SEQ_SE_DP_W028,2,18

	WAIT			10
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5,  WE_TOOL_M1, 0, 2, WE_PAL_YELLOW_2, 12
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 3, 0, 1, 8,  WE_TOOL_M1 | WE_TOOL_SSP,
	WAIT_FLAG


	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
