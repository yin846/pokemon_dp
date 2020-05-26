struct _NTAG_DATA {
	u32	tag_flg[2];	// 表示できるタグ
	u8 now_tag;		// 現在のタグ番号

	u8	scr;		// バッグのスクロールカウンタ
	u8	cur;		// バッグのカーソル位置
	u8	max;		// バッグのカウンタ最大
};

//--------------------------------------------------------------------------------------------
/**
 * 表示可能タグチェック
 *
 * @param	flg		フラグ設定場所
 * @param	id		タグID
 *
 * @retval	"TRUE = 可"
 * @retval	"FALSE = 不可"
 */
//--------------------------------------------------------------------------------------------
extern u8 TagPutFlagGet( u32 * flg, u16 id );
