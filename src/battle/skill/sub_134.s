
//============================================================================
/**
 *
 *@file		sub_134.s
 *@brief	戦闘シーケンス
 *			トリックシーケンス
 *@author	HisashiSogabe
 *@data		2006.02.07
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_134:
	IF				IF_FLAG_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_NOHIT_CHG,Umakukimaran
	MIGAWARI_CHECK	SIDE_DEFENCE,Umakukimaran
	//特性マルチタイプは、うまくきまらん
	IF_PSP			IF_FLAG_EQ,SIDE_DEFENCE,ID_PSP_speabino,TOKUSYU_MARUTITAIPU,Umakukimaran
	//特性マルチタイプは、うまくきまらん
	IF_PSP			IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_speabino,TOKUSYU_MARUTITAIPU,Umakukimaran
	TRICK			Umakukimaran,NoHitTokusei
	GOSUB			SUB_SEQ_WAZA_OUT_EFF
	MESSAGE			TrickMineMsg,TAG_NICK,SIDE_ATTACK
	SERVER_WAIT
	WAIT			MSG_WAIT
	PSP_VALUE_WORK	VAL_GET,SIDE_ATTACK,ID_PSP_item,BUF_PARA_TEMP_WORK
	PSP_VALUE_WORK	VAL_GET,SIDE_DEFENCE,ID_PSP_item,BUF_PARA_CALC_WORK
	PSP_VALUE_WORK	VAL_SET,SIDE_ATTACK,ID_PSP_item,BUF_PARA_CALC_WORK
	PSP_VALUE_WORK	VAL_SET,SIDE_DEFENCE,ID_PSP_item,BUF_PARA_TEMP_WORK
	IF_PSP			IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_item,0,SUB_134_NEXT
	MESSAGE			TrickGetMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_ATTACK
	SERVER_WAIT
	WAIT			MSG_WAIT
SUB_134_NEXT:
	IF_PSP			IF_FLAG_EQ,SIDE_DEFENCE,ID_PSP_item,0,SUB_134_END
	MESSAGE			TrickGetMineMsg,TAG_NICK_ITEM,SIDE_DEFENCE,SIDE_DEFENCE
	SERVER_WAIT
	WAIT			MSG_WAIT
SUB_134_END:
	SEQ_END

NoHitTokusei:
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	MESSAGE			TokuseiNoWazaMineMsg,TAG_NICK_TOKU_WAZA,SIDE_DEFENCE,SIDE_DEFENCE,SIDE_ATTACK
	SERVER_WAIT
	WAIT			MSG_WAIT
	SEQ_END

Umakukimaran:
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_UMAKUKIMARAN
	SEQ_END
	
