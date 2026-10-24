"""Exercise production AEFile with a small synthetic raw PAK."""
import argparse
from pathlib import Path
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    out = args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=out) as temp:
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"aefile_pak_{opt[1:]}.exe"
            subprocess.run([
                args.cxx, "-std=gnu++17", opt, "-I.", "-Isrc",
                "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                "tools/aefile_pak_smoke.cpp", "host/ReadOnlyAssetFileInterface.cpp",
                "src/engine/file/AEFile.cpp", "src/engine/file/AENormalFile.cpp",
                "src/engine/file/AEPakFile.cpp", "src/game/core/String.cpp",
                "-o", str(exe),
            ], cwd=ROOT, check=True)
            subprocess.run([str(exe), temp], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
