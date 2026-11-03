"""Exercise the production station touch-end window/audio branches."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def segment(body, start, end, name):
    first = body.index(start)
    last = body.index(end, first)
    return f"void ModStation::{name}(int x, int y) {{\n" + body[first:last] + "}\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/game/menu/ModStation.cpp").read_text()
    body = function(source, "void ModStation::OnTouchEnd(int x, int y, void *touch)")
    fixture = (ROOT / "tools/modstation_touch_end_audio_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_STATUS_STAR_MAP", segment(
        body, "    if (this->modalFlags.bytes[0] != 0) {",
        "    if (this->subWindowFlags.bytes[2] != 0) {", "testStatusStarMap"))
    fixture = fixture.replace("// PRODUCTION_MISSIONS", segment(
        body, "    if (this->subWindowFlags.bytes[0] != 0) {",
        "    if (this->m_nStarMapWindowOpen.bytes[2] != 0) {", "testMissions"))
    with tempfile.TemporaryDirectory(prefix="modstation-touch-end-audio-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "modstation_touch_end_audio_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"modstation_touch_end_audio_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level, str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
