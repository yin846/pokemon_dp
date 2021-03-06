//==============================================================================
/**
 * @file	we_035.s
 * @brief	まきつく			35
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_MAKITUKU
	
// =============================================================================
//
//
//	■まきつく			35
//
//
// =============================================================================
WEST_MAKITUKU:
	

	LOAD_PARTICLE_DROP	0,W_035_SPA
	
	LOOP_LABEL		2
		SE_REPEAT_R		SEQ_SE_DP_W020B,4,2

		ADD_PARTICLE 	0,W_035_035_BALL_ROLL1,	EMTFUNC_DEFENCE_POS
		FUNC_CALL		WEST_SP_WE_KAITEN, 3, WE_TOOL_M1, 1, 12
		FUNC_CALL		WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_E1 | WE_TOOL_SSP), 100, 70, 100, 100, 100, 1, (5 << 16) | 5
		WAIT_FLAG

	LOOP

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
