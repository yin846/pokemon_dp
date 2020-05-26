//==============================================================================
/**
 * @file	we_284.s
 * @brief	ふんか			284
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_HUNKA
	
// =============================================================================
//
//
//	■ふんか			284
//
//
// =============================================================================
WEST_HUNKA:

	LOAD_PARTICLE_DROP	0,W_284_SPA

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_RED_BROWN,
	WAIT_FLAG


	ADD_PARTICLE 	0,W_284_284_STONE_UP, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_284_284_FIRE_BURN, EMTFUNC_ATTACK_POS

	FUNC_CALL	WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_M1 | WE_TOOL_SSP), 100, 120, 100, 80, 100, 1, (4 << 16) | 4

	SE_L		SEQ_SE_DP_W153
	WAIT_FLAG

	FUNC_CALL	WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_M1 | WE_TOOL_SSP), 100, 80, 100, 160, 100, 1, (4 << 16) | 4

	SE_WAITPLAY_L	SEQ_SE_DP_W153,10
	WAIT_FLAG
	

	LOOP_LABEL		1
		ADD_PARTICLE 	0,W_284_284_STONE_UP, EMTFUNC_ATTACK_POS
		ADD_PARTICLE 	0,W_284_284_FIRE_BURN, EMTFUNC_ATTACK_POS
		FUNC_CALL	WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_M1 | WE_TOOL_SSP), 100, 120, 100, 80, 100, 1, (4 << 16) | 4
		WAIT_FLAG
		FUNC_CALL	WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_M1 | WE_TOOL_SSP), 100, 80, 100, 160, 100, 1, (4 << 16) | 4
		WAIT_FLAG
	
	LOOP

	WAIT			15
	SE_R			SEQ_SE_DP_W088
	
	
	FUNC_CALL		WEST_SP_BG_SHAKE, 5, 0, 5, 0, 4, 3

	ADD_PARTICLE 	0,W_284_284_BIGSTONE_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -6000, 1000, +4000

	WAIT			5
	SE_R			SEQ_SE_DP_W088
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 3, 0, 1, 15,  WE_TOOL_E1 | WE_TOOL_SSP
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 3, 0, 1, 15,  WE_TOOL_E2 | WE_TOOL_SSP

	ADD_PARTICLE 	0,W_284_284_BIGSTONE_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, +6000, -2000, +4000

	WAIT			2
	SE_R			SEQ_SE_DP_W088

	ADD_PARTICLE 	0,W_284_284_BIGSTONE_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, +12000, 0, -4000

	WAIT			3
	SE_R			SEQ_SE_DP_W088

	ADD_PARTICLE 	0,W_284_284_BIGSTONE_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -10000, -1000, -4000
	
	WAIT			2
	SE_R			SEQ_SE_DP_W088
	
	ADD_PARTICLE 	0,W_284_284_BIGSTONE_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -2000, -2000, +4000

	WAIT			4
	SE_R			SEQ_SE_DP_W088
	
	ADD_PARTICLE 	0,W_284_284_BIGSTONE_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, +4000, -3000, +4000

	WAIT			2
	SE_R			SEQ_SE_DP_W088

	ADD_PARTICLE 	0,W_284_284_BIGSTONE_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -5000, -4000, -4000

	WAIT_PARTICLE
	EXIT_PARTICLE	0,

FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_RED_BROWN,
WAIT_FLAG	
	SEQEND
