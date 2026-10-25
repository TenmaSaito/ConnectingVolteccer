"""Functional Clang JSON-AST / policy / report tests; no network required."""
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

TOOL = Path(__file__).resolve().parents[1] / 'tools' / 'check_semantic_naming.py'
POLICY = Path(__file__).resolve().parents[1] / 'naming_policy.json'
sys.path.insert(0, str(TOOL.parent))
from check_semantic_naming import expected_prefix, parse_ast


@unittest.skipUnless(shutil.which('clang++'), 'clang++ is required')
class ClangIntegrationTests(unittest.TestCase):
    SOURCE = '''\
using ItemCount = int;
struct GameData { int nLevel; bool bLive; };
enum class Axis { HORIZONTAL, badValue };
class Player {
  int score;
  float m_fSpeed;
  GameData m_data;
public:
  void Update(const int& nInput, float speed);
};
int badCount = 10;
constexpr int maxCount = 100;
ItemCount hp = 3;
const int& nRef = badCount;
int* pScore = &badCount;
int** ppScores = &pScore;
GameData object;
void Player::Update(const int& nInput, float speed) {}
'''

    def test_ast_type_alias_enum_parameters_member_and_constexpr(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            source = root / 'a.cpp'
            source.write_text(self.SOURCE, encoding='utf-8')
            run = subprocess.run(['clang++', '-std=c++20', '-fsyntax-only', '-Xclang', '-ast-dump=json', str(source)], capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stderr)
            result = parse_ast(json.loads(run.stdout), source, root, {'a.cpp':None}, [], json.loads(POLICY.read_text()))
            names = {x['symbol'] for x in result}
            for name in ('badValue', 'score', 'badCount', 'maxCount', 'hp', 'speed'):
                self.assertIn(name, names)
            for valid in ('nLevel', 'bLive', 'm_fSpeed', 'm_data', 'nRef', 'pScore', 'ppScores', 'object'):
                self.assertNotIn(valid, names)
            self.assertTrue(any(x['symbol'] == 'hp' and x['suggestion'] == 'nHp' for x in result))

    def test_constructor_destructor_operator_and_override_exempt(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td);source=root/'override.cpp'
            source.write_text('struct Base {virtual void foo()=0;};\nstruct Derived: Base { Derived(){} ~Derived(){} void foo() override {} bool operator==(const Derived&) const {return true;} };\n',encoding='utf-8')
            run=subprocess.run(['clang++','-std=c++20','-fsyntax-only','-Xclang','-ast-dump=json',str(source)],capture_output=True,text=True)
            self.assertEqual(run.returncode,0,run.stderr)
            results=parse_ast(json.loads(run.stdout),source,root,{'override.cpp':{2}},[],json.loads(POLICY.read_text()))
            self.assertFalse(any(x['symbol'] in ('foo','Derived','~Derived','operator==') for x in results),results)

    def test_error_report_missing_compdb_even_with_no_fail(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            report=root/'reports/semantic_naming.json'
            result=subprocess.run([sys.executable, str(TOOL), '--all', '--compdb', 'does-not-exist.json',
                                   '--policy', str(POLICY), '--report', str(report), '--no-fail'], cwd=root,
                                  capture_output=True,text=True)
            self.assertEqual(result.returncode,2,result.stdout+result.stderr)
            data=json.loads(report.read_text())
            self.assertEqual(data['status'],'failed')
            self.assertGreater(len(data['failures']),0)

    def test_full_cli_warn_and_strict_on_git_diff(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            def git(*args):
                return subprocess.check_output(['git', *args], cwd=root, text=True).strip()
            git('init', '-q');git('config','user.name','Test');git('config','user.email','test@example.com')
            source=root/'a.cpp';source.write_text('int nCount = 0;\n', encoding='utf-8')
            git('add','.');git('commit','-qm','base')
            base=git('rev-parse','HEAD')
            source.write_text('int nCount = 0;\nfloat invalid = 0.0f;\n',encoding='utf-8')
            git('add','.');git('commit','-qm','add')
            head=git('rev-parse','HEAD')
            (root/'compile_commands.json').write_text(json.dumps([{
               'directory':str(root),'file':str(source),
               'arguments':['clang++','-std=c++20','-c',str(source),'-o',str(root/'a.o')]
            }]),encoding='utf-8')
            args=[sys.executable,str(TOOL),'--base',base,'--head',head,'--compdb','compile_commands.json',
                  '--policy',str(POLICY),'--report','reports/semantic_naming.json']
            warn=subprocess.run(args+['--no-fail'],cwd=root,text=True,capture_output=True)
            self.assertEqual(warn.returncode,0,warn.stdout+warn.stderr)
            self.assertIn('::warning file=',warn.stdout)
            self.assertNotIn('nCount →',warn.stdout)
            strict=subprocess.run(args,cwd=root,text=True,capture_output=True)
            self.assertEqual(strict.returncode,1,strict.stdout+strict.stderr)
            data=json.loads((root/'reports/semantic_naming.json').read_text())
            self.assertEqual(data['status'],'complete')
            self.assertEqual(len(data['issues']),1)
            self.assertEqual(data['issues'][0]['suggestion'],'fInvalid')

    def test_invalid_git_revision_creates_failed_json(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            proc=subprocess.run([sys.executable,str(TOOL),'--base','bad','--head','bad',
                '--policy',str(POLICY)],cwd=root,capture_output=True,text=True)
            self.assertEqual(proc.returncode,2)
            self.assertEqual(json.loads((root/'reports/semantic_naming.json').read_text())['status'],'failed')


if __name__ == '__main__':
    unittest.main()
