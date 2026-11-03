"""Compile and exercise production station launch and five-button routes."""

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
    fixture = (ROOT / "tools/modstation_station_buttons_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_LAUNCH", segment(
        body, "                if (static_cast<TouchButton *>(this->buttonLaunch)->OnTouchEnd(x, y) != 0) {",
        "                if (static_cast<TouchButton *>(this->buttonCredits)->OnTouchEnd(x, y) != 0) {",
        "testLaunch"))
    fixture = fixture.replace("// PRODUCTION_CREDITS", segment(
        body, "                if (static_cast<TouchButton *>(this->buttonCredits)->OnTouchEnd(x, y) != 0) {",
        "                this->selectedButton = -1;", "testCredits"))
    fixture = fixture.replace("// PRODUCTION_ROW", segment(
        body, "                this->selectedButton = -1;",
        "                if (help->OnTouchEnd(x, y) != 0) {", "testRow"))
    with tempfile.TemporaryDirectory(prefix="modstation-station-buttons-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "modstation_station_buttons_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"modstation_station_buttons_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level, str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
