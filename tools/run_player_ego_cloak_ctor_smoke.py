"""Compile and exercise production PlayerEgo cloak-constructor fragments."""

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
    constructor = source.index("PlayerEgo::PlayerEgo(")
    base_start = source.index("    this->cloaked = 0;", constructor)
    base_end = source.index("    this->driveCharge = 0;", base_start)
    item_start = source.index("        Item *cloakItem =", constructor)
    item_end = source.index("        Item *driveItem =", item_start)
    recharge_start = source.index("    float difficulty;", constructor)
    recharge_end = source.index("\n\n    // Android Thumb 0x99a02", recharge_start)
    fixture = (ROOT / "tools/player_ego_cloak_ctor_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_CLOAK_BASE", source[base_start:base_end])
    fixture = fixture.replace("// PRODUCTION_CLOAK_ITEM", source[item_start:item_end])
    fixture = fixture.replace("// PRODUCTION_RECHARGE_MAX", source[recharge_start:recharge_end])
    with tempfile.TemporaryDirectory(prefix="player-cloak-ctor-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "cloak_ctor.cpp"
        generated.write_text(fixture)
        executable = Path(temp) / "cloak_ctor.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(executable)],
                       cwd=ROOT, check=True)
        subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
