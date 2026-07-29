//==================================================================================
// 
// D3DXCOLORの色関連をまとめたヘッダーファイル [color.h]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
#ifndef _COLOR_H_
#define _COLOR_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** 色関連名前空間 ***
//**********************************************************************************
namespace Color
{
	//******************************************************************************
	// *** 色の種類 ***
	//******************************************************************************
	typedef enum
	{
		COLOR_ALICEBLUE,						// アリスブルー（薄い水色）
		COLOR_ANTIQUEWHITE,						// アンティークホワイト（くすんだ真珠色）
		COLOR_AQUA,								// アクア（水色）
		COLOR_AQUAMARINE,						// アクアマリン（藍玉色）
		COLOR_AZURE,							// アジュール（薄い空色）
		COLOR_BEIGE,							// ベージュ
		COLOR_BISQUE,							// ビスキー（薄い茶色）
		COLOR_BLACK,							// 黒
		COLOR_BLANCHEDALMOND,					// ブランシェドアマンド（薄い杏色）
		COLOR_BLUE,								// 青
		COLOR_BLUEVIOLET,						// ブルーバイオレット（青紫）
		COLOR_BROWN,							// 茶色
		COLOR_BURLYWOOD,						// バーリーウッド（濃いベージュ）
		COLOR_CADETBLUE,						// カデットブルー（くすんだ青）
		COLOR_CHARTREUSE,						// シャルトルーズ（黄緑）
		COLOR_CHOCOLATE,						// チョコレート
		COLOR_CORAL,							// コーラル（珊瑚色）
		COLOR_CORNFLOWERBLUE,					// コーンフラワーブルー（矢車菊の青）
		COLOR_CORNSILK,							// コーンシルク（トウモロコシのひげのようなくすんだ白）
		COLOR_CRIMSON,							// クリムゾン（深紅）
		COLOR_CYAN,								// シアン（水色）
		COLOR_DARKBLUE,							// ダークブルー（濃い青）
		COLOR_DARKCYAN,							// ダークシアン（濃い水色）
		COLOR_DARKGOLDENROD,					// ダークゴールデンロド（濃い金紅花色）
		COLOR_DARKGRAY,							// ダークグレー（深灰色）
		COLOR_DARKGREEN,						// ダークグリーン（深緑）
		COLOR_DARKKHAKI,						// ダークカーキ
		COLOR_DARKMAGENTA,						// ダークマゼンタ（濃い赤紫）
		COLOR_DARKOLIVEGREEN,					// ダークオリーブグリーン（暗オリーブ色）
		COLOR_DARKORANGE,						// ダークオレンジ
		COLOR_DARKORCHID,						// ダークオーキッド（濃い蘭色）
		COLOR_DARKRED,							// ダークレッド（濃い赤）
		COLOR_DARKSALMON,						// ダークサーモン
		COLOR_DARKSEAGREEN,						// ダークシーグリーン（くすんだ薄緑）
		COLOR_DARKSLATEBLUE,					// ダークスレートブルー（暗青灰色）
		COLOR_DARKSLATEGRAY,					// ダークスレートグレー（暗石板色）
		COLOR_DARKTURQUOISE,					// ダークターコイズ（濃いトルコ石色）
		COLOR_DARKVIOLET,						// ダークバイオレット（濃い紫）
		COLOR_DEEPPINK,							// ディープピンク
		COLOR_DEEPSKYBLUE,						// ディープスカイブルー
		COLOR_DIMGRAY,							// ディムグレー（薄暗い灰色）
		COLOR_DODGERBLUE,						// ドジャーブルー（明るい青）
		COLOR_FIREBRICK,						// ファイアブリック（煉瓦色）
		COLOR_FLORALWHITE,						// フローラルホワイト（花の白）
		COLOR_FORESTGREEN,						// フォレストグリーン（深い森の緑）
		COLOR_FUCHSIA,							// フクシャ（鮮やかな赤紫）
		COLOR_GAINSBORO,						// ゲインズボロ（明るい灰色）
		COLOR_GHOSTWHITE,						// ゴーストホワイト（幽霊の白）
		COLOR_GOLD,								// 金色
		COLOR_GOLDENROD,						// ゴールデンロド（金紅花色）
		COLOR_GRAY,								// 灰色
		COLOR_GREEN,							// 緑
		COLOR_GREENYELLOW,						// グリーンイエロー（黄緑）
		COLOR_HONEYDEW,							// ハニーデュー（甘露蜜の白）
		COLOR_HOTPINK,							// ホットピンク（鮮やかなピンク）
		COLOR_INDIANRED,						// インディアンレッド（インド赤）
		COLOR_INDIGO,							// インディゴ（藍色）
		COLOR_IVORY,							// アイボリー（象牙色）
		COLOR_KHAKI,							// カーキ
		COLOR_LAVENDER,							// ラベンダー
		COLOR_LAVENDERBLUSH,					// ラベンダーブラッシュ（薄いラベンダーピンク）
		COLOR_LAWNGREEN,						// ローングリーン（芝生の緑）
		COLOR_LEMONCHIFFON,						// レモンシフォン
		COLOR_LIGHTBLUE,						// ライトブルー（薄い青）
		COLOR_LIGHTCORAL,						// ライトコーラル（薄い珊瑚色）
		COLOR_LIGHTCYAN,						// ライトシアン（薄い水色）
		COLOR_LIGHTGOLDENRODYELLOW,				// ライトゴールデンロドイエロー（薄い金紅花黄）
		COLOR_LIGHTGREEN,						// ライトグリーン（薄い緑）
		COLOR_LIGHTGRAY,						// ライトグレー（薄い灰色）
		COLOR_LIGHTPINK,						// ライトピンク（薄いピンク）
		COLOR_LIGHTSALMON,						// ライトサーモン（薄い鮭色）
		COLOR_LIGHTSEAGREEN,					// ライトシーグリーン（明るい海洋緑）
		COLOR_LIGHTSKYBLUE,						// ライトスカイブルー（明るい空色）
		COLOR_LIGHTSLATEGRAY,					// ライトスレートグレー（明るい石板色）
		COLOR_LIGHTSTEELBLUE,					// ライトスチールブルー（明るい鋼色）
		COLOR_LIGHTYELLOW,						// ライトイエロー（薄い黄色）
		COLOR_LIME,								// ライム（鮮やかな黄緑）
		COLOR_LIMEGREEN,						// ライムグリーン
		COLOR_LINEN,							// リネン（亜麻色）
		COLOR_MAGENTA,							// マゼンタ（赤紫）
		COLOR_MAROON,							// マルーン（栗色）
		COLOR_MEDIUMAQUAMARINE,					// ミディアムアクアマリン
		COLOR_MEDIUMBLUE,						// ミディアムブルー
		COLOR_MEDIUMORCHID,						// ミディアムオーキッド（中間の蘭色）
		COLOR_MEDIUMPURPLE,						// ミディアムパープル（中間の紫）
		COLOR_MEDIUMSEAGREEN,					// ミディアムシーグリーン（中間の海洋緑）
		COLOR_MEDIUMSLATEBLUE,					// ミディアムスレートブルー（中間の石板青）
		COLOR_MEDIUMSPRINGGREEN,				// ミディアムスプリンググリーン
		COLOR_MEDIUMTURQUOISE,					// ミディアムターコイズ（中間のトルコ石色）
		COLOR_MEDIUMVIOLETRED,					// ミディアムバイオレットレッド（中間の赤紫）
		COLOR_MIDNIGHTBLUE,						// ミッドナイトブルー（夜が更けた真夜中の濃い青）
		COLOR_MINTCREAM,						// ミントクリーム
		COLOR_MISTYROSE,						// ミスティローズ（霧がかったバラ色）
		COLOR_MOCCASIN,							// モカシン（淡い黄茶色）
		COLOR_NAVAJOWHITE,						// ナバホホワイト
		COLOR_NAVY,								// ネイビー（紺色）
		COLOR_OLDLACE,							// オールドレース（くすんだ白）
		COLOR_OLIVE,							// オリーブ
		COLOR_OLIVEDRAB,						// オリーブドラブ（暗い黄緑）
		COLOR_ORANGE,							// オレンジ
		COLOR_ORANGERED,						// オレンジレッド（朱色）
		COLOR_ORCHID,							// オーキッド（蘭色）
		COLOR_PALEGOLDENROD,					// ペールゴールデンロド（淡い金紅花色）
		COLOR_PALEGREEN,						// ペールグリーン（淡い緑）
		COLOR_PALETURQUOISE,					// ペールターコイズ（淡いトルコ石色）
		COLOR_PALEVIOLETRED,					// ペールバイオレットレッド（淡い赤紫）
		COLOR_PAPAYAWHIP,						// パパイヤホイップ
		COLOR_PEACHPUFF,						// ピーチパフ（桃のうぶ毛のような色）
		COLOR_PERU,								// ペルー（ペルー茶色）
		COLOR_PINK,								// ピンク
		COLOR_PLUM,								// プラム（西洋すもも色）
		COLOR_POWDERBLUE,						// パウダーブルー（おしろいの青）
		COLOR_PURPLE,							// 紫
		COLOR_RED,								// 赤
		COLOR_ROSYBROWN,						// ロージーブラン（バラ色の茶色）
		COLOR_ROYALBLUE,						// ロイヤルブルー（英国王室の青）
		COLOR_SADDLEBROWN,						// サドルブラウン（革の鞍の茶色）
		COLOR_SALMON,							// サーモン（鮭色）
		COLOR_SANDYBROWN,						// サンディブラウン（砂茶色）
		COLOR_SEAGREEN,							// シーグリーン（海洋緑）
		COLOR_SEASHELL,							// シーシェル（貝殻の白）
		COLOR_SIENNA,							// シエナ（黄褐色）
		COLOR_SILVER,							// シルバー（銀色）
		COLOR_SKYBLUE,							// スカイブルー（空色）
		COLOR_SLATEBLUE,						// スレートブルー（石板の青）
		COLOR_SLATEGRAY,						// スレートグレー（石板の灰色）
		COLOR_SNOW,								// スノー（雪の白）
		COLOR_SPRINGGREEN,						// スプリンググリーン（新緑の緑）
		COLOR_STEELBLUE,						// スチールブルー（鋼色）
		COLOR_TAN, 								// タン（日焼け色）
		COLOR_TEAL,								// ティール（鴨の羽の青緑）
		COLOR_THISTLE,							// シッスル（あざみ色）
		COLOR_TOMATO,							// トマト（トマトの赤）
		COLOR_TRANSPARENT,						// 透明（カラーなし）
		COLOR_TURQUOISE,						// ターコイズ（トルコ石色）
		COLOR_VIOLET,							// バイオレット（すみれ色）
		COLOR_WHEAT,							// ウィート（小麦色）
		COLOR_WHITE,							// 白
		COLOR_WHITESMOKE,						// ホワイトスモーク（白い煙色）
		COLOR_YELLOW,							// 黄色
		COLOR_YELLOWGREEN,						// イエローグリーン（黄緑）
		COLOR_MAX
	} COLOR;

	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	D3DXCOLOR GetColor(const COLOR col);
	D3DXCOLOR Lerp(const D3DXCOLOR start, const D3DXCOLOR end, const float t);
}
#endif