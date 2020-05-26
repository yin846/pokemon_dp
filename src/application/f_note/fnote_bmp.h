//============================================================================================
/**
 * @file	fnote_bmp.h
 * @brief	冒険ノート　BMP処理
 * @author	Hiroyuki Nakamura
 * @date	2006.03.06
 */
//============================================================================================
#ifndef FNOTE_BMP_H
#define FNOTE_BMP_H
#undef GLOBAL
#ifdef FNOTE_BMP_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


//============================================================================================
//	定数定義
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BMP追加
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FantasyNote_BmpAdd( FNOTE_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * BMP削除
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FantasyNote_BmpFree( FNOTE_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 文字列表示
 *
 * @param	wk		ワーク
 * @param	flg		表示するBMPのフラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FantasyNote_BmpPut( FNOTE_WORK * wk, u32 flg );





//GLOBAL void Debug_FNOTE_BmpPut( FNOTE_WORK * wk, u32 flg );


#undef GLOBAL
#endif	/* FNOTE_BMP_H */
