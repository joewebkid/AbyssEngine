"""Compile and exercise the production station button-creation block."""

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
    body = function(source, "void ModStation::OnInitialize()")
    state = body.index("} else if (state == 0x50) {")
    first = body.index("        Layout *layout = Globals::layout;", state)
    last = body.index("        next = 0x3c;", first)
    fixture = (ROOT / "tools/modstation_buttons_create_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_BUTTON_CREATION",
                              "void ModStation::buildButtons() {\n"
                              + body[first:last] + "}\n")
    with tempfile.TemporaryDirectory(prefix="modstation-buttons-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "modstation_buttons_create_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"modstation_buttons_create_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level,
                            "-I", str(ROOT / "src"), str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
