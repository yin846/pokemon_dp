//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		texanm_sys.c
 *	@brief		ITPアニメデータの必要なところのみを抜き出しテクスチャをアニメーションデータを取得するシステム
 *	@author	 
 *	@data		2005.10.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#define	__TEXANM_SYS_H_GLOBAL
#include "texanm_sys.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constにはc_を付ける
 *						staticにはs_を付ける
 *						ポインタにはp_を付ける
 *						全て合わさるとcsp_となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *
 *	@brief	frame数に対応するテクスチャインデックス　パレットインデックスの取得
 *
 *	@param	cp_texanm	テクスチャアニメーションデータテーブル
 *	@param	c_frame		現在フレーム数
 *
 *	@return	フレーム数に対応したテクスチャインデックス　パレットインデックス
 *	
		typedef struct {
			u8	tex_idx;		// テクスチャインデックス	
			u8	pltt_idx;		// パレットインデックス
		} TEXANM_DATA;
 *
 *
 */
//-----------------------------------------------------------------------------
TEXANM_DATA TEXANM_GetFrameData( const TEXANM_DATATBL* cp_texanm, const u16 c_frame )
{
	int i;					// ループ用
	TEXANM_DATA	o_tex_anm;	// 戻り値

	// フレーム数に対応するテーブル要素数（ i ）を取得
	for( i = 0; i < cp_texanm->tbl_num - 1; i++ ){

		// i + 1の値が要素数が変わるフレーム数です。
		if( cp_texanm->cp_frame[ i + 1 ] > c_frame ){

			break;
		}
	}
	
	// フレーム数に対応するテクスチャインデックス
	// パレットインデックス代入
	o_tex_anm.tex_idx = cp_texanm->cp_tex_idx[ i ];
	o_tex_anm.pltt_idx = cp_texanm->cp_pltt_idx[ i ];

	return o_tex_anm;
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ROMから読み込んできたデータをテクスチャアニメーションデータテーブル形式にアンパックする
 *
 *	@param	cp_buff			読み込んできたデータ
 *	@param	op_texanm		アンパック先テクスチャアニメーションデータテーブル
 *
 *	@return	none
 *
 * ●op_texanmを使用している期間ではcp_buffのメモリは確保したままにしておいてください
 *
 */
//-----------------------------------------------------------------------------
void TEXANM_UnPackLoadFile( const void* cp_buff, TEXANM_DATATBL* op_texanm )
{	
	const u8* work;

	work = cp_buff;
	
	// テーブル要素数取得
	// 最初の4byteはテーブル要素数
	op_texanm->tbl_num = *((u32*)work);
	work = work + sizeof(u32);		// 4byte移動
	
	// フレームデータテーブル先頭ポインタ設定
	op_texanm->cp_frame = (u16*)work;
	work = work + (sizeof(u16) * op_texanm->tbl_num);	// テーブル分移動

	// テクスチャインデックスデータテーブル先頭ポインタ設定
	op_texanm->cp_tex_idx = (u8*)work;
	work = work + (sizeof(u8) * op_texanm->tbl_num);	// テーブル分移動

	// パレットインデックスデータテーブル先頭ポインタ設定
	op_texanm->cp_pltt_idx = (u8*)work;
}
