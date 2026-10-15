"""Compile the production PlayerFighter::update prelude with recording dependencies."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/ship/PlayerFighter.cpp").read_text()
    begin = source.index("void PlayerFighter::update(int dt) {")
    end = source.index("void PlayerFighter::setPosition(", begin)
    fixture = (ROOT / "tools/player_fighter_update_prelude_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="fighter-update-", dir=ROOT / "_work") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        exe = Path(temp) / "smoke.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(subject), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
