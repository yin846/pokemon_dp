//=============================================================================
/**
 * @file	gym_init.h
 * @bfief	ジム仕掛け初期化とか
 * @author	Nozomu Saito
 *
 */
//=============================================================================
#ifndef __GYM_INIT_H__
#define __GYM_INIT_H__

#include "field_common.h"

//---------------------------------------------------------------------------
/**
 * @brief	水ジム:初期化
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_InitWaterGym(FIELDSYS_WORK *fsys);

//---------------------------------------------------------------------------
/**
 * @brief	ゴーストジム:初期化
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_InitGhostGym(FIELDSYS_WORK *fsys);

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:初期化
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
extern void GYM_InitSteelGym(FIELDSYS_WORK *fsys);

//---------------------------------------------------------------------------
/**
 * @brief	格闘ジム:初期化
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
extern void GYM_InitCombatGym(FIELDSYS_WORK *fsys);

//---------------------------------------------------------------------------
/**
 * @brief	電気ジム:初期化
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
extern void GYM_InitElecGym(FIELDSYS_WORK *fsys, const u8 inRoomNo);


#endif //__GYM_INIT_H__

