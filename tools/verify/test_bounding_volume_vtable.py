"""Check the five ARM virtual slots against the original Android ELF."""

import argparse
import re
import subprocess
from pathlib import Path


TABLES = {
    "BoundingVolume": (0x20DD8C, "14BoundingVolume"),
    "BoundingSphere": (0x20DE68, "14BoundingSphere"),
    "BoundingAAB": (0x20D834, "11BoundingAAB"),
}
METHODS = (
    "18getCollisionNormalE",
    "6updateEfff",
    "7collideEfff",
    "12outerCollideEfff",
    "25projectCollisionOnSurfaceE",
)


def relocations(readelf, path, section):
    output = subprocess.check_output([readelf, "-rW", str(path)], text=True)
    entries = {}
    active = False
    for line in output.splitlines():
        header = re.match(r"^Relocation section '([^']+)'", line)
        if header:
            active = header.group(1) == section
        if not active:
            continue
        match = re.match(r"^\s*([0-9a-fA-F]{8})\s+\S+\s+R_ARM_ABS32\s+\S+\s+(\S+)", line)
        if match:
            entries[int(match.group(1), 16)] = match.group(2)
    return entries


def check_table(readelf, path, section, start, class_id, label):
    symbols = subprocess.check_output([readelf, "-sW", str(path)], text=True)
    sizes = [int(parts[2]) for line in symbols.splitlines()
             if (parts := line.split()) and len(parts) >= 8 and parts[-1] == f"_ZTV{class_id}"]
    if not sizes or any(size != 28 for size in sizes):
        raise AssertionError(f"{label}: expected a 28-byte vtable, got {sizes}")
    entries = relocations(readelf, path, section)
    for index, method in enumerate(METHODS):
        expected = "__cxa_pure_virtual" if class_id == "14BoundingVolume" and index == 4 else f"_ZN{class_id}{method}"
        offset = start + 8 + 4 * index
        actual = entries.get(offset, "<missing>")
        if not actual.startswith(expected):
            raise AssertionError(f"{label} slot {index} at {offset:#x}: expected {expected}, got {actual}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--readelf", required=True)
    parser.add_argument("--original", required=True)
    parser.add_argument("--objects", required=True, help="ARM verifier base/engine/math directory")
    args = parser.parse_args()
    for name, (address, class_id) in TABLES.items():
        check_table(args.readelf, args.original, ".rel.dyn", address, class_id, f"original {name}")
        object_path = Path(args.objects) / f"{name}.o"
        check_table(args.readelf, object_path, ".rel.data.rel.ro", 0, class_id, f"compiled {name}")
    print("BoundingVolume/Sphere/AAB: all 15 ARM virtual slots match the original")


if __name__ == "__main__":
    main()
