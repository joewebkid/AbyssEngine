"""Check the extracted CutScene mode-2 camera setup sequence."""
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
    body = production[production.index("void CutScene::initialize()") :]
    start = body.index("canvas->CameraCreate(this->cameraId74);")
    end_text = "canvas->CameraSetLocal(this->cameraId74, cameraLocal);"
    end = body.index(end_text, start) + len(end_text)
    fragment = body[start:end]
    source = """#include <cassert>
#include <cstdint>
namespace AbyssEngine { namespace AEMath {
struct Matrix { float m[15]; };
float translation[3]{}, rotation[3]{};
Matrix MatrixSetTranslation(Matrix &m, float x, float y, float z) {
    translation[0] = x; translation[1] = y; translation[2] = z;
    m.m[12] = x; m.m[13] = y; m.m[14] = z;
    return m;
}
Matrix MatrixSetRotation(Matrix &m, float x, float y, float z) {
    rotation[0] = x; rotation[1] = y; rotation[2] = z;
    m.m[0] = y;
    return m;
}
}}
using Matrix = AbyssEngine::AEMath::Matrix;
struct AERandom {
    static AERandom *gRandom;
    int bounds[2]{}, count = 0;
    int nextInt(int bound) { bounds[count] = bound; return count++ ? 12345 : 11; }
};
AERandom randomSource;
AERandom *AERandom::gRandom = &randomSource;
struct PaintCanvas {
    Matrix source{}, submitted{};
    unsigned createId = 0, perspectiveId = 0, currentId = 0, readId = 0, writeId = 0;
    float fov = 0, nearPlane = 0, farPlane = 0;
    void CameraCreate(unsigned &id) { id = createId = 7; }
    void CameraSetPerspective(unsigned id, float a, float b, float c) {
        perspectiveId = id; fov = a; nearPlane = b; farPlane = c;
    }
    void CameraSetCurrent(unsigned id) { currentId = id; }
    void *CameraGetLocal(unsigned id) { readId = id; return &source; }
    void CameraSetLocal(unsigned id, const Matrix &m) { writeId = id; submitted = m; }
};
struct CutScene {
    unsigned cameraId74 = 0;
    float cameraRotX = 0;
    void setup(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    PaintCanvas canvas;
    canvas.source.m[0] = 1.0f;
    canvas.source.m[11] = 42.0f;
    CutScene scene;
    scene.setup(&canvas);
    assert(scene.cameraId74 == 7);
    assert(canvas.createId == 7 && canvas.perspectiveId == 7);
    assert(canvas.currentId == 7 && canvas.readId == 7 && canvas.writeId == 7);
    assert(canvas.fov == 0.92f && canvas.nearPlane == 200.0f);
    assert(canvas.farPlane == 200000.0f);
    assert(randomSource.count == 2);
    assert(randomSource.bounds[0] == 20000 && randomSource.bounds[1] == 60000);
    assert(AbyssEngine::AEMath::translation[0] == -19989.0f);
    assert(AbyssEngine::AEMath::translation[1] == 0.0f);
    assert(AbyssEngine::AEMath::translation[2] == 52345.0f);
    assert(scene.cameraRotX == -0.7853982f);
    assert(AbyssEngine::AEMath::rotation[0] == 0.0f);
    assert(AbyssEngine::AEMath::rotation[1] == scene.cameraRotX);
    assert(AbyssEngine::AEMath::rotation[2] == 0.0f);
    assert(canvas.source.m[0] == 1.0f && canvas.source.m[11] == 42.0f);
    assert(canvas.submitted.m[0] == scene.cameraRotX);
    assert(canvas.submitted.m[11] == 42.0f);
    assert(canvas.submitted.m[12] == -19989.0f && canvas.submitted.m[14] == 52345.0f);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-init-mode2-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode2.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
