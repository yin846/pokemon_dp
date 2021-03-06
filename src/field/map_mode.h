//============================================================================================
/**
 * @file	map_mode.h
 * @brief	マップごとの定義
 * @date	2006.04.23
 * @author	tamada GAME FREAK inc.
 */
//============================================================================================
#ifndef	__MAP_MODE_H__
#define	__MAP_MODE_H__

//-----------------------------------------------------------------------------
/**
 * @brief	マップの大別指定
 */
//-----------------------------------------------------------------------------
typedef enum {
	MAP_MODE_GROUND,		///<通常
	MAP_MODE_UNDER,			///<炭鉱
	MAP_MODE_UNION,			///<ユニオンルーム
	MAP_MODE_COLOSSEUM,		///<通信対戦部屋
	MAP_MODE_BTOWER,		///<バトルタワー

	MAP_MODE_MAX,			///<リミット定義
}MAP_MODE;


//-----------------------------------------------------------------------------
/**
 * @brief	MAP_MODEごとの個別パラメータ
 */
//-----------------------------------------------------------------------------
typedef struct {
	u32 subscreen:4;				///<サブ画面アプリ指定
	u32 DivMapMode:4;				///<DivMap用指定
	u32 MapToolMode:4;				///<MapTool用指定
	u32 disp3DSW:4;					///<サブ・メイン画面位置指定
	u32 SpecialAttrFlag:4;			///<特殊アトリビュート処理指定
	u32 UseOverlayFlag:4;			///<フィールドオーバーレイ使用指定
	u32 SpecialAttrNum:8;			///<特殊アトリビュート処理で読み込むデータ数

	u32 HeapSize;					///<フィールドマップで確保するヒープのサイズ
}MAP_MODE_DATA;

#endif	/* __MAP_MODE_H__ */
