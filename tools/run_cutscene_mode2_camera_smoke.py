"""Check the extracted CutScene mode-2 camera matrix call sequence."""
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
    body = production[production.index("void CutScene::process(int"):]
    start = body.index("Matrix local = *static_cast<const Matrix *>")
    end = body.index("canvas->CameraSetLocal(this->cameraId74, local);", start)
    fragment = body[start:end + len("canvas->CameraSetLocal(this->cameraId74, local);")]
    source = """#include <cassert>
#include <cstdint>
namespace AbyssEngine { namespace AEMath {
struct Matrix { float m[15]; };
float capturedX = 1, capturedY = 0, capturedZ = 1;
Matrix MatrixSetRotation(Matrix &m, float x, float y, float z) {
    capturedX = x; capturedY = y; capturedZ = z;
    m.m[0] = y;
    return m;
}
}}
using Matrix = AbyssEngine::AEMath::Matrix;
struct PaintCanvas {
    Matrix source{}, submitted{};
    unsigned readId = 0, writeId = 0;
    void *CameraGetLocal(unsigned id) { readId = id; return &source; }
    void CameraSetLocal(unsigned id, const Matrix &m) { writeId = id; submitted = m; }
};
struct CutScene {
    unsigned cameraId74 = 7;
    float cameraRotX = 0.75f;
    void cameraStep(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    PaintCanvas canvas;
    canvas.source.m[0] = 1.0f;
    canvas.source.m[12] = 42.0f;
    CutScene scene;
    scene.cameraStep(&canvas);
    assert(canvas.readId == 7 && canvas.writeId == 7);
    assert(AbyssEngine::AEMath::capturedX == 0.0f);
    assert(AbyssEngine::AEMath::capturedY == 0.75f);
    assert(AbyssEngine::AEMath::capturedZ == 0.0f);
    assert(canvas.source.m[0] == 1.0f && canvas.source.m[12] == 42.0f);
    assert(canvas.submitted.m[0] == 0.75f && canvas.submitted.m[12] == 42.0f);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-mode2-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode2.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
