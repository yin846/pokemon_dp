//==============================================================================
/**
 * @file	we_341.s
 * @brief	マッドショット			341
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_MADDOSYOTTO
	
// =============================================================================
//
//
//	■マッドショット			341
//
//
// =============================================================================
WEST_MADDOSYOTTO:

	LOAD_PARTICLE_DROP	0,W_341_SPA
	
	PTAT_JP			PTAT
	CONTEST_JP		CONTEST
	
	SE_FLOW_LR		SEQ_SE_DP_W250

	ADD_PARTICLE_SEP	0, W_341_341_MUD_BEAM1, W_341_341_MUD_BEAM2, W_341_341_MUD_BEAM3, W_341_341_MUD_BEAM4, W_341_341_MUD_BEAM5, W_341_341_MUD_BEAM6, EMTFUNC_SEP_POS
	
	WAIT				10
	
	ADD_PARTICLE		0, W_341_341_MUD_HIT, EMTFUNC_DEFENCE_POS
	FUNC_CALL			WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM*3,  WE_TOOL_E1 | WE_TOOL_SSP,

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	

	SEQEND

PTAT:

	SE_FLOW_LR			SEQ_SE_DP_W250

	ADD_PARTICLE_PTAT	0, W_341_341_MUD_BEAM_PARTY1, W_341_341_MUD_BEAM_PARTY2, W_341_341_MUD_BEAM_PARTY1, W_341_341_MUD_BEAM_PARTY2, EMTFUNC_ATTACK_POS
	
	WAIT				10
	
	ADD_PARTICLE		0, W_341_341_MUD_HIT, EMTFUNC_DEFENCE_POS
	FUNC_CALL			WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM*3,  WE_TOOL_E1 | WE_TOOL_SSP,

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

///< コンテスト
CONTEST:

	SE_FLOW_LR			SEQ_SE_DP_W250

	ADD_PARTICLE		0, W_341_341_MUD_BEAM_CNT, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_ON, +PT_LCD_PTP_CHG(64), -PT_LCD_PTP_CHG(48), 0

	WAIT				10
	
	ADD_PARTICLE		0, W_341_341_MUD_HIT, EMTFUNC_DEFENCE_POS
	FUNC_CALL			WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM*3,  WE_TOOL_E1 | WE_TOOL_SSP,
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

