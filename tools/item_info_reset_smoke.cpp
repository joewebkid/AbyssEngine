#include "engine/render/LODManager.h"
#include "game/core/Globals.h"

#include <cstdio>

#define ITEM_SLOT(group, index) char *Globals::cItemList##group##_##index = nullptr
ITEM_SLOT(ID, 00);
ITEM_SLOT(ID, 01);
ITEM_SLOT(ID, 02);
ITEM_SLOT(ID, 03);
ITEM_SLOT(ID, 04);
ITEM_SLOT(Name, 00);
ITEM_SLOT(Name, 01);
ITEM_SLOT(Name, 02);
ITEM_SLOT(Name, 03);
ITEM_SLOT(Name, 04);
ITEM_SLOT(Description, 00);
ITEM_SLOT(Description, 01);
ITEM_SLOT(Description, 02);
ITEM_SLOT(Description, 03);
ITEM_SLOT(Description, 04);
ITEM_SLOT(Currency, 00);
ITEM_SLOT(Currency, 01);
ITEM_SLOT(Currency, 02);
ITEM_SLOT(Currency, 03);
ITEM_SLOT(Currency, 04);
ITEM_SLOT(Price, 00);
ITEM_SLOT(Price, 01);
ITEM_SLOT(Price, 02);
ITEM_SLOT(Price, 03);
ITEM_SLOT(Price, 04);
#undef ITEM_SLOT

int main() {
    char **const slots[] = {
        &Globals::cItemListID_00, &Globals::cItemListID_01,
        &Globals::cItemListID_02, &Globals::cItemListID_03,
        &Globals::cItemListID_04, &Globals::cItemListName_00,
        &Globals::cItemListName_01, &Globals::cItemListName_02,
        &Globals::cItemListName_03, &Globals::cItemListName_04,
        &Globals::cItemListDescription_00, &Globals::cItemListDescription_01,
        &Globals::cItemListDescription_02, &Globals::cItemListDescription_03,
        &Globals::cItemListDescription_04, &Globals::cItemListCurrency_00,
        &Globals::cItemListCurrency_01, &Globals::cItemListCurrency_02,
        &Globals::cItemListCurrency_03, &Globals::cItemListCurrency_04,
        &Globals::cItemListPrice_00, &Globals::cItemListPrice_01,
        &Globals::cItemListPrice_02, &Globals::cItemListPrice_03,
        &Globals::cItemListPrice_04,
    };

    for (char **slot : slots)
        *slot = new char[1]{'\0'};
    ndk_resetNativeItemInformationList();
    for (char **slot : slots) {
        if (*slot != nullptr) {
            std::fputs("item info reset smoke: live slot remains\n", stderr);
            return 1;
        }
    }
    std::puts("item info reset smoke: OK");
    return 0;
}
