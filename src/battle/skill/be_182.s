//============================================================================
/**
 *
 *@file		be_182.s
 *@brief	ν¬V[PXiBattleEffectj
 *			182@Uͺqbg·ιΖA©ͺΜUΝAhδΝͺPiKΊͺι
 *
 *@author	HisashiSogabe
 *@data		2005.10.03
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_182:
	VALUE			VAL_SET,BUF_PARA_ADD_STATUS_INDIRECT,ADD_COND2_BAKADIKARA|ADD_STATUS_ATTACK|ADD_STATUS_WAZAKOUKA
	CRITICAL_CHECK
	DAMAGE_CALC
	SEQ_END
