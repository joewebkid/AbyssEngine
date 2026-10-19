"""Execute the 2,488-slot table and the bounded complete registration tail."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile
import re
import zipfile
import sys

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    parser.add_argument("--obb", type=Path, help="optional original Android OBB path check")
    parser.add_argument("--arm-evidence", action="store_true", help="compare all tail records with original ARM oracle")
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
    tail = source[source.index("    // ARM platform tail begin:"):
                  source.index("    // ARM platform tail end:")]
    hd = "void registerHdResources(AbyssEngine::PaintCanvas *canvas) {\n" + tail + "\n}\n"
    oracle_header = ""
    oracle_counts = []
    if args.arm_evidence:
        sys.path.insert(0, str(ROOT / "tools/verify"))
        from test_build_resource_hd_tail_arm import TailOracle
        oracle = TailOracle()
        oracle_header = "struct ExpectedTail { unsigned short id; int kind; unsigned int image; const char *path; };\n"
        for case in range(16):
            flags = tuple((case >> bit) & 1 for bit in range(4))
            rows = oracle.run(*flags)
            oracle_counts.append(len(rows))
            oracle_header += f"static const ExpectedTail expectedTail{case}[] = {{\n"
            for _, rid, kind, data in rows:
                payload = f'0, "{data}"' if kind == 2 else f"{data}u, nullptr"
                oracle_header += f"{{{rid}, {kind}, {payload}}},\n"
            oracle_header += "};\n"
        oracle_header += "static const ExpectedTail *expectedTails[] = {" + ",".join(
            f"expectedTail{case}" for case in range(16)) + "};\n"
        oracle_header += "static const unsigned int expectedTailCounts[] = {" + ",".join(
            str(count) for count in oracle_counts) + "};\n#define HAVE_ARM_TAIL_ORACLE 1\n"

    if args.obb:
        paths = set(re.findall(r'"(data/[^"\n]+)"', hd))
        with zipfile.ZipFile(args.obb) as archive:
            names = set(archive.namelist())
            missing = sorted(path for path in paths
                             if "47947_GOF2CONTENT_ETC/assets/" + path not in names)
        if missing:
            raise RuntimeError("platform texture paths absent from OBB: " + str(missing))
        print(f"Original Android OBB: {len(paths)}/{len(paths)} platform texture paths present")

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
        function(texture, "ResourceTexture::~ResourceTexture() {"),
        function(material, "ResourceMaterial::ResourceMaterial(uint16_t texId, uint16_t texId2, BlendMode blend) {"),
        function(material, "ResourceMaterial::ResourceMaterial(uint16_t texId, BlendMode blend) {"),
        function(mesh, "ResourceMesh::ResourceMesh(const char *name, unsigned short id, bool flag)"),
    )) + "\n}\n"
    with tempfile.TemporaryDirectory(prefix="full-resource-table-", dir=ROOT / "_work") as temp:
        temp_path = Path(temp)
        (temp_path / "full_resource_table_under_test.inc").write_text(table)
        (temp_path / "full_resource_hd_under_test.inc").write_text(hd)
        (temp_path / "full_resource_tail_oracle.inc").write_text(oracle_header)
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
    print("Full resource table and complete platform/post-platform registration tail -O0/-O2: OK")


if __name__ == "__main__":
    main()
