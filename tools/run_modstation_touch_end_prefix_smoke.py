"""Exercise the early production OnTouchEnd branches before mission dispatch."""

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

    source = (ROOT / "src/game/menu/ModStation.cpp").read_text()
    body = function(source, "void ModStation::OnTouchEnd(int x, int y, void *touch)")
    end = "    int *status = *(int **) g_ote_status;"
    prefix = body[:body.index(end)] + "}\n"
    fixture = (ROOT / "tools/modstation_touch_end_prefix_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_PREFIX", prefix)
    with tempfile.TemporaryDirectory(prefix="modstation-touch-end-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "modstation_touch_end_prefix_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"modstation_touch_end_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level, str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
