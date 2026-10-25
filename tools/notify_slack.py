#!/usr/bin/env python3
"""Trusted-branch Slack notification only. Never execute downloaded PR artifacts."""
import io
import json
import os
import sys
import urllib.request
import zipfile


def esc(s: object) -> str:
    return str(s).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def fetch_json(url: str) -> dict:
    req = urllib.request.Request(url, headers={
        "Accept": "application/vnd.github+json",
        "Authorization": f"Bearer {os.environ['GH_TOKEN']}",
        "X-GitHub-Api-Version": "2022-11-28",
    })
    with urllib.request.urlopen(req, timeout=20) as r:
        return json.load(r)


def report_for(run: dict) -> dict | None:
    repo = os.environ["GITHUB_REPOSITORY"]
    api = os.environ.get("GITHUB_API_URL", "https://api.github.com")
    info = fetch_json(f"{api}/repos/{repo}/actions/runs/{run['id']}/artifacts?per_page=100")
    matching = [a for a in info.get("artifacts", []) if a.get("name") == "pr-quality-report" and not a.get("expired")]
    if not matching:
        return None
    archive_url = matching[0]["archive_download_url"]
    req = urllib.request.Request(archive_url, headers={
        "Authorization": f"Bearer {os.environ['GH_TOKEN']}",
        "Accept": "application/vnd.github+json",
    })
    with urllib.request.urlopen(req, timeout=30) as r:
        data = r.read(1_000_001)
    if len(data) > 1_000_000:
        raise ValueError("Report exceeds the 1 MB safety limit")
    with zipfile.ZipFile(io.BytesIO(data)) as z:
        reports = []
        for name in ('file_names.json', 'identifiers.json', 'pr_metadata.json'):
            if name in z.namelist():
                info = z.getinfo(name)
                if info.file_size > 100_000:
                    raise ValueError("Expanded report exceeds 100 KB limit")
                reports.append(json.loads(z.read(name)))
        return {"reports": reports}


def slack_post(message: str) -> None:
    url = os.environ.get("SLACK_WEBHOOK_URL", "")
    if not url:
        print("Slack通知用のSLACK_WEBHOOK_URLが未設定のため通知を省略しました。")
        return
    data = json.dumps({"text": message}, ensure_ascii=False).encode("utf-8")
    request = urllib.request.Request(url, data=data, headers={"Content-Type": "application/json"}, method="POST")
    with urllib.request.urlopen(request, timeout=15) as response:
        print("Slack HTTP status:", response.status)


def main() -> None:
    kind = sys.argv[1]
    with open(os.environ["GITHUB_EVENT_PATH"], encoding="utf-8") as f:
        event = json.load(f)
    if kind == "opened":
        pr = event["pull_request"]
        msg = (
            f":bell: *PR #{pr['number']} が作成/再オープンされました*\n"
            f"タイトル: {esc(pr['title'])}\n"
            f"<{pr['html_url']}|PRを開く>"
        )
    elif kind == "completed":
        run = event["workflow_run"]
        result = run.get("conclusion") or "unknown"
        status = {"success": "成功", "failure": "失敗", "cancelled": "キャンセル", "skipped": "スキップ"}.get(result, result)
        prs = run.get("pull_requests") or []
        pr_desc = f"PR #{prs[0]['number']}" if prs else "PR"
        icon = ":white_check_mark:" if result == "success" else ":warning:"
        msg = f"{icon} *{pr_desc} チェック完了: {esc(status)}*\n<{run['html_url']}|GitHub Actionsの結果>"
        try:
            report = report_for(run)
            if report is not None:
                for detail in report.get("reports", []):
                    if "pr_number" in detail:
                        if not prs and isinstance(detail.get("pr_number"), int):
                            pr_desc = f"PR #{detail['pr_number']}"
                        continue
                    items = detail.get("issues", [])
                    label = {'filename': 'ファイル名', 'identifiers': 'C++識別子'}.get(detail.get('check'), 'その他の検査')
                    msg += f"\n*{esc(label)}*: {len(items)} 件"
                    for item in items[:12]:
                        msg += f"\n• `{esc(item.get('file', '?'))}:{item.get('line', 1)}` {esc(item.get('rule', item.get('message', '')))} → `{esc(item.get('suggestion', '?'))}`"
                    if len(items) > 12:
                        msg += "\n残りはGitHub Actionsのレポートを参照"
                if not prs:
                    msg = msg.replace("*PR チェック完了:", f"*{esc(pr_desc)} チェック完了:", 1)
            else:
                msg += "\n詳細レポートなし（ワークフローの失敗など）"
        except Exception as exc:
            # Still deliver the run result; never treat artifact data as executable code.
            print("レポート取得に失敗しました:", exc)
            msg += "\n詳細レポートの取得に失敗"
    else:
        raise SystemExit("Usage: notify_slack.py opened|completed")
    slack_post(msg)


if __name__ == "__main__":
    main()
