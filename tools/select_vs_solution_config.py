#!/usr/bin/env python3
"""Select an actual Visual Studio *solution* configuration for GitHub Actions.

Note: solution Release|x86 may map to project Release|Win32. Pass the solution
configuration to MSBuild, not the underlying .vcxproj platform.
"""
import argparse
import os
from pathlib import Path
import sys


class SelectionError(ValueError):
    pass


def read_solution_combinations(solution: Path) -> list[str]:
    if not solution.is_file():
        raise SelectionError(f'ソリューションが見つかりません: {solution}')
    source = solution.read_text(encoding='utf-8-sig', errors='replace')
    in_section = False
    combinations = []
    for raw_line in source.splitlines():
        line = raw_line.strip()
        if line.startswith('GlobalSection(SolutionConfigurationPlatforms)'):
            in_section = True
            continue
        if in_section and line.startswith('EndGlobalSection'):
            break
        if in_section and '=' in line:
            left = line.split('=', 1)[0].strip()
            if '|' in left and left not in combinations:
                combinations.append(left)
    if not combinations:
        raise SelectionError('SolutionConfigurationPlatformsを.slnから取得できませんでした')
    return combinations


def select_configuration(combinations: list[str], configuration: str = '', platform: str = '') -> tuple[str, str]:
    configuration = configuration.strip() or 'Release'
    platform = platform.strip()
    by_configuration = [entry for entry in combinations if entry.split('|', 1)[0].casefold() == configuration.casefold()]
    if not by_configuration:
        raise SelectionError(f'構成 {configuration} は存在しません。有効な構成: {", ".join(combinations)}')
    if platform:
        for entry in by_configuration:
            cfg, plat = entry.split('|', 1)
            if plat.casefold() == platform.casefold():
                return cfg, plat
        raise SelectionError(f'{configuration}|{platform} は存在しません。有効な構成: {", ".join(combinations)}')
    # x86 is deliberately preferred over Win32: .sln platforms and .vcxproj
    # platforms do not necessarily have the same names.
    for preferred in ('x86', 'Win32', 'x64', 'Any CPU'):
        for entry in by_configuration:
            cfg, plat = entry.split('|', 1)
            if plat.casefold() == preferred.casefold():
                return cfg, plat
    return tuple(by_configuration[0].split('|', 1))


def main() -> int:
    ap = argparse.ArgumentParser(description='VSソリューションの有効な構成を選択')
    ap.add_argument('--solution', required=True)
    ap.add_argument('--configuration', default='')
    ap.add_argument('--platform', default='')
    args = ap.parse_args()
    if not args.solution:
        raise SelectionError('GitHub VariablesにVS_SOLUTION（例: c++TKG.sln）を登録してください')
    available = read_solution_combinations(Path(args.solution))
    config, platform = select_configuration(available, args.configuration, args.platform)
    print(f'ソリューションの構成一覧: {", ".join(available)}')
    print(f'ビルドに使用する構成: {config}|{platform}')
    if output_file := os.environ.get('GITHUB_OUTPUT'):
        with open(output_file, 'a', encoding='utf-8') as f:
            f.write(f'configuration={config}\nplatform={platform}\n')
    return 0


if __name__ == '__main__':
    # WindowsのGitHub Actionsランナーでは標準出力がcp1252の場合がある。
    # PYTHONIOENCODINGの設定が漏れても日本語ログで落ちないようにする。
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, 'reconfigure'):
            stream.reconfigure(encoding='utf-8')
    try:
        raise SystemExit(main())
    except (OSError, SelectionError) as exc:
        print(f'::error::{exc}', file=sys.stderr)
        raise SystemExit(1)
