"""Exercise the production MiningGame layer check with local fixture globals."""

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
#include "game/mission/Achievements.h"
#include "engine/core/AERandom.h"
#include "engine/audio/FModSound.h"
'''
    generated += mining[start:end] + "\n"
    generated += function(mining, "bool MiningGame::isInCurrentLayer()")
    generated += function(mining, "int MiningGame::update(int delta)")
    globals_source = (ROOT / "src/game/core/Globals.cpp").read_text()
    generated += function(globals_source, "float Globals::sqrt(float x)")

    with tempfile.TemporaryDirectory(prefix="mining-layer-", dir=ROOT / "_work") as temp:
        production = Path(temp) / "production_mining_layer.cpp"
        production.write_text(generated)
        exe = Path(temp) / "mining_layer_smoke.exe"
        command = [args.cxx, "-std=gnu++14", "-O0", "-fpermissive",
                   "-Isrc", "-Ithird_party/fmod/inc", "-Ithird_party/gl",
                   "-Ithird_party/jni", "tools/mining_layer_smoke.cpp",
                   str(production), "-o", str(exe)]
        build = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
        if build.returncode:
            print(build.stdout[-4000:])
            print(build.stderr[-4000:])
            build.check_returncode()
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
