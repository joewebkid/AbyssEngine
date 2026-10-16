"""Exercise production sprite init, alpha and update calls without a GPU."""

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

    source = (ROOT / "src/engine/render/ParticleSystemSprite.cpp").read_text()
    fixture = (ROOT / "tools/particle_sprite_alpha_smoke.cpp").read_text()
    bodies = "\n".join((
        function(source, "void ParticleSystemSprite::init(uint32_t spriteId, uint16_t idOffset)"),
        function(source, "void ParticleSystemSprite::setAlpha(int index, uint32_t color, float alpha)"),
        function(source, "void ParticleSystemSprite::updateSingle(int index, float dt)"),
    ))
    with tempfile.TemporaryDirectory(prefix="particle-sprite-alpha-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "smoke.cpp"
        generated.write_text(fixture.replace("// PRODUCTION_METHODS", bodies))
        for level in ("-O0", "-O2"):
            exe = Path(temp) / ("smoke-" + level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=gnu++17", level, str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
