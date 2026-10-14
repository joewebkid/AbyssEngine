"""Run the production StarSystem flare handoff with camera fixtures."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/world/StarSystem.cpp").read_text()
    begin = source.index("void StarSystem::render2D()")
    end = source.index("static const uint16_t g_StarSystem_ctor", begin)
    with tempfile.TemporaryDirectory(prefix="starsystem-flare-", dir=ROOT / "_work") as temp:
        extracted = Path(temp) / "starsystem_flare.cpp"
        extracted.write_text(
            '#include "game/world/StarSystem.h"\n'
            '#include "engine/render/PaintCanvas.h"\n'
            '#include "game/core/Globals.h"\n' + source[begin:end]
        )
        exe = Path(temp) / "starsystem_flare_smoke.exe"
        subprocess.run([
            compiler, "-std=gnu++17", "-O0", "-Isrc", "-Ithird_party/fmod/inc",
            "tools/starsystem_flare_smoke.cpp", str(extracted), "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("StarSystem flare handoff host smoke: OK")


if __name__ == "__main__":
    main()
