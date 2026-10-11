#include "platform/android/NdkHooks.h"

#include "engine/render/Engine.h"
#include "game/core/Globals.h"
#include "game/mission/Status.h"

#include <cstdio>
#include <cstring>

namespace AbyssEngine {
String::String() : data(nullptr), length(0) {}
String::~String() {}
}

Status::Status() : field_114(0) {}
Status::~Status() {}

static int visibilityCalls;
static int visibilityIndex;
static bool visibilityValue;

void Status::setSystemVisibility(int index, bool value) {
    ++visibilityCalls;
    visibilityIndex = index;
    visibilityValue = value;
}

Status *Globals::status = nullptr;
unsigned char Globals::options[100] = {};
AbyssEngine::Engine *gEngine = nullptr;

extern int gi_iap_buy_dlc1_pressed;
extern int gi_iap_buy_dlc2_pressed;
extern int gi_iap_buy_dlc3_pressed;
extern int gi_iap_buy_dlc4_pressed;
extern int gi_iap_buy_dlc5_pressed;

static int *const pending[5] = {
    &gi_iap_buy_dlc1_pressed, &gi_iap_buy_dlc2_pressed,
    &gi_iap_buy_dlc3_pressed, &gi_iap_buy_dlc4_pressed,
    &gi_iap_buy_dlc5_pressed,
};

static int failures;

static void check(bool condition, const char *message) {
    if (!condition) {
        std::fprintf(stderr, "premium IAP smoke: %s\n", message);
        ++failures;
    }
}

static void reset(Status &status, unsigned char option) {
    std::memset(Globals::options, option, sizeof(Globals::options));
    for (int *flag : pending)
        *flag = 1;
    status.field_114 = 0;
    visibilityCalls = 0;
    visibilityIndex = -1;
    visibilityValue = false;
}

int main() {
    Status status;
    Globals::status = &status;
    gEngine = reinterpret_cast<AbyssEngine::Engine *>(1); // non-null gate only

    constexpr unsigned masks[5] = {0x01, 0x02, 0x04, 0x08, 0x15};
    for (unsigned pack = 0; pack < 5; ++pack) {
        for (unsigned bought = 0; bought <= 1; ++bought) {
            reset(status, bought ? 0 : 7);
            ndk_iapBoughtPremium(pack, bought);
            for (unsigned i = 0; i < 5; ++i) {
                const unsigned char expected = (masks[pack] & (1u << i))
                    ? static_cast<unsigned char>(bought) : static_cast<unsigned char>(bought ? 0 : 7);
                check(Globals::options[0x35 + i] == expected, "ownership flag");
                check(*pending[i] == (i == pack ? 0 : 1), "pending flag");
            }
            const bool revealsSystem = pack == 0 || pack == 2 || pack == 4;
            check(visibilityCalls == (revealsSystem ? 1 : 0), "system visibility call");
            if (revealsSystem)
                check(visibilityIndex == 25 && visibilityValue, "system visibility value");
            check(status.field_114 == (pack == 1 ? 3 : 0), "campaign state");
        }
    }

    reset(status, 7);
    ndk_iapBoughtPremium(5, 1);
    ndk_iapBoughtPremium(0, 2);
    check(Globals::options[0x35] == 7 && *pending[0] == 1 && visibilityCalls == 0,
          "invalid arguments must not mutate state");

    gEngine = nullptr;
    ndk_iapBoughtPremium(0, 1);
    check(Globals::options[0x35] == 7 && *pending[0] == 1 && visibilityCalls == 0,
          "missing engine must not mutate state");

    if (failures == 0)
        std::puts("premium IAP smoke: OK");
    return failures ? 1 : 0;
}
