"""Exercise production Sprite bodies and ImageFactory reload with a canvas fixture."""

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

    sprite = (ROOT / "src/engine/render/Sprite.cpp").read_text()
    factory = (ROOT / "src/engine/render/ImageFactory.cpp").read_text()
    fixture = (ROOT / "tools/sprite_reload_smoke.cpp").read_text()
    bodies = [function(sprite, name) for name in (
        "Sprite::Sprite(uint32_t image, int frameWidth, int frameHeight)",
        "Sprite::Sprite(uint32_t *frames, int frameCount, int frameWidth, int frameHeight)",
        "Sprite::~Sprite()")]
    bodies += ["void " + function(sprite, name) for name in (
        "Sprite::setFrame(int frame)", "Sprite::setPosition(int x, int y)",
        "Sprite::draw(float scaleX, float scaleY)",
        "Sprite::drawRegion(int srcX, int srcY, int w, int h)")]
    bodies += ["void " + function(factory, "ImageFactory::reload()")]
    fixture = fixture.replace("// PRODUCTION_BODIES", "\n".join(bodies))
    with tempfile.TemporaryDirectory(prefix="sprite-reload-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "sprite_reload_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "sprite_reload_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
