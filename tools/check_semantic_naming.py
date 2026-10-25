#!/usr/bin/env python3
"""C++ Clang AST-based naming checker: PR changed declarations only.

No external Python dependencies. Requires a Clang compiler and a compilation
 database. Never evaluates untrusted source code: invokes Clang -fsyntax-only.
"""
from __future__ import annotations

import argparse
import fnmatch
import json
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys

CPP_EXTENSIONS = {'.cpp', '.cc', '.cxx', '.c++', '.c', '.h', '.hpp', '.hxx'}
UPPER = re.compile(r'^[A-Z][A-Z0-9_]*$')
TYPE_NAME = re.compile(r'^[A-Za-z][A-Za-z0-9]*$')
FUNCTION_NAME = re.compile(r'^[A-Z][A-Za-z0-9]*$')
FUNCTION_EXCEPTIONS = {'main', 'wmain', 'WinMain', 'wWinMain', 'DllMain'}
TYPE_KINDS = {'CXXRecordDecl', 'RecordDecl', 'EnumDecl', 'TypeAliasDecl', 'TypedefDecl'}
FUNCTION_KINDS = {'FunctionDecl', 'CXXMethodDecl'}
VALUE_KINDS = {'VarDecl', 'FieldDecl', 'ParmVarDecl', 'EnumConstantDecl', 'BindingDecl'}
BUILTIN_PREFIXES = {
    'int':'n', 'short':'n', 'long':'n', 'long long':'n', 'size_t':'n',
    'float':'f', 'double':'d', 'bool':'b', 'char':'c', 'wchar_t':'c',
    'unsigned int':'u', 'unsigned':'u', 'unsigned long':'u',
    'UINT':'u', 'DWORD':'dw', 'BYTE':'by', 'HRESULT':'hr', 'BOOL':'b',
    'std::string':'s', 'std::wstring':'s', 'std::string_view':'s',
    'signed int':'n', 'unsigned short':'u', 'char8_t':'c',
}
CONTAINERS = {'vector':'v','array':'a','deque':'d','list':'l'}


def normalized(path: Path) -> str:
    return os.path.normcase(str(path.resolve())).replace('\\', '/').lower() if os.name == 'nt' else str(path.resolve())


def changed_lines(base: str, head: str) -> dict[str, set[int]]:
    raw = subprocess.check_output(
        ['git','diff','--unified=0','--no-ext-diff',f'{base}...{head}','--',
         '*.cpp','*.cc','*.cxx','*.c','*.h','*.hpp','*.hxx'],
        text=True, encoding='utf-8', errors='replace')
    changed: dict[str,set[int]] = {}
    current = None
    for line in raw.splitlines():
        if line.startswith('+++ b/'):
            current = line[6:]
        elif line.startswith('+++ /dev/null'):
            current = None
        elif line.startswith('@@ ') and current:
            match = re.search(r'\+(\d+)(?:,(\d+))?',line)
            if match:
                begin, count = int(match[1]), int(match[2] or '1')
                changed.setdefault(current,set()).update(range(begin, begin+count))
    return changed


def suggest_upper(name: str) -> str:
    body = re.sub(r'([A-Z])([A-Z][a-z])', r'\1_\2', name)
    body = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', body)
    return re.sub(r'_+', '_', body).upper().strip('_') or 'VALUE'


def pascal(name: str) -> str:
    parts = [p for p in re.split(r'[_\W]+',name) if p]
    return ''.join(p[0].upper()+p[1:] for p in parts) or 'Action'


def expected_prefix(typ: str, policy: dict | None = None) -> str | None:
    """Prefer spelling in qualType, rather than desugaredQualType (UINT/DWORD)."""
    typ = re.sub(r'\b(const|volatile|restrict|struct|class|enum)\b','',typ)
    typ = re.sub(r'\s+',' ',typ).strip()
    # Strip references, not pointers. A const T& uses the T prefix.
    typ = re.sub(r'\s*&&?\s*$','',typ).strip()
    if re.search(r'\[[^]]*\]$',typ):
        return 'a'
    if '*' in typ:
        return 'pp' if typ.count('*') >= 2 else 'p'
    if re.search(r'\b(?:unique_ptr|shared_ptr|weak_ptr)\s*<',typ):
        return 'p'
    for container,prefix in CONTAINERS.items():
        if re.match(r'(?:std::)?'+re.escape(container)+r'\s*<',typ):
            return prefix
    primitive = (policy or {}).get('names', {}).get('variables', {}).get('primitive_prefixes', {})
    if typ in primitive:
        return primitive[typ]
    if typ in BUILTIN_PREFIXES:
        return BUILTIN_PREFIXES[typ]
    # MSVC spelling occasionally includes std::basic_string<char>
    if typ.startswith('std::basic_string<'):
        return 's'
    return None  # Class, struct, enum values have no required prefix.


