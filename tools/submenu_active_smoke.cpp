#include <cassert>
#include <cstdint>

struct VirtualKey {
    int touchWindowHandle;
};

VirtualKey g_virtualKeys[2] = {};
constexpr int kTouchWindowBase = 0x578;

struct ApplicationManager {
    int module = 0;
    int calls = 0;

    void *GetCurrentApplicationModule() {
        ++calls;
        return reinterpret_cast<void *>(static_cast<std::intptr_t>(module));
    }
};

struct Globals {
    static int sub_menu_button_count;
    static int is_menu_visible;
    static int topMenuIndex;
    static int subMenuIndex;
    static void *appManager;
};

int Globals::sub_menu_button_count = 0;
int Globals::is_menu_visible = 0;
int Globals::topMenuIndex = 0;
int Globals::subMenuIndex = -1;
void *Globals::appManager = nullptr;

// PRODUCTION_FUNCTION

int main() {
    ApplicationManager manager;
    Globals::appManager = &manager;
    g_virtualKeys[0].touchWindowHandle = 1400;
    g_virtualKeys[1].touchWindowHandle = 1401;
    assert(IsSubMenuActive(0) == 0);
    assert(IsSubMenuActive(1) == 0);

    Globals::sub_menu_button_count = 1;
    Globals::is_menu_visible = 1;
    Globals::topMenuIndex = 1;
    assert(IsSubMenuActive(1) == 0);
    Globals::topMenuIndex = 2;
    assert(IsSubMenuActive(1) == 0);
    Globals::topMenuIndex = 3;
    assert(IsSubMenuActive(1) == 1);
    Globals::topMenuIndex = 0;
    assert(IsSubMenuActive(1) == 1);
    assert(manager.calls == 0);

    Globals::is_menu_visible = 0;
    Globals::subMenuIndex = 0;
    assert(IsSubMenuActive(1) == 1);
    assert(manager.calls == 0);

    Globals::subMenuIndex = -1;
    manager.module = 2;
    assert(IsSubMenuActive(1) == 0);
    manager.module = 5;
    assert(IsSubMenuActive(1) == 1);
    assert(manager.calls == 2);
}
