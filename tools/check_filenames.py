#!/usr/bin/env python3
"""Check only added/renamed C/C++ source filenames in a pull request.

Uses naming_policy.json; unlike the future AST checker, does NOT validate identifiers.
"""
import argparse
import fnmatch
import json
import re
import subprocess
import sys
from pathlib import Path

CAMEL = re.compile(r"^[a-z][a-zA-Z0-9]*$")
SNAKE = re.compile(r"^[a-z][a-z0-9]*(?:_[a-z0-9]+)*$")


def suggestion(stem: str) -> str:
    pieces = [part for part in re.split(r"[^a-zA-Z0-9]+", stem) if part]
    if not pieces:
        return "exampleFile"
    return pieces[0][0].lower() + pieces[0][1:] + "".join(s[:1].upper() + s[1:] for s in pieces[1:])


def changed_paths(base: str, head: str) -> list[str]:
    result = subprocess.run(
        ["git", "diff", "--name-only", "--no-ext-diff", "--diff-filter=AR", "-z", f"{base}...{head}"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=True,
    )
    return [p.decode("utf-8", "surrogateescape") for p in result.stdout.split(b"\x00") if p]


def annotate(path: str, message: str) -> None:
    esc = lambda x: x.replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A")
    print(f"::error file={esc(path).replace(',', '%2C')},line=1::{esc(message)}")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--base", required=True)
    ap.add_argument("--head", required=True)
    ap.add_argument("--policy", default="naming_policy.json", help="Path to trusted naming policy")
    args = ap.parse_args()
    policy = json.loads(Path(args.policy).read_text(encoding="utf-8"))
    allowed_extensions = set(policy["names"]["cpp_files"]["extensions"])
    ignores = policy["enforcement"].get("ignore_paths", [])
    paths = changed_paths(args.base, args.head)
    issues = []
    scanned = 0
    for path in paths:
        path = path.replace("\\", "/")
        if any(fnmatch.fnmatch(path, pat) or fnmatch.fnmatch("/" + path, pat) for pat in ignores):
            continue
        filename = Path(path).name
        suffix = Path(filename).suffix
        if suffix not in allowed_extensions:
            continue
        scanned += 1
        stem = filename[: -len(suffix)]
        if CAMEL.fullmatch(stem) or SNAKE.fullmatch(stem):
            continue
        suggested = suggestion(stem) + suffix
        message = f"ファイル名 '{filename}' は命名規則に違反しています。修正候補: '{suggested}'"
        issues.append({"file": path, "line": 1, "message": message, "suggestion": suggested})
        annotate(path, message)

    report = {"check": "filename", "scanned": scanned, "issues": issues}
    report_dir = Path("reports")
    report_dir.mkdir(exist_ok=True)
    (report_dir / "file_names.json").write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"新規追加・改名したC/C++ファイル: {scanned} 件 / 違反: {len(issues)} 件")
    return 1 if issues else 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (subprocess.CalledProcessError, OSError, ValueError, KeyError) as exc:
        print(f"::error::ファイル名の検査中にエラーが発生しました: {exc}")
        sys.exit(2)
