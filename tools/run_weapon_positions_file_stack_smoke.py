"""Compile production core-resource loaders with a host file boundary."""
import argparse
import os
from pathlib import Path
import subprocess

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--asset-dir", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    asset_dir = args.asset_dir.resolve()
    for name in ("weapons_hd.bin", "items.bin", "ships.bin"):
        if not (asset_dir / name).is_file():
            parser.error(f"--asset-dir must contain {name}")
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    compiler_env = os.environ.copy()
    compiler_env.update({"TMP": str(out), "TEMP": str(out), "TMPDIR": str(out)})
    # Isolate the unchanged production loaders so unrelated FileRead parsers
    # do not drag the whole unfinished game into this host link.
    source = (ROOT / "src/engine/file/FileRead.cpp").read_text()
    begin = source.index("Array<Array<Vector *> *> *FileRead::loadWeaponPositions(")
    end = source.index("Array<SpacePoint *> *FileRead::loadSpacePoints(", begin)
    extracted = out / "weapon_loader_production.cpp"
    items_begin = source.index("Array<Item *> *FileRead::loadItemsBinary(")
    ships_begin = source.index("Array<Ship *> *FileRead::loadShipsBinary(", items_begin)
    item_source = (ROOT / "src/game/mission/Item.cpp").read_text()
    ship_source = (ROOT / "src/game/ship/Ship.cpp").read_text()
    extracted.write_text(
        '#include "engine/file/FileRead.h"\n'
        '#include "game/mission/Item.h"\n#include "game/ship/Ship.h"\n'
        '#include "game/core/Globals.h"\n#include "game/mission/Status.h"\n'
        '#include "game/world/Standing.h"\n'
        'using AbyssEngine::Vector;\n'
        'FileRead::FileRead() {}\nFileRead::~FileRead() {}\n'
        + source[begin:end]
        + source[items_begin:ships_begin]
        + source[ships_begin:]
        + function(item_source, "Item::~Item()")
        + function(item_source, "void Item::init()")
        + function(item_source, "Item::Item(IntArray *ingredients_")
        + function(item_source, "int Item::getAttribute(")
        + function(item_source, "int Item::getSort()")
        + function(item_source, "int Item::getIndex()")
        + function(item_source, "int Item::getTotalPrice()")
        + function(ship_source, "Ship::Ship(int index,")
        + function(ship_source, "Ship::~Ship()")
        + function(ship_source, "int Ship::getCargoValue()")
        + function(ship_source, "void Ship::refreshValue()")
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
        subprocess.run([str(exe), str(asset_dir)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
