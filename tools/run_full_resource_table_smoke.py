"""Compile and execute all 2,488 production resource records and their bulk handoff."""

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

    source = (ROOT / "src/engine/render/BuildResourceList.cpp").read_text()
    prelude = source[source.index("namespace AbyssEngine {\n    struct Resource {"):
                     source.index("\n\nvoid BuildResourceList")]
    start = source.index("    Resource *resources[2488];")
    end_marker = "    canvas->SetResourceList(resources, 2488);"
    end = source.index(end_marker, start) + len(end_marker)
    table = ("\n".join((prelude, "void buildResourceTable(AbyssEngine::PaintCanvas *canvas) {",
                        "    using namespace AbyssEngine;", source[start:end], "}")))
    hd = "void registerHdResources(AbyssEngine::PaintCanvas *canvas) {\n" + \
         function(source, "if (Globals::iPadHD) {") + "\n}\n"

    canvas = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    setter = canvas[canvas.index("void PaintCanvas::SetResourceList("):]
    setter = setter[:setter.index("\n}") + 2]
    texture = (ROOT / "src/engine/render/ResourceTexture.cpp").read_text()
    material = (ROOT / "src/engine/render/ResourceMaterial.cpp").read_text()
    mesh = (ROOT / "src/engine/render/ResourceMesh.cpp").read_text()
    string = (ROOT / "src/game/core/String.cpp").read_text()
    constructors = "namespace AbyssEngine {\n" + "\n".join((
        function(string, "int String::GetStringLength(const char *s) {"),
        function(texture, "ResourceTexture::ResourceTexture(const char *name, float value) {"),
        function(material, "ResourceMaterial::ResourceMaterial(uint16_t texId, uint16_t texId2, BlendMode blend) {"),
        function(material, "ResourceMaterial::ResourceMaterial(uint16_t texId, BlendMode blend) {"),
        function(mesh, "ResourceMesh::ResourceMesh(const char *name, unsigned short id, bool flag)"),
    )) + "\n}\n"
    with tempfile.TemporaryDirectory(prefix="full-resource-table-", dir=ROOT / "_work") as temp:
        temp_path = Path(temp)
        (temp_path / "full_resource_table_under_test.inc").write_text(table)
        (temp_path / "full_resource_hd_under_test.inc").write_text(hd)
        (temp_path / "full_resource_set_under_test.inc").write_text(
            "namespace AbyssEngine {\n" + setter + "\n}\n")
        (temp_path / "full_resource_ctors_under_test.inc").write_text(constructors)
        for opt in ("-O0", "-O2"):
            exe = temp_path / f"full_resource_table_{opt[1:]}.exe"
            result = subprocess.run([
                args.cxx, "-std=gnu++17", opt, "-ffunction-sections", "-fdata-sections",
                "-Isrc", "-I", str(temp_path), "tools/full_resource_table_smoke.cpp",
                "-Wl,--gc-sections", "-o", str(exe),
            ], cwd=ROOT, capture_output=True, text=True)
            if result.returncode:
                raise RuntimeError("host compile failed:\n" + "\n".join(result.stderr.splitlines()[-20:]))
            subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("Full resource table and iPadHD branch -O0/-O2: OK")


if __name__ == "__main__":
    main()
