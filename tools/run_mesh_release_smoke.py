"""Run production mesh and sprite release bodies with GL and transform fixtures."""

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

    source = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    fixture = (ROOT / "tools/mesh_release_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_RELEASE", "\n".join(function(source, signature) for signature in (
        "void MeshRelease(Engine *engine, Mesh **slot)",
        "void MeshReleaseIntern(Engine * /*engine*/, Mesh **slot)",
        "void SpriteSystemRelease(Engine *engine, SpriteSystem **slot)")))
    with tempfile.TemporaryDirectory(prefix="mesh-release-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "mesh_release_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "mesh_release_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
