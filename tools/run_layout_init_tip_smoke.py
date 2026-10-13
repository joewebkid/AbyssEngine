"""Exercise the production Layout::initTip body with fixture services."""

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/game/ui/Layout.cpp").read_text()
    extracted = '''#include "game/ui/Layout.h"
#include "game/core/Globals.h"
#include "engine/core/AERandom.h"
#include "engine/core/GameText.h"
'''
    extracted += function(source, "void Layout::initTip(")

    with tempfile.TemporaryDirectory(prefix="layout-init-tip-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "production_init_tip.cpp"
        generated.write_text(extracted)
        exe = Path(temp) / "layout_init_tip_smoke.exe"
        env = os.environ.copy()
        env.update({"TEMP": temp, "TMP": temp, "TMPDIR": temp})
        command = [args.cxx, "-std=gnu++14", "-O0", "-fpermissive", "-Isrc",
                   "-Ithird_party/fmod/inc", "-Ithird_party/gl", "-Ithird_party/jni",
                   "tools/layout_init_tip_smoke.cpp", str(generated), "-o", str(exe)]
        subprocess.run(command, cwd=ROOT, env=env, check=True)
        subprocess.run([str(exe)], cwd=ROOT, env=env, check=True)


if __name__ == "__main__":
    main()
