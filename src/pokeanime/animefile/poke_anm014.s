//==============================================================================
/**
 * @file	poke_anm014.s
 * @brief	
 * @author	
 * @date	
 *
 */
//==============================================================================

	.text
	
	.include	"past.h"
	.include	"past_def.h"
	
	.global		PAST_ANIME014
	
// =============================================================================
//
//
//	揺れながら左右
//
//
// =============================================================================
PAST_ANIME014:
	//50回の処理で左右0xcccの角度傾ける　左右２回ずつ行うので2周分(0x20000)
	CALL_MF_CURVE_DIVTIME APPLY_SET,0,CURVE_SIN_MINUS,TARGET_ROT,0xccc,0x20000,0,50
	//傾きにあわせて横移動
	CALL_MF_CURVE_DIVTIME APPLY_SET,0,CURVE_SIN,TARGET_DX,8,0x10000,0,50
	HOLD_CMD

	END_ANM
