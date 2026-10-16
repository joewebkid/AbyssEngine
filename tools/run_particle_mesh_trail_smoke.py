"""Exercise the production ParticleSystemMesh::emitTrail body without a GPU."""

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

    source = (ROOT / "src/engine/render/ParticleSystemMesh.cpp").read_text()
    trail_source = (ROOT / "src/engine/render/ParticleSystemMeshTrail.cpp").read_text()
    fixture = (ROOT / "tools/particle_mesh_trail_smoke.cpp").read_text()
    bodies = "\n".join((
        function(source, "void ParticleSystemMesh::emit(int id)"),
        function(trail_source, "void ParticleSystemMesh::emitTrail(int delta)"),
        function(source, "void ParticleSystemMesh::updateSingle(int id, float delta)"),
        function(source, "void ParticleSystemMesh::updateTrailEdges(int id, int delta)"),
        function(source, "void ParticleSystemMesh::updateSingleColor(int id)"),
    ))
    with tempfile.TemporaryDirectory(prefix="particle-trail-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "smoke.cpp"
        generated.write_text(fixture.replace("// PRODUCTION_METHODS", bodies))
        for level in ("-O0", "-O2"):
            exe = Path(temp) / ("smoke-" + level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=gnu++17", level, str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
