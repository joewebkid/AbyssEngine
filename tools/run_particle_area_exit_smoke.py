"""Compile the two production ParticleSystemSprite bodies with focused services."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def function(source, signature):
    start = source.index(signature)
    brace = source.index("{", start)
    depth = 1
    end = brace + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[start:end] + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/engine/render/ParticleSystemSprite.cpp").read_text()
    extracted = '''#include "engine/render/ParticleSystemSprite.h"
#include "engine/render/ParticleSettingsRef.h"
#define GOF2_ENUM_BlendMode
#include "engine/render/PaintCanvas.h"
#include <cstring>
namespace {
constexpr float kMillisecondsToSeconds = 0.001f;
const ParticleSettings::SetDefinition &currentParticleSet(int set) {
    return ParticleSettingsRef::cur.sets[set];
}
}
'''
    extracted += function(source, "void ParticleSystemSprite::setAlpha(")
    extracted += function(source, "void ParticleSystemSprite::updateAreaExitParticle(")

    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="gof2-particle-area-",
                                     dir=ROOT / "_work") as temp:
        body = Path(temp) / "production.cpp"
        exe = Path(temp) / "smoke.exe"
        body.write_text(extracted)
        subprocess.run([args.cxx, "-std=c++14", "-O0", "-Isrc",
                        "tools/particle_area_exit_smoke.cpp", str(body),
                        "src/engine/core/AERandom.cpp", "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
