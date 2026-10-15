"""Compile production PlayerEgo jump-drive initialization and activation in isolation."""

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
    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    constructor = source.index("PlayerEgo::PlayerEgo(")
    base_start = source.index("    this->driveCharge = 0;", constructor)
    base_end = source.index("\n\n    Ship *ship", base_start)
    item_start = source.index("        Item *driveItem =", constructor)
    item_end = source.index("\n    }\n\n    this->hardCoreMode", item_start)
    fixture = (ROOT / "tools/player_ego_jump_drive_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_DRIVE_BASE", source[base_start:base_end])
    fixture = fixture.replace("// PRODUCTION_DRIVE_ITEM", source[item_start:item_end])
    fixture = fixture.replace("// PRODUCTION_START_DRIVE", function(source, "void PlayerEgo::startJumpDrive()"))
    with tempfile.TemporaryDirectory(prefix="player-jump-drive-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "jump_drive.cpp"
        generated.write_text(fixture)
        executable = Path(temp) / "jump_drive.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(executable)],
                       cwd=ROOT, check=True)
        subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
