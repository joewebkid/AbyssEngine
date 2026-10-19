"""Extend the continuous Level gate through production station construction.

All game/geometry bodies are extracted verbatim or compiled from their source.
File access, engine/canvas bootstrap, material/GPU upload and unexecuted
fail-closed links are host boundaries. The fixture documents the stopping point.
"""
import argparse
from pathlib import Path
import shutil
import subprocess
import sys

from run_level_init_asset_gate import ROOT, extract


def replace_once(text, old, new):
    assert text.count(old) == 1, f"expected one host boundary: {old}"
    return text.replace(old, new)


def prepare(out, cxx, asset_root):
    # Retain the earlier gate as a regression and reuse its resource/parser inputs.
    subprocess.run([sys.executable, "tools/run_level_init_asset_gate.py", "--cxx", cxx,
                    "--out-dir", str(out), "--asset-root", str(asset_root)], cwd=ROOT, check=True)
    base = (ROOT / "tools/level_init_asset_gate.cpp").read_text()
    base = base[:base.index("// Unexecuted dependencies below")]
    base = replace_once(base, '#include "mesh_create_smoke.cpp"', '#include "station_mesh_host.inc"')
    base = replace_once(base, 'assert(id == 20010 || id == 20011);',
                        'auto *r = static_cast<Resource *>(FindResource(id));\n'
                        '    assert(id == 20010 || id == 20011 || id == 20004 || id == 20003 || id == 3);\n'
                        '    assert((r && r->kind == 6) || (!r && id == 3));\n'
                        '    stationMaterialIds.push_back(id);')
    (out / "station_level_host.inc").write_text(base)
    mesh_host = (ROOT / "tools/mesh_create_smoke.cpp").read_text()
    # Replace the old successful animation mocks with actual production services.
    start = mesh_host.index("Transform::Transform(Transform *)")
    end = mesh_host.index('\n}\n\n#include "mesh_read_math', start)
    mesh_host = mesh_host[:start] + mesh_host[end:]
    mesh_host = replace_once(mesh_host, 'int MeshConvertToVBO(Mesh *) { std::abort(); }', '')
    mesh_host = replace_once(mesh_host,
        'void MeshRelease(Engine *, Mesh **slot) { ++released; dispose(*slot); *slot = nullptr; }',
        'void MeshRelease(Engine *, Mesh **) { std::abort(); }')
    mesh_host = replace_once(mesh_host, '#include "mesh_read_math_under_test.inc"', '')
    mesh_host = replace_once(mesh_host, '#include "mesh_read_bounds_under_test.inc"', '')
    mesh_host = replace_once(mesh_host, 'std::string assetBase;',
                             'std::string assetBase;\nstd::vector<std::string> stationOpenedPaths;\n'
                             'std::vector<unsigned> stationMaterialIds;')
    mesh_host = replace_once(mesh_host, '++opened;', '++opened;\n    stationOpenedPaths.push_back(path);')
    (out / "station_mesh_host.inc").write_text(mesh_host)

    sources = extract("src/game/ship/KIPlayer.cpp", [
        "KIPlayer::KIPlayer(", "KIPlayer::~KIPlayer() {", "void KIPlayer::setVisible(",
        "uint8_t KIPlayer::isVisible()",
    ])
    sources += extract("src/game/ship/Player.cpp", [
        "Player::Player(", "void Player::updateDamageRate() {",
        "void Player::setKIPlayer(", "void Player::setRadius(", "void Player::setMaxHitpoints(",
    ])
    sources += extract("src/engine/file/FileRead.cpp", [
        "FileRead::FileRead() {", "FileRead::~FileRead() {",
        "Array<int32_t> *FileRead::loadStationCollision(",
        "Array<int32_t> *FileRead::loadStaticCollision(",
    ])
    sources += extract("src/engine/file/AEFile.cpp", ["uint32_t AEFile::Read(int32_t &value,"])
    sources += extract("src/game/mission/Status.cpp", ["bool Status::dlc1Won()", "bool Status::gameWon()"])
    sources += extract("src/game/world/SolarSystem.cpp", [
        "int SolarSystem::getRace()", "bool SolarSystem::currentOrbitHasWarpGate()",
    ])
    sources += extract("src/engine/render/LODManager.cpp", ["void LODManager::addObject("])
    sources += extract("src/engine/render/PaintCanvas.cpp", [
        "void PaintCanvas::TransformCreate(unsigned int &out) {",
        "void PaintCanvas::TransformAddMeshId(", "void PaintCanvas::TransformAddMesh(",
        "void PaintCanvas::TransformAddChild(", "void *PaintCanvas::TransformGetLocal(",
        "void *PaintCanvas::TransformGetTransform(",
        "void PaintCanvas::TransformSetLocal(",
    ])
    star = (ROOT / "src/game/world/StarSystem.cpp").read_text()
    sources += star[star.index("static const uint16_t g_StarSystem_ctor_systemPlanetTextures"):
                    star.index("StarSystem::StarSystem(int mode) {")]
    sources += extract("src/game/world/StarSystem.cpp", [
        "static inline uint32_t flat_count(", "static inline void *flat_data(",
        "StarSystem::StarSystem(int mode) {",
    ])
    sources += extract("src/engine/render/LensFlare.cpp", ["LensFlare::LensFlare("])
    sources += "namespace AbyssEngine {\nint MeshConvertToVBOIntern(Mesh *);\nint TransformConvertToVBO(Transform *);\n"
    sources += extract("src/engine/core/AbyssEngine.cpp", ["int MeshConvertToVBO(Mesh *mesh) {"]) + "}\n"
    (out / "station_chain_under_test.inc").write_text(sources)
    return out


def compile_gate(out, cxx, opt, fixture="tools/level_station_asset_gate.cpp"):
    exe = out / f"level_station_asset_{opt[1:]}.exe"
    sources = [
        "engine/render/Mesh", "engine/math/Vector", "engine/math/Matrix", "engine/math/AEMath",
        "engine/math/BSphere", "engine/math/Transform", "engine/math/Quaternion", "engine/core/KeyFrame",
        "engine/math/BoundingVolume", "engine/math/BoundingAAB", "engine/math/BoundingSphere",
        "engine/core/AERandom", "game/core/String", "engine/render/ResourceMesh",
        "engine/render/ResourceTexture", "game/ship/PlayerStatic", "game/ship/PlayerStaticFar",
        "game/ship/PlayerStation", "engine/render/AEGeometry", "engine/render/AEGeometryPosition",
        "game/ship/PlayerJumpgate",
    ]
    subprocess.run([cxx, "-std=gnu++17", opt, "-ffunction-sections", "-fdata-sections",
                    "-Isrc", "-Ithird_party/jni", "-Ithird_party/gl", "-Itools", "-I", str(out),
                    fixture, *[f"src/{s}.cpp" for s in sources],
                    "-Wl,--gc-sections", "-o", str(exe)], cwd=ROOT, check=True)
    return exe


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
    prepare(out, args.cxx, args.asset_root.resolve())
    for opt in ("-O0", "-O2"):
        print(f"Production station asset gate {opt}", flush=True)
        exe = compile_gate(out, args.cxx, opt)
        subprocess.run([str(exe), str(args.asset_root.resolve())], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
