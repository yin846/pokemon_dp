//============================================================================================
/**
 * @file	misc_local.h
 * @brief	未分類セーブデータを扱うためのヘッダ
 * @author	tamada
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
 */
//============================================================================================
#ifndef	__MISC_LOCAL_H__
#define	__MISC_LOCAL_H__

#include "savedata/misc.h"		//MISC型参照のため

//============================================================================================
//============================================================================================
//----------------------------------------------------------
//	セーブデータシステムが依存する関数
//----------------------------------------------------------
extern int MISC_GetWorkSize(void);
extern void MISC_Copy(const MISC * from, MISC * to);

//----------------------------------------------------------
//	MISC操作のための関数
//----------------------------------------------------------
extern void MISC_Init(MISC * misc);

//----------------------------------------------------------
//----------------------------------------------------------

//----------------------------------------------------------
//----------------------------------------------------------



#endif	__MISC_LOCAL_H__

