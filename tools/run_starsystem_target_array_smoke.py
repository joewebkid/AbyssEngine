"""Check actual constructor allocation statements with pointer-width storage."""
import argparse
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--out-dir", type=Path, required=True)
args = parser.parse_args()
out = args.out_dir.resolve()
out.mkdir(parents=True, exist_ok=True)
source = (ROOT / "src/game/world/StarSystem.cpp").read_text()
start = source.index("    this->playerTargets = new Array<KIPlayer *>();")
end = source.index("    this->planetsArray = new Array<AEGeometry *>();", start)
(out / "starsystem_target_array_under_test.inc").write_text(source[start:end])
for opt in ("-O0", "-O2"):
    exe = out / f"target_array_{opt[1:]}.exe"
    subprocess.run(["C:/msys64/ucrt64/bin/g++.exe", "-std=c++17", opt, "-Isrc", "-I", str(out),
                    "tools/starsystem_target_array_smoke.cpp", "-o", str(exe)], cwd=ROOT, check=True)
    subprocess.run([str(exe)], cwd=ROOT, check=True)
