#!/usr/bin/env python3
"""Compile the current Level constructor body in a small host harness."""

import argparse
import pathlib
import subprocess


def extract_function(source: str, marker: str) -> str:
    start = source.index(marker)
    opening = source.index("{", start)
    depth = 0
    for pos in range(opening, len(source)):
        if source[pos] == "{":
            depth += 1
        elif source[pos] == "}":
            depth -= 1
            if depth == 0:
                return source[start : pos + 1]
    raise ValueError(f"unterminated function: {marker}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", required=True)
    parser.add_argument("--out-dir", type=pathlib.Path, required=True)
    args = parser.parse_args()

    root = pathlib.Path(__file__).resolve().parents[1]
    source = (root / "src/game/world/Level.cpp").read_text(encoding="utf-8")
    ctor = extract_function(source, "Level::Level(int mission) {")
    zero16 = extract_function(source, "static inline void zero16(void *p) {")
    set_stream_out = extract_function(source, "void Level::setInitStreamOut() {")
    args.out_dir.mkdir(parents=True, exist_ok=True)
    extracted = args.out_dir / "level_ctor_extracted.cpp"
    extracted.write_text(
        '#include "game/world/Level.h"\n'
        + "unsigned char Level::initStreamOutPosition;\n"
        + zero16 + "\n" + ctor + "\n" + set_stream_out + "\n",
        encoding="utf-8",
    )

    for opt in ("-O0", "-O2"):
        executable = args.out_dir / f"level_ctor_{opt[2:]}.exe"
        subprocess.run(
            [
                args.cxx, "-std=c++17", opt, "-ffunction-sections", "-fdata-sections",
                "-Wl,--gc-sections", "-I", str(root / "src"),
                str(root / "tools/level_ctor_smoke.cpp"), str(extracted),
                str(root / "src/engine/math/Matrix.cpp"), "-o", str(executable),
            ],
            check=True,
        )
        subprocess.run([str(executable)], check=True)


if __name__ == "__main__":
    main()