def propose(name: str, expected: str, member: bool) -> str:
    original = name[2:] if name.startswith('m_') else name
    # Remove an existing Hungarian prefix BEFORE converting case.
    candidates = sorted(set(BUILTIN_PREFIXES.values()) | set(CONTAINERS.values()) | {'pp','p'},key=len,reverse=True)
    for old in candidates:
        if original.startswith(old) and len(original)>len(old) and original[len(old)].isupper():
            original=original[len(old):]
            break
    parts = re.split(r'_+',original)
    original = ''.join((p[0].upper()+p[1:] if p else '') for p in parts)
    original = (original[0].upper()+original[1:]) if original else 'Value'
    return ('m_' if member else '')+expected+original


def is_constexpr(node: dict) -> bool:
    return node.get('constexpr') is True


def get_location(node: dict, current_file: Path, line_cache: dict) -> tuple[Path, int] | None:
    loc = node.get('loc') or {}
    if not isinstance(loc,dict):
        return None
    if 'spellingLoc' in loc or 'expansionLoc' in loc:
        # Macro expansion: location may refer to macro definition, not source use.
        return None
    path = loc.get('file') or (node.get('range') or {}).get('begin',{}).get('file')
    # Nodes in the primary TU may omit file; inherited location handled by walker.
    path = Path(path) if path else current_file
    if loc.get('line'):
        return path,int(loc['line'])
    offset=loc.get('offset')
    if offset is None:
        return None
    key=str(path)
    try:
        if key not in line_cache:
            line_cache[key]=path.read_bytes()
        return path,line_cache[key].count(b'\n',0,int(offset))+1
    except OSError:
        return None


