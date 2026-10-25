#!/usr/bin/env python3
"""Send Japanese PR notifications to Discord from trusted default-branch code.

The downloaded workflow artifact is UNTRUSTED PR-derived data.  Never execute it,
import it, use artifact-supplied URLs or pass it to a shell.  Only read a small,
fixed list of JSON filenames and display sanitized text with mentions disabled.
"""

import json
import os
from pathlib import Path
import sys
import urllib.error
import urllib.request
from urllib.parse import urlparse

MAX_REPORT_BYTES = 100_000
MAX_MESSAGE_LENGTH = 1900
# Discord message flag: SUPPRESS_EMBEDS (1 << 2).
# Links remain clickable, but GitHub preview cards will not be shown.
SUPPRESS_EMBEDS = 4
REPORT_FILENAMES = ('file_names.json', 'identifiers.json', 'pr_metadata.json')


def plain(value: object, limit: int = 450) -> str:
    text = str(value).replace('\r', ' ').replace('\n', ' ').replace('`', '′')
    text = ''.join(ch for ch in text if ch.isprintable())
    return text.strip()[:limit]


def read_report(name: str, root: Path) -> dict | None:
    """Read only a known JSON filename within a trusted destination directory."""
    # upload-artifact@v4 with 'reports/*.json' strips the 'reports/' prefix.
    # Accommodate either layout to be resilient to future upload path changes.
    for candidate in (root / name, root / 'reports' / name):
        if candidate.is_symlink() or not candidate.is_file():
            continue
        if candidate.stat().st_size > MAX_REPORT_BYTES:
            raise ValueError(f'{name}: 100KBを超えるため安全上読み込みません')
        with candidate.open('rb') as handle:
            data = handle.read(MAX_REPORT_BYTES + 1)
        if len(data) > MAX_REPORT_BYTES:
            raise ValueError(f'{name}: 読み込みサイズが上限を超えました')
        content = json.loads(data.decode('utf-8-sig'))
        if not isinstance(content, dict):
            raise ValueError(f'{name}: JSONオブジェクトではありません')
        return content
    return None


def report_for(run: dict) -> dict:
    """Read artifacts downloaded by actions/download-artifact@v4 (no API calls)."""
    root = Path(os.environ.get('REPORT_DIR', 'downloaded-report'))
    result = {name: read_report(name, root) for name in REPORT_FILENAMES}
    missing = [name for name in ('file_names.json', 'identifiers.json') if result[name] is None]
    if missing:
        raise FileNotFoundError(
            f'{root} に検査レポート（{", ".join(missing)}）がありません。'
            '通知ジョブの「検査レポートを取得」ステップを確認してください'
        )
    metadata = result['pr_metadata.json']
    # Do not allow arbitrarily huge attacker-controlled lists in the Discord message.
    reports = [result[name] for name in ('file_names.json', 'identifiers.json') if result[name]]
    if metadata:
        reports.insert(0, metadata)
    semantic_root = Path(os.environ.get('SEMANTIC_REPORT_DIR', 'downloaded-semantic'))
    semantic = read_report('semantic_naming.json', semantic_root)
    semantic_expected = os.environ.get('SEMANTIC_EXPECTED', '').lower() == 'true'
    if semantic is None and semantic_expected:
        raise FileNotFoundError('Clang AST意味解析のレポートがありません。ビルドまたはAST実行の失敗を確認してください。')
    if semantic is not None:
        reports.append(semantic)
    return {'reports': reports}


def discord_post(message: str) -> None:
    url = os.environ.get('DISCORD_WEBHOOK_URL', '')
    if not url:
        raise SystemExit('GitHub ActionsのSecretsにDISCORD_WEBHOOK_URLが設定されていません。')
    parsed = urlparse(url)
    if (parsed.scheme != 'https'
            or parsed.hostname not in {'discord.com', 'discordapp.com', 'canary.discord.com'}
            or not parsed.path.startswith('/api/webhooks/')):
        raise SystemExit('Discord Webhook URLの形式が不正です。URLを再確認してください。')

    messages = []
    chunk = ''
    for original_line in message.splitlines():
        line = original_line[:MAX_MESSAGE_LENGTH]
        proposed = chunk + ('\n' if chunk else '') + line
        if len(proposed) > MAX_MESSAGE_LENGTH and chunk:
            messages.append(chunk)
            chunk = line
        else:
            chunk = proposed
    if chunk:
        messages.append(chunk)
    for part in messages:
        payload = json.dumps({
            'username': 'PR Review Bot',
            'content': part,
            'allowed_mentions': {'parse': []},
            'flags': SUPPRESS_EMBEDS,  # suppress GitHub link preview cards
        }, ensure_ascii=False).encode('utf-8')
        request = urllib.request.Request(
            url, data=payload,
            headers={'Content-Type': 'application/json', 'User-Agent': 'PRGuard/1.1'},
            method='POST',
        )
        try:
            with urllib.request.urlopen(request, timeout=15) as response:
                print('Discordへの送信成功: HTTP', response.status)
        except urllib.error.HTTPError as error:
            hint = {
                400: 'メッセージの形式を確認してください。',
                401: 'Webhookの認証情報を確認してください。',
                403: 'Webhookのアクセス権を確認してください。',
                404: 'Webhookが削除されていないか確認してください。',
                429: '送信制限です。少し待って再実行してください。',
            }.get(error.code, '送信に失敗しました。')
            raise SystemExit(f'Discord送信失敗: HTTP {error.code}。{hint}') from None
        except urllib.error.URLError:
            raise SystemExit('Discordへの接続に失敗しました。') from None


