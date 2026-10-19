"""Run intact Level ctor/init/createSpace bodies to an explicit station boundary.

This is a bounded host gate, not full Level initialization or a game executable.
External game state and GPU/material services are fixture boundaries; AEM/AEI
bytes, Level layout, managers, resource selection and parsers are production.
"""

import argparse
from pathlib import Path
import shutil
import subprocess
import sys

from run_particle_control_smoke import function

ROOT = Path(__file__).resolve().parents[1]


def extract(path, signatures):
    source = (ROOT / path).read_text()
    return "\n".join(function(source, signature) for signature in signatures)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--asset-root", type=Path, required=True)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")
    out = args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)
    subprocess.run([
        sys.executable, "tools/run_mesh_read_color_smoke.py",
        "--compiler", args.cxx, "--output-dir", str(out), "--create-smoke",
    ], cwd=ROOT, check=True)

    level = (ROOT / "src/game/world/Level.cpp").read_text()
    helper_start = level.index("struct PlayerEgoVtable {")
    helper_end = level.index("#if __SIZEOF_POINTER__", helper_start)
    (out / "level_init_under_test.inc").write_text(
        level[helper_start:helper_end]
        + extract("src/game/world/Level.cpp", [
            "static inline void zero16(void *p) {", "Level::Level(int mission) {",
            "int Level::init() {", "void Level::createSpace() {",
        ])
        + extract("src/engine/render/LODManager.cpp", ["LODManager::LODManager() {"])
        + extract("src/engine/render/ParticleSystemManager.cpp", [
            "ParticleSystemManager::ParticleSystemManager(",
            "void ParticleSystemManager::construct()",
        ])
    )
    (out / "level_image_under_test.inc").write_text(
        "namespace AbyssEngine {\n" + extract("src/engine/core/AbyssEngine.cpp", [
            "int ImageCreateFromFile(Engine *engine, const char *path, Image **out) {",
            "void ImageRelease(Image **slot) {",
        ]) + "\n}\n"
    )
    (out / "level_texture_canvas_under_test.inc").write_text(
        extract("src/engine/render/PaintCanvas.cpp", [
            "void PaintCanvas::TextureCreate(unsigned short resId, void (*callback)",
            "void PaintCanvas::TextureCreate(unsigned short id, unsigned int &out, bool flag) {",
            "void PaintCanvas::SetResourceList(AbyssEngine::Resource * const * list, unsigned int count) {",
        ])
    )
    table = (ROOT / "src/engine/render/BuildResourceList.cpp").read_text()
    start = table.index("#define ADD_RES(")
    macro = table[start:table.index("\n\nvoid BuildResourceList", start)]
    start = table.index("    Resource *resources[2488];")
    end_marker = "    canvas->SetResourceList(resources, 2488);"
    entries = table[start:table.index(end_marker, start) + len(end_marker)]
    low = (ROOT / "src/engine/render/LowResourceTable.cpp").read_text()
    blocks = []
    for resource_id in (0x2767, 0x2753):
        pos = low.index(f"res->id = 0x{resource_id:x};")
        blocks.append(low[low.rfind("    {", 0, pos):low.index("    }", pos) + 5])
    (out / "level_resource_under_test.inc").write_text(
        "namespace AbyssEngine {\n" + function(table, "struct ResourceImage {") + ";\n}\n"
        + function(table, "inline AbyssEngine::ResourceImage *newImage(")
        + macro + "\nvoid registerResources(PaintCanvas *canvas) {\n"
        "using namespace AbyssEngine;\n" + entries + "\n" + "\n".join(blocks) + "\n}\n"
        + "namespace AbyssEngine {\n" + extract("src/engine/render/ResourceMaterial.cpp", [
            "ResourceMaterial::ResourceMaterial(uint16_t texId, uint16_t texId2, BlendMode blend) {",
            "ResourceMaterial::ResourceMaterial(uint16_t texId, BlendMode blend) {",
        ]) + "\n}\n"
    )
    (out / "level_status_under_test.inc").write_text(
        "static int g_emptyOrbitMask = 0;\n"
        + extract("src/game/mission/Status.cpp", [
            "Status::Status() {", "SolarSystem *Status::getSystem() {",
            "Station *Status::getStation() {", "int Status::getCurrentCampaignMission() {",
            "bool Status::inAlienOrbit() {", "int Status::inSupernovaSystem() {",
            "uint32_t Status::inPlanetRingOrbit() {", "bool Status::inStormOrbit() {",
            "bool Status::inFogSkyboxOrbit() {", "bool Status::inEmptyOrbit() {",
        ])
        + extract("src/game/world/Station.cpp", [
            "Station::Station()", "int Station::getIndex() {",
            "bool Station::isAttackedByAliens() {",
        ])
        + extract("src/game/world/SolarSystem.cpp", [
            "SolarSystem::SolarSystem(", "int SolarSystem::getIndex() {",
            "int SolarSystem::getTextureIndex() {",
        ])
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"level_init_asset_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=gnu++17", opt, "-ffunction-sections", "-fdata-sections",
            "-Isrc", "-I", str(out), "tools/level_init_asset_gate.cpp",
            "src/engine/render/Mesh.cpp", "src/engine/math/Vector.cpp",
            "src/engine/math/Matrix.cpp", "src/engine/core/AERandom.cpp",
            "src/game/core/String.cpp", "src/engine/render/ResourceMesh.cpp",
            "src/engine/render/ResourceTexture.cpp", "-Wl,--gc-sections", "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe), str(args.asset_root.resolve())], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
