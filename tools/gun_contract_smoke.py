"""Run real selected Gun bodies without the unrelated native host link backlog.

Usage: python tools/gun_contract_smoke.py
This isolates source bodies, not stubs. It verifies host construction, array
ownership and both offset overloads; it does not test ARM gameplay execution.
"""
from pathlib import Path
import subprocess

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
    source = (ROOT / "src/game/weapons/Gun.cpp").read_text()
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
    selected += "}}\n"
    work = ROOT / "_work/gun-smoke"
    work.mkdir(parents=True, exist_ok=True)
    isolated = work / "selected.cpp"
    isolated.write_text(selected)
    compiler = "C:/msys64/ucrt64/bin/g++.exe"
    executable = work / "gun_contract_smoke.exe"
    subprocess.run([compiler, "-std=c++14", "-fpermissive", "-Isrc",
                    "tools/gun_contract_smoke.cpp", str(isolated),
                    "-o", str(executable)],
                   cwd=ROOT, check=True)
    subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
