
//============================================================================
/**
 *
 *@file		sub_103.s
 *@brief	戦闘シーケンス
 *			天候系追加効果シーケンス
 *@author	HisashiSogabe
 *@data		2006.01.27
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_103:
	GOSUB	SUB_SEQ_MESSAGE_WORK
	//本来は、特性てんきやのチェックを入れる
	SEQ_END
