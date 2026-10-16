"""Exercise the extracted CutScene ship replacement and ARM offset table."""
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
    table_start = production.index("static constexpr int CutScene_shipVerticalOffsets[")
    table_end = production.index("};", table_start) + 2
    method_start = production.index("void CutScene::replacePlayerShip(")
    method_end = production.index("void CutScene::initialize()", method_start)
    source = """#include <cassert>
#include <cstdint>
#include <cstring>
struct Matrix { float values[15]; };
struct Vector { float x, y, z; };
struct AEGeometry {
    static int destroyed;
    unsigned transform = 0;
    unsigned childTransform = 0;
    Matrix matrix{};
    ~AEGeometry() { ++destroyed; }
    const Matrix &getMatrix() const { return matrix; }
    void setMatrix(const Matrix &m) { matrix = m; }
};
int AEGeometry::destroyed = 0;
struct KIPlayer {
    AEGeometry *geometry;
    float x = 0, y = 0, z = 0;
    void setPosition(const Vector &v) { x = v.x; y = v.y; z = v.z; }
};
struct PlayerFighter : KIPlayer {
    bool exhaustVisible = true;
    void setExhaustVisible(bool visible) { exhaustVisible = visible; }
};
template <typename T> struct Array { T items[1]; T operator[](unsigned i) const {
    assert(i == 0); return items[i];
} };
struct LODManager {
    AEGeometry *removed = nullptr;
    void removeObject(AEGeometry *g) { removed = g; }
};
struct Level {
    LODManager *lodManager;
    Array<KIPlayer *> enemies;
    bool noEnemies = false;
    Array<KIPlayer *> *getEnemies() { return noEnemies ? nullptr : &enemies; }
};
struct PaintCanvas {
    static PaintCanvas *gCanvas;
    int removals = 0;
    unsigned parent = 0, child = 0;
    void TransformRemoveChild(unsigned a, unsigned b) {
        ++removals; parent = a; child = b;
    }
};
PaintCanvas *PaintCanvas::gCanvas = nullptr;
struct Globals {
    static Globals *gGlobals;
    AEGeometry *newGroup = nullptr;
    int kind = -1, variant = -1;
    bool wireframe = true;
    AEGeometry *getShipGroup(int a, int b, bool c) {
        kind = a; variant = b; wireframe = c; return newGroup;
    }
};
Globals *Globals::gGlobals = nullptr;
float VectorSignedToFloat(int value, int) { return static_cast<float>(value); }
struct CutScene {
    Level *level;
    AEGeometry *turretGeom;
    int turretChecks = 0;
    void checkForTurret() { ++turretChecks; }
    void replacePlayerShip(int shipIndex, int variant);
};
""" + production[table_start:table_end] + "\n" + production[method_start:method_end] + """
int main() {
    LODManager lod;
    AEGeometry *old = new AEGeometry;
    old->transform = 11;
    old->childTransform = 111;
    old->matrix.values[5] = 42.0f;
    AEGeometry group, turret;
    group.transform = 33;
    turret.transform = 22;
    turret.childTransform = 222;
    PlayerFighter player;
    player.geometry = old;
    Level level{&lod, {{&player}}};
    PaintCanvas canvas;
    PaintCanvas::gCanvas = &canvas;
    Globals globals;
    globals.newGroup = &group;
    Globals::gGlobals = &globals;
    CutScene scene{&level, &turret};
    scene.replacePlayerShip(19, 7);
    assert(globals.kind == 19 && globals.variant == 7 && !globals.wireframe);
    assert(player.geometry == &group && group.matrix.values[5] == 42.0f);
    assert(player.x == 0 && player.y == 460 && player.z == 0);
    assert(!player.exhaustVisible && lod.removed == old);
    assert(canvas.removals == 1 && canvas.parent == 11 && canvas.child == 22);
    assert(scene.turretChecks == 1 && AEGeometry::destroyed == 1);
    level.noEnemies = true;
    scene.replacePlayerShip(63, 2);
    assert(scene.turretChecks == 1 && globals.kind == 19);
    level.noEnemies = false;
    player.geometry = nullptr;
    scene.turretGeom = nullptr;
    scene.replacePlayerShip(63, 2);
    assert(scene.turretChecks == 2 && globals.kind == 19);
    AEGeometry *oldLast = new AEGeometry;
    player.geometry = oldLast;
    scene.replacePlayerShip(63, 2);
    assert(globals.kind == 63 && globals.variant == 2);
    assert(player.geometry == &group && player.y == 170.0f);
    assert(lod.removed == oldLast && canvas.removals == 1);
    assert(scene.turretChecks == 3 && AEGeometry::destroyed == 2);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-replace-ship-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "replace_ship.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
