"""Exercise the production Fighter mission-crate setter on the native host."""

from pathlib import Path
import argparse
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

    source = (ROOT / "src/game/ship/PlayerFighter.cpp").read_text()
    begin = source.index("void PlayerFighter::setMissionCrate(bool on)")
    end = source.index("int PlayerFighter::collide(", begin)
    fixture = (ROOT / "tools/player_fighter_mission_crate_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="fighter-mission-crate-") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        exe = Path(temp) / "smoke.exe"
        for opt in ("-O0", "-O2"):
            subprocess.run([args.cxx, "-std=gnu++17", opt, str(subject), "-o", str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
