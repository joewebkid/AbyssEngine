"""Exercise the production PlayerEgo::update waypoint-distance fragment."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")
    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    update = source.index("void PlayerEgo::update(")
    begin = source.index("            // Android Thumb 0x9e34c-0x9e38e", update)
    end = source.index("            float speed = 4.0f;", begin)
    fixture = (ROOT / "tools/player_ego_autopilot_distance_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_FRAGMENT", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="ego-autopilot-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "autopilot_distance.cpp"
        generated.write_text(fixture)
        executable = Path(temp) / "autopilot_distance.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(executable)],
                       cwd=ROOT, check=True)
        subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
