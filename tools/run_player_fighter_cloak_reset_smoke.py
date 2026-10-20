"""Compile production Fighter cloak/reset methods with recording dependencies."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def extract(source, begin, end):
    start = source.index(begin)
    return source[start:source.index(end, start)]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/ship/PlayerFighter.cpp").read_text()
    methods = "\n".join((
        extract(source, "void PlayerFighter::setCloakingPossible(bool v)",
                "void PlayerFighter::removeTrail()"),
        extract(source, "void PlayerFighter::cloak(int dur, bool b)",
                "void PlayerFighter::setMissionCrate("),
        extract(source, "void PlayerFighter::reset()", "void PlayerFighter::handleCloaking()"),
    ))
    fixture = (ROOT / "tools/player_fighter_cloak_reset_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHODS", methods)
    with tempfile.TemporaryDirectory(prefix="fighter-reset-", dir=ROOT / "_work") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        exe = Path(temp) / "smoke.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(subject), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
