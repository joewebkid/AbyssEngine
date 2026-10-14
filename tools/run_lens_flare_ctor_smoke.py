"""Run production LensFlare construction and rendering with canvas fixtures."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    with tempfile.TemporaryDirectory(prefix="lens-flare-smoke-", dir=ROOT / "_work") as temp:
        source = (ROOT / "src/engine/render/LensFlare.cpp").read_text()
        begin = source.index("// ELF .rodata")
        extracted = Path(temp) / "lens_flare_ctor.cpp"
        extracted.write_text(
            '#include "engine/render/LensFlare.h"\n'
            '#include "engine/render/PaintCanvas.h"\n'
            '#include "game/core/Globals.h"\n' + source[begin:]
        )
        exe = Path(temp) / "lens_flare_smoke.exe"
        subprocess.run([
            compiler, "-std=gnu++17", "-O0", "-Isrc", "-Ithird_party/fmod/inc",
            "tools/lens_flare_ctor_smoke.cpp", str(extracted),
            "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("LensFlare constructor/render host smoke: OK")


if __name__ == "__main__":
    main()
