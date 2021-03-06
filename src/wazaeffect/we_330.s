//==============================================================================
/**
 * @file	we_057.s
 * @brief	なみのり			57
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_NAMINORI
	
// =============================================================================
//
//
//	■なみのり			57
//
//
// =============================================================================
#define BG_POS_X	(0)
#define BG_POS_Y	(256)
#define BG_SPD_X	(-6)
#define BG_SPD_Y	(6)
#define BG_REV		(1)
#define BG_BLD		(16)
#define WE_056_SHAKE_NUM	(SHAKE_M_NUM * 3)

WEST_DAKURYUU:
	
	LOAD_PARTICLE_DROP		0, W_330_SPA

	CATS_RES_INIT			0, 2, 1,1,1,1,0,0
	CATS_CAHR_RES_LOAD		0, EFFECT_057_NCGR_BIN
	CATS_PLTT_RES_LOAD		0, EFFECT_330_NCLR, 1
	CATS_CELL_RES_LOAD		0, EFFECT_057_NCER_BIN
	CATS_CELL_ANM_RES_LOAD	0, EFFECT_057_NANR_BIN
	CATS_ACT_ADD_EZ			0, 0, EFFECT_057_NCGR_BIN, EFFECT_330_NCLR, EFFECT_057_NCER_BIN, EFFECT_057_NANR_BIN, 0, 0
	CATS_ACT_ADD_EZ			0, 1, EFFECT_057_NCGR_BIN, EFFECT_330_NCLR, EFFECT_057_NCER_BIN, EFFECT_057_NANR_BIN, 0, 0
	
	SE_L					SEQ_SE_DP_W057

	FUNC_CALL				WEST_SP_WE_057, 1, 1
	
	WAIT_FLAG
	CATS_RES_FREE			0

	FUNC_CALL	WEST_SP_WE_T22, 8, BG_ID_330, BG_POS_X, BG_POS_Y, BG_SPD_X, BG_SPD_Y, BG_REV, BG_BLD, 45
	
	SE_C					SEQ_SE_DP_W057B
	
	WAIT	15
	
	ADD_PARTICLE	0, W_330_330_BALL_BURN01,	EMTFUNC_DF_SIDE
	ADD_PARTICLE	0, W_330_330_BALL_BURN02,	EMTFUNC_DF_SIDE
	ADD_PARTICLE	0, W_330_330_HITMARK,		EMTFUNC_DF_SIDE

	PTAT_JP			PTAT
	
//	SIDE_JP			0, SIDE_M, SIDE_E

	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_RED_BROWN, 10, 10
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E2, 0, 1, WE_PAL_RED_BROWN, 10, 10
//	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M2, 0, 1, WE_PAL_RED_BROWN, 10, 10	2対選択だから
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, WE_056_SHAKE_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, WE_056_SHAKE_NUM,  WE_TOOL_E2 | WE_TOOL_SSP,

	WAIT_FLAG

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
		
	SEQEND


///< PT
PTAT:

	SIDE_JP			0, SIDE_PM, SIDE_PE
	
	SEQEND

SIDE_PM:
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_RED_BROWN, 10, 10
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_B, 0, 1, WE_PAL_RED_BROWN, 10, 10
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_D, 0, 1, WE_PAL_RED_BROWN, 10, 10
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, WE_056_SHAKE_NUM,  WE_TOOL_OTHER | WE_TOOL_SSP

	WAIT_FLAG
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
	
SIDE_PE:

	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_RED_BROWN, 10, 10
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_A, 0, 1, WE_PAL_RED_BROWN, 10, 10
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_C, 0, 1, WE_PAL_RED_BROWN, 10, 10
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, WE_056_SHAKE_NUM,  WE_TOOL_OTHER | WE_TOOL_SSP

	WAIT_FLAG
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

	
SIDE_M:
	
	SEQEND

SIDE_E:
	
	SEQEND
