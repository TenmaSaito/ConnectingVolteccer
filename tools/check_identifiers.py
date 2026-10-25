#!/usr/bin/env python3
"""Conservative, dependency-free C++ naming checker for PR changed lines.

Parses SIMPLE declarations/parameters, not complete C++. Skips ambiguous syntax.
Report gives suggestions but never silently rewrites C++ source.
Use clang-tidy/LibTooling for complete type-aware validation.
"""
import argparse
import fnmatch
import json
import re
import subprocess
from pathlib import Path

EXTS = {'.cpp', '.cc', '.cxx', '.h', '.hpp', '.hxx'}
ID = r'[A-Za-z_]\w*'
# type spelling is deliberate: DWORD/UINT must not be flattened to their underlying int.
TYPES = r'(?:unsigned\s+int|unsigned|signed\s+int|int|float|double|bool|char|wchar_t|size_t|UINT|DWORD|BYTE|HRESULT|BOOL|std::(?:string|wstring|string_view|vector<[^;=(){}]+>|array<[^;=(){}]+>|deque<[^;=(){}]+>|list<[^;=(){}]+>|unique_ptr<[^;=(){}]+>|shared_ptr<[^;=(){}]+>))'
DECL = re.compile(r'(?<![\w:])(?P<qual>(?:(?:const|constexpr|static|inline|volatile)\s+)*)' +
                  r'(?P<typ>' + TYPES + r')\s*(?P<ptr>(?:\s*\*\s*|\s*&\s*){0,2})\s*' +
                  r'(?P<name>' + ID + r')\s*(?=\[|=|;|,|\)|\{|$)')
CONSTNAME = re.compile(r'^[A-Z][A-Z0-9_]*$')
CAMEL = re.compile(r'^[a-z][A-Za-z0-9]*$')
ALIASES = {'unsigned int':'u', 'unsigned':'u', 'signed int':'n', 'int':'n','float':'f', 'double':'d', 'bool':'b', 'char':'c', 'wchar_t':'c','size_t':'n','UINT':'u','DWORD':'dw','BYTE':'by','HRESULT':'hr','BOOL':'b','std::string':'s','std::wstring':'s','std::string_view':'s'}
CONTAINERS = {'vector':'v','array':'a','deque':'d','list':'l'}
SMART = {'unique_ptr','shared_ptr'}
IGNORES = ('.git/', '.vs/', 'Debug/', 'Release/', 'build/', 'third_party/', 'vendor/', 'generated/')

def git_diff(base, head):
    raw = subprocess.check_output(['git','diff','--unified=0','--no-ext-diff',f'{base}...{head}','--','*.cpp','*.cc','*.cxx','*.h','*.hpp','*.hxx'], text=True, encoding='utf-8', errors='replace')
    paths = {}
    path = None
    for line in raw.splitlines():
        if line.startswith('+++ b/'):
            path = line[6:]
        elif line.startswith('+++ /dev/null'):
            path = None
        elif line.startswith('@@ ') and path:
            m = re.search(r'\+(\d+)(?:,(\d+))?', line)
            if m:
                start = int(m[1]); count = int(m[2] or '1')
                paths.setdefault(path,set()).update(range(start,start+count))
    return paths

def masked(line):
    # Replace string literals and // comments before scanning declarations.
    line = re.sub(r'"(?:\\.|[^"\\])*"', '""', line)
    line = re.sub(r"'(?:\\.|[^'\\])*'", "''", line)
    return line.split('//',1)[0]

def expected(typ, ptr):
    if '*' in ptr: return 'pp' if ptr.count('*') > 1 else 'p'
    if typ in ALIASES: return ALIASES[typ]
    m = re.match(r'std::(\w+)<',typ)
    if m:
        if m[1] in SMART: return 'p'
        return CONTAINERS.get(m[1])
    return None

def fix_name(name, prefix, member=False):
    if member and name.startswith('m_'): name=name[2:]
    # If a known Hungarian prefix is present, strip it prior to replacement.
    for old in sorted(set(ALIASES.values()) | set(CONTAINERS.values()) | {'pp','p','a','v','d','l'},key=len,reverse=True):
        if name.startswith(old) and len(name)>len(old) and name[len(old)].isupper():
            name=name[len(old):]
            break
    base = name[:1].upper()+name[1:] if name else 'Value'
    return ('m_' if member else '') + prefix + base

