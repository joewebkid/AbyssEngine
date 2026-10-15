#!/usr/bin/env python3
"""Audit direct-call destinations inside masked-byte ARM matches.

This is separate from linked_equal: unresolved calls never silently become
proof of equivalence, and existing linked-exact counts are left unchanged.
"""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path

import asmdiff


DIRECT_CALL_RELOCS = {"R_ARM_THM_CALL", "R_ARM_CALL"}
HEADER = re.compile(r"^([0-9a-f]+) <([^>]+)>:$")
CALL = re.compile(r"^\s*([0-9a-f]+):\s+[0-9a-f ]+\s+(blx?|bl\.[nw])\s+.*<([^>]+)>")


def plain_symbol(name):
    """Remove ELF version/PLT suffixes, but not offsets into a callee."""
    name = name.split("@", 1)[0]
    return name if not re.search(r"[+-]0x[0-9a-f]+$", name) else None


def original_direct_calls(disassembly):
    """Return {function: {relative_call_offset: raw_callee_label}} for .text."""
    functions = {}
    section, current, start = None, None, 0
    for line in disassembly.splitlines():
        section_match = re.match(r"^Disassembly of section (.+):$", line)
        if section_match:
            section = section_match.group(1)
            current = None
            continue
        if section != ".text":
            continue
        header = HEADER.match(line)
        if header:
            start = int(header.group(1), 16)
            current = plain_symbol(header.group(2))
            if current:
                functions[current] = {"address": start, "calls": {}}
            continue
        call = CALL.match(line)
        if current and call:
            functions[current]["calls"][int(call.group(1), 16) - start] = call.group(3)
    return functions


def audit_function(unit, symbol, base, target, original):
    """Return one result per direct-call relocation in a masked-byte match."""
    if not asmdiff.linked_equal(target, base):
        return []
    found = original.get(symbol)
    results = []
    for reloc in base["relocation_records"]:
        # shortcut: only direct CALL relocations; GOT/data/indirect targets need
        # a separate resolution model before they can establish equivalence.
        if reloc["type"] not in DIRECT_CALL_RELOCS:
            continue
        original_label = found["calls"].get(reloc["offset"]) if found else None
        compiled = plain_symbol(reloc["symbol"] or "")
        observed = plain_symbol(original_label) if original_label else None
        status = ("unresolved" if not compiled or not observed else
                  "match" if compiled == observed else "mismatch_candidate")
        reason = ("original_function_header_missing" if not found else
                  "original_call_label_missing" if not original_label else
                  "callee_offset_or_unknown_symbol" if not compiled or not observed else None)
        results.append({"unit": unit, "symbol": symbol,
                        "offset": reloc["offset"], "type": reloc["type"],
                        "compiled_target": reloc["symbol"],
                        "original_target": original_label, "status": status,
                        "reason": reason})
    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", default="cmake-build-match/verify")
    parser.add_argument("--original", required=True, help="original Android ELF .so")
    parser.add_argument("--objdump", default=asmdiff.DEFAULT_OBJDUMP)
    parser.add_argument("--unit", help="one object unit, e.g. game/core/String")
    parser.add_argument("--output", help="write detailed JSON report outside Git")
    args = parser.parse_args()
    root = Path(args.build_dir) / "base"
    objects = ([root / (args.unit + ".o")] if args.unit else sorted(root.rglob("*.o")))
    if not objects or any(not path.is_file() for path in objects):
        parser.error("base ARM object missing; build the verifier objects first")
    text = subprocess.check_output(asmdiff.tool_argv(args.objdump, ["-d", args.original]),
                                   text=True, timeout=asmdiff.DISASM_TIMEOUT)
    original = original_direct_calls(text)
    rows = []
    linked_functions = 0
    for base_path in objects:
        unit = base_path.relative_to(root).with_suffix("").as_posix()
        target_path = Path(args.build_dir) / "target" / base_path.relative_to(root)
        if not target_path.is_file():
            parser.error(f"delinked target missing: {target_path}")
        base = asmdiff.disassemble(str(base_path), objdump=args.objdump)
        target = asmdiff.disassemble(str(target_path), objdump=args.objdump)
        for symbol, body in base.items():
            if symbol not in target or not asmdiff.linked_equal(target[symbol], body):
                continue
            linked_functions += 1
            rows.extend(audit_function(unit, symbol, body, target[symbol], original))
    summary = {status: sum(row["status"] == status for row in rows)
               for status in ("match", "mismatch_candidate", "unresolved")}
    report = {"scope": "direct ARM CALL relocations in linked-equal functions",
              "original_sha256": hashlib.sha256(Path(args.original).read_bytes()).hexdigest(),
              "objects": len(objects),
              "linked_functions": linked_functions, "calls": len(rows),
              "summary": summary, "rows": rows}
    if args.output:
        Path(args.output).write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({key: value for key, value in report.items() if key != "rows"},
                     indent=2))
    if summary["mismatch_candidate"]:
        print("Review mismatch candidates in the detailed report; aliases and thunks can differ.")


if __name__ == "__main__":
    main()
