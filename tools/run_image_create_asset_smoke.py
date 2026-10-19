"""Parse original skybox textures using production ImageCreateFromFile."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--asset", action="append", type=Path, required=True)
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")
    for asset in args.asset:
        if not asset.is_file():
            parser.error(f"asset does not exist: {asset}")

    source = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    bodies = "\n".join(function(source, signature) for signature in (
        "int ImageCreateFromFile(Engine *engine, const char *path, Image **out) {",
        "void ImageRelease(Image **slot) {",
    ))
    with tempfile.TemporaryDirectory(prefix="image-asset-", dir=ROOT / "_work") as temp:
        (Path(temp) / "image_create_asset_under_test.inc").write_text(
            "namespace AbyssEngine {\n" + bodies + "\n}\n"
        )
        canvas_source = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
        canvas_bodies = "\n".join(function(canvas_source, signature) for signature in (
            "void paintcanvas_ext_add_resource(void *resource, void *resources) {",
            "void *PaintCanvas::FindResource(unsigned short id) {",
            "void PaintCanvas::AddResource(AbyssEngine::Resource *resource) {",
            "void PaintCanvas::TextureCreate(unsigned short resId, void (*callback)",
            "void PaintCanvas::TextureCreate(unsigned short id, unsigned int &out, bool flag) {",
        ))
        (Path(temp) / "image_canvas_under_test.inc").write_text(canvas_bodies + "\n")
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"image_asset_{opt[1:]}.exe"
            subprocess.run([
                args.cxx, "-std=gnu++17", opt, "-Isrc", "-I", temp,
                "tools/image_create_asset_smoke.cpp", "-o", str(exe),
            ], cwd=ROOT, check=True)
            subprocess.run([str(exe), *(str(asset.resolve()) for asset in args.asset)],
                           cwd=ROOT, check=True)
    print(f"ImageCreateFromFile original textures -O0/-O2: {len(args.asset)} passed")


if __name__ == "__main__":
    main()
