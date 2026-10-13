"""Exercise the production Ship cargo overloads and merge path."""

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

    ship = (ROOT / "src/game/ship/Ship.cpp").read_text()
    item = (ROOT / "src/game/mission/Item.cpp").read_text()
    fixture = (ROOT / "tools/ship_add_cargo_smoke.cpp").read_text()
    methods = [
        function(item, "int Item::getAmount()"),
        function(item, "ItemArray *Item::combineItems(ItemArray *items, ItemArray *stationItems)"),
        function(ship, "void Ship::setCargo(Array<Item *> *cargo)"),
        function(ship, "int Ship::getFreeSpace()"),
        function(ship, "void Ship::addCargo(Array<Item *> *items)"),
        function(ship, "void Ship::addCargo(Item *item)"),
    ]
    fixture = fixture.replace("// PRODUCTION_METHODS", "\n".join(methods))

    with tempfile.TemporaryDirectory(prefix="ship-cargo-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "ship_add_cargo_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "ship_add_cargo_smoke.exe"
        build = subprocess.run([args.cxx, "-std=gnu++14", "-O0", "-Isrc", str(generated),
                                "-o", str(exe)], cwd=ROOT, capture_output=True, text=True)
        if build.returncode:
            print(build.stdout[-4000:])
            print(build.stderr[-4000:])
            build.check_returncode()
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
