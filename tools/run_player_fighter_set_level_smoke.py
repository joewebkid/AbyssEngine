"""Compile the production PlayerFighter::setLevel body against recording managers."""

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
    begin = source.index("void PlayerFighter::setLevel(Level *lvl)")
    end = source.index("PlayerFighter::PlayerFighter(", begin)
    fixture = (ROOT / "tools/player_fighter_set_level_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="fighter-level-", dir=ROOT / "_work") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        exe = Path(temp) / "smoke.exe"
        subprocess.run([compiler, "-std=gnu++17", "-O0", str(subject), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
