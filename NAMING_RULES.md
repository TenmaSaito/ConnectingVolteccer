# cpp_TKGベース 命名規則（初版）

## 対象と方針
- `cpp_TKG.zip` の `.h` 52ファイルと `.cpp` 48ファイルを参考に作成。現行コードには揺れがあるため、**新規・変更箇所を先に検査**する。
- このファイルが仕様。`naming_policy.json` は自作ASTチェッカー用のルール案。`.clang-tidy` は組み込みで対応する部分だけの暫定設定。
- レビュー結果は `場所 / 名前 / 宣言型 / 違反内容 / 改善候補 / 自動修正可否` を出す。

## 変数の基本ルール
| 型 | 接頭辞 | 良い例 |
|---|---|---|
| int | n | `nCount` |
| float | f | `fSpeed` |
| double | d | `dTime` |
| bool | b | `bActive` |
| char | c | `cKey` |
| UINT | u | `uIndex` |
| DWORD | dw | `dwUsage` |
| size_t | n | `nSize` |
| std::string | s | `sName` |
| std::vector | v | `vObjects` |
| std::array | a | `aScores` |
| std::deque | d | `dBuffers` |
| std::list | l | `lObjects` |
| pointer | p | `pObject` |
| pointer-to-pointer | pp | `ppData` |

- **ポインタは `p` を優先**：`int* pCount`。`npCount` とはしない。
- **スマートポインタも `p`**：`std::unique_ptr<CObject> pObject`。
- クラスのフィールドは `m_` を付ける：`m_nScore`, `m_pPlayer`, `m_vObjects`。
- `struct` の公開フィールドは `m_` 不要：`int nCount`, `Vector3 pos`。
- class/struct/enum 型の**オブジェクト名は型接頭辞なし・小文字開始OK**：`CPlayer player`, `Vector3 pos`, `Axis axis`。
- 型宣言名も `CObject`, `Player`, `player`, `Setting`, `setting` を許容し、C/S/E接頭辞を必須にしない（新規型は空白・記号・アンダースコア乱用を避ける）。
- `const T&` は通常変数と同じ：`const int& nCount`, `const Vector3& pos`, `const CPlayer& player`。
- 非const参照は旧コードに `rVec` や `vec` の両方があるため、暫定的に `r` を許容し強制しない。
- `std::string_view` も旧コードに `message`, `path`, `sFilename` が混在するため暫定的に接頭辞無しまたは `s` を許容。

## 本当の定数と `const` の区別
- **`constexpr` として宣言された定数は `UPPER_CASE`**：`constexpr int MAX_COUNT = 10;`。
- **一般の `const` は通常変数**：`const int nCount = GetCount();`、`const float& fSpeed`。
- `const int nCount = 10;` は `constexpr` にできても、**普通のconst変数である限り**一律にconstexprを強制しない。
- `const int MAX_COUNT = 10;` のように名前で定数を意図している場合、可能なら `constexpr int MAX_COUNT = 10;` にするよう提案。
- 非リテラル型などのため `constexpr` 化できない、**意図的な定数だけ** `inline const` などと `UPPER_CASE` の例外を許す。対象は許可リストで明示し、`const` 全般は例外にしない。
- 既存例：`inline const Vector3 VECTOR3_NULL = ...`。`Vector3` が非リテラル型なら `constexpr` にできない。型とコンパイラ条件を検証すること。
- `enum` の列挙子、マクロもデフォルトは `UPPER_CASE`。

## 型・関数・ファイル
- 関数・メソッド：`PascalCase`（`GetScore`, `SetPosition` 等）。`main`, `WinMain`, 演算子、外部APIによるオーバーライド等は除外。
- 型名：既存の `CPlayer`, `CObject`, `Axis`, `Setting` をそのまま許容。接頭辞なし・小文字開始も許容。
- ファイル名：ZIPでは `camera.cpp`, `objectX.cpp`, `objectXAfterImage.cpp`, `tcp_client.h` 等が混在。新規コードでは lowerCamelCase または snake_case を許容し、PascalCaseへ一斉改名しない。
- 含めるのはユーザー作成のソースコード中心。`.vs`, `Debug`, `Release`, `.git`, 外部ライブラリ、生成物は除外。

## 判定例
```cpp
int nCount = 10;                  // OK
const int nCount2 = GetCount();   // OK
const int& nInput = nCount;       // OK
float fSpeed = 1.0f;              // OK
int* pCount = &nCount;            // OK
constexpr int MAX_COUNT = 10;    // OK
const int MAX_SCORE = 500;       // 定数として意図→constexprへの変更提案
CPlayer player;                   // OK: class型変数
const CPlayer& playerRef = player;// OK: const参照でも型接頭辞不要
std::vector<CPlayer> vPlayers;    // OK
```

## チェッカー実装
1. GitHub Actions でPRの変更ファイルを取得。
2. `.clang-tidy` で単純な命名（関数、定数、マクロ、列挙子）をチェック。
3. `clang AST` / `libTooling` で型・const/constexpr・参照・コンテナを判別して `naming_policy.json` に照らし合わせる。型情報が必要なので正規表現だけで完結させない。
4. `git diff` から新規/変更宣言だけ集約し、診断と安全な修正候補をDiscordへ送信。
5. 既存の違反を一括修正せず、導入直後は警告。安定後に新規違反を必須チェックとする。

> 注意: まだ実装済みの完全な検査器ではありません。`.clang-tidy` の設定はclang-tidyを実行できるC++コンパイル環境で検証する必要があります。
