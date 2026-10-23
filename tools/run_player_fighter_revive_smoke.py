"""Exercise the production Fighter revive method with recording dependencies."""

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
    begin = source.index("void PlayerFighter::revive() {")
    end = source.index("// Static data members present", begin)
    fixture = (ROOT / "tools/player_fighter_revive_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="fighter-revive-", dir=ROOT / "_work") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        executable = Path(temp) / "smoke.exe"
        for opt in ("-O0", "-O2"):
            subprocess.run([compiler, "-std=gnu++14", opt, str(subject), "-o", str(executable)],
                           cwd=ROOT, check=True)
            subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
