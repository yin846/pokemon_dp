//=============================================================================================
/**
 * @file	strbuf_family.h
 * @brief	汎用文字列バッファ型オブジェクト（純粋な文字配列を扱うモジュールのみinclude許可）
 * @author	taya
 * @date	2005.11.14
 */
//=============================================================================================
#ifndef __STRBUF_FAMILY_H__
#define __STRBUF_FAMILY_H__

#include "strbuf.h"


//------------------------------------------------------------------
/**
 * 生の文字列（EOM終端）をバッファオブジェクトにセットする
 *
 * @param   strbuf		[out] バッファオブジェクトへのポインタ
 * @param   sz			[in]  EOM で終わる文字配列
 *
 */
//------------------------------------------------------------------
extern void STRBUF_SetStringCode( STRBUF* strbuf, const STRCODE* sz );


//------------------------------------------------------------------
/**
 * 生の文字列（長さ指定）をバッファオブジェクトにセットする
 *
 * @param   strbuf		[out] バッファオブジェクトへのポインタ
 * @param   str			[in]  文字配列の先頭ポインタ
 * @param   len			[in]  セットする文字数（EOMを含む）
 *
 */
//------------------------------------------------------------------
extern void STRBUF_SetStringCodeOrderLength( STRBUF* strbuf, const STRCODE* str, u32 len );


//------------------------------------------------------------------
/**
 * バッファから生の文字配列をコピーする
 *
 * @param   strbuf		[in]  バッファオブジェクトへのポインタ
 * @param   ary			[out] コピー先配列
 * @param   arysize		[in]  コピー先配列の要素数
 *
 */
//------------------------------------------------------------------
extern void STRBUF_GetStringCode( const STRBUF* strbuf, STRCODE* ary, u32 arysize );







//==============================================================================================
// 以下の関数は許可を得ずに呼び出してはいけません！！
//==============================================================================================

//------------------------------------------------------------------
/**
 * バッファオブジェクトが内包する文字配列のアドレスを返す
 * ※この関数を呼び出す箇所は超限定。たぶん文字出力系のみ。
 *
 * @param   strbuf				[in] バッファオブジェクトへのポインタ
 *
 * @retval  const STRCODE*		文字配列のアドレス
 */
//------------------------------------------------------------------
extern const STRCODE* STRBUF_GetStringCodePointer( const STRBUF* strbuf );


//------------------------------------------------------------------
/**
// 文字列の連結操作【【 原則使用禁止 】】
 *
 * @param   dst		[out] 連結されるバッファオブジェクト
 * @param   src		[in]  連結する文字列を持つバッファオブジェクト
 *
 */
//------------------------------------------------------------------
extern void STRBUF_AddStr( STRBUF* dst, const STRBUF* src );


//------------------------------------------------------------------
/**
 * １文字連結【【 原則使用禁止 】】
 *
 * @param   dst			連結されるバッファオブジェクト
 * @param   code		連結する文字コード
 *
 */
//------------------------------------------------------------------
extern void STRBUF_AddChar( STRBUF* dst, STRCODE code );



#endif
