//============================================================================
/**
 *
 *@file		be_166.s
 *@brief	ν¬V[PXiBattleEffectj
 *			166@θΜΑκUπPiK °Δ¬σΤΙ·ι
 *
 *@author	HisashiSogabe
 *@data		2006.02.06
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_166:
	VALUE		VAL_SET,BUF_PARA_ADD_STATUS_DIRECT,ADD_COND2_ODATERU|ADD_STATUS_WAZAKOUKA|ADD_STATUS_DEFENCE
	SEQ_END
