import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]/'tools'))
import notify_discord as discord


class SemanticDiscordTests(unittest.TestCase):
    def render(self, status='complete', semantic_present=True, issues=None):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td); quality=root/'quality';semantic=root/'semantic'
            quality.mkdir();semantic.mkdir()
            (quality/'file_names.json').write_text(json.dumps({'check':'filename','issues':[]}))
            (quality/'identifiers.json').write_text(json.dumps({'check':'identifiers','issues':[]}))
            if semantic_present:
                (semantic/'semantic_naming.json').write_text(json.dumps({
                    'check':'semantic_naming','status':status,'issues':issues or [],
                    'failures': [{'file':'x.cpp','message':'clang-cl failed'}] if status!='complete' else [],
                }))
            with patch.dict(discord.os.environ,{
                'REPORT_DIR':str(quality),'SEMANTIC_REPORT_DIR':str(semantic),
                'SEMANTIC_EXPECTED':'true'}):
                return discord.build_completed_message({'conclusion':'success'})

    def test_complete_report(self):
        msg,ok=self.render(issues=[{'file':'foo.cpp','line':2,'symbol':'invalid','suggestion':'nInvalid','rule':'型'}])
        self.assertTrue(ok)
        self.assertIn('Clang AST意味解析',msg)
        self.assertIn('nInvalid',msg)

    def test_failed_ast_is_not_reported_as_clean(self):
        msg,ok=self.render(status='incomplete')
        self.assertFalse(ok)
        self.assertIn('AST意味解析失敗',msg)
        self.assertNotIn('違反は検出されませんでした',msg)

    def test_missing_semantic_artifact_is_error(self):
        msg,ok=self.render(semantic_present=False)
        self.assertFalse(ok)
        self.assertIn('読み込みに失敗',msg)

if __name__=='__main__':unittest.main()
