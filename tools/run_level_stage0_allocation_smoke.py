"""Exercise the production Level::init stage-0 allocation block on a 64-bit host."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def function(source, signature):
    start = source.index(signature)
    opening = source.index("{", start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[start:end] + "\n"


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    level = (ROOT / "src/game/world/Level.cpp").read_text()
    psm = (ROOT / "src/engine/render/ParticleSystemManager.cpp").read_text()
    lod = (ROOT / "src/engine/render/LODManager.cpp").read_text()
    start = level.index("        LODManager *lod =", level.index("int Level::init()"))
    end = level.index("        this->field_94 = psm;", start) + len("        this->field_94 = psm;")
    block = level[start:end]
    assert block.count("::operator new(sizeof(LODManager))") == 1
    assert block.count("::operator new(sizeof(ParticleSystemManager))") == 10
    with tempfile.TemporaryDirectory(prefix="level-stage0-", dir=ROOT / "_work") as temp:
        extracted = Path(temp) / "level_stage0_under_test.inc"
        extracted.write_text(
            function(lod, "LODManager::LODManager()")
            + function(psm, "ParticleSystemManager::ParticleSystemManager(")
            + function(psm, "void ParticleSystemManager::construct()")
            + "void Stage0Fixture::run() {\n" + block + "\n}\n"
        )
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"level_stage0_{opt[1:]}.exe"
            subprocess.run([
                compiler, "-std=gnu++17", opt, "-Isrc", "-I", temp,
                "tools/level_stage0_allocation_smoke.cpp", "-o", str(exe),
            ], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("Level stage-0 production allocation smoke -O0/-O2: OK")


if __name__ == "__main__":
    main()
