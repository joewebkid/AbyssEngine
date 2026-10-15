"""Exercise the production PlayerEgo::update jump-charge fragment."""

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
    begin = source.index("    // Android Thumb 0x9d8a6-0x9d8f6:", update)
    end = source.index("    if (this->tractorBeam != nullptr)", begin)
    fixture = (ROOT / "tools/player_ego_drive_charge_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_FRAGMENT", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="ego-drive-charge-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "drive_charge.cpp"
        generated.write_text(fixture)
        executable = Path(temp) / "drive_charge.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(executable)],
                       cwd=ROOT, check=True)
        subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
