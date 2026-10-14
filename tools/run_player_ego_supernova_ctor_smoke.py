"""Compile the production PlayerEgo supernova constructor tail in isolation."""

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
    start = source.index("    this->gunYawGeo = nullptr;", source.index("PlayerEgo::PlayerEgo("))
    end = source.index("\n}", start)
    fixture = (ROOT / "tools/player_ego_supernova_ctor_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_SUPERNOVA_TAIL", source[start:end])
    with tempfile.TemporaryDirectory(prefix="player-supernova-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "supernova_ctor.cpp"
        generated.write_text(fixture)
        executable = Path(temp) / "supernova_ctor.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(executable)],
                       cwd=ROOT, check=True)
        subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
