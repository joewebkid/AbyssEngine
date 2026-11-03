#include <cassert>

#include "engine/core/NFC.h"
#include "engine/input/VirtualInput.h"
#include "game/core/Globals.h"

SoftwareStick g_stick{};
int Globals::is_dialogue_window_visible = 0;
int Globals::is_choice_window_visible = 0;
int Globals::is_menu_visible = 0;
unsigned char Globals::isStarMapVisible = 0;
int Globals::subMenuIndex = -1;
int Globals::topMenuIndex = 0;
int Globals::menu_touch_window_type = 0;

int main() {
    assert(IsDialogVisible(0) == 0);
    assert(IsDialogNotVisible(0) == 1);
    assert(IsDialogNotVisible2(0) == 1);
    assert(IsStarMapNotVisible(0) == 1);
    assert(IsInGameSubMenuActive(0) == 0);

    Globals::topMenuIndex = 3;
    assert(IsInGameSubMenuNotActive(0) == 1);
    Globals::subMenuIndex = 0;
    assert(IsInGameSubMenuActive(0) == 1);
    assert(IsInGameSubMenuNotActive(0) == 0);
    Globals::subMenuIndex = -1;

    Globals::is_dialogue_window_visible = 1;
    assert(IsDialogVisible(0) == 1);
    assert(IsDialogNotVisible(0) == 0);
    assert(IsDialogNotVisible2(0) == 0);
    Globals::is_dialogue_window_visible = 0;
    Globals::is_choice_window_visible = 1;
    assert(IsDialogVisible(0) == 1);
    assert(IsDialogNotVisible2(0) == 0);
    Globals::is_choice_window_visible = 0;

    Globals::topMenuIndex = 0;
    Globals::is_menu_visible = 1;
    assert(IsDialogNotVisible2(0) == 0);
    assert(IsInGameSubMenuNotActive(0) == 0);
    assert(IsInPrimaryMenu(0) == 1);
    Globals::is_dialogue_window_visible = 1;
    assert(IsInPrimaryMenu(0) == 0);
    Globals::is_dialogue_window_visible = 0;
    Globals::is_choice_window_visible = 1;
    assert(IsInPrimaryMenu(0) == 0);
    Globals::is_choice_window_visible = 0;
    Globals::topMenuIndex = 1;
    assert(IsInPrimaryMenu(0) == 0);
    Globals::topMenuIndex = 0;
    Globals::menu_touch_window_type = 1;
    assert(IsInPrimaryMenu(0) == 0);
    Globals::menu_touch_window_type = 0;

    Globals::isStarMapVisible = 1;
    assert(IsStarMapNotVisible(0) == 0);
    assert(IsInPrimaryMenu(0) == 0);
    Globals::is_menu_visible = 0;
    assert(IsInGameSubMenuNotActive(0) == 0);
}