def check_line(path,line_no,line, policy):
    findings=[]
    code=masked(line)
    if not code.strip() or code.lstrip().startswith('#'): return findings
    # Avoid interpreting return types and calls as declarations.
    for m in DECL.finditer(code):
        qual=m['qual']; typ=' '.join(m['typ'].split()); ptr=m['ptr']; name=m['name']
        tail=code[m.end():].lstrip()
        if tail.startswith('('): continue
        # constexpr functions (and macro expansions) are not variables.
        is_constant='constexpr' in qual.split()
        if name in {'main','WinMain','nullptr'}: continue
        # Heuristic for simple class-member declarations. Most member symbols already have m_.
        member=name.startswith('m_')
        if is_constant:
            if not CONSTNAME.fullmatch(name):
                proposed=re.sub(r'(?<!^)(?=[A-Z])','_',name.replace('m_','')).upper()
                findings.append({'file':path,'line':line_no,'symbol':name,'type':typ,'rule':'constexpr定数はUPPER_CASE','suggestion':proposed,'severity':'error'})
            continue
        # Uppercase const is reserved for semantic constants (not general const vars).
        if 'const' in qual.split() and CONSTNAME.fullmatch(name):
            findings.append({'file':path,'line':line_no,'symbol':name,'type':typ,'rule':'定数を意図したconst宣言です。constexpr化可能か確認（型/初期化式に依存）','suggestion':f'constexpr {typ} {name} = ...; (可能な場合のみ)','severity':'warning'})
            continue
        prefix=expected(typ,ptr)
        if not prefix: continue
        bare=name[2:] if member else name
        valid=bool(re.fullmatch(re.escape(prefix)+r'[A-Z][A-Za-z0-9]*',bare))
        # Array declaration in source: raw array gets a prefix a.
        if code[m.end():].lstrip().startswith('['):
            prefix='a'; valid=bool(re.fullmatch(r'a[A-Z][A-Za-z0-9]*',bare))
        if valid: continue
        findings.append({'file':path,'line':line_no,'symbol':name,'type':typ,'rule':f'{typ} の変数は {prefix}+PascalCase'+(' (メンバは m_ 付き)' if member else ''),'suggestion':fix_name(name,prefix,member),'severity':'error'})
    return findings

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--base'); ap.add_argument('--head'); ap.add_argument('--all', action='store_true')
    ap.add_argument('--policy', default='naming_policy.json', help='Path to trusted naming policy')
    ap.add_argument('--no-fail',action='store_true',help='Pilot mode: report errors but do not fail CI')
    args=ap.parse_args()
    policy=json.loads(Path(args.policy).read_text(encoding='utf-8'))
    selected=git_diff(args.base,args.head) if args.base and args.head and not args.all else {str(p):None for p in Path('.').rglob('*') if p.suffix in EXTS}
    issues=[]; scanned=0
    for file,changed in selected.items():
        normalized=file.replace('\\','/')
        if any(normalized.startswith(x) for x in IGNORES) or any(fnmatch.fnmatch(normalized,x) for x in policy['enforcement'].get('ignore_paths',[])): continue
        p=Path(file)
        if not p.is_file() or p.suffix not in EXTS: continue
        for num,line in enumerate(p.read_text(encoding='utf-8-sig',errors='replace').splitlines(),1):
            if changed is not None and num not in changed: continue
            scanned+=1
            issues += check_line(file,num,line,policy)
    for x in issues:
        level='warning' if args.no_fail or x['severity']=='warning' else 'error'
        msg=f"{x['rule']} : {x['symbol']} → {x['suggestion']}"
        escaped=msg.replace('%','%25').replace('\r','%0D').replace('\n','%0A')
        print(f"::{level} file={x['file']},line={x['line']}::{escaped}")
    Path('reports').mkdir(exist_ok=True)
    Path('reports/identifiers.json').write_text(json.dumps({'check':'identifiers','mode':'conservative-declaration-pattern','scanned_lines':scanned,'issues':issues},ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(f'変更された宣言の検査: {scanned} 行 / 指摘: {len(issues)} 件（簡易構文検査）')
    return int(not args.no_fail and any(x['severity']=='error' for x in issues))

if __name__=='__main__':
    try: raise SystemExit(main())
    except (OSError, subprocess.CalledProcessError, ValueError) as exc:
        print('::error::識別子チェック中にエラーが発生しました:',exc)
        raise SystemExit(2)
