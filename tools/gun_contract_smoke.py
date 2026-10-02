"""Run real selected Gun bodies without the unrelated native host link backlog.

Usage: python tools/gun_contract_smoke.py [Gun.cpp]
This isolates source bodies, not stubs. It verifies host construction, array
ownership and both offset overloads; it does not test ARM gameplay execution.
Also extracts the actual shootAt mount block for an operand/side-effect check.
"""
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def body(source, signature):
    start = source.index(signature)
    opening = source.index("{", start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[start:end] + "\n"


def main():
    source_path = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "src/game/weapons/Gun.cpp"
    source = source_path.read_text()
    selected = '#include "game/weapons/Gun.h"\n#include <new>\n#include <cstdint>\n'
    start = source.index("using VecArray = Array<Vector>;")
    end = source.index("Gun::~Gun()", start)
    selected += source[start:end]
    start = source.index("static const short gunMountPatterns")
    end = source.index("void Gun::setOffset(int", start)
    selected += source[start:end]
    start = source.index("static const float kZOffset")
    end = source.index("void Gun::setOffset(Vector", start)
    selected += source[start:end]
    for signature in ("Gun::Gun(", "Gun::~Gun()", "void Gun::setEnemy(",
                      "void Gun::removeAllEnemies()", "void Gun::setLevel(", "void Gun::setOffset(int",
                      "void Gun::setOffset(Vector"):
        selected += body(source, signature)
    math = (ROOT / "src/engine/math/AEMath.cpp").read_text()
    selected += "namespace AbyssEngine { namespace AEMath {\n"
    selected += body(math, "Vector &Vector::operator=(const Vector &other)")
    selected += body(math, "Vector &Vector::operator*=(float scale)")
    selected += body(math, "Vector &Vector::operator+=(const Vector &other)")
    selected += body(math, "bool operator!=(const Vector &lhs, const Vector &rhs)")
    selected += "}}\n"
    selected += 'void exerciseGunMount(Gun &gun, const Matrix &m) {\n'
    selected += 'using namespace AbyssEngine::AEMath;\nunsigned i = 0;\nVector zero{0,0,0};\n'
    for member in ('offset', 'targetDir', 'positionArray', 'weaponType',
                   'field_0xa4_b0', 'field_0xa5', 'field_0xa6', 'field_0xa7'):
        selected += f'auto &{member} = gun.{member};\n'
    selected += 'Vector &mountDirection = targetDir;\n'
    shoot = body(source, 'bool Gun::shootAt(')
    selected += body(shoot, 'if (offset != zero)') + '}\n'
    work = ROOT / "_work/gun-smoke"
    work.mkdir(parents=True, exist_ok=True)
    isolated = work / "selected.cpp"
    isolated.write_text(selected)
    compiler = "C:/msys64/ucrt64/bin/g++.exe"
    executable = work / "gun_contract_smoke.exe"
    subprocess.run([compiler, "-std=c++14", "-fpermissive", "-Isrc",
                    "tools/gun_contract_smoke.cpp", "tools/gun_mount_contract_smoke.cpp", str(isolated),
                    "-o", str(executable)],
                   cwd=ROOT, check=True)
    subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
