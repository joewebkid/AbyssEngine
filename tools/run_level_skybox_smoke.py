"""Compile and exercise the production Level::createSpace skybox prelude."""

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
    begin = source.index("void Level::createSpace() {")
    end = source.index("    int mode = this->missionPtr;", begin)
    with tempfile.TemporaryDirectory(prefix="level-skybox-", dir=ROOT / "_work") as temp:
        extracted = Path(temp) / "level_skybox_under_test.inc"
        extracted.write_text(source[begin:end] + "}\n")
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"level_skybox_smoke_{opt[1:]}.exe"
            subprocess.run([
                compiler, "-std=gnu++17", opt, "-I", temp,
                "tools/level_skybox_smoke.cpp", "-o", str(exe),
            ], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("Level skybox source-extracted -O0/-O2 smoke: OK")


if __name__ == "__main__":
    main()
