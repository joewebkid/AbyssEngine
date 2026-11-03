"""Compile and exercise the production station sound-resource transition."""

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
    branch = body.index("    } else if (state == 100) {")
    first = body.index("        Globals *globals = static_cast<Globals *>(Globals::globals);", branch)
    last = body.index("    } else {", first)
    snippet = body[first:last]
    fixture = (ROOT / "tools/modstation_sound_resources_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_SOUND_RESOURCES",
                              "void ModStation::loadSoundResources() {\n"
                              "    int next = -1;\n" + snippet
                              + "    this->finalNext = next;\n}\n")
    with tempfile.TemporaryDirectory(prefix="modstation-sounds-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "modstation_sound_resources_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"modstation_sound_resources_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level,
                            str(generated), "-o", str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
