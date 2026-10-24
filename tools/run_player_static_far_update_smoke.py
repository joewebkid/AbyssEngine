"""Exercise the production far-static camera-distance update body."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    fixture = (ROOT / "tools/player_static_far_update_smoke.cpp").read_text()
    body = function((ROOT / "src/game/ship/PlayerStaticFar.cpp").read_text(),
                    "void PlayerStaticFar::update(")
    assert fixture.count("// PRODUCTION_METHOD") == 1
    with tempfile.TemporaryDirectory(prefix="static-far-update-", dir=ROOT / "_work") as temp:
        source = Path(temp) / "static_far_update.cpp"
        source.write_text(fixture.replace("// PRODUCTION_METHOD", body))
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"static_far_update_{opt[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++17", opt, str(source), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)
            print(f"Production far-static update {opt}: OK", flush=True)


if __name__ == "__main__":
    main()
