#!/usr/bin/env python3
"""Render a safe, concise GitHub Actions Job Summary for Clang AST checking."""
import argparse
import json
import os
from pathlib import Path


def safe(value):
    return str(value).replace('|','\\|').replace('`',"'").replace('\r',' ').replace('\n',' ')


def main():
    p=argparse.ArgumentParser()
    p.add_argument('--report',default='reports/semantic_naming.json')
    args=p.parse_args()
    path=Path(args.report)
    if not path.exists():
        message='## C++ Clang AST意味解析\n\n⚠️ 意味解析レポートがありません。直前のステップを確認してください。\n'
    else:
        report=json.loads(path.read_text(encoding='utf-8'))
        issues=report.get('issues',[])
        failures=report.get('failures',[])
        message=f'## C++ Clang AST意味解析\n\n翻訳単位: {report.get("translation_units",0)} / 違反: {len(issues)} / 解析失敗: {len(failures)}\n\n'
        if report.get('status') != 'complete':
            message+='\n⚠️ **解析は未完了です。違反0件として扱わないでください。**\n\n'
        if issues:
            message+='| 位置 | 違反 | 修正候補 |\n|---|---|---|\n'
            for issue in issues[:35]:
                message+=f'| `{safe(issue.get("file","?"))}:{safe(issue.get("line",0))}` | {safe(issue.get("rule",""))} | `{safe(issue.get("suggestion",""))}` |\n'
            if len(issues)>35:
                message+=f'\nほか {len(issues)-35} 件はJSONレポートを確認してください。\n'
        elif report.get('status') == 'complete':
            message+='命名違反はありません。\n'
        if failures:
            message+='\n### AST解析に失敗したファイル\n'
            for fail in failures[:15]:
                message+=f'- `{safe(fail.get("file","?"))}` : {safe(fail.get("message",""))[:200]}\n'
            message+='\n**注意：解析失敗のある状態を「違反0件」とみなさないでください。**\n'
    print(message)
    summary=os.getenv('GITHUB_STEP_SUMMARY')
    if summary:
        with open(summary,'a',encoding='utf-8') as f:
            f.write(message)

if __name__=='__main__': main()
