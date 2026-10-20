"""Compile and exercise the production Fighter cloak and exhaust methods."""

from pathlib import Path
import argparse
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def extract(source, begin, end):
    start = source.index(begin)
    return source[start:source.index(end, start)]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/game/ship/PlayerFighter.cpp").read_text()
    methods = "\n".join((
        extract(source, "void PlayerFighter::cloak(int dur, bool b)", "void PlayerFighter::setMissionCrate("),
        extract(source, "void PlayerFighter::setExhaustVisible(bool vis)", "void PlayerFighter::render()"),
        extract(source, "void PlayerFighter::handleCloaking()", "void PlayerFighter::revive()"),
    ))
    fixture = (ROOT / "tools/player_fighter_cloak_effect_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHODS", methods)
    with tempfile.TemporaryDirectory(prefix="fighter-cloak-") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        exe = Path(temp) / "smoke.exe"
        for opt in ("-O0", "-O2"):
            subprocess.run([args.cxx, "-std=gnu++14", opt, str(subject), "-o", str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
