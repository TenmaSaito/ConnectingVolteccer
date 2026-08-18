//==================================================================================
// 
// Colorの色関連をまとめたヘッダーファイル [color.h]
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
namespace Colors
{
	//******************************************************************************
	// *** 色の種類 ***
	//******************************************************************************
	typedef enum
	{
		C_ALICEBLUE,						// アリスブルー（薄い水色）
		C_ANTIQUEWHITE,						// アンティークホワイト（くすんだ真珠色）
		C_AQUA,								// アクア（水色）
		C_AQUAMARINE,						// アクアマリン（藍玉色）
		C_AZURE,							// アジュール（薄い空色）
		C_BEIGE,							// ベージュ
		C_BISQUE,							// ビスキー（薄い茶色）
		C_BLACK,							// 黒
		C_BLANCHEDALMOND,					// ブランシェドアマンド（薄い杏色）
		C_BLUE,								// 青
		C_BLUEVIOLET,						// ブルーバイオレット（青紫）
		C_BROWN,							// 茶色
		C_BURLYWOOD,						// バーリーウッド（濃いベージュ）
		C_CADETBLUE,						// カデットブルー（くすんだ青）
		C_CHARTREUSE,						// シャルトルーズ（黄緑）
		C_CHOCOLATE,						// チョコレート
		C_CORAL,							// コーラル（珊瑚色）
		C_CORNFLOWERBLUE,					// コーンフラワーブルー（矢車菊の青）
		C_CORNSILK,							// コーンシルク（トウモロコシのひげのようなくすんだ白）
		C_CRIMSON,							// クリムゾン（深紅）
		C_CYAN,								// シアン（水色）
		C_DARKBLUE,							// ダークブルー（濃い青）
		C_DARKCYAN,							// ダークシアン（濃い水色）
		C_DARKGOLDENROD,					// ダークゴールデンロド（濃い金紅花色）
		C_DARKGRAY,							// ダークグレー（深灰色）
		C_DARKGREEN,						// ダークグリーン（深緑）
		C_DARKKHAKI,						// ダークカーキ
		C_DARKMAGENTA,						// ダークマゼンタ（濃い赤紫）
		C_DARKOLIVEGREEN,					// ダークオリーブグリーン（暗オリーブ色）
		C_DARKORANGE,						// ダークオレンジ
		C_DARKORCHID,						// ダークオーキッド（濃い蘭色）
		C_DARKRED,							// ダークレッド（濃い赤）
		C_DARKSALMON,						// ダークサーモン
		C_DARKSEAGREEN,						// ダークシーグリーン（くすんだ薄緑）
		C_DARKSLATEBLUE,					// ダークスレートブルー（暗青灰色）
		C_DARKSLATEGRAY,					// ダークスレートグレー（暗石板色）
		C_DARKTURQUOISE,					// ダークターコイズ（濃いトルコ石色）
		C_DARKVIOLET,						// ダークバイオレット（濃い紫）
		C_DEEPPINK,							// ディープピンク
		C_DEEPSKYBLUE,						// ディープスカイブルー
		C_DIMGRAY,							// ディムグレー（薄暗い灰色）
		C_DODGERBLUE,						// ドジャーブルー（明るい青）
		C_FIREBRICK,						// ファイアブリック（煉瓦色）
		C_FLORALWHITE,						// フローラルホワイト（花の白）
		C_FORESTGREEN,						// フォレストグリーン（深い森の緑）
		C_FUCHSIA,							// フクシャ（鮮やかな赤紫）
		C_GAINSBORO,						// ゲインズボロ（明るい灰色）
		C_GHOSTWHITE,						// ゴーストホワイト（幽霊の白）
		C_GOLD,								// 金色
		C_GOLDENROD,						// ゴールデンロド（金紅花色）
		C_GRAY,								// 灰色
		C_GREEN,							// 緑
		C_GREENYELLOW,						// グリーンイエロー（黄緑）
		C_HONEYDEW,							// ハニーデュー（甘露蜜の白）
		C_HOTPINK,							// ホットピンク（鮮やかなピンク）
		C_INDIANRED,						// インディアンレッド（インド赤）
		C_INDIGO,							// インディゴ（藍色）
		C_IVORY,							// アイボリー（象牙色）
		C_KHAKI,							// カーキ
		C_LAVENDER,							// ラベンダー
		C_LAVENDERBLUSH,					// ラベンダーブラッシュ（薄いラベンダーピンク）
		C_LAWNGREEN,						// ローングリーン（芝生の緑）
		C_LEMONCHIFFON,						// レモンシフォン
		C_LIGHTBLUE,						// ライトブルー（薄い青）
		C_LIGHTCORAL,						// ライトコーラル（薄い珊瑚色）
		C_LIGHTCYAN,						// ライトシアン（薄い水色）
		C_LIGHTGOLDENRODYELLOW,				// ライトゴールデンロドイエロー（薄い金紅花黄）
		C_LIGHTGREEN,						// ライトグリーン（薄い緑）
		C_LIGHTGRAY,						// ライトグレー（薄い灰色）
		C_LIGHTPINK,						// ライトピンク（薄いピンク）
		C_LIGHTSALMON,						// ライトサーモン（薄い鮭色）
		C_LIGHTSEAGREEN,					// ライトシーグリーン（明るい海洋緑）
		C_LIGHTSKYBLUE,						// ライトスカイブルー（明るい空色）
		C_LIGHTSLATEGRAY,					// ライトスレートグレー（明るい石板色）
		C_LIGHTSTEELBLUE,					// ライトスチールブルー（明るい鋼色）
		C_LIGHTYELLOW,						// ライトイエロー（薄い黄色）
		C_LIME,								// ライム（鮮やかな黄緑）
		C_LIMEGREEN,						// ライムグリーン
		C_LINEN,							// リネン（亜麻色）
		C_MAGENTA,							// マゼンタ（赤紫）
		C_MAROON,							// マルーン（栗色）
		C_MEDIUMAQUAMARINE,					// ミディアムアクアマリン
		C_MEDIUMBLUE,						// ミディアムブルー
		C_MEDIUMORCHID,						// ミディアムオーキッド（中間の蘭色）
		C_MEDIUMPURPLE,						// ミディアムパープル（中間の紫）
		C_MEDIUMSEAGREEN,					// ミディアムシーグリーン（中間の海洋緑）
		C_MEDIUMSLATEBLUE,					// ミディアムスレートブルー（中間の石板青）
		C_MEDIUMSPRINGGREEN,				// ミディアムスプリンググリーン
		C_MEDIUMTURQUOISE,					// ミディアムターコイズ（中間のトルコ石色）
		C_MEDIUMVIOLETRED,					// ミディアムバイオレットレッド（中間の赤紫）
		C_MIDNIGHTBLUE,						// ミッドナイトブルー（夜が更けた真夜中の濃い青）
		C_MINTCREAM,						// ミントクリーム
		C_MISTYROSE,						// ミスティローズ（霧がかったバラ色）
		C_MOCCASIN,							// モカシン（淡い黄茶色）
		C_NAVAJOWHITE,						// ナバホホワイト
		C_NAVY,								// ネイビー（紺色）
		C_OLDLACE,							// オールドレース（くすんだ白）
		C_OLIVE,							// オリーブ
		C_OLIVEDRAB,						// オリーブドラブ（暗い黄緑）
		C_ORANGE,							// オレンジ
		C_ORANGERED,						// オレンジレッド（朱色）
		C_ORCHID,							// オーキッド（蘭色）
		C_PALEGOLDENROD,					// ペールゴールデンロド（淡い金紅花色）
		C_PALEGREEN,						// ペールグリーン（淡い緑）
		C_PALETURQUOISE,					// ペールターコイズ（淡いトルコ石色）
		C_PALEVIOLETRED,					// ペールバイオレットレッド（淡い赤紫）
		C_PAPAYAWHIP,						// パパイヤホイップ
		C_PEACHPUFF,						// ピーチパフ（桃のうぶ毛のような色）
		C_PERU,								// ペルー（ペルー茶色）
		C_PINK,								// ピンク
		C_PLUM,								// プラム（西洋すもも色）
		C_POWDERBLUE,						// パウダーブルー（おしろいの青）
		C_PURPLE,							// 紫
		C_RED,								// 赤
		C_ROSYBROWN,						// ロージーブラン（バラ色の茶色）
		C_ROYALBLUE,						// ロイヤルブルー（英国王室の青）
		C_SADDLEBROWN,						// サドルブラウン（革の鞍の茶色）
		C_SALMON,							// サーモン（鮭色）
		C_SANDYBROWN,						// サンディブラウン（砂茶色）
		C_SEAGREEN,							// シーグリーン（海洋緑）
		C_SEASHELL,							// シーシェル（貝殻の白）
		C_SIENNA,							// シエナ（黄褐色）
		C_SILVER,							// シルバー（銀色）
		C_SKYBLUE,							// スカイブルー（空色）
		C_SLATEBLUE,						// スレートブルー（石板の青）
		C_SLATEGRAY,						// スレートグレー（石板の灰色）
		C_SNOW,								// スノー（雪の白）
		C_SPRINGGREEN,						// スプリンググリーン（新緑の緑）
		C_STEELBLUE,						// スチールブルー（鋼色）
		C_TAN, 								// タン（日焼け色）
		C_TEAL,								// ティール（鴨の羽の青緑）
		C_THISTLE,							// シッスル（あざみ色）
		C_TOMATO,							// トマト（トマトの赤）
		C_TRANSPARENT,						// 透明（カラーなし）
		C_TURQUOISE,						// ターコイズ（トルコ石色）
		C_VIOLET,							// バイオレット（すみれ色）
		C_WHEAT,							// ウィート（小麦色）
		C_WHITE,							// 白
		C_WHITESMOKE,						// ホワイトスモーク（白い煙色）
		C_YELLOW,							// 黄色
		C_YELLOWGREEN,						// イエローグリーン（黄緑）
		C_MAX
	} COLOR;

	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	Color GetColor(const COLOR col);
	Color Lerp(const Color &start, const Color &end, const float t);
}
#endif