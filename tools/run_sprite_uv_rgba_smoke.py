"""Compile the three production sprite data mutators in a small host fixture."""

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

    source = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    fixture = (ROOT / "tools/sprite_uv_rgba_smoke.cpp").read_text()
    for marker, signature in (
        ("// PRODUCTION_SET_ALL_UV", "void SpriteSystemSetAllUv("),
        ("// PRODUCTION_SET_UV", "void SpriteSystemSetUv("),
        ("// PRODUCTION_SET_RGBA", "void SpriteSystemSetRGBA("),
    ):
        fixture = fixture.replace(marker, function(source, signature))

    with tempfile.TemporaryDirectory(prefix="sprite-uv-rgba-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "sprite_uv_rgba_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "sprite_uv_rgba_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
