//============================================================================================
/**
 * @file	pokegra_get.c
 * @brief	ポケモングラフィック取得処理
 * @author	Hiroyuki Nakamura
 * @date	2005.05.09
 */
//============================================================================================
#define POKEGRA_GET_H_GLOBAL

#include "common.h"
#include "system/pm_str.h"
#include "pokegra_get.h"
#include "monsno.h"


//============================================================================================
//	定数定義
//============================================================================================
#define	STR_LEN_NUM		( 5 )	// 開発ナンバーの文字サイズ ( "_001" + '\0' )

// パス名の最大サイズ ( "/data/pokeicon/pmdp_XXXX_000_X.XXXX" + '\0' )
#define	STR_LEN_PATH	( 36 )

#define	MONS_NUM_MASK		( 0x0000ffff )	// パラメータマスク：開発ナンバー
#define	EX_CODE_MASK		( 0x00ff0000 )	// パラメータマスク：特殊ポケモン
#define	FILE_TYPE_MASK		( 0x03000000 )	// パラメータマスク：ファイルタイプ
#define	GRAPHIC_TYPE_MASK	( 0x0c000000 )	// パラメータマスク：グラフィックタイプ
#define	MONS_SEX_MASK		( 0x10000000 )	// パラメータマスク：性別
#define	MONS_COLOR_MASK		( 0x20000000 )	// パラメータマスク：カラー


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void GraphicTypeSet( char * path, u32 param );
static void MonsNumberSet( char * path, u32 param );
static void CodeSet( char * path, u32 param );
static void FileTypeSet( char * path, u32 param );


//============================================================================================
//	グローバル変数
//============================================================================================
static const char * AnnoonTbl[] = {
	"unb", "unc", "und", "une", "unf", "ung", "unh", "uni", "unj",
	"unk", "unl", "unm", "unn", "uno", "unp", "unq", "unr", "uns",
	"unt", "unu", "unv", "unw", "unx", "uny", "unz", "exc", "que"
};


//--------------------------------------------------------------------------------------------
/**
 * パス名作成
 *
 * @param	path	パス名格納場所
 * @param	param	パラメータ
 *
 * @return	none
 *
 * @li	作成されるパス名の例："/data/pmdp_frnt_001_m.ncgr"（フシギダネ♂の前面のキャラ）
 *
 *	例１）キモリ♂の前面キャラ
 *	  PokeGraGetPath( path, MONSNO_KIMORI | POKEGRA_CGX | POKEGRA_FRNT | POKEGRA_MALE );
 *	例２）キモリ♂のパレット
 *	  PokeGraGetPath( path, MONSNO_KIMORI | POKEGRA_PAL | POKEGRA_FRNT | POKEGRA_N_COL );
 *	例３）キモリのアイコンキャラ
 *	  PokeGraGetPath( path, MONSNO_KIMORI | POKEGRA_CGX | POKEGRA_ICON );
 *	例４）キモリのアイコンパレット
 *	  PokeGraGetPath( path, MONSNO_KIMORI | POKEGRA_PAL | POKEGRA_ICON );
 */
//--------------------------------------------------------------------------------------------
void PokeGraGetPath( char * path, u32 param )
{
	GraphicTypeSet( path, param );		// グラフィックタイプ（前面、背面、アイコンなど）
	MonsNumberSet( path, param );		// 開発ナンバー
	CodeSet( path, param );				// 識別コード
	FileTypeSet( path, param );			// 拡張子
}

//--------------------------------------------------------------------------------------------
/**
 * グラフィックデータ取得
 *
 * @param	param	パラメータ
 * @param	mode	メモリ取得モード
 *
 * @return	グラフィックデータ
 *
 * @li	mallocを使用するので、開放を忘れずに！
 */
//--------------------------------------------------------------------------------------------
void * PokeGraGet( u32 param, u8 mode )
{
	void * data;
	char * path;

	path = (char *)sys_AllocMemory( mode, STR_LEN_PATH );
	PokeGraGetPath( path, param );
	data = sys_LoadFile( mode, path );
	sys_FreeMemory( mode, path );

	return data;
}


