"""Compile and exercise the production station-active button-position block."""

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
    first = body.index("        Globals::sound->play(0x7a, nullptr, nullptr, 0.0f);")
    last = body.index("        return;", first)
    fixture = (ROOT / "tools/modstation_active_buttons_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_ACTIVE_BUTTONS",
                              "void ModStation::activate() {\n"
                              + body[first:last] + "}\n")
    with tempfile.TemporaryDirectory(prefix="modstation-active-buttons-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "modstation_active_buttons_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"modstation_active_buttons_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level,
                            "-I", str(ROOT / "src"), str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