def build_completed_message(run: dict) -> tuple[str, bool]:
    result = run.get('conclusion') or 'unknown'
    status = {
        'success': '成功', 'failure': '失敗', 'cancelled': 'キャンセル',
        'skipped': 'スキップ', 'timed_out': 'タイムアウト', 'action_required': '要対応',
    }.get(result, plain(result))
    prs = run.get('pull_requests') or []
    pr_number = prs[0].get('number') if prs else None
    title = f'PR #{pr_number}' if pr_number else 'PR'
    icon = '✅' if result == 'success' else '⚠️'
    # The workflow link is generated by GitHub, not the untrusted artifact.
    run_url = run.get('html_url', '')
    if not str(run_url).startswith('https://github.com/'):
        run_url = ''
    lines = [f'{icon} **{title} 自動検査完了：{status}**']
    if run_url:
        lines.append(f'[GitHub Actionsの結果]({run_url})')
    try:
        report = report_for(run)
        count = 0
        semantic_ok = True
        for detail in report.get('reports', []):
            if 'pr_number' in detail:
                if isinstance(detail.get('pr_number'), int):
                    lines[0] = f'{icon} **PR #{detail["pr_number"]} 自動検査完了：{status}**'
                continue
            items = detail.get('issues', [])
            if not isinstance(items, list):
                raise ValueError('issuesは配列である必要があります')
            count += len(items)
            if detail.get('check') == 'semantic_naming':
                status = detail.get('status')
                failures = detail.get('failures', [])
                if status != 'complete' or not isinstance(failures, list) or failures:
                    semantic_ok = False
                    lines.append(f'⚠️ **Clang AST意味解析失敗（status={plain(status)}）**')
                    for failure in failures[:6] if isinstance(failures, list) else []:
                        if isinstance(failure, dict):
                            lines.append(f'• {plain(failure.get("file", "?"))}: {plain(failure.get("message", "?"), 180)}')
            label = {'filename': 'ファイル名', 'identifiers': 'C++識別子',
                     'semantic_naming': 'Clang AST意味解析'}.get(
                detail.get('check'), 'その他の検査'
            )
            lines.append(f'**{label}：{len(items)}件**')
            for item in items[:12]:
                if not isinstance(item, dict):
                    continue
                filename = plain(item.get('file', '?'))
                lineno = plain(item.get('line', 1))
                reason = plain(item.get('rule', item.get('message', '')))
                suggestion = plain(item.get('suggestion', '?'))
                # Identifier reports contain the original identifier in "symbol".
                # File-name reports have only "file", so use its basename instead.
                original = item.get('symbol')
                if not isinstance(original, str) or not original.strip():
                    original = Path(filename).name if detail.get('check') == 'filename' else '?'
                original = plain(original)
                lines.append(f'• `{filename}:{lineno}` `{original}` → `{suggestion}` （{reason}）')
            if len(items) > 12:
                lines.append('残りの違反はGitHub Actionsのレポートを参照')
        if count == 0 and semantic_ok:
            lines.append('命名規則違反は検出されませんでした。')
        return '\n'.join(lines), semantic_ok
    except (FileNotFoundError, ValueError, OSError, json.JSONDecodeError) as error:
        print('::error::Discord通知用の詳細レポート読込に失敗:', plain(error))
        lines.append('⚠️ **詳細レポートの取得・読み込みに失敗しました。**')
        lines.append('GitHub Actionsの通知ジョブ内「検査レポートを取得」を確認してください。')
        return '\n'.join(lines), False


def completed_message(run: dict) -> str:
    """Convenience helper for tests and callers that need only the message."""
    return build_completed_message(run)[0]


def main() -> None:
    if len(sys.argv) != 2 or sys.argv[1] not in ('opened', 'completed', 'test'):
        raise SystemExit('使い方: notify_discord.py opened|completed|test')
    if sys.argv[1] == 'test':
        repo = plain(os.environ.get('GITHUB_REPOSITORY', '(リポジトリ名不明)'))
        discord_post(f'✅ **GitHub ActionsからDiscordへのテスト通知成功！**\nリポジトリ：{repo}')
        return
    with open(os.environ['GITHUB_EVENT_PATH'], encoding='utf-8') as handle:
        event = json.load(handle)
    if sys.argv[1] == 'opened':
        if event.get('action') != 'opened':
            # Defensive: never notify reopened PR even if invoked manually.
            print('PRの新規作成ではないため、作成通知は送りません。')
            return
        pr = event['pull_request']
        msg = (f'🔔 **PR #{pr["number"]} が作成されました**\n'
               f'タイトル：{plain(pr["title"])}\n'
               f'[PRを開く]({pr["html_url"]})')
        discord_post(msg)
    else:
        msg, report_ok = build_completed_message(event['workflow_run'])
        discord_post(msg)
        # A missing/invalid report should not produce a green notification job.
        if not report_ok:
            raise SystemExit('詳細レポートを読み取れませんでした。通知ジョブのログを確認してください。')


if __name__ == '__main__':
    main()
