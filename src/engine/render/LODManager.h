#ifndef GOF2_LODMANAGER_H
#define GOF2_LODMANAGER_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/render/AEGeometry.h"

#include "engine/math/AEMath.h"
#include "engine/math/Vector.h"



#include "engine/render/Touch.h"
class AEGeometry;



void AddTouch(int x, int y, int id, int action);

int GetTouchCount();

void RemoveTouches();

Touch GetTouch(int index);

extern "C" void ndk_resetNativeItemInformationList();

class LODManager {
public:
    Array<AEGeometry *> *objects;
    AbyssEngine::AEMath::Vector cameraPos;
    int timer;

    LODManager();

    ~LODManager();

    void addObject(AEGeometry *g);

    void removeObject(AEGeometry *g);

    void forceUpdate(int dt, bool useParent);

    void update(int dt);
};
#endif