def parse_ast(ast: dict, translation_unit: Path, root: Path,
              selected: dict[str,set[int] | None], ignore: list[str],
              policy: dict | None = None) -> list[dict]:
    issues=[]
    dedup=set()
    line_cache={}
    policy = policy or {}
    def emit(node, category, rule, suggestion, path, line, typ=''):
        try:
            relative = path.resolve().relative_to(root.resolve()).as_posix()
        except ValueError:
            return
        if any(fnmatch.fnmatch(relative,x) for x in ignore):
            return
        selected_lines = selected.get(relative)
        if selected_lines is None and relative not in selected:
            return
        if selected_lines is not None and line not in selected_lines:
            # Multiline declarations may change the return/type/qualifier line
            # while the symbol's own location remains unchanged.  Only look
            # between the declaration start and the name; function bodies do
            # not cause historical naming violations to be re-reported.
            begin = (node.get('range') or {}).get('begin') or {}
            begin_loc = get_location({'loc': begin}, path, line_cache)
            first_line = begin_loc[1] if begin_loc else line
            if first_line > line or not any(first_line <= n <= line for n in selected_lines):
                return
        name=node.get('name','')
        key=(relative,line,category,name)
        if not name or key in dedup:
            return
        dedup.add(key)
        issues.append({'file':relative,'line':line,'symbol':name,'type':typ or category,
                       'kind':category,'rule':rule,'suggestion':suggestion,
                       'severity':'error','source':'clang-ast'})

    def walk(node, current_file: Path, record_kind: str | None = None):
        if not isinstance(node,dict):
            return
        if node.get('isImplicit'):
            return
        kind = node.get('kind','')
        loc = node.get('loc') or {}
        # When a file is explicit, use it for descendants; avoid incorrectly
        # attributing included header declarations to primary source.
        node_file = Path(loc['file']) if isinstance(loc,dict) and loc.get('file') else current_file
        if not node_file.is_absolute():
            node_file = root / node_file
        at = get_location(node, node_file, line_cache)
        name = node.get('name','')
        rec = record_kind
        if kind in ('CXXRecordDecl','RecordDecl'):
            rec=node.get('tagUsed') or record_kind
        elif kind in FUNCTION_KINDS | {'CXXConstructorDecl','CXXDestructorDecl'}:
            rec=None  # Locals in a method are not fields.
        if at and name and not name.startswith('__'):
            path, line=at
            if not path.is_absolute():
                path = root / path
            if kind in TYPE_KINDS:
                if kind == 'CXXRecordDecl' and not node.get('completeDefinition'):
                    # Forward declarations still count, except implicit nodes.
                    pass
                if not TYPE_NAME.fullmatch(name):
                    emit(node,'type','型名はPascalCaseかlowerCamelCase（アンダースコアなし）',pascal(name),path,line)
            elif kind in FUNCTION_KINDS:
                if (name not in FUNCTION_EXCEPTIONS and not name.startswith('operator')
                    and not any(child.get('kind') in ('OverrideAttr', 'FinalAttr') for child in node.get('inner',[]))
                    and not FUNCTION_NAME.fullmatch(name)):
                    emit(node,'function','関数・メソッドはPascalCase',pascal(name),path,line)
            elif kind == 'EnumConstantDecl':
                if not UPPER.fullmatch(name):
                    emit(node,'enumerator','列挙子はUPPER_CASE',suggest_upper(name),path,line)
            elif kind in VALUE_KINDS:
                typ=(node.get('type') or {}).get('qualType','')
                if is_constexpr(node):
                    if not UPPER.fullmatch(name):
                        emit(node,'constexpr','constexpr定数はUPPER_CASE',suggest_upper(name),path,line,typ)
                else:
                    # Nonstatic class fields require m_; struct fields do not.
                    member = kind == 'FieldDecl' and record_kind=='class'
                    # Clang represents static class data as VarDecl.
                    if kind == 'VarDecl' and record_kind=='class':
                        member=True
                    if member and not name.startswith('m_'):
                        prefix=expected_prefix(typ, policy)
                        if not prefix and (node.get('type') or {}).get('desugaredQualType'):
                            prefix=expected_prefix(node['type']['desugaredQualType'], policy)
                        candidate=propose(name,prefix or '', True)
                        emit(node,'member','classメンバ変数はm_接頭辞が必要',candidate,path,line,typ)
                    else:
                        prefix=expected_prefix(typ, policy)
                        if not prefix and (node.get('type') or {}).get('desugaredQualType'):
                            prefix=expected_prefix(node['type']['desugaredQualType'], policy)
                        if prefix:
                            bare=name[2:] if name.startswith('m_') else name
                            ok=re.fullmatch(re.escape(prefix)+r'[A-Z][A-Za-z0-9]*',bare)
                            # Mutable references may optionally use 'r' prefix.
                            # string_view deliberately allows an unprefixed lowerCamelCase name.
                            sv = 'string_view' in typ and re.fullmatch(r'[a-z][A-Za-z0-9]*', bare)
                            if not ok and not sv and not (typ.rstrip().endswith('&') and not typ.lstrip().startswith('const ') and re.fullmatch(r'r[A-Z][A-Za-z0-9]*',bare)):
                                emit(node,'variable',f'{typ} は {prefix}+PascalCase'+('（classメンバはm_付き）' if member else ''),
                                     propose(name,prefix,member),path,line,typ)
        # Clang's JSON AST omits `file` for subsequent declarations in the
        # same file, even when they are siblings (e.g. two classes in a header).
        sibling_file=node_file
        for child in node.get('inner',[]):
            child_loc=child.get('loc') or {}
            if isinstance(child_loc,dict) and child_loc.get('file'):
                sibling_file=Path(child_loc['file'])
                if not sibling_file.is_absolute():
                    sibling_file=root/sibling_file
            walk(child,sibling_file,rec)

    walk(ast,translation_unit)
    return issues


