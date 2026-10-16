"""Compile and exercise both production PaintCanvas::DrawTextLines overloads."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
SIGNATURE = "void PaintCanvas::DrawTextLines(unsigned int font,"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    starts = []
    cursor = 0
    while (start := source.find(SIGNATURE, cursor)) != -1:
        starts.append(start)
        cursor = start + len(SIGNATURE)
    assert len(starts) == 3
    methods = "\n".join(function(source[start:], SIGNATURE) for start in starts[1:])
    fixture = (ROOT / "tools/paintcanvas_draw_text_lines_smoke.cpp").read_text()

    with tempfile.TemporaryDirectory(prefix="draw-text-lines-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "smoke.cpp"
        generated.write_text(fixture.replace("// PRODUCTION_METHODS", methods))
        exe = Path(temp) / "smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
