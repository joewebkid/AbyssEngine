"""Exercise the production Level wingman-removal method."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/world/Level.cpp").read_text()
    begin = source.index("void Level::wingmanDied(")
    end = source.index("static inline void zero16", begin)
    fixture = (ROOT / "tools/level_wingman_died_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="level-wingman-died-", dir=ROOT / "_work") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        executable = Path(temp) / "smoke.exe"
        for opt in ("-O0", "-O2"):
            subprocess.run([compiler, "-std=gnu++14", opt, "-Isrc", str(subject), "-o", str(executable)],
                           cwd=ROOT, check=True)
            subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
