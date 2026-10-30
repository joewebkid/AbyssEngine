

#include "engine/input/VirtualInput.h"

#include "engine/core/ApplicationManager.h"
#include "engine/math/AEMath.h"
#include "engine/core/NFC.h"
#include "engine/render/Engine.h"
#include "game/core/Globals.h"

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <initializer_list>

using AbyssEngine::AEMath::VectorSignedToFloat;

extern int g_mouseCaptured __attribute__((visibility("hidden")));
extern int g_useJoystick __attribute__((visibility("hidden")));

bool g_virtualInputSlowMotion;
bool g_virtualInputSpeedUp;
int stackX;
int stackY;
int wheelStack;
SoftwareStick g_stick = {1, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};

static_assert(sizeof(SoftwareStick) == 28, "Android software-stick layout");
static_assert(offsetof(SoftwareStick, savedX) == 20, "Android gamepad X slot");
static_assert(offsetof(SoftwareStick, savedY) == 24, "Android gamepad Y slot");

struct VirtualKey {
        char name[0x14];
        int pressed;
        int keyCode;
        int altCode1;
        int altCode2;
        int altCode3;
        int *touchX;
        int *touchY;
        int baseX;
        int baseY;
        int offsetX;
        int offsetY;

        union {
            int touchWindowHandle;
            void *touch;
        };

        int field_0x44;
        int field_0x48;
        int action;
        int field_0x50;
        bool (*predicate)(int);
};

#if UINTPTR_MAX == 0xffffffffu
static_assert(sizeof(VirtualKey) == 0x58, "Android virtual-key stride");
static_assert(offsetof(VirtualKey, touchX) == 0x28, "Android touch X slot");
static_assert(offsetof(VirtualKey, predicate) == 0x54, "Android predicate slot");
#endif

#define VKEY(name, key, alt1, alt2, alt3, touchX, touchY, baseX, baseY, handle, module, action, pass, predicate) \
    {name, 0, key, alt1, alt2, alt3, touchX, touchY, baseX, baseY, 0, 0, {handle}, module, 0, action, pass, predicate}
#define PRED(fn) &fn