def parse_command(entry: dict) -> tuple[list[str],Path,Path]:
    cwd=Path(entry['directory']).resolve()
    source=Path(entry['file'])
    if not source.is_absolute():
        source=cwd/source
    if 'arguments' in entry:
        args=list(entry['arguments'])
    else:
        args=shlex.split(entry['command'],posix=(os.name!='nt'))
    if not args:
        raise ValueError(f'コンパイルコマンドがありません: {source}')
    compiler_name=Path(args[0].replace('\\','/')).name.lower()
    is_msvc = compiler_name in ('cl.exe','cl','clang-cl.exe','clang-cl')
    # Emscripten retains its sysroot and target-specific header resolution.
    is_emcc = compiler_name in ('em++','em++.bat','em++.cmd','emcc','emcc.bat')
    compiler='clang-cl' if is_msvc else ('em++' if is_emcc else 'clang++')
    result=[compiler]
    if is_msvc and os.name == 'nt' and os.environ.get('VSCMD_ARG_TGT_ARCH','').lower() == 'x86':
        result.append('--target=i686-pc-windows-msvc')
    i=1
    while i<len(args):
        arg=args[i]
        low=arg.lower()
        # Remove compilation output and PCH flags, sanitizers that interfere
        # with parsing, and GCC linker/compiler driver-only modes.
        if low in ('-c','/c','-o','-mf','-mt','-mq'):
            i += 2 if low in ('-o','-mf','-mt','-mq') else 1
            continue
        if low.startswith(('/fo','/fd','/fp','/yu','/yc','/fioutput','/fe','/fa','/tc','/tp')) and is_msvc:
            # /FI (forced include) is intentionally preserved below.
            if low in ('/tp', '/tc'):
                i+=1; continue
            if low.startswith(('/fo','/fd','/fp','/yu','/yc','/fe','/fa')):
                i+=1; continue
        if is_msvc:
            if low in ('/d','/i','/fi'):
                if i+1<len(args):
                    result.extend([arg,args[i+1]])
                    i+=2
                    continue
            if low.startswith(('/d','/i','/fi','/std:','/imsvc','/external:i','/source-charset:','/execution-charset:')):
                result.append(arg)
            i+=1
            continue
        if arg in ('-o','-MF','-MT','-MQ','-include-pch'):
            i+=2; continue
        if arg.startswith(('-o','-MF','-MT','-MQ')):
            i+=1; continue
        if not arg.startswith('-') and Path(arg).suffix.lower() in CPP_EXTENSIONS:
            i+=1;continue
        if arg.startswith(('-Wl,','-fuse-ld=','-fplugin=','-fpass-plugin=','-Xclang')):
            # No compiler plugins or raw cc1 options from PR-controlled commands.
            i+=2 if arg == '-Xclang' else 1; continue
        result.append(arg)
        i+=1
    result += ['-Xclang','-ast-dump=json','-fsyntax-only','-Wno-everything']
    result.append(str(source))
    return result,cwd,source.resolve()


def annotate(issue: dict, warning: bool):
    value=f'{issue["rule"]}: {issue["symbol"]} → {issue["suggestion"]}'
    msg=value.replace('%','%25').replace('\r','%0D').replace('\n','%0A')
    filename=issue['file'].replace('%','%25').replace(',','%2C')
    print(f'::{"warning" if warning else "error"} file={filename},line={issue["line"]}::{msg}')


