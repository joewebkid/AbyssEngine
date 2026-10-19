"""Continuous Level entry to the first GL operation, plus cached-atlas CPU tests.

No new GPU call returns success. The earlier skybox upload/material boundaries
remain explicitly inherited from the station gate. Cached texture handles in
the separate CPU test are preconditions, not evidence of an upload.
"""
import argparse
from pathlib import Path
import shutil
import subprocess

from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import prepare, compile_gate, replace_once
from run_particle_control_smoke import function


def prepare_flare(out, cxx, assets):
    prepare(out, cxx, assets)
    host = (out / "station_level_host.inc").read_text()
    marker = "namespace AbyssEngine {\nstatic unsigned int nextTexture = 100;"
    host = replace_once(host, marker,
        "namespace AbyssEngine {\nint flareTextureBoundary(Engine *, const char *, ImageCallback, "
        "void *, unsigned int *, float, AELoadedTexture *, bool);\nstatic unsigned int nextTexture = 100;")
    host = replace_once(host,
        "void *user, unsigned int *out, float,\n                                AELoadedTexture *, bool) {",
        "void *user, unsigned int *out, float scale,\n                                AELoadedTexture *record, bool managed) {\n"
        "    if (std::strstr(path, \"data/textures/gof2_interface\") == path)\n"
        "        return flareTextureBoundary(engine, path, callback, user, out, scale, record, managed);\n"
        "    assert(std::strstr(path, \"/skyboxes/\"));")
    (out / "station_level_host.inc").write_text(host)

    canvas = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    bodies = function(canvas, "struct PCImage2DView {") + ";\n"
    bodies += extract("src/engine/render/PaintCanvas.cpp", [
        "void PaintCanvas::Image2DCreate(", "int PaintCanvas::GetWidth() {",
        "int PaintCanvas::GetHeight() {", "int PaintCanvas::GetImage2DWidth(",
        "int PaintCanvas::GetImage2DHeight(",
    ])
    bodies += "namespace AbyssEngine {\n" + extract("src/engine/render/Engine.cpp", [
        "uint32_t Engine::GetDisplayWidth() {", "uint32_t Engine::GetDisplayHeight() {",
    ]) + extract("src/engine/core/AbyssEngine.cpp", [
        "int MeshCreate(Engine * /*engine*/", "int ImageCreateRegionFromFile(",
    ]) + "}\n"
    bodies += extract("src/engine/render/LensFlare.cpp", [
        "LensFlare::~LensFlare() {", "void LensFlare::update(",
    ])
    # Keep the entire upload body intact. The namespace only separates it from
    # the inherited skybox test service; its first glGetError throws.
    bodies += "namespace FlareUpload {\nusing namespace AbyssEngine;\n"
    bodies += extract("src/engine/core/AbyssEngine.cpp", ["int TextureCreateFromFileIntern(Engine *engine,"])
    bodies += "}\n"
    (out / "lensflare_cpu_under_test.inc").write_text(bodies)

    source = (ROOT / "src/engine/render/BuildResourceList.cpp").read_text()
    platform = function(source, "inline AbyssEngine::Resource *makeRes(")
    platform += "\nvoid registerFlarePlatform(PaintCanvas *canvas) {\n"
    platform += function(source, "if (Globals::iPadHD) {")
    platform += function(source, "else if (Globals::retinaDisplay) {")
    platform += function(source[source.index("else if (Globals::retinaDisplay) {"):], "else {")
    platform += "\n}\n"
    (out / "lensflare_platform_under_test.inc").write_text(platform)

    traps = (ROOT / "tools/level_station_traps.inc").read_text()
    for symbol in ("_ZN11AbyssEngine11PaintCanvas8GetWidthEv", "_ZN11AbyssEngine11PaintCanvas9GetHeightEv"):
        traps = replace_once(traps, f"TRAP({symbol})", "")
    (out / "lensflare_traps.inc").write_text(traps)
    gate = (ROOT / "tools/level_station_asset_gate.cpp").read_text()
    start = gate.index("// Reaching the next unvalidated object")
    end = gate.index("#define TRAP(symbol)", start)
    gate = gate[:start] + '#include "lensflare_gate_services.inc"\n\n' + gate[end:]
    gate = replace_once(gate, '"level_station_traps.inc"', '"lensflare_traps.inc"')
    gate = replace_once(gate, "registerResources(&canvas);", "registerResources(&canvas);\n    registerFlarePlatform(&canvas);")
    gate = replace_once(gate, "canvas.resources.size() == 2490", "canvas.resources.size() == 2505")
    gate = replace_once(gate, "checkManagers(*level, &canvas);", "checkGlBoundary(canvas);\n    checkManagers(*level, &canvas);")
    gate = gate.replace("LensFlare image boundary", "LensFlare -> TextureCreateFromFileIntern -> glGetError boundary")
    gate = replace_once(gate, "int main(int argc, char **argv)", "int stationEntryMain(int argc, char **argv)")
    gate = gate.rstrip()[:-1] + "    return 0;\n}\n"
    (out / "lensflare_station_under_test.inc").write_text(gate)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--asset-root", type=Path, required=True)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")
    out, assets = args.out_dir.resolve(), args.asset_root.resolve()
    out.mkdir(parents=True, exist_ok=True)
    prepare_flare(out, args.cxx, assets)
    for opt in ("-O0", "-O2"):
        print(f"LensFlare continuous/CPU asset gate {opt}", flush=True)
        exe = compile_gate(out, args.cxx, opt, "tools/lensflare_asset_gate.cpp")
        subprocess.run([str(exe), str(assets)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
