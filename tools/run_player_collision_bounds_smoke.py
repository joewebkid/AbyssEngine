"""Run the production asteroid/far-static collision bodies on host edge cases."""

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

    fixture = (ROOT / "tools/player_collision_bounds_smoke.cpp").read_text()
    bodies = "\n".join([
        function((ROOT / "src/game/ship/PlayerAsteroid.cpp").read_text(),
                 "int PlayerAsteroid::collide("),
        function((ROOT / "src/game/ship/PlayerStaticFar.cpp").read_text(),
                 "int PlayerStaticFar::collide("),
    ])
    assert fixture.count("// PRODUCTION_METHODS") == 1
    with tempfile.TemporaryDirectory(prefix="collision-bounds-", dir=ROOT / "_work") as temp:
        source = Path(temp) / "collision_bounds.cpp"
        source.write_text(fixture.replace("// PRODUCTION_METHODS", bodies))
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"collision_bounds_{opt[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++17", opt, str(source), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)
            print(f"Production collision bounds {opt}: OK", flush=True)


if __name__ == "__main__":
    main()
