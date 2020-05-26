//============================================================================================
/**
 * @file	misc.c
 * @brief	未分類セーブデータのアクセス
 * @author	tamada	GAME FREAK Inc.
 * @date	2006.01.26
 *
 * 分類されていないセーブデータはとりあえずここに追加される。
 * アプリケーションレベル、セーブデータアクセス関数レベルからは
 * 直接MISC構造体を取り出すことはできない。
 * MISC構造体はメンバに持つ構造体へのアクセスを提供するだけの
 * インターフェイスとして機能している。
 *
 * セーブデータとしての性質がきちんと定義できるようになった時点で、
 * それらのセーブ構造体はきちんと別ソースに切り分けられることになる。
 *
 * 2006.06.02
 * 時間切れで分類がわからんものはここに追加することにしました。
 * 無念。
 */
//============================================================================================

#include "common.h"
#include "gflib/system.h"

#include "system/gamedata.h"

#include "savedata/savedata.h"

#include "savedata/misc.h"
#include "misc_local.h"

#include "seedbed_local.h"
#include "savedata/seedbed.h"
#include "gimmickwork_local.h"
#include "savedata/gimmickwork.h"


#include "gflib/strbuf_family.h"
#include "system/pm_str.h"

//============================================================================================
//============================================================================================

//---------------------------------------------------------------------------
/**
 * @brief	MISC構造体の定義
 *
 * 下記のようなセーブデータブロックは、MISCのメンバーとして確保する。
 * -どこに分類するべきか、判断に迷うもの。今は分類を保留しておきたいもの
 * -規模が小さく、そのためにセーブブロックを確保するのがもったいないもの
 */
//---------------------------------------------------------------------------
struct _MISC {
	SEEDBED seedbed[SEEDBED_MAX];
	GIMMICKWORK gimmick;
	STRCODE rivalname[PERSON_NAME_SIZE + EOM_SIZE];
	STRCODE monument_name[MONUMENT_NAME_SIZE + EOM_SIZE];
};

#ifdef	PM_DEBUG
#if	PM_VERSION == VERSION_DIAMOND
static const STRCODE RivalName[] = {PA_, bou_,RU_, EOM_ };
#elif	PM_VERSION == VERSION_PEARL
static const STRCODE RivalName[] = {DA_, I_, YA_, EOM_ };
#endif
#endif
//============================================================================================
//============================================================================================

//---------------------------------------------------------------------------
/**
 * @brief	MISC構造体のサイズ取得
 * @return	int		MISC構造体のサイズ
 */
//---------------------------------------------------------------------------
int MISC_GetWorkSize(void)
{
	return sizeof(MISC);
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void MISC_Copy(const MISC * from, MISC * to)
{
	MI_CpuCopy8(from ,to, sizeof(MISC));
}

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	色々セーブデータ保持ワークの初期化
 * @param	misc	色々セーブデータ保持ワークへのポインタ
 */
//---------------------------------------------------------------------------
void MISC_Init(MISC * misc)
{
	MI_CpuClearFast(misc, sizeof(MISC));
	/* 以下に個別部分の初期化処理をおく */
	SEEDBED_Init(misc->seedbed);
	GIMMICKWORK_Init(&misc->gimmick);
	MI_CpuFill16(misc->rivalname, EOM_, PERSON_NAME_SIZE + EOM_SIZE);
	MI_CpuFill16(misc->monument_name, EOM_, MONUMENT_NAME_SIZE + EOM_SIZE);
#ifdef	PM_DEBUG
	PM_strcpy(misc->rivalname, RivalName);
#endif
}

//============================================================================================
//
//	セーブデータ取得のための関数
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	MISCセーブデータの取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	MISC構造体へのポインタ
 */
//---------------------------------------------------------------------------
MISC * SaveData_GetMisc(SAVEDATA * sv)
{
	return SaveData_Get(sv, GMDATA_ID_MISC);
}

//---------------------------------------------------------------------------
/**
 * @brief	MISCセーブデータの取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	MISC構造体へのポインタ
 */
//---------------------------------------------------------------------------
const MISC * SaveData_GetMiscReadOnly(const SAVEDATA * sv)
{
	return SaveData_GetReadOnlyData(sv, GMDATA_ID_MISC);
}

//---------------------------------------------------------------------------
/**
 * @brief	きのみ状態データへのポインタ取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	CONFIG		設定情報保持ワークへのポインタ
 */
//---------------------------------------------------------------------------
SEEDBED * SaveData_GetSeedBed(SAVEDATA * sv)
{
	MISC * misc;
	misc = SaveData_Get(sv, GMDATA_ID_MISC);
	return misc->seedbed;
}

//---------------------------------------------------------------------------
/**
 * @brief	マップ固有の仕掛けワークへのポインタ取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	GIMMICKWORK	仕掛け用ワークへのポインタ
 */
//---------------------------------------------------------------------------
GIMMICKWORK * SaveData_GetGimmickWork(SAVEDATA * sv)
{
	MISC * misc;
	misc = SaveData_Get(sv, GMDATA_ID_MISC);
	return &misc->gimmick;
}

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
const STRCODE * MISC_GetRivalName(const MISC * misc)
{
	return misc->rivalname;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void MISC_SetRivalName(MISC * misc, STRBUF * str)
{
	STRBUF_GetStringCode( str, misc->rivalname, PERSON_NAME_SIZE + EOM_SIZE );
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
const STRCODE * MISC_GetMonumentName(const MISC * misc)
{
	return misc->monument_name;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void MISC_SetMonumentName(MISC * misc, STRBUF * str)
{
	STRBUF_GetStringCode( str, misc->monument_name, MONUMENT_NAME_SIZE + EOM_SIZE );
}

