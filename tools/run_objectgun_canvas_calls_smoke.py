"""Exercise the production ObjectGun::replaceGun canvas contract."""

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

    source = (ROOT / "src/game/weapons/ObjectGun.cpp").read_text()
    fixture = (ROOT / "tools/objectgun_canvas_calls_smoke.cpp").read_text()
    body = function(source, "void ObjectGun::replaceGun(unsigned int mesh, int /*unused*/)")
    with tempfile.TemporaryDirectory(prefix="objectgun-canvas-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "smoke.cpp"
        generated.write_text(fixture.replace("// PRODUCTION_METHODS", body))
        for optimization in ("-O0", "-O2"):
            exe = Path(temp) / f"smoke-{optimization[2:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++17", optimization,
                            str(generated), "-o", str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
