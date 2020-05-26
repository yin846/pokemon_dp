//==============================================================================
/**
 * @file	we_145.s
 * @brief	あわ			145
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_AWA
	
// =============================================================================
//
//
//	■あわ			145
//
//
// =============================================================================
WEST_AWA:
	
	LOAD_PARTICLE_DROP	0,W_145_SPA
	LOAD_PARTICLE_DROP	1,W_145_SPA
	
	CONTEST_JP		CONTEST
	
	ADD_PARTICLE 	0,W_145_145_AWA_BUBBLE1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_145, OPERATOR_AXIS_145, OPERATOR_FLD_NONE, OPERATOR_CAMERA_145

	SE_REPEAT_C		SEQ_SE_DP_W145,2,5
	WAIT			10
	
	ADD_PARTICLE 	1,W_145_145_AWA_BUBBLE2, EMTFUNC_DF_SIDE

	WAIT			10
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E2 | WE_TOOL_SSP,

	SE_REPEAT_R		SEQ_SE_DP_W145C,2,2
	WAIT			10

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	EXIT_PARTICLE	1,
	
	SEQEND

CONTEST:
	
	ADD_PARTICLE 	0,W_145_145_AWA_BUBBLE1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_NONE, OPERATOR_POS_145_CON, OPERATOR_AXIS_145_CON, OPERATOR_FLD_NONE, OPERATOR_CAMERA_145

	SE_REPEAT_C		SEQ_SE_DP_W145,2,5
	WAIT			10
	
	ADD_PARTICLE 	1,W_145_145_AWA_BUBBLE2, EMTFUNC_DF_SIDE

	WAIT			10
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E2 | WE_TOOL_SSP,

	SE_REPEAT_R		SEQ_SE_DP_W145C,2,2
	WAIT			10

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	EXIT_PARTICLE	1,
	
	SEQEND
