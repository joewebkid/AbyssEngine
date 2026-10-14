"""Run the production SystemPathFinder against a tiny route fixture."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    with tempfile.TemporaryDirectory(prefix="pathfinder-smoke-", dir=ROOT / "_work") as temp:
        exe = Path(temp) / "pathfinder_smoke.exe"
        subprocess.run([
            compiler, "-std=gnu++17", "-O0", "-Isrc", "-Ithird_party/fmod/inc",
            "tools/system_pathfinder_smoke.cpp",
            "src/game/world/SystemPathFinder.cpp", "src/engine/core/Node.cpp",
            "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("SystemPathFinder host smoke: OK")


if __name__ == "__main__":
    main()
