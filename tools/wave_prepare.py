#!/usr/bin/env python3
"""wave_prepare.py — turn worklist.json into a wave of jobs (Workflow `args`).

A wave takes the FIRST subbatch of each selected component (components are file-disjoint, so the
selected jobs never share a file -> safe to run in parallel). Selection can be filtered by dominant
kind and capped, so waves can be composed (drain the safe `absent` bucket first, etc).

  wave_prepare.py [--kinds absent,extra,...] [--limit N] [--exclude-kinds template,...]
                  [--out path]

Writes the wave JSON to --out (default cmake-build-match/verify/wave.json) and prints a summary.
"""
import argparse
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import dump_asm  # noqa: E402  — bake original .so disassembly into jobs (Ghidra-free ground truth)
VDIR = os.path.join(REPO, "cmake-build-match", "verify")

KEEP = ("symbol", "demangled", "ghidra_addr", "kind", "files", "ours",
        "paired_extra", "paired_absent")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--kinds", default=None, help="comma list of dominant kinds to include")
    ap.add_argument("--exclude-kinds", default=None)
    ap.add_argument("--limit", type=int, default=None)
    ap.add_argument("--max-entries", type=int, default=None,
                    help="skip components whose first subbatch exceeds this size")
    ap.add_argument("--out", default=os.path.join(VDIR, "wave.json"))
    ap.add_argument("--script-out", default=os.path.join(VDIR, "wave_wf.js"),
                    help="write a self-contained Workflow script with the jobs baked in "
                         "(Workflow `args` stringification is unreliable, so we embed)")
    args = ap.parse_args()

    wl = json.load(open(os.path.join(VDIR, "worklist.json")))
    entries = wl["entries"]
    include = set(args.kinds.split(",")) if args.kinds else None
    exclude = set(args.exclude_kinds.split(",")) if args.exclude_kinds else set()

    jobs = []
    for ci, c in enumerate(wl["components"]):
        if not c["subbatches"]:
            continue
        dk = c["dominant_kind"]
        if include is not None and dk not in include:
            continue
        if dk in exclude:
            continue
        sub = c["subbatches"][0]
        if args.max_entries and len(sub) > args.max_entries:
            continue
        je = []
        for i in sub:
            d = {k: entries[i].get(k) for k in KEEP if entries[i].get(k) is not None}
            # template instantiations are mechanical (no body to recover) — skip the asm to keep the
            # baked script small; everything else gets its authoritative disassembly.
            if entries[i].get("kind") != "template":
                asm = dump_asm.get_asm(entries[i].get("ghidra_addr"))
                if asm:
                    # cap very long bodies — ~50 lines is plenty to understand behavior, and keeps
                    # the baked Workflow script under the 512KB limit on wide waves.
                    lines = asm.split("\n")
                    if len(lines) > 50:
                        asm = "\n".join(lines[:50]) + "\n       ... (truncated; decompile via Ghidra for the full body)"
                    d["orig_asm"] = asm
            je.append(d)
        # a short, file-derived label
        base = (c["files"][0].split("/")[-1].rsplit(".", 1)[0]
                if c["files"] else (c.get("target_file") or dk).split("/")[-1].rsplit(".", 1)[0])
        jobs.append({"label": f"{dk}:{base}:{ci}", "kind": dk, "files": c["files"],
                     "target_file": c.get("target_file"), "entries": je})
        if args.limit and len(jobs) >= args.limit:
            break

    json.dump({"jobs": jobs}, open(args.out, "w"), indent=1)

    # bake a self-contained Workflow script (jobs as a JSON literal) — invoke with
    # Workflow({scriptPath: <script-out>}) and NO args.
    template = open(os.path.join(HERE, "wave_workflow.js")).read()
    literal = "const EMBEDDED_JOBS = " + json.dumps(jobs, separators=(",", ":"))
    marker = "const EMBEDDED_JOBS = null // __WAVE_JOBS__ (wave_prepare.py replaces this whole line)"
    if marker not in template:
        print("ERROR: marker line not found in tools/wave_workflow.js", file=sys.stderr)
        return 1
    open(args.script_out, "w").write(template.replace(marker, literal))

    n_entries = sum(len(j["entries"]) for j in jobs)
    from collections import Counter
    bk = Counter(j["kind"] for j in jobs)
    print(f"wave: {len(jobs)} components, {n_entries} entries  ({dict(bk)})")
    print(f"  jobs   -> {os.path.relpath(args.out, REPO)}")
    # fail loud if the generated script doesn't parse (e.g. a stray backtick in the prompt) — a
    # broken script silently runs 0 agents otherwise.
    chk = subprocess.run(["node", "--check", args.script_out], capture_output=True, text=True)
    if chk.returncode != 0:
        print("ERROR: generated wave script does not parse:\n" + chk.stderr.strip(), file=sys.stderr)
        return 1
    sz = os.path.getsize(args.script_out)
    warn = "  !! exceeds 512KB Workflow limit — lower --limit/--max-entries" if sz > 520000 else ""
    print(f"  script -> {os.path.relpath(args.script_out, REPO)}  ({sz} bytes){warn}  "
          f"(invoke: Workflow scriptPath={os.path.relpath(args.script_out, REPO)}, no args)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
