//==============================================================================================
/**
 * @file	snd_perap.h
 * @brief	サウンド ペラップ関数
 * @author	Satoshi Nohara
 * @date	2006.04.15
 */
//==============================================================================================
#ifndef __SND_PERAP_H__
#define __SND_PERAP_H__


#include "common.h"
#include "system/snd_tool.h"
#include "savedata/savedata.h"				//
#include "savedata/perapvoice.h"			//


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	サウンドメインから呼ばれる
 *			ぺラップの録音したデータの再生終了を監視してチャンネル開放をする
 *
 * @param	none
 *
 * @retval	"処理した=TRUE、何もしない=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_MainPerapCheck( void );

//--------------------------------------------------------------
/**
 * @brief	ペラップの録音したデータが再生可能かチェック
 *
 * @param	no		ポケモンナンバー
 *
 * @retval	"再生可能=TRUE、再生不可=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_PerapVoiceCheck( const PERAPVOICE* perap );

//--------------------------------------------------------------
/**
 * @brief	ペラップの録音したデータを再生しているかチェック
 *
 * @param	no		ポケモンナンバー
 *
 * @retval	"再生中=TRUE、再生していない=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_PerapVoicePlayingCheck( const PERAPVOICE* perap );

//--------------------------------------------------------------
/**
 * @brief	ペラップの録音したデータを再生
 *
 * @param	perap	PERAPVOICE型のポインタ
 * @param	sex		性別
 * @param	vol		ボリューム
 * @param	pan		パン
 *
 * @retval	"再生成功=TRUE、失敗=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_PerapVoicePlaySub( const PERAPVOICE* perap, u32 sex, int vol, int pan );

//--------------------------------------------------------------
/**
 * @brief	ペラップの録音したデータを停止
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
extern void Snd_PerapVoiceStop( void );

//--------------------------------------------------------------
/**
 * @brief	ペラップデータ録音開始(サウンドシステムの領域に一時保存する)
 *
 * @param	none
 *
 * @retval	"MIC_RESULT_SUCCESS		処理が正常に完了"
 * @retval	"それ以外				何らかの原因で失敗"
 */
//--------------------------------------------------------------
extern MICResult Snd_PerapVoiceRecStart( void );

//--------------------------------------------------------------
/**
 * @brief	ペラップデータ録音停止
 *
 * @param	none
 *
 * @retval	"MIC_RESULT_SUCCESS		処理が正常に完了"
 * @retval	"それ以外				何らかの原因で失敗"
 */
//--------------------------------------------------------------
extern MICResult Snd_PerapVoiceRecStop( void );

//--------------------------------------------------------------
/**
 * @brief	録音したデータをセーブデータにセット
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
extern void Snd_PerapVoiceDataSave( PERAPVOICE* perap );


//==============================================================================================
//
//	サウンドシステム内で、
//
//	●ぺラップナンバーかチェック
//	●デフォルトの鳴き声を鳴らすフラグをチェック
//
//	録音したデータを再生するかに対応される関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ぺラップのデフォルトの鳴き声を再生するフラグ操作
 *
 * @param	no		フラグ操作
 *
 * @retval	none
 */
//--------------------------------------------------------------
extern void Snd_PerapVoiceDefaultFlagSet( u8 no );


//==============================================================================================
//
//	通信確保されるぺラップデータのポインタを受け取り、再生する関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	サウンドシステムのぺラップポインタにセット
 *
 * @param	perap	PERAPVOICE型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
extern void Snd_PerapVoicePtrSet( PERAPVOICE* perap );

//--------------------------------------------------------------
/**
 * @brief	ペラップの録音したデータを再生
 *
 * @param	perap	ぺラップデータのポインタ
 * @param	sex		性別
 * @param	vol		ボリューム
 * @param	pan		パン
 *
 * @retval	"再生成功=TRUE、失敗=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_PerapVoicePlay( PERAPVOICE* perap, u32 sex, int vol, int pan );


//==============================================================================================
//
//	技「おしゃべり」
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	技のパラメータを取得
 *
 * @param	id		どのぺラップを鳴らすか(通信ID)
 *
 * @retval	"パラメータ 0〜2"
 */
//--------------------------------------------------------------
//extern int Snd_PerapVoiceWazaParamGet( u32 id );
extern	int Snd_PerapVoiceWazaParamGet( PERAPVOICE* perap );


//--------------------------------------------------------------
/**
 * @brief	録音データ、デフォルトデータどちらを再生するかチェック
 *
 * @param	type		再生パターン
 *
 * @retval	"TRUE=録音したデータを再生する、FALSE=デフォルトの鳴き声を再生する"
 */
//--------------------------------------------------------------
extern BOOL Snd_PerapVoicePMVoiceTypeCheck( int type );


#endif


