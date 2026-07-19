#ifndef GOF2_MTITLE_H
#define GOF2_MTITLE_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/core/IApplicationModule.h"

class MTitle : public IApplicationModule {
public:
    int renderPriority;
    uint32_t logoImage;
    uint32_t logoImage2;
    int step;
    int timer;

    MTitle();

    ~MTitle();

    int OnInitialize();

    void OnRelease();

    long long OnKeyPress(long long key, long long mod);

    long long OnKeyRelease(long long key, long long mod);

    void OnTouchBegin(int x, int y);

    void OnTouchMove(int x, int y);

    void OnUpdate();

    void OnRender2D();

    void OnRender3D();

    void OnTouchEnd(int x, int y);

    int ShowLoadingScreen();
};
#endif
