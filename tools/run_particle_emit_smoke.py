"""Compile and exercise the production IParticleSystem::emit body in isolation."""
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

    ips = (ROOT / "src/engine/render/IParticleSystem.cpp").read_text()
    math = (ROOT / "src/engine/math/AEMath.cpp").read_text()
    source = '''#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSettingsRef.h"
#include <cstdint>
char *MatrixGetPosition(char *, Matrix const *);
char *MatrixGetRight(char *, Matrix const *);
char *MatrixGetUp(char *, Matrix const *);
char *MatrixGetDir(char *, Matrix const *);
namespace AbyssEngine { namespace AEMath {
Vector operator-(const Vector &);
Vector operator-(const Vector &, const Vector &);
Vector operator+(const Vector &, const Vector &);
Vector operator*(const Vector &, float);
Vector operator*(float, const Vector &);
float VectorDot(const Vector &, const Vector &);
'''
    for signature in ("Vector &Vector::operator=(", "Vector &Vector::operator+=(",
                      "Vector &Vector::operator-=(", "Vector &Vector::operator/=(",
                      "Vector operator-(const Vector &value)",
                      "Vector operator-(const Vector &lhs, const Vector &rhs)",
                      "Vector operator+(const Vector &lhs, const Vector &rhs)",
                      "Vector operator*(const Vector &lhs, float rhs)",
                      "Vector operator*(float lhs, const Vector &rhs)",
                      "float VectorDot(const Vector &lhs, const Vector &rhs)"):
        source += function(math, signature)
    source += "}}\n"
    for signature in ("static char *particleSetDefinition(",
                      "static inline int float_bits(", "static inline float bits_float(",
                      "static inline void zero_vec(", "void IParticleSystem::emit(int delta)",
                      "float *IParticleSystem::rotateUVs("):
        source += function(ips, signature)
    source += '''
static char *matrix_column(char *out, Matrix const *matrix, int column) {
    Vector *v = reinterpret_cast<Vector *>(out);
    v->x = matrix->m[column];
    v->y = matrix->m[column + 4];
    v->z = matrix->m[column + 8];
    return out;
}
char *MatrixGetPosition(char *out, Matrix const *matrix) { return matrix_column(out, matrix, 3); }
char *MatrixGetRight(char *out, Matrix const *matrix) { return matrix_column(out, matrix, 0); }
char *MatrixGetUp(char *out, Matrix const *matrix) { return matrix_column(out, matrix, 1); }
char *MatrixGetDir(char *out, Matrix const *matrix) { return matrix_column(out, matrix, 2); }
'''

    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="gof2-particle-emit-", dir=ROOT / "_work") as temp:
        body = Path(temp) / "production.cpp"
        body.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / ("smoke-" + level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, "-Isrc",
                            "tools/particle_emit_smoke.cpp", str(body),
                            "src/engine/render/ParticleSettings.cpp",
                            "src/engine/render/ParticleSettingsRef.cpp",
                            "src/engine/core/AERandom.cpp", "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
