//============================================================================================
/**
 * @file	ookisa.h
 * @bfief	おおきさくらべ用関数
 * @author	Tomomichi Ohta
 * @date	06.03.23
 */
//============================================================================================
#ifndef OOKISA_H
#define OOKISA_H

//最初の記録の基準となる個体値
#define OOKISA_KURABE_DEFAULT_RECORD	(0x8200)

//--------------------------------------------------------------------------------------------
/**
 *　おおきさを比べる
 *
 * @param	fsys		FIELDSYS_WORK構造体へのポインタ
 * @param	num			選んだ手持ち番号
 *
 * @return	"0:更新せず、1:同じ、2=記録更新"
 */
//--------------------------------------------------------------------------------------------
extern u8 OokisaRecordChk( FIELDSYS_WORK* fsys,u16 num);

//--------------------------------------------------------------------------------------------
/**
 *　おおきさのレコード(個体値)を更新
 *
 * @param	fsys		FIELDSYS_WORK構造体へのポインタ
 * @param	num			選んだ手持ち番号
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern void OokisaRecordSet(FIELDSYS_WORK* fsys,u16 num);

//--------------------------------------------------------------------------------------------
/**
 *　レコードを数字に変換して、文字バッファへセット
 *
 * @param	fsys		FIELDSYS_WORK構造体へのポインタ
 * @param	buf_no1		文字バッファ１
 * @param	buf_no2		文字バッファ２
 * @param	monsno		ポケモンナンバー
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern void OokisaRecordBufSet(FIELDSYS_WORK* fsys,u8 buf_no1,u8 buf_no2,u16 monsno);

//--------------------------------------------------------------------------------------------
/**
 *　手持ちポケモン高さを数字に変換して、文字バッファへセット
 *
 * @param	fsys		FIELDSYS_WORK構造体へのポインタ
 * @param	buf_no1		文字バッファ１
 * @param	buf_no2		文字バッファ２
 * @param	num			手持ち番号
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern void OokisaTemotiBufSet(FIELDSYS_WORK* fsys,u8 buf_no1,u8 buf_no2,u16 num);


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#endif	/* OOKISA_H */

