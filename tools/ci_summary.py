#!/usr/bin/env python3
"""Render on-GitHub Actions summary using the checker JSON reports."""
import json
import os
from pathlib import Path

summary = ["## PR Guard: 命名規則チェック（ファイル名・簡易識別子）", ""]
for file, title in [("file_names.json", "ファイル名"), ("identifiers.json", "C++識別子")]:
    path = Path("reports") / file
    if not path.is_file():
        summary.extend([f"### {title}", "検査結果がありません（チェッカーに失敗した可能性があります）", ""])
        continue
    report = json.loads(path.read_text(encoding="utf-8"))
    issues = report.get("issues", [])
    summary.extend([f"### {title}: {len(issues)} 件", ""])
    if issues:
        summary.extend(["| ファイル:行 | 違反内容 | 修正案 |", "|---|---|---|"])
        for item in issues[:30]:
            def safe(value):
                return str(value).replace("|", "\\|").replace("\n", " ").replace("\r", " ").replace("`", "'")
            summary.append(f"| `{safe(item.get('file', '?'))}:{safe(item.get('line', '?'))}` | "
                           f"{safe(item.get('rule', item.get('message', '')))} | "
                           f"`{safe(item.get('suggestion', ''))}` |")
        if len(issues) > 30:
            summary.append(f"その他 {len(issues)-30} 件はJSONレポートを確認してください。")
        summary.append("")
    else:
        summary.extend(["違反は見つかりませんでした。", ""])
summary.append("注意: 識別子チェックは簡易的なパターン検査です。Clang ASTの検査結果はビルドジョブのSummaryとsemantic_naming.jsonを参照してください。")
text = "\n".join(summary) + "\n"
print(text)
path = os.environ.get("GITHUB_STEP_SUMMARY")
if path:
    with open(path, "a", encoding="utf-8") as f:
        f.write(text)
