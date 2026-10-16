"""Check extracted CutScene turret detach/attach transform IDs."""
import argparse
from pathlib import Path
import re
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
    detach_start = method.index("if (this->turretGeom != nullptr) {")
    detach_end = method.index("\n\n    Ship *ship", detach_start)
    detach = method[detach_start:detach_end]
    children = re.findall(r"(?:geom0|geom1)->addChild\(c->\w+\);", method)
    assert len(children) == 3
    attach_start = method.index("this->turretGeom->addChild(geom0->")
    attach_end = method.index("\n\n    if (positions", attach_start)
    attach = method[attach_start:attach_end]
    source = """#include <cassert>
#include <vector>
struct AEGeometry {
    unsigned transform, childTransform;
    std::vector<unsigned> children;
    void addChild(unsigned id) { children.push_back(id); }
};
struct KIPlayer { AEGeometry *geometry; };
template <typename T> struct Array {
    T item;
    T operator[](unsigned i) const { assert(i == 0); return item; }
};
struct Level {
    Array<KIPlayer *> enemies;
    Array<KIPlayer *> *getEnemies() { return &enemies; }
};
struct PaintCanvas {
    static PaintCanvas *gCanvas;
    unsigned parent = 0, child = 0;
    int calls = 0;
    void TransformRemoveChild(unsigned a, unsigned b) {
        ++calls; parent = a; child = b;
    }
};
PaintCanvas *PaintCanvas::gCanvas = nullptr;
struct CutScene {
    Level *level;
    AEGeometry *turretGeom;
    void detach(PaintCanvas *canvas) {""" + detach + """\n    }
    void addComponents(AEGeometry *geom0, AEGeometry *geom1, AEGeometry *c) {
""" + "\n".join(children) + """
    }
    void attach(AEGeometry *geom0, AEGeometry *geom1) {""" + attach + """\n    }
};
int main() {
    AEGeometry lead{11, 111}, turret{22, 222}, component{33, 333};
    AEGeometry geom0{44, 444}, geom1{55, 555};
    KIPlayer player{&lead};
    Level level{{&player}};
    PaintCanvas canvas;
    PaintCanvas::gCanvas = &canvas;
    CutScene scene{&level, &turret};
    scene.detach(&canvas);
    assert(canvas.calls == 1 && canvas.parent == 11 && canvas.child == 22);
    scene.addComponents(&geom0, &geom1, &component);
    assert(geom0.children == std::vector<unsigned>{33});
    assert(geom1.children == std::vector<unsigned>({33, 33}));
    scene.attach(&geom0, &geom1);
    assert(turret.children == std::vector<unsigned>({44, 55}));
    assert(lead.children == std::vector<unsigned>{22});
    scene.turretGeom = nullptr;
    scene.detach(&canvas);
    assert(canvas.calls == 1);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-turret-ids-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "turret_ids.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
