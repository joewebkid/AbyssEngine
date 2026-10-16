#include <cassert>
#include <cstdint>

struct PaintCanvas;
struct Globals { static PaintCanvas *Canvas; };

struct PaintCanvas {
    int removeCalls = 0, addCalls = 0;
    unsigned removedTransform = 0, addedTransform = 0;
    unsigned short removedMesh = 0, addedMesh = 0;
    bool addedFlag = true;
    PaintCanvas *replacementOnRemove = nullptr;

    void TransformRemoveMesh(unsigned transform, unsigned short mesh) {
        ++removeCalls;
        removedTransform = transform;
        removedMesh = mesh;
        if (replacementOnRemove) Globals::Canvas = replacementOnRemove;
    }
    void TransformAddMesh(unsigned transform, unsigned short mesh, bool flag) {
        ++addCalls;
        addedTransform = transform;
        addedMesh = mesh;
        addedFlag = flag;
    }
};

PaintCanvas *Globals::Canvas = nullptr;

struct ObjectGun {
    unsigned transform = 17;
    int meshId = 0x12345;
    void replaceGun(unsigned mesh, int unused);
};

// PRODUCTION_METHODS

int main() {
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    ObjectGun gun;
    gun.replaceGun(0x23456, 71);
    assert(canvas.removeCalls == 1 && canvas.addCalls == 1);
    assert(canvas.removedTransform == 17 && canvas.removedMesh == 0x2345);
    assert(gun.meshId == 0x23456);
    assert(canvas.addedTransform == 17 && canvas.addedMesh == 0x3456 &&
           !canvas.addedFlag);

    PaintCanvas replacement;
    PaintCanvas afterRemove;
    replacement.replacementOnRemove = &afterRemove;
    Globals::Canvas = &replacement;
    gun.replaceGun(23, -1);
    assert(replacement.removeCalls == 1 && replacement.addCalls == 0);
    assert(replacement.removedMesh == 0x3456);
    assert(afterRemove.addCalls == 1 && afterRemove.addedMesh == 23 &&
           afterRemove.addedTransform == 17 && !afterRemove.addedFlag);
    assert(gun.meshId == 23 && canvas.addCalls == 1);
}
