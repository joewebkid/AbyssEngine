"""Exercise unchanged portrait factory/part bodies with fixture canvas services."""

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

    factory = (ROOT / "src/engine/render/ImageFactory.cpp").read_text()
    part = (ROOT / "src/engine/render/ImagePart.cpp").read_text()
    fixture = (ROOT / "tools/image_factory_portrait_smoke.cpp").read_text()
    bodies = [function(factory, "ImageFactory::ImageFactory()"),
              "int *" + function(factory, "ImageFactory::createChar(int race)"),
              "int *" + function(factory, "ImageFactory::createChar(bool isMale, int race)"),
              "void *" + function(factory, "ImageFactory::loadImage(int row, int col, int frameBase)"),
              "Array<ImagePart *> *" + function(factory, "ImageFactory::loadChar(int *desc)"),
              "void " + function(factory, "ImageFactory::drawChar(Array<ImagePart *> *parts, int x, int y, bool flag)"),
              "void " + function(factory, "ImageFactory::drawShip(int shipId, int x, int y)"),
              "void " + function(factory, "ImageFactory::drawItem(int itemId, int x, int y)"),
              "void " + function(factory, "ImageFactory::drawItem(int itemId, int frame, int x, int y)"),
              function(part, "ImagePart::ImagePart(unsigned id, int field04, int posY)"),
              "void " + function(part, "ImagePart::draw(int x, int y, bool b)")]
    fixture = fixture.replace("// PRODUCTION_BODIES", "\n".join(bodies))
    with tempfile.TemporaryDirectory(prefix="image-factory-portrait-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "portrait_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "portrait_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", "-Isrc", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
