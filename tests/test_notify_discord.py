import importlib.util
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

SCRIPT = Path(__file__).resolve().parents[1] / 'tools' / 'notify_discord.py'
spec = importlib.util.spec_from_file_location('notify_discord', SCRIPT)
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)


def reports(root):
    (root / 'file_names.json').write_text(json.dumps({
        'check': 'filename', 'issues': [{
            'file': 'BadName.cpp', 'line': 1,
            'rule': 'ファイル名の形式', 'suggestion': 'badName.cpp',
        }]
    }), encoding='utf-8')
    (root / 'identifiers.json').write_text(json.dumps({
        'check': 'identifiers', 'issues': [{
            'file': 'main.cpp', 'line': 37, 'rule': 'int の変数は n+PascalCase',
            'suggestion': 'nCount',
        }]
    }), encoding='utf-8')
    (root / 'pr_metadata.json').write_text(json.dumps({
        'pr_number': 4, 'head_sha': 'deadbeef',
    }), encoding='utf-8')


class TestDiscordNotify(unittest.TestCase):
    def test_report_from_downloaded_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            reports(Path(tmp))
            with patch.dict(os.environ, {'REPORT_DIR': tmp}):
                msg, ok = mod.build_completed_message({
                    'id': 4, 'conclusion': 'failure', 'html_url': 'https://github.com/a/b/actions/runs/4',
                    'pull_requests': [],
                })
            self.assertTrue(ok)
            self.assertIn('PR #4', msg)
            self.assertIn('main.cpp:37', msg)
            self.assertIn('nCount', msg)
            self.assertIn('badName.cpp', msg)

    def test_report_nested_reports_directory(self):
        with tempfile.TemporaryDirectory() as tmp:
            folder = Path(tmp) / 'reports'
            folder.mkdir()
            reports(folder)
            with patch.dict(os.environ, {'REPORT_DIR': tmp}):
                data = mod.report_for({'id': 4})
            self.assertEqual(len(data['reports']), 3)

    def test_missing_report_is_not_silent_success(self):
        with tempfile.TemporaryDirectory() as tmp:
            with patch.dict(os.environ, {'REPORT_DIR': tmp}):
                msg, ok = mod.build_completed_message({
                    'id': 4, 'conclusion': 'failure', 'html_url': 'https://github.com/a/b/actions/runs/4',
                    'pull_requests': [],
                })
            self.assertFalse(ok)
            self.assertIn('詳細レポートの取得・読み込みに失敗', msg)

    def test_webhook_mentions_disabled(self):
        payloads = []
        class Response:
            status = 204
            def __enter__(self): return self
            def __exit__(self, *_args): return None
        def capture(request, timeout):
            payloads.append(json.loads(request.data))
            return Response()
        with patch.dict(os.environ, {'DISCORD_WEBHOOK_URL': 'https://discord.com/api/webhooks/test'}):
            with patch.object(mod.urllib.request, 'urlopen', side_effect=capture):
                mod.discord_post('通知 @everyone\n' + 'a' * 1400 + '\n' + 'b' * 1400)
        self.assertGreater(len(payloads), 1)
        for payload in payloads:
            self.assertEqual(payload['allowed_mentions'], {'parse': []})
            self.assertEqual(payload['flags'], 4)  # All previews are suppressed
            self.assertLessEqual(len(payload['content']), 2000)

    def test_github_link_is_clickable_without_embed(self):
        payloads = []
        class Response:
            status = 204
            def __enter__(self): return self
            def __exit__(self, *_args): return None
        def capture(request, timeout):
            payloads.append(json.loads(request.data))
            return Response()
        with patch.dict(os.environ, {'DISCORD_WEBHOOK_URL': 'https://discord.com/api/webhooks/test'}):
            with patch.object(mod.urllib.request, 'urlopen', side_effect=capture):
                mod.discord_post('GitHub Actionsの結果: [PR #4](https://github.com/a/b/pull/4)')
        self.assertEqual(len(payloads), 1)
        self.assertEqual(payloads[0]['flags'], mod.SUPPRESS_EMBEDS)
        self.assertIn('https://github.com/a/b/pull/4', payloads[0]['content'])

    def test_reopened_action_sends_nothing(self):
        event = {'action': 'reopened', 'pull_request': {
            'number': 4, 'title': 'test', 'html_url': 'https://github.com/a/b/pull/4'
        }}
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'event.json'
            path.write_text(json.dumps(event), encoding='utf-8')
            with patch.dict(os.environ, {'GITHUB_EVENT_PATH': str(path)}):
                with patch.object(mod, 'discord_post') as post:
                    with patch.object(mod.sys, 'argv', ['x', 'opened']):
                        mod.main()
                    post.assert_not_called()

    def test_opened_action_sends_message(self):
        event = {'action': 'opened', 'pull_request': {
            'number': 4, 'title': '変数名修正', 'html_url': 'https://github.com/a/b/pull/4'
        }}
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'event.json'
            path.write_text(json.dumps(event), encoding='utf-8')
            with patch.dict(os.environ, {'GITHUB_EVENT_PATH': str(path)}):
                with patch.object(mod, 'discord_post') as post:
                    with patch.object(mod.sys, 'argv', ['x', 'opened']):
                        mod.main()
            self.assertIn('PR #4 が作成されました', post.call_args.args[0])
            self.assertNotIn('再オープン', post.call_args.args[0])

    def test_missing_webhook_is_error(self):
        with patch.dict(os.environ, {'DISCORD_WEBHOOK_URL': ''}):
            with self.assertRaises(SystemExit) as error:
                mod.discord_post('test')
        self.assertIn('DISCORD_WEBHOOK_URL', str(error.exception))

    def test_test_mode_needs_no_event(self):
        with patch.dict(os.environ, {'GITHUB_REPOSITORY': 'test-org/test-repo'}, clear=True):
            with patch.object(mod, 'discord_post') as post:
                with patch.object(mod.sys, 'argv', ['x', 'test']):
                    mod.main()
        self.assertIn('テスト通知', post.call_args.args[0])

    def test_invalid_webhook_url_rejected(self):
        with patch.dict(os.environ, {'DISCORD_WEBHOOK_URL': 'https://evil.example/api/webhooks/test'}):
            with self.assertRaises(SystemExit):
                mod.discord_post('test')

    def test_json_array_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / 'identifiers.json').write_text('[]', encoding='utf-8')
            with patch.dict(os.environ, {'REPORT_DIR': tmp}):
                msg, ok = mod.build_completed_message({'conclusion': 'success'})
            self.assertFalse(ok)
            self.assertIn('読み込みに失敗', msg)


if __name__ == '__main__':
    unittest.main()
