#include "engine/render/LODManager.h"
#include "engine/render/AEGeometry.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"

#include <cstdlib>

int curTouchSize = 0;
int maxTouchSize = 0;
Touch *touches = nullptr;

uint32_t CameraGetCurrent(void *canvas);

Matrix *CameraGetLocal(void *canvas, uint32_t index);

static PaintCanvas **g_LOD_canvas = nullptr;

// g_LOD_settings is an untyped engine settings handle. The only field accessed
// here is a float LOD distance factor at byte offset 0x28. Model it with a
// minimal struct carrying named members at the accessed offsets so the access
// can be expressed as a named struct-member read instead of pointer arithmetic.
struct LODSettings {
    unsigned char reserved_0x00[0x28];
    float distanceFactor; // 0x28
};
#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(LODSettings, distanceFactor) == 0x28,
              "LODSettings::distanceFactor must sit at offset 0x28");
#endif

static LODSettings *g_LOD_settings = nullptr;

LODManager::LODManager() {
    this->cameraPos.x = 0;
    this->cameraPos.y = 0;
    this->cameraPos.z = 0;
    this->timer = 0x3e9;
    this->objects = new Array<AEGeometry *>();
}

LODManager::~LODManager() {
    delete this->objects;
    this->objects = nullptr;
}

void LODManager::addObject(AEGeometry *g) {
    if (!g->hasLod())
        return;
    ArrayAdd(g, *this->objects);
}

void LODManager::removeObject(AEGeometry *g) {
    for (uint32_t i = 0; i < this->objects->size(); i++) {
        Array<AEGeometry *> *arr = this->objects;
        if ((*arr)[i] == g)
            ArrayRemove(g, *arr);
    }
}

void LODManager::forceUpdate(int dt, bool useParent) {
    void *canvas = *g_LOD_canvas;
    float factor = g_LOD_settings->distanceFactor;

    uint32_t cam = CameraGetCurrent(canvas);
    Matrix *m = CameraGetLocal(canvas, cam);
    this->cameraPos = AbyssEngine::AEMath::MatrixGetPosition(*m);

    for (uint32_t i = 0; i < this->objects->size(); i++) {
        AEGeometry *g = (*this->objects)[i];
        Vector ref = useParent ? g->getParentPosition() : this->cameraPos;
        g->updateLod(ref, factor);
    }
}

void LODManager::update(int dt) {
    this->timer += dt;
    if (this->timer > 1000) {
        this->timer = 0;
        forceUpdate(0, false);
    }
}

void AddTouch(int x, int y, int id, int action) {
    int index = curTouchSize;
    curTouchSize = index + 1;

    Touch *buf;
    if (index < maxTouchSize) {
        buf = touches;
    } else {
        maxTouchSize = index + 1;
        buf = static_cast<Touch *>(realloc(touches, (index + 1) * sizeof(Touch)));
        touches = buf;
    }

    buf[curTouchSize - 1].x = x;
    Touch *slot = &buf[curTouchSize - 1];
    slot->y = y;
    slot->id = id;
    slot->action = action;
}

int GetTouchCount() {
    return curTouchSize;
}

void RemoveTouches() {
    curTouchSize = 0;
}

Touch GetTouch(int index) {
    return touches[index];
}

extern "C" void ndk_resetNativeItemInformationList() {
    if (Globals::cItemListID_00 != nullptr && Globals::cItemListID_01 != nullptr &&
        Globals::cItemListID_02 != nullptr && Globals::cItemListID_03 != nullptr &&
        Globals::cItemListID_04 != nullptr) {
        operator delete[](Globals::cItemListID_00);
        operator delete[](Globals::cItemListID_01);
        operator delete[](Globals::cItemListID_02);
        operator delete[](Globals::cItemListID_03);
        operator delete[](Globals::cItemListID_04);
        Globals::cItemListID_01 = nullptr;
        Globals::cItemListID_00 = nullptr;
        Globals::cItemListID_02 = nullptr;
        Globals::cItemListID_03 = nullptr;
        Globals::cItemListID_04 = nullptr;
    }
    if (Globals::cItemListID_05 != nullptr && Globals::cItemListID_06 != nullptr &&
        Globals::cItemListID_07 != nullptr && Globals::cItemListID_08 != nullptr &&
        Globals::cItemListID_09 != nullptr) {
        operator delete[](Globals::cItemListID_05);
        operator delete[](Globals::cItemListID_06);
        operator delete[](Globals::cItemListID_07);
        operator delete[](Globals::cItemListID_08);
        operator delete[](Globals::cItemListID_09);
        Globals::cItemListID_06 = nullptr;
        Globals::cItemListID_05 = nullptr;
        Globals::cItemListID_07 = nullptr;
        Globals::cItemListID_08 = nullptr;
        Globals::cItemListID_09 = nullptr;
    }
    if (Globals::cItemListID_10 != nullptr && Globals::cItemListID_11 != nullptr &&
        Globals::cItemListID_12 != nullptr && Globals::cItemListID_13 != nullptr &&
        Globals::cItemListID_14 != nullptr) {
        operator delete[](Globals::cItemListID_10);
        operator delete[](Globals::cItemListID_11);
        operator delete[](Globals::cItemListID_12);
        operator delete[](Globals::cItemListID_13);
        operator delete[](Globals::cItemListID_14);
        Globals::cItemListID_11 = nullptr;
        Globals::cItemListID_10 = nullptr;
        Globals::cItemListID_12 = nullptr;
        Globals::cItemListID_13 = nullptr;
        Globals::cItemListID_14 = nullptr;
    }
    if (Globals::cItemListID_15 != nullptr && Globals::cItemListID_16 != nullptr &&
        Globals::cItemListID_17 != nullptr && Globals::cItemListID_18 != nullptr &&
        Globals::cItemListID_19 != nullptr) {
        operator delete[](Globals::cItemListID_15);
        operator delete[](Globals::cItemListID_16);
        operator delete[](Globals::cItemListID_17);
        operator delete[](Globals::cItemListID_18);
        operator delete[](Globals::cItemListID_19);
        Globals::cItemListID_16 = nullptr;
        Globals::cItemListID_15 = nullptr;
        Globals::cItemListID_17 = nullptr;
        Globals::cItemListID_18 = nullptr;
        Globals::cItemListID_19 = nullptr;
    }
    if (Globals::cItemListID_20 != nullptr && Globals::cItemListID_21 != nullptr &&
        Globals::cItemListID_22 != nullptr && Globals::cItemListID_23 != nullptr &&
        Globals::cItemListID_24 != nullptr) {
        operator delete[](Globals::cItemListID_20);
        operator delete[](Globals::cItemListID_21);
        operator delete[](Globals::cItemListID_22);
        operator delete[](Globals::cItemListID_23);
        operator delete[](Globals::cItemListID_24);
        Globals::cItemListID_21 = nullptr;
        Globals::cItemListID_20 = nullptr;
        Globals::cItemListID_22 = nullptr;
        Globals::cItemListID_23 = nullptr;
        Globals::cItemListID_24 = nullptr;
    }
}
