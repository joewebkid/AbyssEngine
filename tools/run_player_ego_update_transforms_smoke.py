"""Exercise the production PlayerEgo auxiliary-transform update block."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    begin = source.index("// Android Thumb 0x9d940-0x9da58:")
    end = source.index("// Android Thumb 0x9da58-0x9db60:", begin)
    fixture = (ROOT / "tools/player_ego_update_transforms_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_BLOCK", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="ego-update-transforms-", dir=ROOT / "_work") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        executable = Path(temp) / "smoke.exe"
        for opt in ("-O0", "-O2"):
            subprocess.run([compiler, "-std=gnu++14", opt, str(subject), "-o", str(executable)],
                           cwd=ROOT, check=True)
            subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
