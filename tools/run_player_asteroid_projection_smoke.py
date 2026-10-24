"""Exercise production asteroid projection and center static side effects."""

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

    fixture = (ROOT / "tools/player_asteroid_projection_smoke.cpp").read_text()
    source = (ROOT / "src/game/ship/PlayerAsteroid.cpp").read_text()
    body = function(source, "Vector PlayerAsteroid::getProjectionVector(")
    center_body = function(source, "void PlayerAsteroid::setAsteroidCenter(")
    assert fixture.count("// PRODUCTION_METHOD") == 1
    assert fixture.count("// PRODUCTION_CENTER_METHOD") == 1
    with tempfile.TemporaryDirectory(prefix="asteroid-projection-", dir=ROOT / "_work") as temp:
        source = Path(temp) / "asteroid_projection.cpp"
        source.write_text(fixture.replace("// PRODUCTION_METHOD", body)
                          .replace("// PRODUCTION_CENTER_METHOD", center_body))
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"asteroid_projection_{opt[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++17", opt, str(source), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)
            print(f"Production asteroid projection and center {opt}: OK", flush=True)


if __name__ == "__main__":
    main()
