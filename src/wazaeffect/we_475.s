//==============================================================================
/**
 * @file	we_001.s
 * @brief	はたく
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_Hataku
	
// =============================================================================
//
//
//	■はたく
//
//
// =============================================================================
WEST_Hataku:
	
	FUNC_CALL	WEST_SP_TEST_1, 0, 0
	FUNC_CALL	WEST_SP_TEST_2, 0, 0
	FUNC_CALL	WEST_SP_TEST_3, 0, 0
	FUNC_CALL	WEST_SP_TEST_4, 0, 0
	
	SE			WAZA_EFF_SE1
	
	WAIT		60
	
	SEQEND

