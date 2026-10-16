"""Exercise production AEGeometry transform, construction, and LOD routing."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
METHODS = (
    "AEGeometry::AEGeometry(PaintCanvas *canvas)",
    "AEGeometry::AEGeometry(uint16_t mesh, PaintCanvas *canvas, bool flag)",
    "Vector AEGeometry::getPosition()",
    "Vector AEGeometry::getRightVector()",
    "Vector AEGeometry::getUpVector()",
    "Vector AEGeometry::getDirection()",
    "Vector AEGeometry::getParentPosition()",
    "Matrix &AEGeometry::getMatrix()",
    "void AEGeometry::updateReferenceMatrix()",
    "void AEGeometry::addChild(uint32_t child)",
    "void AEGeometry::setMatrix(const Matrix &m)",
    "void AEGeometry::setRotation(float x, float y, float z)",
    "void AEGeometry::rotate(float x, float y, float z)",
    "void AEGeometry::setScaling(float x, float y, float z)",
    "void AEGeometry::setLodChildMeshes(uint16_t *meshes)",
    "void AEGeometry::setLodChildTransform(uint32_t param)",
)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/engine/render/AEGeometry.cpp").read_text()
    fixture = (ROOT / "tools/aegeometry_canvas_calls_smoke.cpp").read_text()
    methods = "\n".join(function(source, signature) for signature in METHODS)
    with tempfile.TemporaryDirectory(prefix="aegeometry-canvas-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "smoke.cpp"
        generated.write_text(fixture.replace("// PRODUCTION_METHODS", methods))
        exe = Path(temp) / "smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
