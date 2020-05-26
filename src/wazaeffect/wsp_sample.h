//==============================================================================
/**
 * @file	wsp_sample.h
 * @brief	サポートサンプル
 * @author	goto
 * @date	2005.07.14(木)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef	_WSP_SAMPLE_H_
#define _WSP_SAMPLE_H_



// -----------------------------------------
//
//	技エフェクト OAM利用
//
// -----------------------------------------
extern void WestSp_CAT_SampleTCB(WE_SYS_PTR we_sys, CATS_SYS_PTR csp, CATS_RES_PTR crp, CATS_ACT_PTR cap);


// -----------------------------------------
//
//	サポートサンプル関数
//		ただの関数呼び出し
//
// -----------------------------------------
extern void WestSp_Sample(WE_SYS_PTR we_sys);


// -----------------------------------------
//
//	サポートサンプル関数
//		エフェクト用のTCBの作成サンプル
//
// -----------------------------------------
extern void WestSp_SampleEffectTCB(WE_SYS_PTR we_sys);


// -----------------------------------------
//
//	サポートサンプル関数
//		サウンド用のTCBの作成サンプル
//
// -----------------------------------------
extern void WestSp_SampleSoundTCB(WE_SYS_PTR we_sys);


// -----------------------------------------
//
//	サポートサンプル関数
//		通常のTCBの作成サンプル
//
// -----------------------------------------
extern void WestSp_SampleTCB(WE_SYS_PTR we_sys);

#endif

