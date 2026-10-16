"""Exercise the extracted mode-23 transform update from CutScene::process."""
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
    process = production[production.index("void CutScene::process(int"):]
    lead = process.index("AEGeometry *leadGeom = (*enemies)[0]->geometry;")
    start = process.rfind("Array<KIPlayer *> *enemies = this->level->getEnemies();", 0, lead)
    end = process.index("\n    } else if (this->mode == 4)", lead)
    fragment = process[start:end]
    source = """#include <cassert>
#include <cstdint>
struct Matrix { float value[15]{}; };
namespace AbyssEngine {
namespace AEMath {
Matrix MatrixSetRotation(Matrix &m, float x, float y, float z) {
    assert(x == 0.0f && y == 0.5f && z == 0.0f);
    m.value[0] = y;
    return m;
}
}
struct Transform {
    int calls = 0;
    long long lastDelta = 0;
    void Update(long long delta, bool flag) {
        assert(!flag);
        lastDelta = delta;
        ++calls;
    }
};
}
struct PaintCanvas {
    Matrix source{}, submitted{};
    AbyssEngine::Transform first, second;
    unsigned readId = 0, writeId = 0;
    void *TransformGetLocal(unsigned id) { readId = id; return &source; }
    void TransformSetLocal(unsigned id, const Matrix &m) { writeId = id; submitted = m; }
    void *TransformGetTransform(unsigned id) {
        assert(id == 11 || id == 22);
        return id == 11 ? static_cast<void *>(&first) : static_cast<void *>(&second);
    }
};
struct AEGeometry { unsigned transform; };
struct KIPlayer { AEGeometry *geometry; };
template <typename T> struct Array {
    T items[3];
    unsigned size() const { return 3; }
    T operator[](unsigned i) const { return items[i]; }
};
struct Level {
    Array<KIPlayer *> enemies;
    Array<KIPlayer *> *getEnemies() { return &enemies; }
};
struct CutScene {
    Level *level;
    float cameraRotX = 0.5f;
    uint32_t frameDelta;
    void transformStep(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    AEGeometry firstGeom{11}, secondGeom{22};
    KIPlayer first{&firstGeom}, second{&secondGeom};
    Level level{{{&first, &second, nullptr}}};
    const uint32_t deltas[] = {33, 0x80000001u};
    for (uint32_t dt : deltas) {
        PaintCanvas canvas;
        canvas.source.value[5] = 42.0f;
        CutScene scene{&level, 0.5f, dt};
        scene.transformStep(&canvas);
        assert(canvas.readId == 11 && canvas.writeId == 11);
        assert(canvas.source.value[0] == 0.0f && canvas.submitted.value[0] == 0.5f);
        assert(canvas.submitted.value[5] == 42.0f);
        assert(canvas.first.calls == 1 && canvas.second.calls == 1);
        assert(canvas.first.lastDelta == static_cast<int32_t>(dt));
        assert(canvas.second.lastDelta == static_cast<int32_t>(dt));
    }
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-process-mode23-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode23_transform.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
