"""Exercise production PaintCanvas mesh/sprite creation bodies on a 64-bit host."""

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
    parser.add_argument("--opt", choices=("-O0", "-O2"), default="-O0")
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    signatures = (
        "void PaintCanvas::SpriteSystemCreate(unsigned short resId, bool flag,",
        "void PaintCanvas::SpriteSystemCreate(unsigned short resId,\n",
        "void PaintCanvas::MeshCreate(unsigned short vertexCount, unsigned short triangleCount,",
        "void PaintCanvas::MeshCreate(unsigned short a, unsigned short b,",
    )
    bodies = "\n".join(function(source, signature) for signature in signatures)
    fixture = (ROOT / "tools/paintcanvas_resource_create_smoke.cpp").read_text()
    with tempfile.TemporaryDirectory(prefix="resource-create-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "smoke.cpp"
        generated.write_text(fixture.replace("// PRODUCTION_METHODS", bodies))
        exe = Path(temp) / "smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++17", args.opt, str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
