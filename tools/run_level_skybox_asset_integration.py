"""Run the production Level skybox selector through canvas loaders on original assets."""

import argparse
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
ASSETS = (
    "data/assets/main/3d/meshes/skyboxes/skybox_stars.aem",
    "data/assets/main/3d/textures/low/etc/skyboxes/skybox_stars_001.aei",
    "data/assets/main/3d/meshes/skyboxes/skybox_002.aem",
    "data/assets/main/3d/textures/low/etc/skyboxes/skybox_002.aei",
)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--asset-root", type=Path, required=True)
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")
    root = args.asset_root.resolve()
    for name in ASSETS:
        if not (root / name).is_file():
            parser.error(f"asset missing: {name}")

    with tempfile.TemporaryDirectory(prefix="level-skybox-asset-", dir=ROOT / "_work") as temp:
        subprocess.run([
            sys.executable, "tools/run_mesh_read_color_smoke.py", "--compiler", args.cxx,
            "--output-dir", temp, "--create-smoke",
        ], cwd=ROOT, check=True)
        source = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
        bodies = "\n".join(function(source, signature) for signature in (
            "int ImageCreateFromFile(Engine *engine, const char *path, Image **out) {",
            "void ImageRelease(Image **slot) {",
        ))
        (Path(temp) / "level_image_under_test.inc").write_text(
            "namespace AbyssEngine {\n" + bodies + "\n}\n"
        )
        canvas = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
        (Path(temp) / "level_texture_canvas_under_test.inc").write_text(
            "\n".join(function(canvas, signature) for signature in (
                "void PaintCanvas::TextureCreate(unsigned short resId, void (*callback)",
                "void PaintCanvas::TextureCreate(unsigned short id, unsigned int &out, bool flag) {",
            )) + "\n"
        )
        level = (ROOT / "src/game/world/Level.cpp").read_text()
        begin = level.index("void Level::createSpace() {")
        end = level.index("    int mode = this->missionPtr;", begin)
        (Path(temp) / "level_skybox_under_test.inc").write_text(level[begin:end] + "}\n")
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"level_skybox_asset_{opt[1:]}.exe"
            subprocess.run([
                args.cxx, "-std=gnu++17", opt, "-ffunction-sections", "-fdata-sections",
                "-Isrc", "-I", temp, "tools/level_skybox_asset_integration.cpp",
                "src/engine/render/Mesh.cpp", "src/engine/math/Vector.cpp",
                "-Wl,--gc-sections", "-o", str(exe),
            ], cwd=ROOT, check=True)
            subprocess.run([str(exe), str(root)], cwd=ROOT, check=True)
    print("Level skybox selector -> canvas -> original assets -O0/-O2: OK")


if __name__ == "__main__":
    main()
