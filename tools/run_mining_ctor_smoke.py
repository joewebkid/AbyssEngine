"""Run the production MiningGame constructor against small service doubles."""

import argparse
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

    mining = (ROOT / "src/game/menu/MiningGame.cpp").read_text()
    start = mining.index('extern "C" {\nint LAYER_DIAMETERS[49] = {')
    end = mining.index("};\n}", start) + len("};\n}")
    generated = '''#include "game/menu/MiningGame.h"
#include "game/core/Globals.h"
#include "game/ui/Layout.h"
#include "game/mission/Status.h"
#include "game/mission/Item.h"
#include "game/ship/Ship.h"
#include "engine/render/PaintCanvas.h"
'''
    generated += mining[start:end] + "\n"
    generated += function(mining, "MiningGame::MiningGame(int layer, int station, Hud *hud)")

    with tempfile.TemporaryDirectory(prefix="mining-ctor-", dir=ROOT / "_work") as temp:
        production = Path(temp) / "production_mining_ctor.cpp"
        production.write_text(generated)
        exe = Path(temp) / "mining_ctor_smoke.exe"
        command = [args.cxx, "-std=gnu++14", "-O0", "-fpermissive",
                   "-Isrc", "-Ithird_party/fmod/inc", "-Ithird_party/gl",
                   "-Ithird_party/jni", "tools/mining_ctor_smoke.cpp",
                   str(production), "-o", str(exe)]
        build = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
        if build.returncode:
            print(build.stdout[-4000:])
            print(build.stderr[-4000:])
            build.check_returncode()
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
