//============================================================================================
/**
 * @file	ashiato.c
 * @bfief	tg}L³ρp«ΥΦS
 * @author	Tomomichi Ohta
 * @date	06.05.19
 */
//============================================================================================
#include <nitro.h>
#include <nnsys.h>
#include "common.h"
#include "system/bmp_list.h"
#include "system/bmp_menu.h"
#include "fld_bmp.h"
#include "system/lib_pack.h"
#include "fntsys.h"
#include "system/fontproc.h"
#include "talk_msg.h"
#include "fieldsys.h"
#include "poketool/monsno.h"

#include "ashiato.h"
#include "ashiato_def.h"

//============================================================================================
//
//	θ`
//
//============================================================================================

typedef struct{
	u16	group;
	u16	ato;
}ASHIATO_TT;


//============================================================================================
//
//	vg^CvιΎ
//
//============================================================================================



//============================================================================================
//
//	Ο
//
//============================================================================================
//060609Ε
static const ASHIATO_TT ashi_data[MONSNO_END+1]={
	// ͺή			 «ΥΜL³
	{ 0	, 0},			//dummy(1originΜΧνΉ)
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ1
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ2
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ3
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ4
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ5
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ6
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ7
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ8
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ9
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ10
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ11
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ12
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ13
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ14
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ15
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ16
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ17
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ18
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ19
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ20
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ21
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ22
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ23
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ24
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ25
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ26
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ27
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ28
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ29
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ30
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ31
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ32
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ33
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ34
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ35
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ36
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ37
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ38
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ39
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ40
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ41
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ42
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ43
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ44
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ45
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ46
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ47
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ48
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ49
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ50
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ51
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ52
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ53
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ54
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ55
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ56
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ57
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ58
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ59
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ60
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ61
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ62
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ63
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ64
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ65
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ66
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ67
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ68
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ69
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ70
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ71
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ72
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ73
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ74
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ75
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ76
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ77
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ78
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ79
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ80
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ81
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ82
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ83
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ84
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ85
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ86
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ87
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ88
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ89
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ90
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ91
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ92
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ93
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ94
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ95
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ96
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ97
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ98
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ99
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ100
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ101
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ102
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ103
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ104
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ105
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ106
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ107
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ108
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ109
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ110
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ111
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ112
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ113
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ114
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ115
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ116
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ117
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ118
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ119
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ120
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ121
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ122
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ123
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ124
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ125
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ126
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ127
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ128
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ129
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ130
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ131
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ132
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ133
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ134
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ135
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ136
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ137
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ138
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ139
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ140
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ141
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ142
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ143
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ144
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ145
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ146
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ147
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ148
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ149
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ150
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ151
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ152
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ153
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ154
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ155
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ156
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ157
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ158
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ159
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ160
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ161
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ162
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ163
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ164
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ165
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ166
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ167
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ168
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ169
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ170
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ171
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ172
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ173
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ174
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ175
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ176
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ177
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ178
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ179
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ180
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ181
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ182
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ183
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ184
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ185
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ186
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ187
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ188
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ189
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ190
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ191
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ192
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ193
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ194
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ195
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ196
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ197
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ198
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ199
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ200
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ201
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ202
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ203
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ204
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ205
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ206
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ207
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ208
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ209
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ210
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ211
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ212
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ213
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ214
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ215
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ216
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ217
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ218
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ219
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ220
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ221
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ222
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ223
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ224
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ225
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ226
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ227
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ228
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ229
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ230
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ231
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ232
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ233
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ234
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ235
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ236
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ237
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ238
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ239
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ240
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ241
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ242
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ243
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ244
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ245
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ246
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ247
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ248
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ249
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ250
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ251
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ252
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ253
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ254
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ255
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ256
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ257
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ258
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ259
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ260
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ261
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ262
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ263
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ264
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ265
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ266
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ267
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ268
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ269
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ270
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ271
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ272
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ273
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ274
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ275
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ276
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ277
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ278
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ279
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ280
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ281
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ282
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ283
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ284
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ285
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ286
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ287
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ288
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ289
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ290
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ291
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ292
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ293
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ294
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ295
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ296
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ297
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ298
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ299
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ300
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ301
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ302
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ303
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ304
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ305
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ306
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ307
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ308
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ309
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ310
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ311
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ312
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ313
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ314
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ315
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ316
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ317
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ318
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ319
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ320
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ321
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ322
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ323
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ324
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ325
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ326
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ327
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ328
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ329
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ330
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ331
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ332
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ333
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ334
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ335
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ336
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ337
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ338
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ339
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ340
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ341
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ342
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ343
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ344
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ345
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ346
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ347
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ348
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ349
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ350
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ351
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ352
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ353
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ354
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ355
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ356
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ357
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ358
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ359
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ360
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ361
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ362
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ363
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ364
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ365
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ366
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ367
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ368
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ369
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ370
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ371
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ372
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ373
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ374
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ375
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ376
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ377
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ378
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ379
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ380
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ381
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ382
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ383
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ384
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ385
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ386
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ387
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ388
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ389
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ390
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ391
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ392
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ393
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ394
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ395
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ396
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ397
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ398
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ399
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ400
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ401
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ402
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ403
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ404
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ405
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ406
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ407
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ408
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ409
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ410
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ411
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ412
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ413
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ414
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ415
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ416
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ417
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ418
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ419
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ420
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ421
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ422
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ423
	{ KAWAII_KEI	,ASHIATO_ARI},		//}ΣΤ424
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ425
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ426
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ427
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ428
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ429
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ430
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ431
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ432
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ433
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ434
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ435
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ436
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ437
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ438
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ439
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ440
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ441
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ442
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ443
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ444
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ445
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ446
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ447
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ448
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ449
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ450
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ451
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ452
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ453
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ454
	{ OTOBOKE_KEI		,ASHIATO_NASHI},		//}ΣΤ455
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ456
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ457
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ458
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ459
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ460
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ461
	{ UNOWN_KEI		,ASHIATO_NASHI},		//}ΣΤ462
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ463
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ464
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ465
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ466
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ467
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ468
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ469
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ470
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ471
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ472
	{ OTOBOKE_KEI		,ASHIATO_ARI},		//}ΣΤ473
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ474
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ475
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ476
	{ KOWAI_KEI		,ASHIATO_NASHI},		//}ΣΤ477
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ478
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ479
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ480
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ481
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ482
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ483
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ484
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ485
	{ UNOWN_KEI		,ASHIATO_ARI},		//}ΣΤ486
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ487
	{ KAKKOII_KEI		,ASHIATO_NASHI},		//}ΣΤ488
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ489
	{ KAWAII_KEI		,ASHIATO_NASHI},		//}ΣΤ490
	{ KOWAI_KEI		,ASHIATO_ARI},		//}ΣΤ491
	{ KAWAII_KEI		,ASHIATO_ARI},		//}ΣΤ492
	{ KAKKOII_KEI		,ASHIATO_ARI},		//}ΣΤ493
};

//============================================================================================
//
//	ΦQ
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 *	|PΤ©η«ΥΜͺήΙΟ·
 *
 * @param	monsno		|PΤ
 *
 * @return	ͺή(1`5)
 */
//--------------------------------------------------------------------------------------------
u16 ashiato_group_chk(u16 mons_no)
{
	return ashi_data[mons_no].group;
}

//--------------------------------------------------------------------------------------------
/**
 *	|PΤ©η«ΥΜL³π`FbN
 *
 * @param	monsno		|PΤ
 *
 * @return	0:«ΥΘ΅ 1:«Υ θ
 */
//--------------------------------------------------------------------------------------------
u16 ashiato_ato_chk(u16 mons_no)
{
	return ashi_data[mons_no].ato;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
