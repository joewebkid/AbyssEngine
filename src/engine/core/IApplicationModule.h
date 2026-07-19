#ifndef GOF2_IAPPLICATIONMODULE_H
#define GOF2_IAPPLICATIONMODULE_H
#include "engine/core/Array.h"
#include "AEString.h"
namespace AbyssEngine {
    class ApplicationManager;
    class PaintCanvas;
}

namespace AbyssEngine {
    class IApplicationModule;
}
using ::AbyssEngine::ApplicationManager;


using ::AbyssEngine::PaintCanvas;

namespace AbyssEngine {
    class IApplicationModule {
    public:
        PaintCanvas *paintCanvas;
        ApplicationManager *applicationManager;

        IApplicationModule() {
        }

        virtual ~IApplicationModule() {}

        virtual int OnInitialize() = 0;

        virtual void OnRelease() = 0;

        virtual long long OnKeyPress(long long key, long long) = 0;
        virtual long long OnKeyRelease(long long key, long long) = 0;

        virtual void OnTouchBegin(int, int) = 0;

        virtual void OnTouchMove(int, int) = 0;

        virtual void OnTouchEnd(int, int) = 0;

        virtual void OnTouchBegin(int x, int y, void *data);

        virtual void OnTouchMove(int x, int y, void *data);

        virtual void OnTouchEnd(int x, int y, void *data);

        virtual void OnUpdate() = 0;

        virtual void OnRender3D() = 0;

        virtual void OnRender2D() = 0;

        virtual void OnSuspend();

        virtual void OnResume();

        virtual int ShowLoadingScreen() = 0;

        void SetApplicationManager(ApplicationManager *manager);
    };
}

using ::AbyssEngine::IApplicationModule;
#endif
