"""Exercise the extracted CutScene turret type and motion selection."""
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
    method = production[production.index("void CutScene::checkForTurret()"):]
    selection = method[method.index("int idx = item->getIndex();"):method.index("AEGeometry *geom0 =")]
    motion = method[method.index("Vector *posVec ="):method.index("this->turretGeom->addChild(geom0->")]
    source = """#include <cassert>
struct Vector { float x, y, z; };
struct AEGeometry {
    Vector position{}, translation{}, rotation{};
    int translations = 0, rotations = 0;
    void setPosition(const Vector &v) { position = v; }
    void translate(float x, float y, float z) {
        ++translations; translation = {x, y, z};
    }
    void rotate(float x, float y, float z) {
        ++rotations; rotation = {x, y, z};
    }
};
template <typename T> struct Array {
    T items[3];
    T operator[](unsigned i) const { assert(i < 3); return items[i]; }
};
struct Item {
    int index, calls = 0;
    int getIndex() { ++calls; return index; }
};
void run(Item *item, Array<Array<Vector *> *> *positions,
         AEGeometry *geom0, AEGeometry *geom1) {
""" + selection + motion + """}
void check(int index, float x, float y, float z, bool turns) {
    Vector wrong0{1, 2, 3}, wrong1{4, 5, 6}, right{7, 8, 9};
    Array<Vector *> slot0{{&wrong0}}, slot1{{&wrong1}}, slot2{{&right}};
    Array<Array<Vector *> *> positions{{&slot0, &slot1, &slot2}};
    AEGeometry geom0, geom1;
    Item item{index};
    run(&item, &positions, &geom0, &geom1);
    assert(geom0.position.x == 7 && geom1.position.z == 9);
    assert(geom0.translations == 0 && geom1.translations == 1);
    assert(geom1.translation.x == x && geom1.translation.y == y);
    assert(geom1.translation.z == z);
    assert(geom0.rotations == int(turns) && geom1.rotations == int(turns));
    if (turns) {
        assert(geom0.rotation.x == 0 && geom0.rotation.y == 3.1415927f);
        assert(geom0.rotation.z == 0 && geom1.rotation.y == 3.1415927f);
    }
    assert(item.calls == (index >= 198 ? 3 : 2));
}
int main() {
    check(47, 0, 60, 0, true);
    check(48, 0, 55, 0, true);
    check(49, 0, 60, 0, true);
    check(180, 0, 84, 0, true);
    check(181, 0, 88, -11, true);
    check(182, 34, 88, 1.6f, true);
    check(198, 0, 80, 0, false);
    check(199, 0, 38, 0, false);
    check(200, 0, 75, 0, false);
    check(224, 0, 82, 0, true);
    check(201, 0, 0, 0, true);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-turret-motion-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "turret_motion.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
