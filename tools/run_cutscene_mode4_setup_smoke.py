"""Exercise the extracted CutScene mode-4 setup for each race branch."""
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
    initialize = production[production.index("void CutScene::initialize() {"):]
    start = initialize.index("canvas->CameraCreate(this->cameraId6c);", initialize.index("this->mode == 4"))
    end = initialize.index("\n    }\n\n    this->rootGeom", start)
    fragment = initialize[start:end]
    source = """#include <cassert>
#include <cstdint>
namespace AbyssEngine {
enum AnimationMode { Still = 0 };
struct Transform {
    int calls = 0;
    void SetAnimationState(AnimationMode mode, void *data) {
        assert(mode == Still && data == nullptr);
        ++calls;
    }
};
}
using AbyssEngine::AnimationMode;
struct TargetFollowCamera {
    static int destroyed;
    ~TargetFollowCamera() { ++destroyed; }
};
int TargetFollowCamera::destroyed = 0;
struct SolarSystem {
    int race, queries = 0;
    int getRace() { ++queries; return race; }
};
struct Status {
    static Status *gStatus;
    SolarSystem *system;
    SolarSystem *getSystem() { return system; }
};
Status *Status::gStatus = nullptr;
struct PaintCanvas {
    unsigned perspectiveId = 0, currentId = 0, transformId = 0;
    AbyssEngine::Transform transform;
    void CameraCreate(unsigned &id) { id = 7; }
    void CameraSetPerspective(unsigned id, float fov, float near, float far) {
        assert(fov == 1.2f && near == 200.0f && far == 48000.0f);
        perspectiveId = id;
    }
    void CameraSetCurrent(unsigned id) { currentId = id; }
    void *TransformGetTransform(unsigned id) { transformId = id; return &transform; }
};
struct AEGeometry {
    unsigned transform;
    static unsigned lastChild;
    AEGeometry(uint16_t mesh, PaintCanvas *, bool flag) {
        assert(!flag);
        transform = mesh == 0x36d6 ? 41 : mesh == 0x37c8 ? 42 : 43;
    }
    void addChild(unsigned id) { lastChild = id; }
};
unsigned AEGeometry::lastChild = 0;
struct CutScene {
    unsigned cameraId6c = 0;
    TargetFollowCamera *followCamera = nullptr;
    AEGeometry *geom2c = nullptr, *geom30 = nullptr, *geom34 = nullptr;
    int resetCalls = 0;
    void resetCamera() { ++resetCalls; }
    void initializeMode4(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    const int races[] = {3, 0, 1};
    for (int race : races) {
        SolarSystem system{race};
        Status status{&system};
        Status::gStatus = &status;
        PaintCanvas canvas;
        CutScene scene;
        scene.followCamera = new TargetFollowCamera;
        scene.initializeMode4(&canvas);
        assert(scene.cameraId6c == 7);
        assert(canvas.perspectiveId == 7 && canvas.currentId == 7);
        assert(scene.followCamera == nullptr && scene.resetCalls == 1);
        if (race == 3) {
            assert(scene.geom2c && scene.geom2c->transform == 41);
            assert(canvas.transformId == 41 && canvas.transform.calls == 1);
            delete scene.geom2c;
        } else if (race == 0) {
            assert(scene.geom30 && scene.geom30->transform == 42);
            assert(scene.geom34 == nullptr && AEGeometry::lastChild == 43);
            delete scene.geom30;
        } else {
            assert(!scene.geom2c && !scene.geom30 && !scene.geom34);
        }
    }
    assert(TargetFollowCamera::destroyed == 3);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-mode4-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode4.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
