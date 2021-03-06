//==============================================================================
/**
 * @file	we_231.s
 * @brief	アイアンテール			231
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_AIANTEERU
	
// =============================================================================
//
//
//	■アイアンテール			231
//
//
// =============================================================================
WEST_AIANTEERU:
	LOAD_PARTICLE_DROP	0,W_231_SPA
	
	SE_REPEAT_L			SEQ_SE_DP_W231,8,2
	
	ADD_PARTICLE 	0,W_231_231_IRON_KIRA,		EMTFUNC_ATTACK_POS
	
	POKEOAM_RES_INIT
	POKEOAM_RES_LOAD	WEDEF_POKE_RES_0,
	POKEOAM_RES_LOAD	WEDEF_POKE_RES_1,

	POKEOAM_DROP		WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_0, WEDEF_POKE_RES_0,
	POKEOAM_DROP		WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_1, WEDEF_POKE_RES_0,
	POKEOAM_DROP		WEDEF_DROP_M2, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_2, WEDEF_POKE_RES_1,

	FUNC_CALL	ST_EFF_METAL, 2, STEFF_GRA_METAL, 0

	WAIT_FLAG
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_1
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_2
	POKEOAM_RES_FREE

	WAIT			30

	SE_R			SEQ_SE_DP_BRADE
	SE_WAITPLAY_R	SEQ_SE_DP_W025B,6

	ADD_PARTICLE 	0,W_231_407_LINE_L,			EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_231_231_KEMRURI_BURN,	EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_231_231_CHIP_BURN,		EMTFUNC_DEFENCE_POS
	
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_S_X, 0, SHAKE_S_WAIT, SHAKE_S_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
