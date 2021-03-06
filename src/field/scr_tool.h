//============================================================================================
/**
 * @file	scr_tool.h
 * @bfief	スクリプトで使用するプログラム
 * @author	Satoshi Nohara
 * @date	05.11.16
 */
//============================================================================================
#ifndef __SCR_TOOL_H__
#define __SCR_TOOL_H__


//============================================================================================
//
//	extern宣言
//
//============================================================================================

//============================================================================================
//
//	その他
//
//============================================================================================

//--------------------------------------------------------------
/**
 * 渡された値の桁数を取得
 *
 * @param   num			値
 *
 * @retval  "桁数"
 */
//--------------------------------------------------------------
extern u16 GetNumKeta(u32 num);

//--------------------------------------------------------------
/**
 * 技マシンのアイテムナンバーかチェック
 *
 * @param   itemno		アイテムナンバー
 *
 * @retval  TRUE		技マシン
 * @retval  FALSE		それ以外
 */
//--------------------------------------------------------------
extern u16 WazaMachineItemNoCheck( u16 itemno );


//============================================================================================
//
//	図鑑評価
//
//============================================================================================

//--------------------------------------------------------------
/**
 * シンオウ図鑑評価メッセージID取得
 *
 * @param   num		隠しを除いた見た数
 * @param	c04_arrive	c04ハクタイシティの到着フラグ
 *
 * @retval  "メッセージID"
 */
 //--------------------------------------------------------------
extern u16 GetShinouZukanHyoukaMsgID( u16 num ,u16 c04_arrive);

//--------------------------------------------------------------
/**
 * 評価メッセージID取得
 *
 * @param   num		隠しを除いた捕まえた数
 * @param	sex		主人公の性別
 *
 * @retval  "メッセージID"
 */
//--------------------------------------------------------------
extern u16 GetZenkokuZukanHyoukaMsgID( u16 num ,u16 sex);


//==============================================================================================
//
//	全滅関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ゲームオーバー画面呼び出し
 *
 * @param	fsys	FIELDSYS_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
extern void GameOverCall( FIELDSYS_WORK* fsys, GMEVENT_CONTROL* event );



//--------------------------------------------------------------
/**
 * 先頭ポケモン算出
 *
 * @param   sv		ポケモンセーブ情報構造体
 *
 * @retval  番号
 */
//--------------------------------------------------------------
extern u16 GetFrontPokemon(SAVEDATA * sv);
//--------------------------------------------------------------
/**
 *	@brief	レジキングイベント　キング解放できるかチェック
 *
 *	手持ちにアイス・ロック・スチルがいるかどうか？
 */
//--------------------------------------------------------------
extern BOOL EventCheck_IsReziUnseal(SAVEDATA* sv);

//--------------------------------------------------------------
/**
 * @brief	フィールドオブジェを揺らすイベント 
 * @param	ev	GMEVENT_CONTROL *
 * @param	obj	FIELD_OBJ_PTR	フィールドオブジェへのポインタ
 * @param	time	揺らす回数
 * @param	spd		揺らすスピード(360を割り切れる数字がよい)
 * @param	ofsx	揺らす幅X	
 * @param	ofsz	揺らす幅Z	
 * @retval	BOOL	TRUE=イベント終了
 */
//--------------------------------------------------------------
extern void EventCmd_ObjShakeAnm( GMEVENT_CONTROL *event,FIELD_OBJ_PTR obj,
		u16 time,u16 spd,u16 ofsx,u16 ofsz);

//--------------------------------------------------------------
/**
 * @brief	フィールドオブジェをBlinkさせるアニメイベント 
 * @param	ev		GMEVENT_CONTROL *
 * @param	obj		FIELD_OBJ_PTR	フィールドオブジェへのポインタ
 * @param	count	Blinkさせる回数
 * @param	time	onoffを切り替えるインターバル
 */
//--------------------------------------------------------------
extern void EventCmd_ObjBlinkAnm(GMEVENT_CONTROL *event,FIELD_OBJ_PTR obj,u16 count,u16 time);

#endif


