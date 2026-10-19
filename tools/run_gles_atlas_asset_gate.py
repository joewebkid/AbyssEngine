"""Continuous original-asset Level -> LensFlare -> real EGL/GLES atlas gate.

Windows uses existing ANGLE DLLs in place; Android builds use system EGL/GLES.
No original assets or binaries are copied. Earlier skybox/material/bootstrap
fixture services remain bounded and are not evidence of a complete runtime.
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess

from run_level_init_asset_gate import ROOT, extract
from run_lensflare_asset_gate import prepare_flare
from run_level_station_asset_gate import replace_once


def prepare_gles(out, cxx, assets, reuse=False):
    if not reuse:
        prepare_flare(out, cxx, assets)
    mesh = (out / "station_mesh_host.inc").read_text()
    old = '''std::ifstream file(assetBase + "/" + path, std::ios::binary);
        assert(file.good());
        input.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());'''
    if old in mesh:
        mesh = replace_once(mesh, old, "input = readFixtureAsset(assetBase, path);")
    mesh = mesh.replace('offsetof(PCResourceView, payload) == 16',
                        'offsetof(PCResourceView, payload) == (sizeof(void *) == 4 ? 12 : 16)')
    mesh = mesh.replace('offsetof(PCMeshInfoView, matResId) == 8',
                        'offsetof(PCMeshInfoView, matResId) == sizeof(void *)')
    (out / "station_mesh_host.inc").write_text(mesh)
    host = (out / "station_level_host.inc").read_text()
    marker = '    assert(id == 20010 || id == 20011 || id == 20004 || id == 20003 || id == 3);'
    diagnostic = ('    if (!(id == 20010 || id == 20011 || id == 20004 || id == 20003 || id == 3))\n'
                  '        std::fprintf(stderr, "Unexpected material %u after %d opens\\n", id, opened);\n')
    if diagnostic not in host:
        host = replace_once(host, marker, diagnostic + marker)
    (out / "station_level_host.inc").write_text(host)
    gate = (out / "lensflare_station_under_test.inc").read_text()
    gate = replace_once(gate, 'std::ifstream file(assetBase + "/" + name, std::ios::binary);',
                        'bool present = !readFixtureAsset(assetBase, name).empty();')
    gate = replace_once(gate, 'return file.good();', 'return present;')
    gate = replace_once(gate, '"lensflare_gate_services.inc"', '"gles_atlas_services.inc"')
    gate = replace_once(gate, "PaintCanvas canvas(&engine);", """PaintCanvas canvas(&engine);
    ApplicationManager manager(&engine);
    manager.paintCanvas = &canvas;
    engine.appManager = &manager;
    engine.textureByteCounter = 0;
    engine.linearFilterFlag = 0;
    engine.displayWidth = 1280;
    engine.displayHeight = 720;""")
    gate = replace_once(gate, "opened == 14", "opened == 18")
    gate = replace_once(gate, '"data/assets/main/3d/meshes/jumpgates/jumpgate_terran_lod_2.aem",',
        '"data/assets/main/3d/meshes/jumpgates/jumpgate_terran_lod_2.aem",\n'
        + '        "data/textures/gof2_interface.aei",\n' * 4)
    gate = gate.replace("LensFlare -> TextureCreateFromFileIntern -> glGetError boundary",
                        "LensFlare completed -> SolarSystem::getStations boundary")
    (out / "gles_atlas_station_under_test.inc").write_text(gate)
    traps = (out / "lensflare_traps.inc").read_text()
    traps = traps.replace("TRAP(_ZN11SolarSystem11getStationsEv)", "")
    (out / "lensflare_traps.inc").write_text(traps)
    (out / "gles_atlas_compat_under_test.inc").write_text(
        extract("src/engine/core/AbyssEngine.cpp", ["unsigned int AELabelObject("])
        + '\nextern "C" {\n' + extract("src/engine/render/GLES1Compat.cpp", ["void glTexEnvi("]) + "\n}\n")
    # Always refresh the changing production body, including with --reuse-inputs.
    bodies = (out / "lensflare_cpu_under_test.inc").read_text()
    bodies = bodies[:bodies.index("namespace FlareUpload {")]
    bodies += "namespace FlareUpload {\nusing namespace AbyssEngine;\n"
    bodies += extract("src/engine/core/AbyssEngine.cpp", ["int TextureCreateFromFileIntern(Engine *engine,"])
    (out / "lensflare_cpu_under_test.inc").write_text(bodies + "\n}\n")


SOURCES = [
    "engine/render/Mesh", "engine/math/Vector", "engine/math/Matrix", "engine/math/AEMath",
    "engine/math/BSphere", "engine/math/Transform", "engine/math/Quaternion", "engine/core/KeyFrame",
    "engine/math/BoundingVolume", "engine/math/BoundingAAB", "engine/math/BoundingSphere",
    "engine/core/AERandom", "game/core/String", "engine/render/ResourceMesh",
    "engine/render/ResourceTexture", "game/ship/PlayerStatic", "game/ship/PlayerStaticFar",
    "game/ship/PlayerStation", "engine/render/AEGeometry", "engine/render/AEGeometryPosition",
    "game/ship/PlayerJumpgate",
]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--asset-root", type=Path, required=True)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--angle-dir", type=Path, required=True)
    parser.add_argument("--egl-include", type=Path, required=True)
    parser.add_argument("--cxx", default=shutil.which("g++"))
    parser.add_argument("--reuse-inputs", action="store_true", help="reuse already generated inherited fixtures")
    args = parser.parse_args()
    out, assets, angle = args.out_dir.resolve(), args.asset_root.resolve(), args.angle_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)
    prepare_gles(out, args.cxx, assets, args.reuse_inputs)
    environment = os.environ.copy()
    environment["PATH"] = str(angle) + os.pathsep + environment["PATH"]
    for opt in ("-O0", "-O2"):
        exe = out / f"gles_atlas_{opt[1:]}.exe"
        subprocess.run([args.cxx, "-std=gnu++17", opt, "-g0", "-ffunction-sections", "-fdata-sections",
            "-Isrc", "-Ithird_party/jni", "-Ithird_party/gl", "-Itools", "-I", str(out),
            "-idirafter", str(args.egl_include), "tools/gles_atlas_asset_gate.cpp",
            *[f"src/{s}.cpp" for s in SOURCES], str(angle / "libEGL.dll"), str(angle / "libGLESv2.dll"),
            "-Wl,--gc-sections", "-o", str(exe)], cwd=ROOT, check=True)
        print(f"Real EGL/GLES original-asset gate {opt}", flush=True)
        subprocess.run([str(exe), str(assets)], env=environment, cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
