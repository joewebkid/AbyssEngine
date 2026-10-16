#include <cassert>
#include <cstdint>

struct Matrix {
    int marker = 42;
    bool scaled = false;
    float x = 0, y = 0, z = 0;
};
void MatrixSetScaling(Matrix *matrix, float x, float y, float z) {
    matrix->scaled = true;
    matrix->x = x;
    matrix->y = y;
    matrix->z = z;
}

struct PaintCanvas;
struct Globals {
    static PaintCanvas *Canvas;
    static unsigned char iPad;
};
namespace AbyssEngine {
    enum AnimationMode : int { AnimationMode_0 = 0 };
    struct Transform {
        int calls = 0;
        int mode = -1;
        void *argument = this;
        void SetAnimationState(AnimationMode value, void *arg) {
            ++calls;
            mode = static_cast<int>(value);
            argument = arg;
        }
    };
}

struct PaintCanvas {
    int removeCalls = 0, addCalls = 0;
    int getCalls = 0;
    int setLocalCalls = 0, drawCalls = 0;
    unsigned removedTransform = 0, addedTransform = 0;
    unsigned retrievedTransform = 0;
    unsigned localTransform = 0, drawnTransform = 0;
    int localMarker = 0;
    const Matrix *drawnMatrix = nullptr;
    unsigned short removedMesh = 0, addedMesh = 0;
    bool addedFlag = true;
    PaintCanvas *replacementOnRemove = nullptr;
    PaintCanvas *replacementOnGet = nullptr;
    PaintCanvas *replacementOnSetLocal = nullptr;
    AbyssEngine::Transform transform;

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
    void *TransformGetTransform(unsigned index) {
        ++getCalls;
        retrievedTransform = index;
        if (replacementOnGet) Globals::Canvas = replacementOnGet;
        return &transform;
    }
    void TransformSetLocal(unsigned index, const Matrix &matrix) {
        ++setLocalCalls;
        localTransform = index;
        localMarker = matrix.marker;
        if (replacementOnSetLocal) Globals::Canvas = replacementOnSetLocal;
    }
    void DrawTransform(unsigned index, const Matrix *matrix) {
        ++drawCalls;
        drawnTransform = index;
        drawnMatrix = matrix;
    }
};

PaintCanvas *Globals::Canvas = nullptr;
unsigned char Globals::iPad = 0;

struct Gun { int delayActive = 0; };
struct AEGeometry { unsigned transform = 77; };

struct ObjectGun {
    unsigned transform = 17;
    int meshId = 0x12345;
    Gun *gun = nullptr;
    AEGeometry *geometry = nullptr;
    Matrix orientation;
    float scaleX = 0.6f, scaleY = 0.7f, scaleZ = 0.8f;
    void replaceGun(unsigned mesh, int unused);
    void applyInactive();
    void applyRenderScale(Matrix &local);
    void renderFinal();
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

    PaintCanvas first, second, third;
    first.replacementOnGet = &second;
    second.replacementOnGet = &third;
    Gun inactive;
    AEGeometry geometry;
    gun.gun = &inactive;
    gun.geometry = &geometry;
    Globals::Canvas = &first;
    gun.applyInactive();
    assert(first.getCalls == 1 && second.getCalls == 1 && third.getCalls == 1);
    assert(first.retrievedTransform == 77 && second.retrievedTransform == 77 &&
           third.retrievedTransform == 77);
    assert(first.transform.calls == 1 && first.transform.mode == 0 &&
           first.transform.argument == nullptr);
    assert(second.transform.calls == 1 && second.transform.mode == 3 &&
           second.transform.argument == nullptr);
    assert(third.transform.calls == 1 && third.transform.mode == 1 &&
           third.transform.argument == nullptr);

    Matrix local;
    Globals::iPad = 0;
    gun.applyRenderScale(local);
    assert(!local.scaled);
    Globals::iPad = 1;
    gun.applyRenderScale(local);
    assert(local.scaled && local.x == 0.6f && local.y == 0.7f && local.z == 0.8f);

    PaintCanvas beforeDraw, afterSetLocal;
    beforeDraw.replacementOnSetLocal = &afterSetLocal;
    Globals::Canvas = &beforeDraw;
    gun.renderFinal();
    assert(beforeDraw.setLocalCalls == 1 && beforeDraw.drawCalls == 0);
    assert(beforeDraw.localTransform == 17 && beforeDraw.localMarker == 42);
    assert(afterSetLocal.drawCalls == 1 && afterSetLocal.drawnTransform == 17 &&
           afterSetLocal.drawnMatrix == nullptr);
}
