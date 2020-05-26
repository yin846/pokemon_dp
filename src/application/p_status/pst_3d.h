//============================================================================================
/**
 * @file	pst_3d.h
 * @brief	ポケモンステータス画面3D処理
 * @author	Hiroyuki Nakamura
 * @date	2006.01.24
 */
//============================================================================================
#ifndef PST_3D_H
#define PST_3D_H
#undef GLOBAL
#ifdef PST_3D_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 3D関連初期化
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeStatus_3DInit( PST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 3D関連メイン
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeStatus_3DMain( PST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 3D関連削除
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeStatus_3DRelease( PST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * カメラ設定
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeStatus_CameraInit( PST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * コンディションゲージ初期化
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeStatus_ConditionParamInit( PST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * コンディションゲージ動作パラメータ作成
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeStatus_ConditionParamMake( PST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ソフトウェアスプライトセット
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeStatus_SoftSpriteSet( PST_WORK * wk );


GLOBAL void PokeStatus_SoftSpriteAnmSet( PST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ソフトウェアスプライト再セット
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeStatus_SoftSpriteReset( PST_WORK * wk );


#undef GLOBAL
#endif	/* PST_3D_H */
