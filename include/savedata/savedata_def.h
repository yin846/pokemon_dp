//=============================================================================
/**
 * @file	savedata_def.h
 * @brief	セーブデータ参照用不完全型定義
 * @author	tamada	GAME FREAK Inc.
 * @date	2005.10.13
 */
//=============================================================================
#ifndef	__SAVEDATA_DEF_H__
#define	__SAVEDATA_DEF_H__

//------------------------------------------------------------
/**
 * @brief	セーブデータ管理構造への不完全型ポインタ
 *
 * 中身は見えないけどポインタ経由で参照はできる
 */
//------------------------------------------------------------
typedef struct _SAVEDATA SAVEDATA;

//------------------------------------------------------------
///	ロード結果定義
//------------------------------------------------------------
typedef enum{
	LOAD_RESULT_NULL = 0,		///<データなし
	LOAD_RESULT_OK,				///<データ正常読み込み
	LOAD_RESULT_NG,				///<データ異常
	LOAD_RESULT_BREAK,			///<破壊、復旧不能 
}LOAD_RESULT;

//------------------------------------------------------------
///	セーブ結果定義
//------------------------------------------------------------
typedef enum {
	SAVE_RESULT_CONTINUE = 0,		///<セーブ処理継続中
	SAVE_RESULT_LAST,				///<セーブ処理継続中、最後の一つ前
	SAVE_RESULT_OK,					///<セーブ正常終了
	SAVE_RESULT_NG,					///<セーブ失敗終了
}SAVE_RESULT;

#endif	/*	__SAVEDATA_DEF_H__ */
