#include "engine/core/IApplicationModule.h"
#include "engine/core/ApplicationManager.h"

namespace AbyssEngine {
    void IApplicationModule::SetApplicationManager(ApplicationManager *manager) {
        this->applicationManager = manager;
        this->paintCanvas = static_cast<PaintCanvas *>(manager->paintCanvas);
    }

    void IApplicationModule::OnTouchBegin(int x, int y, void *data) {
    }

    void IApplicationModule::OnTouchMove(int x, int y, void *data) {
    }

    void IApplicationModule::OnTouchEnd(int x, int y, void *data) {
    }

    void IApplicationModule::OnSuspend() {
    }

    void IApplicationModule::OnResume() {
    }
}
