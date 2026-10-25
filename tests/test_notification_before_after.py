import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from test_notify_discord import mod


class BeforeAfterDiscordTests(unittest.TestCase):
    def render(self, check, issues):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / 'file_names.json').write_text(json.dumps({'check':'filename','issues':issues if check=='filename' else []}), encoding='utf-8')
            (root / 'identifiers.json').write_text(json.dumps({'check':'identifiers','issues':issues if check=='identifiers' else []}), encoding='utf-8')
            with patch.dict(mod.os.environ, {'REPORT_DIR': temp}):
                return mod.build_completed_message({'conclusion':'failure','html_url':'https://github.com/a/b/actions/runs/123'})

    def test_identifiers_keep_original_and_suggestion(self):
        msg, ok = self.render('identifiers', [
            {'file':'main.cpp','line':37,'symbol':'i','suggestion':'nI','rule':'int の変数は n+PascalCase'},
            {'file':'main.cpp','line':38,'symbol':'a','suggestion':'fA','rule':'float の変数は f+PascalCase'},
        ])
        self.assertTrue(ok)
        self.assertIn('`i` → `nI`', msg)
        self.assertIn('`a` → `fA`', msg)
        self.assertIn('int の変数は n+PascalCase', msg)

    def test_filename_before_and_after(self):
        msg, ok = self.render('filename', [{'file':'src/Bad-File.cpp','line':1,'suggestion':'badFile.cpp','message':'不正なファイル名'}])
        self.assertTrue(ok)
        self.assertIn('`Bad-File.cpp` → `badFile.cpp`', msg)
        self.assertIn('`src/Bad-File.cpp:1`', msg)

    def test_missing_symbol_degrades_gracefully(self):
        msg, ok = self.render('identifiers', [{'file':'x.cpp','line':9,'suggestion':'nCount','rule':'不正'}])
        self.assertTrue(ok)
        self.assertIn('`?` → `nCount`', msg)
