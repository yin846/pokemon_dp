
//============================================================================
/**
 *
 *@file		sub_175.s
 *@brief	戦闘シーケンス
 *			とんぼがえり追加効果シーケンス
 *@author	HisashiSogabe
 *@data		2006.02.16
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_175:
	//相手側が全滅なら、入れ替えなし
	ZENMETSU_CHECK		SIDE_DEFENCE,SUB_175_END
	RESHUFFLE_CHECK		SIDE_ATTACK,POKE_LIST_ON,SUB_175_END

	//技がヒットした時にチェックする特性をチェック
	WAZA_HIT_TOKUSEI_CHECK	SoubiItemCheck
	GOSUB_WORK		BUF_PARA_TEMP_WORK

SoubiItemCheck:
	//技がヒットした時にチェックする装備効果をチェック
	WAZA_HIT_SOUBI_ITEM_CHECK_TONBOGAERI	MichidureCheck
	GOSUB_WORK			BUF_PARA_TEMP_WORK

MichidureCheck:
	//気絶しているか
	IF_PSP			IF_FLAG_NE,SIDE_DEFENCE,ID_PSP_hp,0,NoMichidure
	//みちづれフラグが立っているかチェック
	IF_PSP			IF_FLAG_NBIT,SIDE_DEFENCE,ID_PSP_condition2,CONDITION2_MICHIDURE,NoMichidure
	//味方には発動しない
	SIDE_CHECK		SIDE_ATTACK,SIDE_DEFENCE,NoMichidure
	//HP0（自爆系）には発動しない
	IF_PSP			IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_hp,0,NoMichidure
	MESSAGE			MichidureM2MMsg,TAG_NICK_NICK,SIDE_DEFENCE,SIDE_ATTACK
	//AttackClientのHPをダメージに
	PSP_VALUE_WORK	VAL_GET,SIDE_ATTACK,ID_PSP_hp,BUF_PARA_HP_CALC_WORK
	VALUE			VAL_MUL,BUF_PARA_HP_CALC_WORK,-1
	//HP操作対象をAttackClientに
	VALUE_WORK		VAL_SET,BUF_PARA_CLIENT_WORK,BUF_PARA_ATTACK_CLIENT
	//ダメージエフェクトで点滅しないフラグを立てる
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_BLINK
	GOSUB			SUB_SEQ_HP_CALC
	KIZETSU_EFFECT
	SERVER_WAIT
	HP_GAUGE_OUT	SIDE_KIZETSU
	MESSAGE			KizetsuMineMsg,TAG_NICK,SIDE_KIZETSU
	SERVER_WAIT
	WAIT			MSG_WAIT
	//気絶カウントをカウントアップ
	INC_RECORD		SIDE_KIZETSU,CLIENT_BOOT_TYPE_NOMINE,RECID_KILL_POKE
	INC_RECORD		SIDE_KIZETSU,CLIENT_BOOT_TYPE_MINE,RECID_TEMOTI_KIZETU

NoMichidure:
	//KizetsuClientを退避
	VALUE_WORK		VAL_GET,BUF_PARA_KIZETSU_CLIENT,BUF_PARA_PUSH_CLIENT
	//KizetsuClientをDefenceClientに
	VALUE_WORK		VAL_SET,BUF_PARA_KIZETSU_CLIENT,BUF_PARA_DEFENCE_CLIENT
	//気絶しているか
	IF_PSP			IF_FLAG_NE,SIDE_DEFENCE,ID_PSP_hp,0,NoOnnen
	//おんねんチェック
	ONNEN			NoOnnen
	MESSAGE			OnnenDamageMineMsg,TAG_NICK_WAZA,SIDE_ATTACK,SIDE_WORK
	SERVER_WAIT
	WAIT			MSG_WAIT
NoOnnen:
	//KizetsuClientを復帰
	VALUE_WORK		VAL_SET,BUF_PARA_KIZETSU_CLIENT,BUF_PARA_PUSH_CLIENT

	//すでに気絶しているならいれかえ動作なし
	IF_PSP				IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_hp,0,SUB_175_END

	VALUE_WORK			VAL_SET,BUF_PARA_RESHUFFLE_CLIENT,BUF_PARA_ATTACK_CLIENT
	MESSAGE				TonbogaeriMineMsg,TAG_NICK_TRNAME,SIDE_RESHUFFLE,SIDE_RESHUFFLE
	SERVER_WAIT
	WAIT				MSG_WAIT
	GOSUB				SUB_SEQ_OIUCHI

SUB_175_KIZETSU_CHECK:
	//気絶しているならいれかえ動作なし
	IF_PSP				IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_hp,0,SUB_175_END
	//特性しぜんかいふくをもっていたら、状態異常を直す
	SIZENKAIHUKU_CHECK	SIDE_ATTACK,SUB_175_START
	PSP_VALUE			VAL_SET,SIDE_ATTACK,ID_PSP_condition,0
SUB_175_START:
	//自分の手元に戻る技エフェクトを起動
	VALUE				VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_WAZA_EFFECT
	VALUE				VAL_SET,BUF_PARA_WAZA_EFF_CNT,1
	WAZA_EFFECT			SIDE_ATTACK
	SERVER_WAIT
	POKEMON_DELETE		SIDE_ATTACK
	SERVER_WAIT
	HP_GAUGE_OUT		SIDE_ATTACK
	SERVER_WAIT
SUB_175_NEXT2:
	VALUE				VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_TONBOGAERI
	JUMP				SUB_SEQ_POKEMON_LIST
SUB_175_END:
	SEQ_END

