#!/usr/bin/env python3
"""Convert real MSBuild CL.command.1.tlog (UTF-16) to Clang compilation database.

Run AFTER MSBuild. Paths and flags are taken from the current runner's build,
not from a cached .tlog made on another PC. Does not build or run program code.
"""
from __future__ import annotations
import argparse
import ctypes
import json
import os
from pathlib import Path
import shlex
import sys


def win_argv(command: str) -> list[str]:
    if os.name != 'nt':
        return shlex.split(command,posix=False)
    shell32=ctypes.windll.shell32
    kernel32=ctypes.windll.kernel32
    shell32.CommandLineToArgvW.argtypes=[ctypes.c_wchar_p,ctypes.POINTER(ctypes.c_int)]
    shell32.CommandLineToArgvW.restype=ctypes.POINTER(ctypes.c_wchar_p)
    kernel32.LocalFree.argtypes=[ctypes.c_void_p]
    count=ctypes.c_int()
    ptr=shell32.CommandLineToArgvW(command,ctypes.byref(count))
    if not ptr:
        raise OSError('CommandLineToArgvW failed')
    try:
        return [ptr[i] for i in range(count.value)]
    finally:
        kernel32.LocalFree(ptr)


def read_tlog(file: Path, repo: Path) -> list[dict]:
    raw=file.read_text(encoding='utf-16')
    groups=[]
    current=[]
    for row in raw.splitlines():
        if row.startswith('^'):
            if current:
                groups.append(current)
            current=[row[1:].strip()]
        elif current and row.strip():
            current.append(row.strip())
    if current:
        groups.append(current)
    # MSBuild normally invokes cl from the .vcxproj directory.  The .tlog is
    # underneath its intermediate-output directory; resolve the owning project
    # so relative /I and /FI arguments are interpreted like the original build.
    project_dir = repo
    for parent in file.resolve().parents:
        if parent == repo.parent:
            break
        if any(parent.glob('*.vcxproj')):
            project_dir = parent
            break
    entries=[]
    for group in groups:
        # A group has a source path or pipe-delimited sources followed by command.
        sources=group[0].split('|')
        for source in sources:
            path=Path(source)
            if not path.is_absolute() or not path.is_file():
                # Old build logs may carry paths from another computer: ignore.
                continue
            try:
                path.resolve().relative_to(repo.resolve())
            except ValueError:
                continue
            if path.suffix.lower() not in ('.cpp','.cc','.cxx','.c'):
                continue
            # MSBuild tlog has a full command line, often with absolute source path.
            args=win_argv(' '.join(group[1:]))
            if args and Path(args[0].replace('\\', '/')).name.lower() in ('cl.exe', 'cl', 'clang-cl.exe', 'clang-cl'):
                args=args[1:]
            if not args:
                continue
            # Source path is taken from ^ record, not a guessed trailing token.
            args=[a for a in args if a.casefold()!=str(path).casefold()]
            # Absolute clang-cl source path keeps this compilation DB portable
            # across build-directory locations on the same runner.
            entries.append({'directory':str(project_dir),'file':str(path),
                            'arguments':['clang-cl',*args,str(path)]})
    return entries


def main():
    p=argparse.ArgumentParser()
    p.add_argument('--root',default='.')
    p.add_argument('--glob',default='**/CL.command.1.tlog')
    p.add_argument('--out',default='compile_commands.json')
    args=p.parse_args()
    root=Path(args.root).resolve()
    database=[]
    tlogs=list(root.glob(args.glob))
    if not tlogs:
        print('::error::MSBuildのCL.command.1.tlogが見つかりません。MSBuild実行後に呼んでください。')
        return 2
    for tlog in tlogs:
        print('読み込み:',tlog)
        database.extend(read_tlog(tlog,root))
    by_path={}
    for entry in database:
        by_path[entry['file'].lower()]=entry
    database=list(by_path.values())
    if not database:
        print('::error::.tlogが存在しますが、有効な.cppファイルがありません。古い.tlogや作業ディレクトリを確認してください。')
        return 2
    Path(args.out).write_text(json.dumps(database,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print(f'コンパイルデータベースを生成: {len(database)} TU / {len(tlogs)} tlog')
    return 0


if __name__=='__main__':
    try:
        raise SystemExit(main())
    except (OSError,ValueError) as e:
        print(f'::error::{type(e).__name__}: {e}')
        raise SystemExit(2)
