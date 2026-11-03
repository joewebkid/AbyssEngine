"""Compile and exercise the production DLC close branch of ModStation::OnTouchEnd."""

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
    first = body.index("    if (this->m_nStarMapWindowOpen.bytes[2] != 0) {")
    last = body.index("    if (this->m_nStarMapWindowOpen.bytes[1] != 0) {", first)
    fixture = (ROOT / "tools/modstation_dlc_close_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_DLC_CLOSE",
                              "void ModStation::testDlcClose(int x, int y, void *touch) {\n"
                              + body[first:last] + "}\n")
    with tempfile.TemporaryDirectory(prefix="modstation-dlc-close-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "modstation_dlc_close_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"modstation_dlc_close_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level, str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
