"""Exercise the production world-vector screen projection with a tiny camera fixture."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    fixture = (ROOT / "tools/getscreen_world_smoke.cpp").read_text()
    signature = "bool PaintCanvas::GetScreenPosition(AbyssEngine::AEMath::Matrix &m,"
    fixture = fixture.replace("// PRODUCTION_METHOD", function(source, signature))
    fixture = fixture.replace("// PRODUCTION_WRAPPER", function(
        source, "int PaintCanvas::GetScreenPosition(const AbyssEngine::AEMath::Vector &a,"))

    with tempfile.TemporaryDirectory(prefix="getscreen-world-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "getscreen_world_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "getscreen_world_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
