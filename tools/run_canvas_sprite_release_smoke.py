"""Run production canvas sprite release with a mesh-release fixture."""

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

    canvas = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    core = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    fixture = (ROOT / "tools/canvas_sprite_release_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_CANVAS", function(
        canvas, "void PaintCanvas::ReleaseSpriteSystemResource(unsigned int index)"))
    fixture = fixture.replace("// PRODUCTION_RELEASE", function(
        core, "void SpriteSystemRelease(Engine *engine, SpriteSystem **slot)"))
    with tempfile.TemporaryDirectory(prefix="canvas-sprite-release-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "canvas_sprite_release_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "canvas_sprite_release_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