//--------------------------------------------------------------------------------------------
/**
 * 文字列作成：グラフィックタイプ（前面、背面、アイコンなど）
 *
 * @param	path	文字列格納場所
 * @param	param	パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void GraphicTypeSet( char * path, u32 param )
{
	switch( ( param & GRAPHIC_TYPE_MASK ) ){
	case POKEGRA_FRNT:	// 前面
		strcpy( path, "/data/pokegra/pmdp_frnt" );
		break;
	case POKEGRA_BACK:	// 背面
		strcpy( path, "/data/pokegra/pmdp_back" );
		break;
	case POKEGRA_ICON:	// アイコン
		strcpy( path, "/data/pokeicon/pmdp_icon" );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 文字列作成：開発ナンバー
 *
 * @param	path	文字列格納場所
 * @param	param	パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MonsNumberSet( char * path, u32 param )
{
	u16		mons;
	u8		type;
	char	str[STR_LEN_NUM];

	mons = (u16)( param & MONS_NUM_MASK );
	type = (u8)( ( param & EX_CODE_MASK ) >> POKEGRA_EXCODE_SHIFT );

	strcat( path, "_" );

	// アンノーン（A以外のキャラ取得）
	if( ( mons == MONSNO_ANNOON ) && ( type != 0 ) &&
		( ( param & FILE_TYPE_MASK ) == POKEGRA_CGX ) ){

		strcat( path, AnnoonTbl[type-1] );
	}else{
		StrNumSet( &path[1], mons, 3 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 文字列作成：識別コード
 *
 * @param	path	文字列格納場所
 * @param	param	パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CodeSet( char * path, u32 param )
{
	// アイコンは無視
	if( ( param & GRAPHIC_TYPE_MASK ) == POKEGRA_ICON ){
		return;
	}

	switch( ( param & FILE_TYPE_MASK ) ){
	case POKEGRA_CGX:	// キャラ
		// 現状は♂しかないので、♀の場合は性別不明のファイルにする
		if( ( param & MONS_SEX_MASK ) == POKEGRA_MALE ){
			strcat( path, "_m" );
		}else{
//			strcat( path, "_f" );
			strcat( path, "_o" );
		}
		break;

	case POKEGRA_PAL:	// パレット
		if( ( param & MONS_COLOR_MASK ) == POKEGRA_N_COL ){
			strcat( path, "_n" );
		}else{
			strcat( path, "_r" );
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 文字列作成：拡張子
 *
 * @param	path	文字列格納場所
 * @param	param	パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void FileTypeSet( char * path, u32 param )
{
	switch( ( param & FILE_TYPE_MASK ) ){
	case POKEGRA_CGX:
		strcat( path, ".ncgr" );
		break;
	case POKEGRA_PAL:
		strcat( path, ".nclr" );
		break;
	}
}



/*
const u8 IconPalAtr[] =
{
	0,		//000
	1,		//001
	1,		//002
	1,		//003
	0,		//004
	0,		//005
	0,		//006
	0,		//007
	2,		//008
	2,		//009
	1,		//010
	1,		//011
	0,		//012
	1,		//013
	2,		//014
	2,		//015
	0,		//016
	0,		//017
	0,		//018
	2,		//019
	1,		//020
	0,		//021
	0,		//022
	2,		//023
	2,		//024
	2,		//025
	0,		//026
	2,		//027
	2,		//028
	2,		//029
	2,		//030
	2,		//031
	2,		//032
	2,		//033
	2,		//034
	0,		//035
	0,		//036
	2,		//037
	1,		//038
	0,		//039
	0,		//040
	2,		//041
	2,		//042
	1,		//043
	0,		//044
	0,		//045
	0,		//046
	0,		//047
	0,		//048
	2,		//049
	2,		//050
	2,		//051
	1,		//052
	1,		//053
	1,		//054
	2,		//055
	1,		//056
	2,		//057
	0,		//058
	0,		//059
	0,		//060
	0,		//061
	0,		//062
	2,		//063
	2,		//064
	2,		//065
	0,		//066
	2,		//067
	0,		//068
	1,		//069
	1,		//070
	1,		//071
	2,		//072
	2,		//073
	1,		//074
	1,		//075
	1,		//076
	0,		//077
	0,		//078
	0,		//079
	0,		//080
	0,		//081
	0,		//082
	1,		//083
	2,		//084
	2,		//085
	2,		//086
	2,		//087
	2,		//088
	2,		//089
	2,		//090
	2,		//091
	2,		//092
	2,		//093
	2,		//094
	2,		//095
	2,		//096
	1,		//097
	2,		//098
	2,		//099
	0,		//100
	0,		//101
	0,		//102
	1,		//103
	1,		//104
	1,		//105
	2,		//106
	2,		//107
	1,		//108
	2,		//109
	2,		//110
	1,		//111
	1,		//112
	0,		//113
	0,		//114
	1,		//115
	0,		//116
	0,		//117
	0,		//118
	0,		//119
	2,		//120
	2,		//121
	0,		//122
	1,		//123
	2,		//124
	1,		//125
	0,		//126
	2,		//127
	2,		//128
	0,		//129
	0,		//130
	2,		//131
	2,		//132
	2,		//133
	0,		//134
	0,		//135
	0,		//136
	0,		//137
	0,		//138
	0,		//139
	2,		//140
	2,		//141
	0,		//142
	1,		//143
	0,		//144
	0,		//145
	0,		//146
	0,		//147
	0,		//148
	2,		//149
	2,		//150
	0,		//151
	1,		//152
	1,		//153
	1,		//154
	1,		//155
	1,		//156
	1,		//157
	2,		//158
	2,		//159
	2,		//160
	2,		//161
	2,		//162
	2,		//163
	2,		//164
	0,		//165
	0,		//166
	1,		//167
	0,		//168
	2,		//169
	2,		//170
	0,		//171
	0,		//172
	0,		//173
	1,		//174
	2,		//175
	2,		//176
	0,		//177
	0,		//178
	2,		//179
	0,		//180
	0,		//181
	1,		//182
	2,		//183
	2,		//184
	1,		//185
	1,		//186
	1,		//187
	1,		//188
	2,		//189
	2,		//190
	1,		//191
	1,		//192
	1,		//193
	0,		//194
	0,		//195
	2,		//196
	2,		//197
	2,		//198
	0,		//199
	0,		//200
	0,		//201
	0,		//202
	1,		//203
	0,		//204
	2,		//205
	2,		//206
	2,		//207
	0,		//208
	0,		//209
	2,		//210
	0,		//211
	0,		//212
	1,		//213
	2,		//214
	0,		//215
	0,		//216
	2,		//217
	0,		//218
	0,		//219
	2,		//220
	2,		//221
	0,		//222
	0,		//223
	0,		//224
	0,		//225
	2,		//226
	0,		//227
	0,		//228
	0,		//229
	0,		//230
	0,		//231
	0,		//232
	0,		//233
	2,		//234
	1,		//235
	2,		//236
	2,		//237
	1,		//238
	1,		//239
	1,		//240
	1,		//241
	1,		//242
	0,		//243
	2,		//244
	0,		//245
	1,		//246
	0,		//247
	1,		//248
	0,		//249
	1,		//250
	1,		//251
	0,		//252
	0,		//253
	0,		//254
	0,		//255
	0,		//256
	0,		//257
	0,		//258
	0,		//259
	0,		//260
	0,		//261
	0,		//262
	0,		//263
	0,		//264
	0,		//265
	0,		//266
	0,		//267
	0,		//268
	0,		//269
	0,		//270
	0,		//271
	0,		//272
	0,		//273
	0,		//274
	0,		//275
	0,		//276
	1,		//277
	0,		//278
	1,		//279
	0,		//280
	0,		//281
	0,		//282
	0,		//283
	0,		//284
	0,		//285
	2,		//286
	2,		//287
	2,		//288
	2,		//289
	0,		//290
	2,		//291
	0,		//292
	2,		//293
	1,		//294
	1,		//295
	1,		//296
	1,		//297
	1,		//298
	1,		//299
	0,		//300
	1,		//301
	1,		//302
	1,		//303
	2,		//304
	2,		//305
	1,		//306
	1,		//307
	1,		//308
	0,		//309
	0,		//310
	2,		//311
	0,		//312
	2,		//313
	0,		//314
	0,		//315
	2,		//316
	1,		//317
	1,		//318
	0,		//319
	0,		//320
	1,		//321
	2,		//322
	0,		//323
	0,		//324
	0,		//325
	0,		//326
	0,		//327
	2,		//328
	0,		//329
	0,		//330
	0,		//331
	1,		//332
	1,		//333
	1,		//334
	2,		//335
	1,		//336
	1,		//337
	0,		//338
	1,		//339
	0,		//340
	2,		//341
	2,		//342
	0,		//343
	1,		//344
	1,		//345
	2,		//346
	0,		//347
	1,		//348
	0,		//349
	2,		//350
	0,		//351
	2,		//352
	0,		//353
	0,		//354
	2,		//355
	0,		//356
	0,		//357
	0,		//358
	0,		//359
	0,		//360
	0,		//361
	0,		//362
	0,		//363
	2,		//364
	2,		//365
	1,		//366
	1,		//367
	2,		//368
	1,		//369
	0,		//370
	2,		//371
	2,		//372
	0,		//373
	0,		//374
	0,		//375
	0,		//376
	0,		//377
	0,		//378
	2,		//379
	0,		//380
	1,		//381
	2,		//382
	2,		//383
	2,		//384
	0,		//385
	0,		//386
	2,		//387
	2,		//388
	0,		//389
	0,		//390
	0,		//391
	1,		//392
	1,		//393
	1,		//394
	2,		//395
	2,		//396
	0,		//397
	0,		//398
	0,		//399
	0,		//400
	2,		//401
	2,		//402
	2,		//403
	2,		//404
	0,		//405
	1,		//406
	0,		//407
	2,		//408
	0,		//409
	0,		//410
	0,		//411
	1,		//412
	0,		//412
	0,		//413
	0,		//414
	0,		//415
	0,		//416
	0,		//417
	0,		//418
	0,		//419
	0,		//420
	0,		//421
	0,		//422
	0,		//423
	0,		//424
	0,		//425
	0,		//426
	0,		//427
	0,		//428
	0,		//429
	0,		//430
	0,		//431
	0,		//432
	0,		//433
	0,		//434
	0,		//435
	0,		//436
	0,		//437
	0,		//438
};

*/
