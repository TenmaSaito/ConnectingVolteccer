//==================================================================================
// 
// Colorの色関連をまとめたソースファイル [color.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "color.h"
#include <DirectXMath.h>
#include <DirectXColors.h>

//**********************************************************************************
// *** プロトタイプ宣言 ***
//**********************************************************************************
const DirectX::XMVECTORF32 *FindXMVECTORF32Color(const Colors::COLOR color);

//==================================================================================
// --- 色取得処理 ---
//==================================================================================
Color Colors::GetColor(const COLOR color)
{ // 色を検索
	const DirectX::XMVECTORF32 *pColor = FindXMVECTORF32Color(color);

	// 見つかった色を返す
	return Color(pColor->f);
}

//==================================================================================
// --- 色の線形補間処理 ---
//==================================================================================
Color Colors::Lerp(const Color &start, const Color &end, const float t)
{
	Color col;		// 線形補間後の色

	// 色の差分を求める
	col = end - start;

	// 線形補間後の色を返す
	return start + (col * t);
}

//==================================================================================
// --- 色の検索処理 ---
//==================================================================================
const DirectX::XMVECTORF32 *FindXMVECTORF32Color(const Colors::COLOR color)
{
	static const DirectX::XMVECTORF32 *apColor[] =		// 各色へのポインタ
	{
		&DirectX::Colors::AliceBlue            ,	// アリスブルー（薄い水色）		
		&DirectX::Colors::AntiqueWhite         ,	// アンティークホワイト（くすんだ真珠色）
		&DirectX::Colors::Aqua                 ,	// アクア（水色）
		&DirectX::Colors::Aquamarine           ,	// アクアマリン（藍玉色）
		&DirectX::Colors::Azure                ,	// アジュール（薄い空色）
		&DirectX::Colors::Beige                ,	// ベージュ
		&DirectX::Colors::Bisque               ,	// ビスキー（薄い茶色）
		&DirectX::Colors::Black                ,	// 黒
		&DirectX::Colors::BlanchedAlmond       ,	// ブランシェドアマンド（薄い杏色）
		&DirectX::Colors::Blue                 ,	// 青
		&DirectX::Colors::BlueViolet           ,	// ブルーバイオレット（青紫）
		&DirectX::Colors::Brown                ,	// 茶色
		&DirectX::Colors::BurlyWood            ,	// バーリーウッド（濃いベージュ）
		&DirectX::Colors::CadetBlue            ,	// カデットブルー（くすんだ青）
		&DirectX::Colors::Chartreuse           ,	// シャルトルーズ（黄緑）
		&DirectX::Colors::Chocolate            ,	// チョコレート
		&DirectX::Colors::Coral                ,	// コーラル（珊瑚色）
		&DirectX::Colors::CornflowerBlue       ,	// コーンフラワーブルー（矢車菊の青）
		&DirectX::Colors::Cornsilk             ,	// コーンシルク（トウモロコシのひげのようなくすんだ白）
		&DirectX::Colors::Crimson              ,	// クリムゾン（深紅）
		&DirectX::Colors::Cyan                 ,	// シアン（水色）
		&DirectX::Colors::DarkBlue             ,	// ダークブルー（濃い青）
		&DirectX::Colors::DarkCyan             ,	// ダークシアン（濃い水色）
		&DirectX::Colors::DarkGoldenrod        ,	// ダークゴールデンロド（濃い金紅花色）
		&DirectX::Colors::DarkGray             ,	// ダークグレー（深灰色）
		&DirectX::Colors::DarkGreen            ,	// ダークグリーン（深緑）
		&DirectX::Colors::DarkKhaki            ,	// ダークカーキ
		&DirectX::Colors::DarkMagenta          ,	// ダークマゼンタ（濃い赤紫）
		&DirectX::Colors::DarkOliveGreen       ,	// ダークオリーブグリーン（暗オリーブ色）
		&DirectX::Colors::DarkOrange           ,	// ダークオレンジ
		&DirectX::Colors::DarkOrchid           ,	// ダークオーキッド（濃い蘭色）
		&DirectX::Colors::DarkRed              ,	// ダークレッド（濃い赤）
		&DirectX::Colors::DarkSalmon           ,	// ダークサーモン
		&DirectX::Colors::DarkSeaGreen         ,	// ダークシーグリーン（くすんだ薄緑）
		&DirectX::Colors::DarkSlateBlue        ,	// ダークスレートブルー（暗青灰色）
		&DirectX::Colors::DarkSlateGray        ,	// ダークスレートグレー（暗石板色）
		&DirectX::Colors::DarkTurquoise        ,	// ダークターコイズ（濃いトルコ石色）
		&DirectX::Colors::DarkViolet           ,	// ダークバイオレット（濃い紫）
		&DirectX::Colors::DeepPink             ,	// ディープピンク
		&DirectX::Colors::DeepSkyBlue          ,	// ディープスカイブルー
		&DirectX::Colors::DimGray              ,	// ディムグレー（薄暗い灰色）
		&DirectX::Colors::DodgerBlue           ,	// ドジャーブルー（明るい青）
		&DirectX::Colors::Firebrick            ,	// ファイアブリック（煉瓦色）
		&DirectX::Colors::FloralWhite          ,	// フローラルホワイト（花の白）
		&DirectX::Colors::ForestGreen          ,	// フォレストグリーン（深い森の緑）
		&DirectX::Colors::Fuchsia              ,	// フクシャ（鮮やかな赤紫）
		&DirectX::Colors::Gainsboro            ,	// ゲインズボロ（明るい灰色）
		&DirectX::Colors::GhostWhite           ,	// ゴーストホワイト（幽霊の白）
		&DirectX::Colors::Gold                 ,	// 金色
		&DirectX::Colors::Goldenrod            ,	// ゴールデンロド（金紅花色）
		&DirectX::Colors::Gray                 ,	// 灰色
		&DirectX::Colors::Green                ,	// 緑
		&DirectX::Colors::GreenYellow          ,	// グリーンイエロー（黄緑）
		&DirectX::Colors::Honeydew             ,	// ハニーデュー（甘露蜜の白）
		&DirectX::Colors::HotPink              ,	// ホットピンク（鮮やかなピンク）
		&DirectX::Colors::IndianRed            ,	// インディアンレッド（インド赤）
		&DirectX::Colors::Indigo               ,	// インディゴ（藍色）
		&DirectX::Colors::Ivory                ,	// アイボリー（象牙色）
		&DirectX::Colors::Khaki                ,	// カーキ
		&DirectX::Colors::Lavender             ,	// ラベンダー
		&DirectX::Colors::LavenderBlush        ,	// ラベンダーブラッシュ（薄いラベンダーピンク）
		&DirectX::Colors::LawnGreen            ,	// ローングリーン（芝生の緑）
		&DirectX::Colors::LemonChiffon         ,	// レモンシフォン
		&DirectX::Colors::LightBlue            ,	// ライトブルー（薄い青）
		&DirectX::Colors::LightCoral           ,	// ライトコーラル（薄い珊瑚色）
		&DirectX::Colors::LightCyan            ,	// ライトシアン（薄い水色）
		&DirectX::Colors::LightGoldenrodYellow ,	// ライトゴールデンロドイエロー（薄い金紅花黄）
		&DirectX::Colors::LightGreen           ,	// ライトグリーン（薄い緑）
		&DirectX::Colors::LightGray            ,	// ライトグレー（薄い灰色）
		&DirectX::Colors::LightPink            ,	// ライトピンク（薄いピンク）
		&DirectX::Colors::LightSalmon          ,	// ライトサーモン（薄い鮭色）
		&DirectX::Colors::LightSeaGreen        ,	// ライトシーグリーン（明るい海洋緑）
		&DirectX::Colors::LightSkyBlue         ,	// ライトスカイブルー（明るい空色）
		&DirectX::Colors::LightSlateGray       ,	// ライトスレートグレー（明るい石板色）
		&DirectX::Colors::LightSteelBlue       ,	// ライトスチールブルー（明るい鋼色）
		&DirectX::Colors::LightYellow          ,	// ライトイエロー（薄い黄色）
		&DirectX::Colors::Lime                 ,	// ライム（鮮やかな黄緑）
		&DirectX::Colors::LimeGreen            ,	// ライムグリーン
		&DirectX::Colors::Linen                ,	// リネン（亜麻色）
		&DirectX::Colors::Magenta              ,	// マゼンタ（赤紫）
		&DirectX::Colors::Maroon               ,	// マルーン（栗色）
		&DirectX::Colors::MediumAquamarine     ,	// ミディアムアクアマリン
		&DirectX::Colors::MediumBlue           ,	// ミディアムブルー
		&DirectX::Colors::MediumOrchid         ,	// ミディアムオーキッド（中間の蘭色）
		&DirectX::Colors::MediumPurple         ,	// ミディアムパープル（中間の紫）
		&DirectX::Colors::MediumSeaGreen       ,	// ミディアムシーグリーン（中間の海洋緑）
		&DirectX::Colors::MediumSlateBlue      ,	// ミディアムスレートブルー（中間の石板青）
		&DirectX::Colors::MediumSpringGreen    ,	// ミディアムスプリンググリーン
		&DirectX::Colors::MediumTurquoise      ,	// ミディアムターコイズ（中間のトルコ石色）
		&DirectX::Colors::MediumVioletRed      ,	// ミディアムバイオレットレッド（中間の赤紫）
		&DirectX::Colors::MidnightBlue         ,	// ミッドナイトブルー（夜が更けた真夜中の濃い青）
		&DirectX::Colors::MintCream            ,	// ミントクリーム
		&DirectX::Colors::MistyRose            ,	// ミスティローズ（霧がかったバラ色）
		&DirectX::Colors::Moccasin             ,	// モカシン（淡い黄茶色）
		&DirectX::Colors::NavajoWhite          ,	// ナバホホワイト
		&DirectX::Colors::Navy                 ,	// ネイビー（紺色）
		&DirectX::Colors::OldLace              ,	// オールドレース（くすんだ白）
		&DirectX::Colors::Olive                ,	// オリーブ
		&DirectX::Colors::OliveDrab            ,	// オリーブドラブ（暗い黄緑）
		&DirectX::Colors::Orange               ,	// オレンジ
		&DirectX::Colors::OrangeRed            ,	// オレンジレッド（朱色）
		&DirectX::Colors::Orchid               ,	// オーキッド（蘭色）
		&DirectX::Colors::PaleGoldenrod        ,	// ペールゴールデンロド（淡い金紅花色）
		&DirectX::Colors::PaleGreen            ,	// ペールグリーン（淡い緑）
		&DirectX::Colors::PaleTurquoise        ,	// ペールターコイズ（淡いトルコ石色）
		&DirectX::Colors::PaleVioletRed        ,	// ペールバイオレットレッド（淡い赤紫）
		&DirectX::Colors::PapayaWhip           ,	// パパイヤホイップ
		&DirectX::Colors::PeachPuff            ,	// ピーチパフ（桃のうぶ毛のような色）
		&DirectX::Colors::Peru                 ,	// ペルー（ペルー茶色）
		&DirectX::Colors::Pink                 ,	// ピンク
		&DirectX::Colors::Plum                 ,	// プラム（西洋すもも色）
		&DirectX::Colors::PowderBlue           ,	// パウダーブルー（おしろいの青）
		&DirectX::Colors::Purple               ,	// 紫
		&DirectX::Colors::Red                  ,	// 赤
		&DirectX::Colors::RosyBrown            ,	// ロージーブラン（バラ色の茶色）
		&DirectX::Colors::RoyalBlue            ,	// ロイヤルブルー（英国王室の青）
		&DirectX::Colors::SaddleBrown          ,	// サドルブラウン（革の鞍の茶色）
		&DirectX::Colors::Salmon               ,	// サーモン（鮭色）
		&DirectX::Colors::SandyBrown           ,	// サンディブラウン（砂茶色）
		&DirectX::Colors::SeaGreen             ,	// シーグリーン（海洋緑）
		&DirectX::Colors::SeaShell             ,	// シーシェル（貝殻の白）
		&DirectX::Colors::Sienna               ,	// シエナ（黄褐色）
		&DirectX::Colors::Silver               ,	// シルバー（銀色）
		&DirectX::Colors::SkyBlue              ,	// スカイブルー（空色）
		&DirectX::Colors::SlateBlue            ,	// スレートブルー（石板の青）
		&DirectX::Colors::SlateGray            ,	// スレートグレー（石板の灰色）
		&DirectX::Colors::Snow                 ,	// スノー（雪の白）
		&DirectX::Colors::SpringGreen          ,	// スプリンググリーン（新緑の緑）
		&DirectX::Colors::SteelBlue            ,	// スチールブルー（鋼色）
		&DirectX::Colors::Tan                  ,	// タン（日焼け色）
		&DirectX::Colors::Teal                 ,	// ティール（鴨の羽の青緑）
		&DirectX::Colors::Thistle              ,	// シッスル（あざみ色）
		&DirectX::Colors::Tomato               ,	// トマト（トマトの赤）
		&DirectX::Colors::Transparent          ,	// 透明（カラーなし）
		&DirectX::Colors::Turquoise            ,	// ターコイズ（トルコ石色）
		&DirectX::Colors::Violet               ,	// バイオレット（すみれ色）
		&DirectX::Colors::Wheat                ,	// ウィート（小麦色）
		&DirectX::Colors::White                ,	// 白
		&DirectX::Colors::WhiteSmoke           ,	// ホワイトスモーク（白い煙色）
		&DirectX::Colors::Yellow               ,	// 黄色
		&DirectX::Colors::YellowGreen          ,	// イエローグリーン（黄緑）
	};
	
	// 指定された色へのポインタを返す
	return apColor[color];
}