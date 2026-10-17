"""Compile the production file stack with one host asset boundary."""
import argparse
import os
from pathlib import Path
import subprocess


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--asset-dir", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    asset = args.asset_dir.resolve() / "weapons_hd.bin"
    if not asset.is_file():
        parser.error("--asset-dir must contain weapons_hd.bin")
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    compiler_env = os.environ.copy()
    compiler_env.update({"TMP": str(out), "TEMP": str(out), "TMPDIR": str(out)})
    # Isolate the unchanged production loader so unrelated FileRead parsers
    # do not drag the whole unfinished game into this host link.
    source = (ROOT / "src/engine/file/FileRead.cpp").read_text()
    begin = source.index("Array<Array<Vector *> *> *FileRead::loadWeaponPositions(")
    end = source.index("Array<SpacePoint *> *FileRead::loadSpacePoints(", begin)
    extracted = out / "weapon_loader_production.cpp"
    extracted.write_text(
        '#include "engine/file/FileRead.h"\n'
        'using AbyssEngine::Vector;\n'
        'FileRead::FileRead() {}\nFileRead::~FileRead() {}\n'
        + source[begin:end]
    )
    sources = [
        "tools/weapon_positions_file_stack_smoke.cpp",
        "src/engine/file/AEFile.cpp",
        "src/engine/file/AENormalFile.cpp",
        "src/engine/file/AEPakFile.cpp",
        str(extracted),
        "src/game/core/String.cpp",
    ]
    for opt in ("-O0", "-O2"):
        exe = out / f"weapon_file_stack_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=c++17", opt, "-fpermissive", "-ffunction-sections",
            "-fdata-sections", "-Wl,--gc-sections", "-Isrc", "-Ithird_party/jni",
            "-Ithird_party/gl", "-Ithird_party/fmod/inc", *sources,
            "-o", str(exe),
        ], cwd=ROOT, env=compiler_env, check=True)
        subprocess.run([str(exe), str(asset.parent)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
