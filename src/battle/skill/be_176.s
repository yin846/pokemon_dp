//============================================================================
/**
 *
 *@file		be_176.s
 *@brief	ν¬V[PXiBattleEffectj
 *			176@θ―΅½θΜZΜΠΝπPDT{Ι·ι
 *
 *@author	HisashiSogabe
 *@data		2005.10.03
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_176:
	TEDASUKE		Umakukimaran
	MESSAGE_PARAM	TedasukeM2MMsg,TAG_NICK_NICK,SIDE_ATTACK,SIDE_WORK
	VALUE			VAL_SET,BUF_PARA_ADD_STATUS_INDIRECT,ADD_COND2_MESSAGE_WORK|ADD_STATUS_WAZAKOUKA
	SEQ_END

Umakukimaran:
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_UMAKUKIMARAN
	SEQ_END
