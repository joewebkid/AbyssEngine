"""Compile the real control bodies in isolation, without GPU/resource shims.

The host PE linker retains unrelated unresolved references even with section GC.
This extracts unchanged function text from the production files; no tested body
is replaced. Automatic emit is the only untested base-vtable boundary stub.
Usage: python tools/run_particle_control_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
"""
import argparse
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def function(text, signature):
    start = text.index(signature)
    brace = text.index("{", start)
    depth = 1
    end = brace + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end] + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")
    out = ROOT / "_work/particle-smoke"
    out.mkdir(parents=True, exist_ok=True)
    ips = (ROOT / "src/engine/render/IParticleSystem.cpp").read_text()
    manager = (ROOT / "src/engine/render/ParticleSystemManager.cpp").read_text()
    math = (ROOT / "src/engine/math/AEMath.cpp").read_text()
    text = '''#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSystemManager.h"
#include "engine/render/ParticleSettingsRef.h"
#include <cstdlib>
namespace AbyssEngine { namespace AEMath {
Vector operator*(const Vector &, float);
Vector operator*(float, const Vector &);
'''
    for signature in ("Vector &Vector::operator=(", "Vector &Vector::operator+=(",
                      "Vector &Vector::operator-=(", "Vector operator*(const Vector &lhs, float",
                      "Vector operator*(float lhs, const Vector &"):
        text += function(math, signature)
    text += "}}\nvoid IParticleSystem::emit(int) { std::abort(); }\n"
    for signature in ("static char *particleSetDefinition(", "static inline float bits_float(",
                      "void IParticleSystem::enableUpdate(", "void IParticleSystem::enableEmit(",
                      "void IParticleSystem::enableRender(", "void IParticleSystem::setParticleSetIndex(",
                      "float *IParticleSystem::rotateUVs(", "void IParticleSystem::emitManual("):
        text += function(ips, signature)
    text += function(manager, "ParticleSystemManager::ParticleSystemManager(")
    text += function(manager, "void ParticleSystemManager::construct(")
    for signature in ("setParticleSetByIndex", "enableSystemUpdate", "enableSystemRender",
                      "enableSystemEmit", "resetSystem"):
        text += function(manager, "void ParticleSystemManager::" + signature + "(")
    first = manager.index("void ParticleSystemManager::emitManual(")
    text += function(manager[first:], "void ParticleSystemManager::emitManual(")
    second = manager.index("void ParticleSystemManager::emitManual(", first + 1)
    text += function(manager[second:], "void ParticleSystemManager::emitManual(")
    source = out / "production_control_bodies.cpp"
    source.write_text(text)
    exe = out / "particle_control_smoke.exe"
    command = [args.cxx, "-std=c++17", "-O0", "-Isrc", "tools/particle_control_smoke.cpp",
               str(source), "src/engine/render/ParticleSettings.cpp",
               "src/engine/render/ParticleSettingsRef.cpp", "src/engine/core/AERandom.cpp",
               "-o", str(exe)]
    subprocess.run(command, cwd=ROOT, check=True)
    subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
