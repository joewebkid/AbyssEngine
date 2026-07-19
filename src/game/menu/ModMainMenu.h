#ifndef GOF2_MODMAINMENU_H
#define GOF2_MODMAINMENU_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
class CutScene;
class MenuTouchWindow;


class ModMainMenu {
public:
    int paintCanvas;
    void *appManager;
    int state;
    uint8_t initialized;
    int frameTime;
    MenuTouchWindow *touchWindow;
    CutScene *cutScene;
    int logoImage;
    int fadeTimer;
    uint8_t logoActive;
    uint8_t hasSavedGame;

    ModMainMenu();

    virtual ~ModMainMenu();

    virtual void OnInitialize();

    virtual void OnRelease();

    virtual long long OnKeyPress(long long key, long long mod);

    virtual long long OnKeyRelease(long long key, long long mod);

    virtual void OnTouchBegin(int x, int y);

    virtual void OnTouchMove(int x, int y);

    virtual void OnTouchEnd(int x, int y);

    virtual void OnTouchBegin(int x, int y, void *touch);

    virtual void OnTouchMove(int x, int y, void *touch);

    virtual void OnTouchEnd(int x, int y, void *touch);

    virtual void OnUpdate();

    virtual void OnRender3D();

    virtual void OnRender2D();

    virtual void OnSuspend();

    virtual void OnResume();

    virtual int ShowLoadingScreen();
};
#endif
