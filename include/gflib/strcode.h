#ifndef __STRCODE_H__
#define __STRCODE_H__
enum {
	EOM_ = 0xffff,
	spc_ = 0x01,   		// "　"
	aa_ = 0x02,    		// "ぁ"
	a_ = 0x03,     		// "あ"
	ii_ = 0x04,    		// "ぃ"
	i_ = 0x05,     		// "い"
	uu_ = 0x06,    		// "ぅ"
	u_ = 0x07,     		// "う"
	ee_ = 0x08,    		// "ぇ"
	e_ = 0x09,     		// "え"
	oo_ = 0x0a,    		// "ぉ"
	o_ = 0x0b,     		// "お"
	ka_ = 0x0c,    		// "か"
	ga_ = 0x0d,    		// "が"
	ki_ = 0x0e,    		// "き"
	gi_ = 0x0f,    		// "ぎ"
	ku_ = 0x10,    		// "く"
	gu_ = 0x11,    		// "ぐ"
	ke_ = 0x12,    		// "け"
	ge_ = 0x13,    		// "げ"
	ko_ = 0x14,    		// "こ"
	go_ = 0x15,    		// "ご"
	sa_ = 0x16,    		// "さ"
	za_ = 0x17,    		// "ざ"
	si_ = 0x18,    		// "し"
	zi_ = 0x19,    		// "じ"
	su_ = 0x1a,    		// "す"
	zu_ = 0x1b,    		// "ず"
	se_ = 0x1c,    		// "せ"
	ze_ = 0x1d,    		// "ぜ"
	so_ = 0x1e,    		// "そ"
	zo_ = 0x1f,    		// "ぞ"
	ta_ = 0x20,    		// "た"
	da_ = 0x21,    		// "だ"
	ti_ = 0x22,    		// "ち"
	di_ = 0x23,    		// "ぢ"
	ttu_ = 0x24,   		// "っ"
	tu_ = 0x25,    		// "つ"
	du_ = 0x26,    		// "づ"
	te_ = 0x27,    		// "て"
	de_ = 0x28,    		// "で"
	to_ = 0x29,    		// "と"
	do_ = 0x2a,    		// "ど"
	na_ = 0x2b,    		// "な"
	ni_ = 0x2c,    		// "に"
	nu_ = 0x2d,    		// "ぬ"
	ne_ = 0x2e,    		// "ね"
	no_ = 0x2f,    		// "の"
	ha_ = 0x30,    		// "は"
	ba_ = 0x31,    		// "ば"
	pa_ = 0x32,    		// "ぱ"
	hi_ = 0x33,    		// "ひ"
	bi_ = 0x34,    		// "び"
	pi_ = 0x35,    		// "ぴ"
	hu_ = 0x36,    		// "ふ"
	bu_ = 0x37,    		// "ぶ"
	pu_ = 0x38,    		// "ぷ"
	he_ = 0x39,    		// "へ"
	be_ = 0x3a,    		// "べ"
	pe_ = 0x3b,    		// "ぺ"
	ho_ = 0x3c,    		// "ほ"
	bo_ = 0x3d,    		// "ぼ"
	po_ = 0x3e,    		// "ぽ"
	ma_ = 0x3f,    		// "ま"
	mi_ = 0x40,    		// "み"
	mu_ = 0x41,    		// "む"
	me_ = 0x42,    		// "め"
	mo_ = 0x43,    		// "も"
	yya_ = 0x44,   		// "ゃ"
	ya_ = 0x45,    		// "や"
	yyu_ = 0x46,   		// "ゅ"
	yu_ = 0x47,    		// "ゆ"
	yyo_ = 0x48,   		// "ょ"
	yo_ = 0x49,    		// "よ"
	ra_ = 0x4a,    		// "ら"
	ri_ = 0x4b,    		// "り"
	ru_ = 0x4c,    		// "る"
	re_ = 0x4d,    		// "れ"
	ro_ = 0x4e,    		// "ろ"
	wa_ = 0x4f,    		// "わ"
	wo_ = 0x50,    		// "を"
	n_ = 0x51,     		// "ん"
	AA_ = 0x52,    		// "ァ"
	A_ = 0x53,     		// "ア"
	II_ = 0x54,    		// "ィ"
	I_ = 0x55,     		// "イ"
	UU_ = 0x56,    		// "ゥ"
	U_ = 0x57,     		// "ウ"
	EE_ = 0x58,    		// "ェ"
	E_ = 0x59,     		// "エ"
	OO_ = 0x5a,    		// "ォ"
	O_ = 0x5b,     		// "オ"
	KA_ = 0x5c,    		// "カ"
	GA_ = 0x5d,    		// "ガ"
	KI_ = 0x5e,    		// "キ"
	GI_ = 0x5f,    		// "ギ"
	KU_ = 0x60,    		// "ク"
	GU_ = 0x61,    		// "グ"
	KE_ = 0x62,    		// "ケ"
	GE_ = 0x63,    		// "ゲ"
	KO_ = 0x64,    		// "コ"
	GO_ = 0x65,    		// "ゴ"
	SA_ = 0x66,    		// "サ"
	ZA_ = 0x67,    		// "ザ"
	SI_ = 0x68,    		// "シ"
	ZI_ = 0x69,    		// "ジ"
	SU_ = 0x6a,    		// "ス"
	ZU_ = 0x6b,    		// "ズ"
	SE_ = 0x6c,    		// "セ"
	ZE_ = 0x6d,    		// "ゼ"
	SO_ = 0x6e,    		// "ソ"
	ZO_ = 0x6f,    		// "ゾ"
	TA_ = 0x70,    		// "タ"
	DA_ = 0x71,    		// "ダ"
	TI_ = 0x72,    		// "チ"
	DI_ = 0x73,    		// "ヂ"
	TTU_ = 0x74,   		// "ッ"
	TU_ = 0x75,    		// "ツ"
	DU_ = 0x76,    		// "ヅ"
	TE_ = 0x77,    		// "テ"
	DE_ = 0x78,    		// "デ"
	TO_ = 0x79,    		// "ト"
	DO_ = 0x7a,    		// "ド"
	NA_ = 0x7b,    		// "ナ"
	NI_ = 0x7c,    		// "ニ"
	NU_ = 0x7d,    		// "ヌ"
	NE_ = 0x7e,    		// "ネ"
	NO_ = 0x7f,    		// "ノ"
	HA_ = 0x80,    		// "ハ"
	BA_ = 0x81,    		// "バ"
	PA_ = 0x82,    		// "パ"
	HI_ = 0x83,    		// "ヒ"
	BI_ = 0x84,    		// "ビ"
	PI_ = 0x85,    		// "ピ"
	HU_ = 0x86,    		// "フ"
	BU_ = 0x87,    		// "ブ"
	PU_ = 0x88,    		// "プ"
	HE_ = 0x89,    		// "ヘ"
	BE_ = 0x8a,    		// "ベ"
	PE_ = 0x8b,    		// "ペ"
	HO_ = 0x8c,    		// "ホ"
	BO_ = 0x8d,    		// "ボ"
	PO_ = 0x8e,    		// "ポ"
	MA_ = 0x8f,    		// "マ"
	MI_ = 0x90,    		// "ミ"
	MU_ = 0x91,    		// "ム"
	ME_ = 0x92,    		// "メ"
	MO_ = 0x93,    		// "モ"
	YYA_ = 0x94,   		// "ャ"
	YA_ = 0x95,    		// "ヤ"
	YYU_ = 0x96,   		// "ュ"
	YU_ = 0x97,    		// "ユ"
	YYO_ = 0x98,   		// "ョ"
	YO_ = 0x99,    		// "ヨ"
	RA_ = 0x9a,    		// "ラ"
	RI_ = 0x9b,    		// "リ"
	RU_ = 0x9c,    		// "ル"
	RE_ = 0x9d,    		// "レ"
	RO_ = 0x9e,    		// "ロ"
	WA_ = 0x9f,    		// "ワ"
	WO_ = 0xa0,    		// "ヲ"
	N_ = 0xa1,     		// "ン"
	n0_ = 0xa2,    		// "０"
	n1_ = 0xa3,    		// "１"
	n2_ = 0xa4,    		// "２"
	n3_ = 0xa5,    		// "３"
	n4_ = 0xa6,    		// "４"
	n5_ = 0xa7,    		// "５"
	n6_ = 0xa8,    		// "６"
	n7_ = 0xa9,    		// "７"
	n8_ = 0xaa,    		// "８"
	n9_ = 0xab,    		// "９"
	A__ = 0xac,    		// "Ａ"
	B__ = 0xad,    		// "Ｂ"
	C__ = 0xae,    		// "Ｃ"
	D__ = 0xaf,    		// "Ｄ"
	E__ = 0xb0,    		// "Ｅ"
	F__ = 0xb1,    		// "Ｆ"
	G__ = 0xb2,    		// "Ｇ"
	H__ = 0xb3,    		// "Ｈ"
	I__ = 0xb4,    		// "Ｉ"
	J__ = 0xb5,    		// "Ｊ"
	K__ = 0xb6,    		// "Ｋ"
	L__ = 0xb7,    		// "Ｌ"
	M__ = 0xb8,    		// "Ｍ"
	N__ = 0xb9,    		// "Ｎ"
	O__ = 0xba,    		// "Ｏ"
	P__ = 0xbb,    		// "Ｐ"
	Q__ = 0xbc,    		// "Ｑ"
	R__ = 0xbd,    		// "Ｒ"
	S__ = 0xbe,    		// "Ｓ"
	T__ = 0xbf,    		// "Ｔ"
	U__ = 0xc0,    		// "Ｕ"
	V__ = 0xc1,    		// "Ｖ"
	W__ = 0xc2,    		// "Ｗ"
	X__ = 0xc3,    		// "Ｘ"
	Y__ = 0xc4,    		// "Ｙ"
	Z__ = 0xc5,    		// "Ｚ"
	a__ = 0xc6,    		// "ａ"
	b__ = 0xc7,    		// "ｂ"
	c__ = 0xc8,    		// "ｃ"
	d__ = 0xc9,    		// "ｄ"
	e__ = 0xca,    		// "ｅ"
	f__ = 0xcb,    		// "ｆ"
	g__ = 0xcc,    		// "ｇ"
	h__ = 0xcd,    		// "ｈ"
	i__ = 0xce,    		// "ｉ"
	j__ = 0xcf,    		// "ｊ"
	k__ = 0xd0,    		// "ｋ"
	l__ = 0xd1,    		// "ｌ"
	m__ = 0xd2,    		// "ｍ"
	n__ = 0xd3,    		// "ｎ"
	o__ = 0xd4,    		// "ｏ"
	p__ = 0xd5,    		// "ｐ"
	q__ = 0xd6,    		// "ｑ"
	r__ = 0xd7,    		// "ｒ"
	s__ = 0xd8,    		// "ｓ"
	t__ = 0xd9,    		// "ｔ"
	u__ = 0xda,    		// "ｕ"
	v__ = 0xdb,    		// "ｖ"
	w__ = 0xdc,    		// "ｗ"
	x__ = 0xdd,    		// "ｘ"
	y__ = 0xde,    		// "ｙ"
	z__ = 0xdf,    		// "ｚ"
	gyoe_ = 0xe1,  		// "！"
	hate_ = 0xe2,  		// "？"
	ten_ = 0xe3,   		// "、"
	kten_ = 0xe4,  		// "。"
	tenten_ = 0xe5,		// "…"
	nakag_ = 0xe6, 		// "・"
	sura_ = 0xe7,  		// "／"
	kako_ = 0xe8,  		// "「"
	kakot_ = 0xe9, 		// "」"
	kako2_ = 0xea, 		// "『"
	kakot2_ = 0xeb,		// "』"
	MaruKako__ = 0xec,		// "（"
	MaruKakot__ = 0xed,		// "）"
	osu_ = 0xee,   		// "♂"
	mesu_ = 0xef,  		// "♀"
	Plus__ = 0xf0, 		// "＋"
	bou_ = 0xf1,   		// "ー"
	times_ = 0xf2, 		// "×"
	divide_ = 0xf3,		// "÷"
	equal_ = 0xf4, 		// "＝"
	wave_ = 0xf5,  		// 波線
	colon_ = 0xf6, 		// "："
	semicolon_ = 0xf7,		// "；"
	period_ = 0xf8,		// "．"
	comma_ = 0xf9, 		// "，"
	spade_ = 0xfa, 		// トランプのスペード
	club_ = 0xfb,  		// トランプのクラブ
	heart_ = 0xfc, 		// トランプのハート
	diamond_ = 0xfd,		// トランプのダイヤ
	star_ = 0xfe,  		// 星形
	double_circle_ = 0xff,		// "◎"
	circle_ = 0x100,		// "○"
	square_ = 0x101,		// "□"
	triangle_ = 0x102,		// "△"
	lozenge__ = 0x103,		// "◇"
	atmark_ = 0x104,		// "＠"
	note_ = 0x105, 		// "♪"
	percent_ = 0x106,		// "％"
	sun_ = 0x107,  		// 晴れマーク
	cloud_ = 0x108,		// くもりマーク
	rain_ = 0x109, 		// 雨マーク
	snow_ = 0x10a, 		// 雪マーク
	org_face_normal_ = 0x10b,		// 顔マーク：すまし
	org_face_smile_ = 0x10c,		// 顔マーク：えがお
	org_face_cry_ = 0x10d,		// 顔マーク：泣き
	org_face_angry_ = 0x10e,		// 顔マーク：怒り
	org_upper_ = 0x10f,		// 上カーブやじるし
	org_downer_ = 0x110,		// 下カーブやじるし
	org_sleep_ = 0x111,		// 熟睡マーク
	yen_ = 0x112,  		// "円"
	pocket_item = 0x113,		// ポケットアイコン：どうぐ
	pocket_keyitem = 0x114,		// ポケットアイコン：だいじなもの
	pocket_wazamachine = 0x115,		// ポケットアイコン：わざマシン
	pocket_seal = 0x116,		// ポケットアイコン：シール
	pocket_medicine = 0x117,		// ポケットアイコン：くすり
	pocket_nut = 0x118,		// ポケットアイコン：きのみ
	pocket_ball = 0x119,		// ポケットアイコン：モンスターボール
	pocket_battle = 0x11a,		// ポケットアイコン：戦闘用
	ArrowL__ = 0x11b,		// "←"
	ArrowU__ = 0x11c,		// "↑"
	ArrowD__ = 0x11d,		// "↓"
	ArrowR__ = 0x11e,		// "→"
	cursor_ = 0x11f,		// カーソル
	anpersand_ = 0x120,		// "＆"
	h_n0_ = 0x121, 		// "0"
	h_n1_ = 0x122, 		// "1"
	h_n2_ = 0x123, 		// "2"
	h_n3_ = 0x124, 		// "3"
	h_n4_ = 0x125, 		// "4"
	h_n5_ = 0x126, 		// "5"
	h_n6_ = 0x127, 		// "6"
	h_n7_ = 0x128, 		// "7"
	h_n8_ = 0x129, 		// "8"
	h_n9_ = 0x12a, 		// "9"
	h_A__ = 0x12b, 		// "A"
	h_B__ = 0x12c, 		// "B"
	h_C__ = 0x12d, 		// "C"
	h_D__ = 0x12e, 		// "D"
	h_E__ = 0x12f, 		// "E"
	h_F__ = 0x130, 		// "F"
	h_G__ = 0x131, 		// "G"
	h_H__ = 0x132, 		// "H"
	h_I__ = 0x133, 		// "I"
	h_J__ = 0x134, 		// "J"
	h_K__ = 0x135, 		// "K"
	h_L__ = 0x136, 		// "L"
	h_M__ = 0x137, 		// "M"
	h_N__ = 0x138, 		// "N"
	h_O__ = 0x139, 		// "O"
	h_P__ = 0x13a, 		// "P"
	h_Q__ = 0x13b, 		// "Q"
	h_R__ = 0x13c, 		// "R"
	h_S__ = 0x13d, 		// "S"
	h_T__ = 0x13e, 		// "T"
	h_U__ = 0x13f, 		// "U"
	h_V__ = 0x140, 		// "V"
	h_W__ = 0x141, 		// "W"
	h_X__ = 0x142, 		// "X"
	h_Y__ = 0x143, 		// "Y"
	h_Z__ = 0x144, 		// "Z"
	h_a__ = 0x145, 		// "a"
	h_b__ = 0x146, 		// "b"
	h_c__ = 0x147, 		// "c"
	h_d__ = 0x148, 		// "d"
	h_e__ = 0x149, 		// "e"
	h_f__ = 0x14a, 		// "f"
	h_g__ = 0x14b, 		// "g"
	h_h__ = 0x14c, 		// "h"
	h_i__ = 0x14d, 		// "i"
	h_j__ = 0x14e, 		// "j"
	h_k__ = 0x14f, 		// "k"
	h_l__ = 0x150, 		// "l"
	h_m__ = 0x151, 		// "m"
	h_n__ = 0x152, 		// "n"
	h_o__ = 0x153, 		// "o"
	h_p__ = 0x154, 		// "p"
	h_q__ = 0x155, 		// "q"
	h_r__ = 0x156, 		// "r"
	h_s__ = 0x157, 		// "s"
	h_t__ = 0x158, 		// "t"
	h_u__ = 0x159, 		// "u"
	h_v__ = 0x15a, 		// "v"
	h_w__ = 0x15b, 		// "w"
	h_x__ = 0x15c, 		// "x"
	h_y__ = 0x15d, 		// "y"
	h_z__ = 0x15e, 		// "z"
	Agrave_ = 0x15f,		// Aアクサングラーブ 
	Aacute_ = 0x160,		// Aアクサンテギュ 
	Acirc_ = 0x161,		// Aサーカムフレックス 
	Atilde_ = 0x162,		// Aティルド 
	Auml_ = 0x163, 		// Aウムラウト 
	Aring_ = 0x164,		// Aリング 
	AElig_ = 0x165,		// AE合字 
	Ccedil_ = 0x166,		// Cセディラ 
	Egrave_ = 0x167,		// Eアクサングラーブ 
	Eacute_ = 0x168,		// Eアクサンテギュ 
	Ecirc_ = 0x169,		// Eサーカムフレックス 
	Euml_ = 0x16a, 		// Eウムラウト 
	Igrave_ = 0x16b,		// Iアクサングラーブ 
	Iacute_ = 0x16c,		// Iアクサンテギュ 
	Icirc_ = 0x16d,		// Iサーカムフレックス 
	Iuml_ = 0x16e, 		// Iウムラウト 
	ETH_ = 0x16f,  		// 音声記号eth 
	Ntilde_ = 0x170,		// Nティルド 
	Ograve_ = 0x171,		// Oアクサングラーブ 
	Oacute_ = 0x172,		// Oアクサンテギュ 
	Ocirc_ = 0x173,		// Oサーカムフレックス 
	Otilde_ = 0x174,		// Oティルド 
	Ouml_ = 0x175, 		// Oウムラウト 
	h_times_ = 0x176,		// "["
	Oslash_ = 0x177,		// Oスラッシュ 
	Ugrave_ = 0x178,		// Uアクサングラーブ 
	Uacute_ = 0x179,		// Uアクサンテギュ 
	Ucirc_ = 0x17a,		// Uサーカムフレックス 
	Uuml_ = 0x17b, 		// Uウムラウト 
	Yacute_ = 0x17c,		// Yアクサンテギュ 
	THORN_ = 0x17d,		// 音声記号th 
	szlig_ = 0x17e,		// sz合字 
	agrave_ = 0x17f,		// aアクサングラーブ 
	aacute_ = 0x180,		// aアクサンテギュ 
	acirc_ = 0x181,		// aサーカムフレックス 
	atiled_ = 0x182,		// aティルド 
	auml_ = 0x183, 		// aウムラウト 
	aring_ = 0x184,		// aリング 
	aelig_ = 0x185,		// ae合字 
	ccedil_ = 0x186,		// cセディラ 
	egrave_ = 0x187,		// eアクサングラーブ 
	eacute_ = 0x188,		// eアクサンテギュ 
	ecirc_ = 0x189,		// eサーカムフレックス 
	euml_ = 0x18a, 		// eウムラウト 
	igrave_ = 0x18b,		// iアクサングラーブ 
	iacute_ = 0x18c,		// iアクサンテギュ 
	icirc_ = 0x18d,		// iサーカムフレックス 
	iuml_ = 0x18e, 		// iウムラウト 
	eth_ = 0x18f,  		// eth合字 
	ntiled_ = 0x190,		// nティルド 
	ograve_ = 0x191,		// oアクサングラーブ 
	oacute_ = 0x192,		// oアクサンテギュ 
	ocirc_ = 0x193,		// oサーカムフレックス 
	otilde_ = 0x194,		// oティルド 
	ouml_ = 0x195, 		// oウムラウト 
	h_divide_ = 0x196,		// "]"
	oslash_ = 0x197,		// oスラッシュ 
	ugrave_ = 0x198,		// uアクサングラーブ 
	uacute_ = 0x199,		// uアクサンテギュ 
	ucirc_ = 0x19a,		// uサーカムフレックス 
	uuml_ = 0x19b, 		// uウムラウト 
	yacute_ = 0x19c,		// yアクサンテギュ 
	thorn_ = 0x19d,		// 音声記号th 
	yuml_ = 0x19e, 		// yウムラウト 
	OElig_ = 0x19f,		// OE合字 
	oelig_ = 0x1a0,		// oe合字 
	Scedil_ = 0x1a1,		// Sセディラ 
	scedil_ = 0x1a2,		// sセディラ 
	sup_a_ = 0x1a3,		// "?"
	sup_o_ = 0x1a4,		// "?"
	sup_er_ = 0x1a5,		// "?"
	sup_re_ = 0x1a6,		// "?"
	sup_r_ = 0x1a7,		// "?"
	pokedoru_ = 0x1a8,		// ポケドル
	rgyoe_ = 0x1a9,		// 反転！
	rhate_ = 0x1aa,		// 反転？
	h_gyoe_ = 0x1ab,		// "!"
	h_hate_ = 0x1ac,		// "?"
	h_comma_ = 0x1ad,		// ","
	h_period_ = 0x1ae,		// "."
	h_tenten_ = 0x1af,		// "|"
	h_nakag_ = 0x1b0,		// "･"
	h_sura_ = 0x1b1,		// "/"
	us_quote1_ = 0x1b2,		// 英語全角引用符（開き）／独語引用符（閉じ）
	us_h_quote1_ = 0x1b3,		// 英語半角引用符／アポストロフィー
	us_quote2_ = 0x1b4,		// 英語全角２重引用符（開き）／独語２重引用符（閉じ）
	us_quote2d_ = 0x1b5,		// 英語全角２重引用符（閉じ）
	ger_quote2_ = 0x1b6,		// 独語２重引用符（開き）
	fre_quote_ = 0x1b7,		// 仏語引用符（開き）
	fre_quoted_ = 0x1b8,		// 仏語引用符（閉じ）
	h_MaruKako__ = 0x1b9,		// "("
	h_MaruKakot__ = 0x1ba,		// ")"
	h_osu_ = 0x1bb,		// 海外版♂記号
	h_mesu_ = 0x1bc,		// 海外版♀記号
	h_plus_ = 0x1bd,		// "+"
	h_bou_ = 0x1be,		// "-"
	h_asterisk_ = 0x1bf,		// "*"
	h_sharp_ = 0x1c0,		// "#"
	h_equal_ = 0x1c1,		// "="
	h_ampersand_ = 0x1c2,		// "&"
	h_tilda_ = 0x1c3,		// "~"
	h_colon_ = 0x1c4,		// ":"
	h_semicolon_ = 0x1c5,		// ";"
	h_spade_ = 0x1c6,		// "?"
	h_club_ = 0x1c7,		// "?"
	h_heart_ = 0x1c8,		// "?"
	h_diamond_ = 0x1c9,		// "?"
	h_star_ = 0x1ca,		// "?"
	h_double_circle_ = 0x1cb,		// "?"
	h_circle_ = 0x1cc,		// "?"
	h_square_ = 0x1cd,		// "?"
	h_triangle_ = 0x1ce,		// "?"
	h_lozenge__ = 0x1cf,		// "?"
	h_atmark_ = 0x1d0,		// "@"
	h_note_ = 0x1d1,		// "?"
	h_percent_ = 0x1d2,		// "%"
	h_sun_ = 0x1d3,		// "?"
	h_cloud_ = 0x1d4,		// "?"
	h_rain_ = 0x1d5,		// "?"
	h_snow_ = 0x1d6,		// "?"
	h_org_face_normal_ = 0x1d7,		// "?"
	h_org_face_smile_ = 0x1d8,		// "?"
	h_org_face_cry_ = 0x1d9,		// "?"
	h_org_face_angry_ = 0x1da,		// "?"
	h_org_upper_ = 0x1db,		// "?"
	h_org_downer_ = 0x1dc,		// "?"
	h_org_sleep_ = 0x1dd,		// "?"
	h_spc_ = 0x1de,		// " "
	sup_e_ = 0x1df,		// "?"
	charcode_reserve1_ = 0x1fe,		// "■"
	charcode_reserve2_ = 0x1ff,		// "▼"
	charcode_reserve3_ = 0x200,		// "▽"
	STRCODE_END,
};

#endif