extern "C" {
VirtualKey keys[48] = {
    VKEY("Left", 97, 0xf702, -100, -13, nullptr, nullptr, 0, 0, 0, 2, -1, 0, PRED(IsInGameSubMenuNotActive)),
    VKEY("Right", 100, 0xf703, -200, -14, nullptr, nullptr, 0, 0, 0, 2, -1, 0, PRED(IsInGameSubMenuNotActive)),
    VKEY("Down", 119, 0xf700, -300, -11, nullptr, nullptr, 0, 0, 0, 2, -1, 0, PRED(IsInGameSubMenuNotActive)),
    VKEY("Up", 115, 0xf701, -400, -12, nullptr, nullptr, 0, 0, 0, 2, -1, 0, PRED(IsInGameSubMenuNotActive)),
    VKEY("SwipeLeft", 121, 0, -4, 0, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("SwipeRight", 120, 0, -5, 0, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("RotateLeft", 44, 0, 0, 0, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("RotateRight", 46, 0, 0, 0, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("Accelerate", 43, 0, 0, 0, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("Decelerate", 45, 0, 0, 0, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("Break", 1, 0, -2, 0, nullptr, nullptr, 0, 0, 0, 2, -1, 1, PRED(IsInGameSubMenuNotActive)),
    VKEY("Left", 97, 0xf702, -100, -13, nullptr, nullptr, 0, 0, 0, 5, -1, 0, nullptr),
    VKEY("Right", 100, 0xf703, -200, -14, nullptr, nullptr, 0, 0, 0, 5, -1, 0, nullptr),
    VKEY("Down", 119, 0xf700, -300, -11, nullptr, nullptr, 0, 0, 0, 5, -1, 0, nullptr),
    VKEY("Up", 115, 0xf701, -400, -12, nullptr, nullptr, 0, 0, 0, 5, -1, 0, nullptr),
    VKEY("Left", 97, 0xf702, -100, -13, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("Right", 100, 0xf703, -200, -14, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("Down", 119, 0xf700, -300, -11, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("Up", 115, 0xf701, -400, -12, nullptr, nullptr, 0, 0, 0, 2, -1, 0, nullptr),
    VKEY("Wingmen", 0, 0, 0, 0, nullptr, nullptr, 0, 0, 213, 2, -1, 1, nullptr),
    VKEY("Boost", 101, 98, -1, 0, &Globals::boost_x, &Globals::boost_y, 0, 0, 100, 2, -1, 0, nullptr),
    VKEY("Dock", 102, 60, -3, 0, &Globals::fire_x, &Globals::fire_y, 4, 4, 1111, 2, -1, 0, PRED(IsDialogNotVisible2)),
    VKEY("Fire", 49, 13, -10000, 0, &Globals::fire_x, &Globals::fire_y, 4, 4, 23, 2, -1, 0, PRED(IsDialogNotVisible2)),
    VKEY("Fire", 49, 13, -10000, 0, &Globals::fire_x, &Globals::fire_y, 4, 4, 26, 1, -1, 0, nullptr),
    VKEY("Fire", 49, 13, -10000, 0, &Globals::right_edge, &Globals::bottom_edge, 4, 4, 24, 5, -1, 0, PRED(IsDialogNotVisible2)),
    VKEY("Fire", 49, 13, -10000, 0, &Globals::other_buttons_x[3], &Globals::other_buttons_y[3], 4, 4, 25, -1, -1, 1, PRED(IsDialogVisible)),
    VKEY("SecondaryFire", 114, 118, 0, 0, &Globals::sec_fire_x, &Globals::sec_fire_y, 0, 0, 99, 2, -1, 0, nullptr),
    VKEY("TurretView", 103, 0, 0, 0, &Globals::turret_view_x, &Globals::turret_view_y, 0, 0, 4, 2, -1, 0, nullptr),
    VKEY("ActionMenu", 109, 0, 0, 0, &Globals::action_menu_x, &Globals::action_menu_y, 0, 0, 109, 2, 2, 0, nullptr),
    VKEY("Pause", 53, 0, -8, 0, &Globals::pause_x, &Globals::pause_y, 0, 0, 108, 2, -1, 0, PRED(IsStarMapNotVisible)),
    VKEY("AutoPilot", 113, 110, -10, 0, &Globals::autopilot_x, &Globals::autopilot_y, 0, 0, 102, 2, 1, 0, nullptr),
    VKEY("FastForward", 9, 0, 0, 0, &Globals::fast_forward_x, &Globals::fast_forward_y, 0, 0, 103, 2, -1, 0, nullptr),
    VKEY("Back", 53, 0, -9, 0, &Globals::other_buttons_x[0], &Globals::other_buttons_y[0], 0, 0, 201, -1, -1, 0, PRED(IsDialogNotVisible)),
    VKEY("Back", 53, 0, -9, 0, &Globals::other_buttons_x[2], &Globals::other_buttons_y[2], 0, 0, 202, -1, -1, 1, PRED(IsDialogVisible)),
    VKEY("Back", 53, 0, -9, 0, &Globals::pause_x, &Globals::pause_y, 0, 0, 203, -1, -1, 0, PRED(IsInGameSubMenuNotActive)),
    VKEY("Back", 53, 0, -9, 0, &Globals::left_edge, &Globals::bottom_edge, 0, 0, 204, -1, -1, 1, PRED(IsInPrimaryMenu)),
    VKEY("KhadorDrive", 40, 0, 0, 0, nullptr, nullptr, 0, 0, 211, 2, -1, 1, PRED(IsInGameSubMenuNotActive)),
    VKEY("Cloak", 99, 0, 0, 0, nullptr, nullptr, 0, 0, 212, 2, -1, 1, PRED(IsInGameSubMenuNotActive)),
    VKEY("1", 18, 0, 0, 0, &Globals::sub_menu_buttons_x[0], &Globals::sub_menu_buttons_y[0], 10, 0, 1401, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("2", 19, 0, 0, 0, &Globals::sub_menu_buttons_x[1], &Globals::sub_menu_buttons_y[1], 10, 0, 1402, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("3", 20, 0, 0, 0, &Globals::sub_menu_buttons_x[2], &Globals::sub_menu_buttons_y[2], 10, 0, 1403, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("4", 21, 0, 0, 0, &Globals::sub_menu_buttons_x[3], &Globals::sub_menu_buttons_y[3], 10, 0, 1404, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("5", 53, 0, 0, 0, &Globals::sub_menu_buttons_x[4], &Globals::sub_menu_buttons_y[4], 10, 0, 1405, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("6", 22, 0, 0, 0, &Globals::sub_menu_buttons_x[5], &Globals::sub_menu_buttons_y[5], 10, 0, 1406, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("7", 26, 0, 0, 0, &Globals::sub_menu_buttons_x[6], &Globals::sub_menu_buttons_y[6], 10, 0, 1407, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("8", 28, 0, 0, 0, &Globals::sub_menu_buttons_x[7], &Globals::sub_menu_buttons_y[7], 10, 0, 1408, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("9", 25, 0, 0, 0, &Globals::sub_menu_buttons_x[8], &Globals::sub_menu_buttons_y[8], 10, 0, 1409, -1, 0, 0, PRED(IsSubMenuActive)),
    VKEY("0", 29, 0, 0, 0, &Globals::sub_menu_buttons_x[9], &Globals::sub_menu_buttons_y[9], 10, 0, 1410, -1, 0, 0, PRED(IsSubMenuActive)),
};
}

#undef PRED
#undef VKEY

namespace {
    constexpr int kVirtualKeyCount = 48;
    VirtualKey (&g_virtualKeys)[kVirtualKeyCount] = keys;

    VirtualKey *const g_arrowKeys = g_virtualKeys;

    int g_actionKeyState;

    constexpr int kDelayedKeyMax = 0x401;
    int g_delayedKeys[kDelayedKeyMax];
    int g_delayedKeyCount;

    constexpr int kTouchWindowBase = 0x578;

    int *g_touchWindowCount;

    int *g_touchWindowMode;
    int *g_touchWindowFlags;

    int *g_touchWindowFocus;

    const unsigned int *const *g_hashTable;

    constexpr float kAxisCenter = 0.5f;
    constexpr float kAxisStep = 0.1f;

    int g_followUpAction;
    int g_comboProgress;

    int g_mousePointerLatch = 1;

    int g_simEdgesInit;

    unsigned char g_simTouchActive;
}

extern "C" unsigned int F(unsigned int value) {
    const unsigned int *table = *g_hashTable;
    const unsigned int *col0 = table;
    const unsigned int *col1 = table + 0x100;
    const unsigned int *col2 = table + 0x200;
    const unsigned int *col3 = table + 0x300;

    const unsigned int byte0 = value & 0xff;
    const unsigned int byte1 = (value >> 8) & 0xff;
    const unsigned int byte2 = (value >> 16) & 0xff;
    const unsigned int byte3 = value >> 24;

    return (col2[byte1] ^ (col0[byte3] + col1[byte2])) + col3[byte0];
}

void MouseInput(int dx, int dy) {
    if (!g_mouseCaptured)
        return;

    if (g_stick.useJoystick) {
        stackX = dx;
    } else {
        stackX += dx;
        dy += stackY;
    }
    stackY = dy;
}

void LowerMouseWheel() {
    int value = Globals::mouse_wheel;
    if (value > 0) {
        int scaled = static_cast<int>(static_cast<double>(value) * 0.95);
        Globals::mouse_wheel = scaled < 0 ? 0 : scaled;
    } else if (value < 0) {
        int scaled = static_cast<int>(static_cast<double>(value) * 0.95);
        Globals::mouse_wheel = scaled > 0 ? 0 : scaled;
    }

    value = Globals::mouse_wheelX;
    if (value > 0) {
        int scaled = static_cast<int>(static_cast<double>(value) * 0.95);
        Globals::mouse_wheelX = scaled < 0 ? 0 : scaled;
    } else if (value < 0) {
        int scaled = static_cast<int>(static_cast<double>(value) * 0.95);
        Globals::mouse_wheelX = scaled > 0 ? 0 : scaled;
    }

    value = Globals::mouse_wheelY;
    if (value > 0) {
        int scaled = static_cast<int>(static_cast<double>(value) * 0.95);
        Globals::mouse_wheelY = scaled < 0 ? 0 : scaled;
    } else if (value < 0) {
        int scaled = static_cast<int>(static_cast<double>(value) * 0.95);
        Globals::mouse_wheelY = scaled > 0 ? 0 : scaled;
    }
}

void MouseWheel(float delta, float residual) {
    if (delta < -4.0f &&
        static_cast<ApplicationManager *>(Globals::appManager)
                ->GetCurrentApplicationModule() == reinterpret_cast<void *>(2))
        Globals::keyboard_swipe = 1;

    if (delta > 4.0f &&
        static_cast<ApplicationManager *>(Globals::appManager)
                ->GetCurrentApplicationModule() == reinterpret_cast<void *>(2))
        Globals::keyboard_swipe = -1;

    wheelStack = static_cast<int>((delta + residual) + static_cast<float>(wheelStack));
}

void keyReleasedWithDelay(AbyssEngine::Engine * /*engine*/, int key) {
    if (g_delayedKeyCount > kDelayedKeyMax - 1)
        return;
    g_delayedKeys[g_delayedKeyCount++] = key;
}

void SendStoredKeyUpEvents(AbyssEngine::Engine *engine) {
    for (int i = 0; i < g_delayedKeyCount; ++i)
        keyReleased(engine, g_delayedKeys[i]);
    g_delayedKeyCount = 0;
}

void keyEventReleased(AbyssEngine::Engine *engine, char *name) {
    for (int i = 0; i < kVirtualKeyCount; ++i) {
        VirtualKey &key = g_virtualKeys[i];
        if (std::strcmp(key.name, name) == 0)
            keyReleased(engine, key.keyCode);
    }
}

void keyEventPressed(AbyssEngine::Engine *engine, char *name) {
    for (int i = 0; i < kVirtualKeyCount; ++i) {
        VirtualKey &key = g_virtualKeys[i];
        if (std::strcmp(key.name, name) == 0)
            keyPressed(engine, key.keyCode);
    }
}

void SetKeyCode(const char *name, int slot, int code) {
    if (name == nullptr)
        return;

    for (VirtualKey &key: g_virtualKeys) {
        if (std::strcmp(key.name, name) != 0)
            continue;
        switch (slot) {
            case 0: key.keyCode = code;
                break;
            case 1: key.altCode1 = code;
                break;
            case 2: key.altCode2 = code;
                break;
            case 3: key.altCode3 = code;
                break;
        }
    }
}

bool IsSubMenuActive(int player) {
    const int handle = g_virtualKeys[player].touchWindowHandle;
    if (handle <= kTouchWindowBase)
        return false;

    const int window = handle - (kTouchWindowBase + 1);
    if (window >= *g_touchWindowCount)
        return false;

    if (*g_touchWindowMode != 0) {
        const unsigned int mode = static_cast<unsigned int>(*g_touchWindowFlags - 1);
        return mode > 1u ? 1 : 0;
    }

    if (*g_touchWindowFocus != -1)
        return true;

    return ApplicationManager::gAppManager->currentModuleId == 5 ? 1 : 0;
}

void keyReleased(AbyssEngine::Engine *engine, int key) {
    Globals::is_choice_window_visible = 0;
    Globals::is_dialogue_window_visible = 0;
    Globals::is_menu_visible = 0;
    Globals::is_hacking_visible = 0;

    if (engine == nullptr)
        return;

    ApplicationManager::gAppManager->GetCurrentApplicationModule();
    int releasedIndex = -1;

    for (int i = 0; i < kVirtualKeyCount; ++i) {
        VirtualKey &vk = g_virtualKeys[i];
        if (vk.keyCode != key && vk.altCode1 != key && vk.altCode2 != key &&
            vk.altCode3 != key)
            continue;

        if (vk.pressed != 1)
            continue;
        vk.pressed = 0;

        if (i < 0xb)
            releasedIndex = i;

        if (vk.touchX == nullptr)
            continue;

        engine->appManager->OnTouchEnd(vk.baseX + vk.offsetX, vk.baseY + vk.offsetY,
                                       vk.touch);
        engine->appManager->OnTouchEnd();
        engine->DrawQuad(vk.offsetX, vk.offsetY, 10, 10);
        if (vk.action != 0)
            g_followUpAction = vk.action;
    }

    switch (releasedIndex) {
        case 0:
            g_stick.inputX = (g_virtualKeys[1].pressed == 0) ? kAxisCenter : kAxisStep;
            break;
        case 1:
            g_stick.inputX = (g_virtualKeys[0].pressed == 0) ? kAxisCenter : -kAxisStep;
            break;
        case 2:
            g_stick.inputY = (g_virtualKeys[3].pressed == 0) ? kAxisCenter : -kAxisStep;
            break;
        case 3:
            g_stick.inputY = (g_virtualKeys[2].pressed == 0) ? kAxisCenter : kAxisStep;
            break;
        case 6:
        case 7:
            Globals::bankZ = 0.0f;
            break;
        default:
            break;
    }
}

void keyPressed(AbyssEngine::Engine *engine, int key) {
    if (engine == nullptr)
        return;

    int progress = g_comboProgress;
    if (key == 14) {
        switch (progress) {
            case 4: progress = 5;
                break;
            case 6: progress = 7;
                break;
            case 9: break;
            default: progress = 1;
                break;
        }
    } else if (key == 0x40000000 && progress == 1) {
        progress = 2;
    } else if (key == 1 && progress == 2) {
        progress = 3;
    } else if (key == 0x11 && progress == 3) {
        progress = 4;
    } else if (key == 0xf && progress == 5) {
        progress = 6;
    } else if (key == 5 && progress == 7) {
        progress = 8;
    } else if (key == 5 && progress == 8) {
        progress = 9;
    } else if (progress != 9) {
        progress = 0;
    }
    if (progress != 9 || g_comboProgress != 9)
        g_comboProgress = progress;

    const int currentModule = static_cast<int>(reinterpret_cast<std::intptr_t>(
        ApplicationManager::gAppManager->GetCurrentApplicationModule()));
    int matchedIndex = -1;

    for (int pass = 2; pass >= 0; --pass) {
        for (int i = 0; i < kVirtualKeyCount; ++i) {
            VirtualKey &vk = g_virtualKeys[i];
            if (vk.field_0x50 != pass)
                continue;
            if (vk.keyCode != key && vk.altCode1 != key && vk.altCode2 != key &&
                vk.altCode3 != key)
                continue;
            if (vk.pressed != 0)
                continue;

            if (vk.field_0x44 != currentModule && vk.field_0x44 + 1 != 0)
                continue;

            if (vk.field_0x48 != 0 && g_followUpAction != vk.field_0x48)
                continue;

            if (vk.predicate != nullptr) {
                if (vk.predicate(i) == 0)
                    continue;
            }

            vk.pressed = 1;
            if (i < 0xb)
                matchedIndex = i;
            if (i == 19)
                matchedIndex = 0x13;

            if (vk.touchX != nullptr) {
                engine->appManager->OnTouchBegin(vk.baseX + *vk.touchX,
                                                 vk.baseY + *vk.touchY, vk.touch);
                vk.offsetX = *vk.touchX;
                vk.offsetY = *vk.touchY;
                if (pass != 0)
                    goto matched;
            }
        }
    }

matched:

    if (g_comboProgress == 9) {
        if (GetKeyState(const_cast<char *>("Accelerate")) != 0) {
            g_virtualInputSlowMotion = false;
            g_virtualInputSpeedUp = true;
        } else if (GetKeyState(const_cast<char *>("Decelerate")) != 0) {
            g_virtualInputSpeedUp = false;
            g_virtualInputSlowMotion = true;
        } else if (ApplicationManager::gAppManager->engine == nullptr) {
            g_virtualInputSpeedUp = false;
            g_virtualInputSlowMotion = false;
        }
    }

    switch (matchedIndex) {
        case 0: g_stick.inputX = -kAxisStep;
            break;
        case 1: g_stick.inputX = kAxisStep;
            break;
        case 2: g_stick.inputY = kAxisStep;
            break;
        case 3: g_stick.inputY = -kAxisStep;
            break;
        case 4: Globals::keyboard_swipe = -1;
            break;
        case 5: Globals::keyboard_swipe = 1;
            break;
        case 6: Globals::bankZ = -0.01f;
            break;
        case 7: Globals::bankZ = 0.01f;
            break;
        case 8: engine->field_0x360 = 0x3f800000;
            break;
        case 9: engine->field_0x360 = 0xbf800000;
            break;
        case 0x13: Globals::showWingmanMenu = 1;
            break;
        default: break;
    }
}

int GetKeyState(int index) {
    if (static_cast<unsigned int>(index) > kVirtualKeyCount - 1)
        return 0;
    return g_virtualKeys[index].pressed;
}

int GetKeyState(char *name) {
    for (int i = 0; i < kVirtualKeyCount; ++i) {
        if (std::strcmp(g_virtualKeys[i].name, name) == 0 &&
            g_virtualKeys[i].pressed != 0)
            return 1;
    }
    return 0;
}

bool ArrowKeyPressed() {
    return g_arrowKeys[0].pressed != 0 || g_arrowKeys[1].pressed != 0 ||
           g_arrowKeys[2].pressed != 0 || g_arrowKeys[3].pressed != 0;
}

bool keyIsPressed() {
    return ArrowKeyPressed() || g_actionKeyState != 0;
}

namespace {
    inline bool currentModuleIs(int id) {
        return reinterpret_cast<std::intptr_t>(
                   ApplicationManager::gAppManager->GetCurrentApplicationModule()) == id;
    }

    inline bool noBlockingOverlay() {
        return (Globals::is_dialogue_window_visible | Globals::is_choice_window_visible |
                Globals::is_hacking_visible) == 0;
    }
}

int ActualizeMouseVisibilty(int force) {
    const bool arrowPressed = ArrowKeyPressed();

    if (Globals::mouseCursorActivated != 0) {
        if ((g_mousePointerLatch + 1 != 0 || Globals::isCinematicModeActive != 0) &&
            currentModuleIs(2)) {
            bool keep = false;
            bool steerActive = false;
            if (IsInGameSubMenuNotActive(2)) {
                if (noBlockingOverlay()) {
                    keep = true;
                    steerActive = Globals::isCinematicModeActive != 0;
                }
            } else if (noBlockingOverlay()) {
                keep = true;
                steerActive = true;
            }

            if (keep) {
                const bool menuHidden = Globals::is_menu_visible == 0;
                if ((static_cast<int>(steerActive) | static_cast<int>(menuHidden)) == 1 &&
                    Globals::isStarMapVisible == 0 && force == 0 &&
                    Globals::showMouseDuringGameOver == 0) {
                    if (arrowPressed && currentModuleIs(2)) {
                        Globals::mouseCursorActivated = 0;
                        {
                            const bool latchArmed = (g_mousePointerLatch + 1 != 0);
                            g_mousePointerLatch = (Globals::is_hacking_visible == 0 && latchArmed) ? 1 : 0;
                        }
                        if (arrowPressed || Globals::showMouseDuringGameOver != 0)
                            g_mousePointerLatch = 0;
                        return 1;
                    }
                    if (g_mousePointerLatch != 0 && currentModuleIs(5) && Globals::keyBindings[4] != 0)
                        g_mousePointerLatch = 0;
                    return 0;
                }
            }
        }

        Globals::mouseCursorActivated = 0;
        if (force == 0) {
            const bool latchArmed = (g_mousePointerLatch + 1 != 0);
            g_mousePointerLatch = (Globals::is_hacking_visible == 0 && latchArmed) ? 1 : 0;
        }
        if (arrowPressed || Globals::showMouseDuringGameOver != 0)
            g_mousePointerLatch = 0;
        return 1;
    }

    bool reshow = false;
    if (g_mousePointerLatch == 1 || Globals::isCinematicModeActive != 0) {
        if (Globals::isCinematicModeActive != 0) {
            reshow = true;
        } else if (currentModuleIs(2) && IsInGameSubMenuNotActive(2) &&
                   (Globals::is_dialogue_window_visible | Globals::is_choice_window_visible |
                    Globals::is_menu_visible) == 0 &&
                   Globals::is_hacking_visible == 0 && Globals::isStarMapVisible == 0) {
            reshow = true;
        }
    }

    if (reshow) {
        g_mousePointerLatch = -static_cast<int>(
            (g_mousePointerLatch - 1 != 0) && Globals::isCinematicModeActive != 0);
        Globals::mouseCursorActivated = 1;
        return -1;
    }

    if (g_mousePointerLatch != 0 && currentModuleIs(5) && Globals::keyBindings[4] != 0)
        g_mousePointerLatch = 0;
    return 0;
}

void KeyboardAnimationTimer(AbyssEngine::Engine * /*engine*/) {
}

void actualizeButtonPositions(AbyssEngine::Engine * /*engine*/) {
}

namespace {
    inline float bitsToFloat(int v) {
        float f;
        std::memcpy(&f, &v, sizeof f);
        return f;
    }

    inline int floatToBits(float f) {
        int v;
        std::memcpy(&v, &f, sizeof v);
        return v;
    }

    inline bool keyHeld(int index) { return g_virtualKeys[index].pressed != 0; }

    inline float easeAxis(float t) {
        float eased;
        if (t <= 0.0f)
            eased = -(1.0f - (t + 1.0f) * (t + 1.0f));
        else
            eased = 1.0f - (t - 1.0f) * (t - 1.0f);
        return eased + 0.5f;
    }
}

void simulateTouch(AbyssEngine::Engine *engine) {
    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(ApplicationManager::gAppManager) + 0xbc) != 0)
        AbyssEngine::Engine::EnablePostEffect = !AbyssEngine::Engine::EnablePostEffect;

    if (g_simEdgesInit == 0) {
        Globals::left_edge = 10;
        Globals::right_edge = static_cast<int>(engine->GetDisplayWidth()) - 10;
        Globals::top_edge = 10;
        Globals::bottom_edge = static_cast<int>(engine->GetDisplayHeight()) - 10;
        g_simEdgesInit = 1;
    }

    if (Globals::resetKeyboard != 0) {
        g_stick.easedX = 0.5f;
        g_stick.easedY = 0.5f;
        stackX = 0;
        Globals::resetKeyboard = 0;
        stackY = 0;
    }

    if (keyHeld(8))
        engine->field_0x360 = static_cast<uint32_t>(floatToBits(-1.0f));
    if (keyHeld(9))
        engine->field_0x360 = static_cast<uint32_t>(floatToBits(1.0f));

    if (keyHeld(11)) {
        Globals::rotateShipInStation = floatToBits(keyHeld(14) ? -40.0f : -20.0f);
    } else if (keyHeld(12)) {
        Globals::rotateShipInStation = floatToBits(keyHeld(13) ? 40.0f : 20.0f);
    } else if (keyHeld(13)) {
        Globals::rotateShipInStation = floatToBits(20.0f);
    } else if (keyHeld(14)) {
        Globals::rotateShipInStation = floatToBits(-20.0f);
    }

    if ((Globals::is_choice_window_visible | Globals::is_menu_visible |
         Globals::is_dialogue_window_visible) == 0) {
        float steer = bitsToFloat(Globals::rotateShipInStation) +
                      VectorSignedToFloat(wheelStack, 0);
        wheelStack = static_cast<int>(VectorSignedToFloat(wheelStack, 0) * 0.9f);
        if (steer > 40.0f)
            steer = 40.0f;
        else if (steer < -40.0f)
            steer = -40.0f;
        Globals::rotateShipInStation = floatToBits(steer);
    } else {
        wheelStack = 0;
        Globals::rotateShipInStation = floatToBits(0.0f);
    }

    if (keyHeld(11) || keyHeld(15))
        Globals::translateStarMapInXDirection = floatToBits(10.0f);
    else if (keyHeld(12) || keyHeld(16))
        Globals::translateStarMapInXDirection = floatToBits(-10.0f);
    else
        Globals::translateStarMapInXDirection = floatToBits(0.0f);

    if (keyHeld(13) || keyHeld(17))
        Globals::translateStarMapInYDirection = floatToBits(-10.0f);
    else if (keyHeld(14) || keyHeld(18))
        Globals::translateStarMapInYDirection = floatToBits(10.0f);
    else
        Globals::translateStarMapInYDirection = floatToBits(0.0f);

    if (g_mouseCaptured != 0) {
        int px = stackX;
        int stepX = 0;
        if (px < 0) { g_stick.easedX = 0.0f; } else if (px != 0) {
            stepX = 1;
            g_stick.easedX = 1.0f;
        }

        int py = stackY;
        int stepY = 0;
        if (py < 0) { g_stick.easedY = 0.0f; } else if (py != 0) {
            stepY = 1;
            g_stick.easedY = 1.0f;
        }

        if (stepX != 0 || (px - stepX) < 0) {
            int dx = px - stepX;
            stackX = dx - (dx >> 31);
        }
        if (stepY != 0 || (py - stepY) < 0) {
            int dy = py - stepY;
            stackY = dy - (dy >> 31);
        }

        if (g_stick.easedX > 1.0f) g_stick.easedX = 1.0f;
        else if (g_stick.easedX < 0.0f) g_stick.easedX = 0.0f;
        if (g_stick.easedY > 1.0f) g_stick.easedY = 1.0f;
        else if (g_stick.easedY < 0.0f) g_stick.easedY = 0.0f;
    }

    if (g_stick.inputX == 0.5f) {
        if (g_stick.easedX != 0.5f)
            g_stick.easedX = static_cast<float>(static_cast<double>(g_stick.easedX) +
                                                (g_stick.easedX < 0.5f ? 0.05 : -0.05));
        if (static_cast<double>(g_stick.easedX) > 0.4 &&
            static_cast<double>(g_stick.easedX) < 0.6)
            g_stick.easedX = 0.5f;
    } else {
        float v = std::min(g_stick.inputX + g_stick.easedX, 1.0f);
        g_stick.easedX = v;
        if (v < 0.0f) g_stick.easedX = 0.0f;
    }

    if (g_stick.inputY == 0.5f) {
        if (g_stick.easedY != 0.5f)
            g_stick.easedY = static_cast<float>(static_cast<double>(g_stick.easedY) +
                                                (g_stick.easedY < 0.5f ? 0.05 : -0.05));
        if (static_cast<double>(g_stick.easedY) > 0.4 &&
            static_cast<double>(g_stick.easedY) < 0.6)
            g_stick.easedY = 0.5f;
    } else {
        float v = std::min(g_stick.inputY + g_stick.easedY, 1.0f);
        g_stick.easedY = v;
        if (v < 0.0f) g_stick.easedY = 0.0f;
    }

    if (g_mouseCaptured != 0) {
        float magX = 500.0f;
        float absX = std::fabs(g_stick.easedX);
        if (absX > magX)
            magX = absX;
        float absY = std::fabs(g_stick.easedY);
        int mag = static_cast<int>(absY);
        if (absY <= static_cast<float>(static_cast<int>(magX)))
            mag = static_cast<int>(magX);

        float nx = std::max(std::min(VectorSignedToFloat(stackX, 0) / static_cast<float>(mag), 1.0f), -1.0f);
        float ny = std::max(std::min(VectorSignedToFloat(stackY, 0) / (static_cast<float>(mag) * 0.5f), 1.0f), -1.0f);

        float ex = easeAxis(nx);
        float ey = easeAxis(ny);
        g_stick.easedX = ex;
        g_stick.easedY = ey;
        if (ex < 0.0f) { g_stick.easedX = 0.0f; }
        if (ey < 0.0f) { g_stick.easedY = 0.0f; }
        if (ex > 1.0f) g_stick.easedX = 1.0f;
        if (ey > 1.0f) g_stick.easedY = 1.0f;
    }

    if (g_useJoystick != 0) {
        g_stick.easedX = g_stick.savedX;
        g_stick.easedY = g_stick.savedY;
    }

    if (static_cast<ApplicationManager *>(Globals::appManager)
                ->GetCurrentApplicationModule() == reinterpret_cast<void *>(2)) {
        const float dim = VectorSignedToFloat(Globals::smallButton_dim, 0);
        const float cy = VectorSignedToFloat(Globals::touch_stick_y, 0);
        const float cx = VectorSignedToFloat(Globals::touch_stick_x, 0);
        const float ex = g_stick.easedX;
        const float ey = g_stick.easedY;

        const int down = keyIsPressed();
        unsigned char active = g_simTouchActive;
        const bool inactive = (active ^ 1) != 0;

        if (down != 0 || !inactive) {
            const int touchX = static_cast<int>((cx - dim) + (dim + dim) * ex);
            const int touchY = static_cast<int>((cy - dim) + (dim + dim) * ey);
            AbyssEngine::ApplicationManager *mgr = engine->appManager;

            if (down == 0 && !inactive) {
                if (ex == 0.5f && ey == 0.5f) {
                    mgr->OnTouchEnd(touchX, touchY, reinterpret_cast<void *>(0xe8b));
                    g_simTouchActive = 0;
                } else {
                    mgr->OnTouchMove(touchX, touchY, reinterpret_cast<void *>(0xe8b));
                }
            } else if (down == 1 && !inactive) {
                mgr->OnTouchMove(touchX, touchY, reinterpret_cast<void *>(0xe8b));
            } else if (active == 0 && down == 1) {
                mgr->OnTouchBegin(touchX, touchY, reinterpret_cast<void *>(0xe8b));
                mgr->OnTouchMove(touchX, touchY, reinterpret_cast<void *>(0xe8b));
                g_simTouchActive = 1;
            }
        }
    }

    if (g_stick.useJoystick == 0) {
        stackX = static_cast<int>(VectorSignedToFloat(stackX, 0) * 0.96f);
        stackY = static_cast<int>(VectorSignedToFloat(stackY, 0) * 0.96f);
    }
}
