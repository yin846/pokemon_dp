//==============================================================================
/**
 * @file	poke_anm_b004_2.s
 * @brief	横にブルブル
 * @author	saito
 * @date	
 *
 */
//==============================================================================

	.text
	
	.include	"past.h"
	.include	"past_def.h"
	
	.global		PAST_ANIME_B004_2
	
// =============================================================================
//
//
//	背面	横にブルブル
//
//
// =============================================================================
PAST_ANIME_B004_2:
	//19回の処理で9周を処理
	CALL_MF_CURVE_DIVTIME	APPLY_SET,0,CURVE_SIN,TARGET_DX,3,0x10000*9,0,19
	HOLD_CMD

	END_ANM
