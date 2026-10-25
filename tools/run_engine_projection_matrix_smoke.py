"""Exercise Engine projection calls with the production matrix multiply body."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
engine = (ROOT / "src/engine/render/Engine.cpp").read_text()
math = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
multiply = function(math, "void esMatrixMultiply(ESMatrix *out, ESMatrix *a, ESMatrix *b) {")
ortho = function(engine, "void Engine::SetOrthoMatrix(float *projection, float *view, bool multiply) {")
world = function(engine, "void Engine::SetWorldViewMatrix(const Matrix &matrix) {")

HOST = r'''
#include <cassert>
#include <cmath>
#include <cstring>

namespace AbyssEngine {
struct ESMatrix { float m[4][4]; };
'''

ENGINE = r'''
}
struct Matrix {
    float value[12] = {};
    operator const float *() const { return value; }
};
int g_Engine_useShaders = 1;
int matrixGetGlCalls = 0, glLoadCalls = 0;
void MatrixGetGL(const Matrix &, float *) { ++matrixGetGlCalls; }
void glLoadMatrixf(const float *) { ++glLoadCalls; }
struct Engine {
    float projMatrix[16] = {};
    float worldViewMatrixGL[16] = {};
    float worldViewProjMatrix[16] = {};
    float uvMatrixGL[16] = {};
    void SetOrthoMatrix(float *projection, float *view, bool multiply);
    void SetWorldViewMatrix(const Matrix &matrix);
};
'''

MAIN = r'''
int main() {
    Engine engine;
    float projection[16] = {2,0,0,0, 0,3,0,0, 0,0,4,0, 0,0,0,1};
    float translation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 5,7,11,1};

    engine.SetOrthoMatrix(projection, translation, false);
    assert(engine.projMatrix[0] == 2 && engine.projMatrix[12] == 0);
    engine.SetOrthoMatrix(projection, translation, true);
    assert(engine.projMatrix[0] == 2 && engine.projMatrix[5] == 3);
    assert(engine.projMatrix[10] == 4 && engine.projMatrix[15] == 1);
    assert(engine.projMatrix[12] == 10 && engine.projMatrix[13] == 21);
    assert(engine.projMatrix[14] == 44);

    Matrix pose;
    pose.value[0] = pose.value[5] = pose.value[10] = 1;
    pose.value[3] = 5;
    pose.value[7] = 7;
    pose.value[11] = 11;
    engine.SetOrthoMatrix(projection, translation, false);
    engine.SetWorldViewMatrix(pose);
    assert(engine.worldViewMatrixGL[12] == 5);
    assert(engine.worldViewMatrixGL[13] == 7);
    assert(engine.worldViewMatrixGL[14] == 11);
    assert(engine.worldViewProjMatrix[12] == 10);
    assert(engine.worldViewProjMatrix[13] == 21);
    assert(engine.worldViewProjMatrix[14] == 44);

    g_Engine_useShaders = 0;
    engine.SetWorldViewMatrix(pose);
    assert(matrixGetGlCalls == 1 && glLoadCalls == 1);
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="gof2-engine-matrix-") as directory:
        cpp = Path(directory) / "engine_matrix.cpp"
        cpp.write_text(HOST + multiply + ENGINE + ortho + world + MAIN)
        for optimization in ("-O0", "-O2"):
            executable = Path(directory) / ("engine_matrix" + optimization + ".exe")
            subprocess.run(["g++", "-std=c++17", optimization, str(cpp), "-o", str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
    print("Engine projection multiplication: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
