//==============================================================================
/**
 * @file	poke_anm_b021_2.s
 * @brief	高速回転
 * @author	
 * @date	
 *
 */
//==============================================================================

	.text
	
	.include	"past.h"
	.include	"past_def.h"
	
	.global		PAST_ANIME_B021_2
	
// =============================================================================
//
//
//	背面	高速回転スロー
//
//
// =============================================================================
#define	WIDTH	(8)
#define CALC_NUM	(18)
#define HEIGHT	(8)
PAST_ANIME_B021_3:
	SET_ADD_PARAM	PARAM_DY,USE_VAL,HEIGHT,PARAM_SET
	APPLY_TRANS
	
	//2周をCALC_NUM回で処理
	CALL_MF_CURVE_DIVTIME	APPLY_SET,0,CURVE_SIN_MINUS,TARGET_DX,WIDTH,0x20000,0,CALC_NUM
	CALL_MF_CURVE_DIVTIME	APPLY_ADD,0,CURVE_COS_MINUS,TARGET_DY,HEIGHT,0x20000,0,CALC_NUM
	HOLD_CMD
	
	END_ANM