def main():
    p=argparse.ArgumentParser()
    p.add_argument('--compdb',default='compile_commands.json')
    p.add_argument('--base')
    p.add_argument('--head')
    p.add_argument('--all',action='store_true')
    p.add_argument('--policy',default='naming_policy.json')
    p.add_argument('--report',default='reports/semantic_naming.json')
    p.add_argument('--no-fail',action='store_true')
    p.add_argument('--allow-partial',action='store_true',help='Do not fail if some translation units cannot be parsed')
    p.add_argument('--max-units',type=int,default=250)
    args=p.parse_args()
    root=Path.cwd().resolve()
    policy=json.loads(Path(args.policy).read_text(encoding='utf-8'))
    ignore=policy.get('enforcement',{}).get('ignore_paths',[])
    if args.all:
        selected={p.relative_to(root).as_posix():None for p in root.rglob('*')
                  if p.suffix.lower() in CPP_EXTENSIONS and '.git' not in p.parts}
    else:
        if not args.base or not args.head:
            p.error('Specify --base and --head, or --all')
        selected=changed_lines(args.base,args.head)
    selected={k:v for k,v in selected.items() if not any(fnmatch.fnmatch(k,pattern) for pattern in ignore)}
    # Always write a machine-readable failure, even if setup/compdb fails.
    def write_report(status, visited=0, issues=None, failures=None):
        report={'check':'semantic_naming','engine':'clang-ast-json','status':status,
                'translation_units':visited,'changed_files':len(selected),
                'issues':issues or [],'failures':failures or []}
        out=Path(args.report)
        out.parent.mkdir(parents=True,exist_ok=True)
        out.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')

    try:
        entries=json.loads(Path(args.compdb).read_text(encoding='utf-8-sig'))
    except (OSError, ValueError) as exc:
        write_report('failed',failures=[{'file':args.compdb,'message':f'compdbの読み込み失敗: {exc}'}])
        print('::error::Clangコンパイルデータベースを読めません:', exc)
        return 2
    if not isinstance(entries,list):
        write_report('failed',failures=[{'file':args.compdb,'message':'compile_commands.json は配列である必要があります'}])
        return 2
    changed_headers={k for k in selected if Path(k).suffix.lower() in {'.h','.hpp','.hxx'}}
    relevant=[]
    for entry in entries:
        try:
            cmd,cwd,src=parse_command(entry)
        except (KeyError, TypeError, ValueError) as exc:
            write_report('failed',failures=[{'file':args.compdb,'message':f'不正なコンパイルエントリ: {exc}'}])
            return 2
        try:
            rel=src.relative_to(root).as_posix()
        except ValueError:
            continue
        if changed_headers or rel in selected:
            relevant.append((cmd,cwd,src))
    # An empty compilation database must not produce a fake green result.
    if selected and not relevant:
        error='変更されたC++コードに対応するコンパイル単位がありません。compdbのパス・ソースとヘッダーの依存関係を確認してください。'
        write_report('failed',failures=[{'file':args.compdb,'message':error}])
        print('::error::'+error)
        return 2
    if relevant and len(relevant)>args.max_units:
        error=f'AST検査対象が上限を超えています（{len(relevant)} > {args.max_units}）'
        write_report('failed',failures=[{'file':args.compdb,'message':error}])
        return 2
    failures=[]
    issues=[]
    visited=0
    for cmd,cwd,src in relevant:
        visited+=1
        try:
            proc=subprocess.run(cmd,cwd=cwd,capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=120)
            if proc.returncode!=0:
                failures.append({'file':str(src),'message':proc.stderr[-2000:] or f'exit {proc.returncode}'})
                continue
            ast=json.loads(proc.stdout)
            issues += parse_ast(ast,src,root,selected,ignore,policy)
        except (OSError,subprocess.TimeoutExpired,ValueError,json.JSONDecodeError) as exc:
            failures.append({'file':str(src),'message':str(exc)})
    # Dedupe because headers appear in many translation units.
    unique={ (x['file'],x['line'],x['kind'],x['symbol']):x for x in issues }
    issues=sorted(unique.values(),key=lambda x:(x['file'],x['line'],x['kind']))
    for issue in issues:
        annotate(issue,args.no_fail)
    for fail in failures:
        msg=fail['message'].replace('%','%25').replace('\r','%0D').replace('\n','%0A')[:1800]
        print(f'::error::Clang AST解析失敗: {fail["file"]}: {msg}')
    write_report('incomplete' if failures else 'complete',visited,issues,failures)
    print(f'Clang AST 意味解析: TU {visited}件, 指摘 {len(issues)}件, 解析失敗 {len(failures)}件')
    if failures and not args.allow_partial:
        return 2
    if issues and not args.no_fail:
        return 1
    return 0


if __name__=='__main__':
    try:
        raise SystemExit(main())
    except (OSError,subprocess.CalledProcessError,ValueError,KeyError,TypeError,json.JSONDecodeError) as exc:
        # Early failures (invalid policy, git history, malformed input) still get
        # an explicit failed JSON report, never a misleading empty result.
        report_path = 'reports/semantic_naming.json'
        if '--report' in sys.argv:
            at = sys.argv.index('--report')
            if at+1 < len(sys.argv):
                report_path = sys.argv[at+1]
        try:
            output = Path(report_path)
            output.parent.mkdir(parents=True,exist_ok=True)
            output.write_text(json.dumps({'check':'semantic_naming','engine':'clang-ast-json',
                'status':'failed','translation_units':0,'changed_files':0,'issues':[],
                'failures':[{'file':'(setup)','message':f'{type(exc).__name__}: {exc}'}]},
                ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
        except OSError:
            pass
        print(f'::error::意味解析チェッカーを開始できません: {type(exc).__name__}: {exc}')
        raise SystemExit(2)
