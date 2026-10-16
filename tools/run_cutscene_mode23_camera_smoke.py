"""Exercise the extracted CutScene mode-23 camera setup on the host."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    production = (ROOT / "src/game/core/CutScene.cpp").read_text()
    initialize = production[production.index("void CutScene::initialize() {"):]
    start = initialize.index("this->turretGeom = nullptr;", initialize.index("this->mode == 0x17"))
    end = initialize.index("} else if (this->mode == 4)", start)
    fragment = initialize[start:end]
    source = """#include <cassert>
#include <cmath>
struct Vector { float x, y, z; };
struct Matrix { float v[15]{}; };
namespace AbyssEngine { namespace AEMath {
Matrix MatrixSetRotation(Matrix &m, float x, float y, float z) {
    assert(x == 0.0f && std::fabs(y - 0.3926991f) < 0.000001f && z == 0.0f);
    m.v[0] = y;
    return m;
}
Matrix MatrixSetTranslation(Matrix &m, float x, float y, float z) {
    m.v[12] = x; m.v[13] = y; m.v[14] = z;
    return m;
}
}}
struct AEGeometry { Vector getPosition() { return {11.0f, 22.0f, 33.0f}; } };
struct KIPlayer { AEGeometry *geometry; };
struct Enemies { KIPlayer *lead; KIPlayer *operator[](int i) { assert(i == 0); return lead; } };
struct Level { Enemies *enemies; Enemies *getEnemies() { return enemies; } };
struct PaintCanvas {
    Matrix camera{}, transform{}, submitted{};
    unsigned cameraId = 0, transformId = 0, currentId = 0, attachedParent = 0, attachedChild = 0;
    void CameraCreate(unsigned &id) { id = 7; }
    void CameraSetPerspective(unsigned id, float fov, float near, float far) {
        assert(id == 7 && fov == 0.8f && near == 200.0f && far == 100000.0f);
    }
    void CameraSetCurrent(unsigned id) { currentId = id; }
    void *CameraGetLocal(unsigned id) { cameraId = id; return &camera; }
    void CameraSetLocal(unsigned id, const Matrix &value) { assert(id == 7); submitted = value; }
    void TransformCreate(unsigned &id) { id = 9; }
    void *TransformGetLocal(unsigned id) { transformId = id; return &transform; }
    void TransformAddChild(unsigned parent, unsigned child) {
        attachedParent = parent; attachedChild = child;
    }
};
struct CutScene {
    void *turretGeom = reinterpret_cast<void *>(1);
    unsigned cameraId70 = 0, transformId78 = 0;
    Level *level;
    int resetCalls = 0, turretCalls = 0;
    void resetCamera() { ++resetCalls; }
    void checkForTurret() { ++turretCalls; }
    void initializeMode23(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    AEGeometry geometry;
    KIPlayer player{&geometry};
    Enemies enemies{&player};
    Level level{&enemies};
    PaintCanvas canvas;
    canvas.camera.v[5] = 42.0f;
    CutScene scene;
    scene.level = &level;
    scene.initializeMode23(&canvas);
    assert(scene.turretGeom == nullptr);
    assert(scene.cameraId70 == 7 && scene.transformId78 == 9);
    assert(canvas.currentId == 7 && canvas.cameraId == 7 && canvas.transformId == 9);
    assert(canvas.submitted.v[0] == 0.3926991f && canvas.submitted.v[5] == 42.0f);
    assert(canvas.submitted.v[12] == 300.0f && canvas.submitted.v[13] == 700.0f);
    assert(canvas.submitted.v[14] == 3000.0f && canvas.camera.v[12] == 0.0f);
    assert(canvas.transform.v[12] == 11.0f && canvas.transform.v[13] == 22.0f);
    assert(canvas.transform.v[14] == 33.0f);
    assert(canvas.attachedParent == 9 && canvas.attachedChild == 7);
    assert(scene.resetCalls == 1 && scene.turretCalls == 1);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-mode23-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode23.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
