//============================================================================
/**
 *
 *@file		be_267.s
 *@brief	ν¬V[PXiBattleEffectj
 *			267@^Ή΅½ΉΚΕ¬m¦ͺΟνιB
 *				 NP0ANQ10ANR30BΉf[^ͺΘ’AΨbvΘOΝNP΅’B
 *
 *@author	HisashiSogabe
 *@data		2006.05.18
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_267:
	VALUE			VAL_SET,BUF_PARA_ADD_STATUS_INDIRECT,ADD_COND2_OSYABERI|ADD_STATUS_DEFENCE|ADD_STATUS_WAZAKOUKA
	CRITICAL_CHECK
	DAMAGE_CALC
	SEQ_END
