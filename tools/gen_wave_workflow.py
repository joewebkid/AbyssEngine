#!/usr/bin/env python3
"""Generate a self-contained wrong-signature-fix workflow script for one wave.

Reuses the validated pilot script's prompt/schema/body verbatim, but bakes the wave's groups in
as an embedded JSON literal (so we don't depend on Workflow `args` stringification). Run:

    python3 tools/gen_wave_workflow.py <batch.json> <wave_name> <out.js>

then invoke Workflow({scriptPath: out.js}) with no args.
"""
import json
import re
import sys

PILOT = ("/Users/fionera/.claude/projects/-Users-fionera-Downloads-GalaxyOnFire2-gof2-decomp/"
         "3949c3ca-f962-4b08-85ee-8ac471a70897/workflows/scripts/"
         "wrongtype-fix-pilot-wf_337c4eec-e49.js")


def main():
    batch_path, wave_name, out_path = sys.argv[1], sys.argv[2], sys.argv[3]
    groups = json.load(open(batch_path))
    src = open(PILOT).read()

    # 1. embed groups as a JSON literal (JSON has no backticks / ${, safe inside a template string)
    embedded = "const groups = JSON.parse(" + repr(json.dumps(groups)).replace("`", "\\`") + ")"
    # repr() gives a single-quoted python string; convert to a JS backtick string instead
    embedded = "const groups = JSON.parse(`" + json.dumps(groups).replace("\\", "\\\\").replace("`", "\\`") + "`)"
    src = re.sub(r"const groups = .*", embedded, src, count=1)

    # 2. rename the workflow + the final log line so progress/telemetry are wave-specific
    src = src.replace("name: 'wrongtype-fix-pilot'", f"name: '{wave_name}'")
    src = src.replace("`pilot done:", f"`{wave_name} done:")

    open(out_path, "w").write(src)
    print(f"wrote {out_path}: {len(groups)} groups, "
          f"{sum(len(g['entries']) for g in groups)} entries")


if __name__ == "__main__":
    main()
