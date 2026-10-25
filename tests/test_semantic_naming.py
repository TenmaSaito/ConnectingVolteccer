"""Run: python -m unittest discover -s tests -v (from the package root)."""
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from check_semantic_naming import expected_prefix, propose, parse_ast, parse_command
from msbuild_tlog_to_compdb import read_tlog

ROOT=Path(__file__).resolve().parents[1]

class SemanticTests(unittest.TestCase):
    def test_type_resolution(self):
        self.assertEqual(expected_prefix('int'), 'n')
        self.assertEqual(expected_prefix('const float &'), 'f')
        self.assertEqual(expected_prefix('DWORD'), 'dw')
        self.assertEqual(expected_prefix('UINT'), 'u')
        self.assertEqual(expected_prefix('MyStruct *'), 'p')
        self.assertEqual(expected_prefix('MyStruct **'), 'pp')
        self.assertEqual(expected_prefix('std::vector<int>'), 'v')
        self.assertEqual(expected_prefix('std::unique_ptr<Foo>'), 'p')
        self.assertIsNone(expected_prefix('MyStruct'))
        self.assertEqual(propose('nHealth','n',True),'m_nHealth')

    def test_clang_ast_headers(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            (root/'header.h').write_text('class CBad { public: int nHp; void foo(); };\n')
            (root/'test.cpp').write_text('#include "header.h"\nvoid CBad::foo() {}\n')
            cmd=['clang++','-std=c++20','-fsyntax-only','-Xclang','-ast-dump=json',str(root/'test.cpp')]
            r=subprocess.run(cmd,capture_output=True,text=True,cwd=td)
            self.assertEqual(r.returncode,0,r.stderr)
            result=parse_ast(json.loads(r.stdout),root/'test.cpp',root,
                             {'header.h':None,'test.cpp':None},[])
            names={(x['file'],x['symbol']) for x in result}
            self.assertIn(('header.h','nHp'),names)
            self.assertIn(('header.h','foo'),names)
            self.assertIn(('test.cpp','foo'),names)  # out-of-class definition is independently checked

    def test_diff_filter(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            s='int good = 1;\nint wrong = 0;\n'
            (root/'test.cpp').write_text(s)
            r=subprocess.run(['clang++','-fsyntax-only','-Xclang','-ast-dump=json',str(root/'test.cpp')],capture_output=True,text=True)
            self.assertEqual(r.returncode,0,r.stderr)
            issues=parse_ast(json.loads(r.stdout),root/'test.cpp',root,{'test.cpp':{2}},[])
            self.assertEqual([i['symbol'] for i in issues],['wrong'])

    def test_msbuild_tlog_project_working_directory(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            project=root/'game';project.mkdir()
            (project/'game.vcxproj').write_text('<Project/>')
            src=project/'src';src.mkdir()
            source=src/'game.cpp';source.write_text('int main() {return 0;}')
            logs=project/'obj'/'project.tlog';logs.mkdir(parents=True)
            tlog=logs/'CL.command.1.tlog'
            tlog.write_text(f'^{source}\ncl.exe /c /Iinclude /std:c++20 {source}\n', encoding='utf-16')
            commands=read_tlog(tlog,root)
            self.assertEqual(commands[0]['directory'],str(project))
            cmd,cwd,file=parse_command(commands[0])
            self.assertEqual(cwd,project)
            self.assertIn('/Iinclude',cmd)
            self.assertEqual(cmd.count(str(source)),1)

    def test_msbuild_tlog(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            source=root/'test.cpp'
            source.write_text('int main(){return 0;}')
            tlog=root/'CL.command.1.tlog'
            tlog.write_text(f'^{source}\n/c /D WIN32 /std:c++20 /Foobj/ {source}\n',encoding='utf-16')
            comp=read_tlog(tlog,root)
            self.assertEqual(len(comp),1)
            self.assertEqual(comp[0]['file'],str(source))
            command,cwd,src=parse_command(comp[0])
            self.assertIn('/std:c++20',command)
            self.assertEqual(command.count(str(source)),1)

if __name__=='__main__': unittest.main()
