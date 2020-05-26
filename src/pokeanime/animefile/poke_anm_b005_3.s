//==============================================================================
/**
 * @file	poke_anm_b005_3.s
 * @brief	縦に引っ張る
 * @author	
 * @date	
 *
 */
//==============================================================================

	.text
	
	.include	"past.h"
	.include	"past_def.h"
	
	.global		PAST_ANIME_B005_3
	
// =============================================================================
//
//
//	背面	縦に引っ張る
//
//
// =============================================================================
PAST_ANIME_B005_3:
	//20回のループで180°（0x8000）処理
	CALL_MF_CURVE_DIVTIME APPLY_SET,0,CURVE_SIN,TARGET_RY,0x28,0x8000,0,20
	//ウェイト5　10回のループで4周0x40000）分処理
	CALL_MF_CURVE_DIVTIME APPLY_SYNTHE,5,CURVE_SIN,TARGET_RY,0x8,0x40000,0,10
	HOLD_CMD
	
	END_ANM
