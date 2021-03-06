//=============================================================================================
/**
 * @file	debug_3d_draw_count.h
 * @brief	デバッグ用描画関連カウント（ポリゴン数とか）
 */
//=============================================================================================
#ifdef PM_DEBUG

#ifndef __DEBUG_3D_DRAW_COUNT__
#define __DEBUG_3D_DRAW_COUNT__

#include <nitro.h>
#include <nnsys.h>
#define DEBUG_3DDRAW_COUNT	//有効にするとポリゴン描画の回数、描画呼び出しの数え上げを行います

//----TextureVRAMマネージャーの管理構造体はライブラリ内に隠されているのでコピペしてきました
typedef struct 
{
    u32                         addr;       // 領域開始アドレス
    u32                         szByte;     // 領域サイズ(ゼロは不正です)
    
    u32        pBlkPrev;   // 前の領域(アドレス位置的な関連はない)
    u32        pBlkNext;   // 次の領域(アドレス位置的な関連はない)
    
}NNSiGfdLnkVramBlock;

//------------------------------------------------------------------
/**
 * 3D描画のために調べた数値を退避
 *
*/
//------------------------------------------------------------------
extern void D_3D_DC_DrawPolygonCountUp(void);

//------------------------------------------------------------------
/**
 * NITROSYSTEMに渡したポリゴンの数を返す
 *
 * @param	int ポリゴン数
*/
//------------------------------------------------------------------
extern int D_3D_DC_DrawPolygonGet(void);

//------------------------------------------------------------------
/**
 * NITROSYSTEMに渡した頂点の数を返す
 *
 * @param	int	頂点数
*/
//------------------------------------------------------------------
extern int D_3D_DC_DrawVertexGet(void);

//------------------------------------------------------------------
/**
 * NITROSYSTEMからポリゴン描画した回数を返す
 *
 * @return	int	描画数
*/
//------------------------------------------------------------------
extern int D_3D_DC_DrawNumGet(void);

//==============================================================================
/**
 * リンクドＶＲＡＭマネージャーとパレットマネージャーのポインタを保存
 *
 * @param   texp			テクスチャマネージャー
 * @param   palp			パレットマネージャー
 * @param	inTexBlock		テクスチャブロックサイズ
 * @param	inPlttBlockSize	パレットブロックサイズ
 *
 * @retval  none		
 */
//==============================================================================
extern void D_3D_DC_VramManPSet(NNSiGfdLnkVramBlock *texp, NNSiGfdLnkVramBlock *palp,
								const int inTexBlock, const int inPlttBlock);

//==============================================================================
/**
 * VRAMへのテクスチゃ転送状態を取得する
 *
 *
 * @retval  int		使用しているＶＲＡＭ容量
 */
//==============================================================================
extern int  D_3D_DC_VramTexSizeGet(void);

//==============================================================================
/**
 * リンクドPALマネージャーの転送状態を取得する
 *
 *
 * @retval  int		リンクドPALマネージャーの転送バイト数
 */
//==============================================================================
extern int  D_3D_DC_VramPalSizeGet(void);

//==============================================================================
/**
 * カウントアップ
 *
 * @param   object_p		レンダーＯＢＪ		
 *
 * @retval  none
 */
//==============================================================================
extern void D_3D_DC_CountNum(NNSG3dRenderObj* object_p);

#endif	//__DEBUG_3D_DRAW_COUNT__

#endif	//PM_DEBUG
