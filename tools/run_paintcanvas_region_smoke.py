"""Compile and exercise the production ten-argument PaintCanvas region body."""

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
    fixture = (ROOT / "tools/paintcanvas_region_smoke.cpp").read_text()
    signature = "void PaintCanvas::DrawRegion2D(unsigned int index, int srcX, int srcY,"
    fixture = fixture.replace("// PRODUCTION_METHOD", function(source, signature))

    with tempfile.TemporaryDirectory(prefix="region-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "paintcanvas_region_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "paintcanvas_region_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
