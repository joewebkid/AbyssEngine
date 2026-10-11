#ifndef GOF2_GAMEDATA_H
#define GOF2_GAMEDATA_H
#include "engine/core/Array.h"
#include "AEString.h"
#include <cstddef>
#include <cstdint>
class Globals;
namespace AbyssEngine { class Engine; }

struct __attribute__((packed)) GameDataLateFlags {
    uint8_t field_0x6c;
    int32_t field_0x6d;
    int32_t field_0x71;
    uint16_t field_0x75;
    uint8_t field_0x77;
    uint16_t field_0x78;
    uint8_t field_0x7a;
    uint8_t pad_0x7b;
};
static_assert(sizeof(GameDataLateFlags) == 0x10, "GameData late flags @ 0x6c..0x7b");

class GameData {
public:
    Globals *globals;
    union {
        uint16_t field_0x04;
        struct {
            uint8_t flag_0x04;
            union { uint8_t screenshotResultFlag, modalActive; };
        };
    };
    union { int32_t field_0x08, storeResultCode; };
    union {
        uint16_t field_0x0c;
        struct {
            union { uint8_t storeInitFlag, transitionActive; };
            union { uint8_t rateGameFlagA, radioHiddenFlag; };
        };
    };
    union { uint8_t field_0x0e, rateGameFlagB; };
    int32_t field_0x10;
    union {
        uint16_t field_0x14;
        struct { uint8_t flag_0x14, flag_0x15; };
    };
    AbyssEngine::String field_0x18;
    AbyssEngine::String field_0x24;
    AbyssEngine::String field_0x30;

    union {
        int32_t field_0x3c;
        struct {
            uint8_t dlcMenuAcceptedFlag, dlcMenuRequestFlag, flag_0x3e,
                    dlcRestoreRequestFlag;
        };
    };
    union {
        int32_t field_0x40;
        struct {
            uint8_t dlcStoreReadyFlag, purchaseResultFlag, purchaseErrorFlag,
                    flag_0x43;
        };
    };
    uint8_t field_0x44;
    union { int32_t field_0x48; uint32_t purchaseCode; };
    union { uint8_t field_0x4c, dlcMenuAckFlag; };
    int32_t field_0x50;
    Array<AbyssEngine::String *> *productIdList;
    int32_t *purchaseCount;
    int32_t field_0x5c;
    Array<int32_t> *purchaseCreditResults;
    int32_t field_0x64;
    int32_t field_0x68;
    GameDataLateFlags lateFlags;
    AbyssEngine::String field_0x7c;
    AbyssEngine::String field_0x88;
    AbyssEngine::String field_0x94;
    union {
        int32_t field_0xa0;
        struct { uint8_t flag_0xa0, flag_0xa1, flag_0xa2, flag_0xa3; };
    };
    uint8_t field_0xa4;
    uint8_t field_0xa5;
    AbyssEngine::String field_0xa8;
    uint8_t field_0xb4;
    AbyssEngine::String field_0xb8;
    uint8_t field_0xc4;

    GameData();

    ~GameData();
};

#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(GameData) == 0xc8, "GameData Android ARM size");
static_assert(offsetof(GameData, screenshotResultFlag) == 0x05, "app data +0x05");
static_assert(offsetof(GameData, storeInitFlag) == 0x0c, "app data +0x0c");
static_assert(offsetof(GameData, radioHiddenFlag) == 0x0d, "app data +0x0d");
static_assert(offsetof(GameData, flag_0x15) == 0x15, "app data +0x15");
static_assert(offsetof(GameData, dlcMenuAcceptedFlag) == 0x3c, "app data +0x3c");
static_assert(offsetof(GameData, dlcMenuRequestFlag) == 0x3d, "app data +0x3d");
static_assert(offsetof(GameData, dlcStoreReadyFlag) == 0x40, "app data +0x40");
static_assert(offsetof(GameData, purchaseCode) == 0x48, "app data +0x48");
static_assert(offsetof(GameData, dlcMenuAckFlag) == 0x4c, "app data +0x4c");
static_assert(offsetof(GameData, productIdList) == 0x54, "app data +0x54");
static_assert(offsetof(GameData, purchaseCount) == 0x58, "app data +0x58");
static_assert(offsetof(GameData, purchaseCreditResults) == 0x60, "app data +0x60");
static_assert(offsetof(GameData, lateFlags) == 0x6c, "app data +0x6c");
static_assert(offsetof(GameData, field_0x7c) == 0x7c, "app data +0x7c");
static_assert(offsetof(GameData, flag_0xa0) == 0xa0, "app data +0xa0");
#else
static_assert(offsetof(GameData, dlcMenuRequestFlag) >=
              offsetof(GameData, field_0x30) + sizeof(AbyssEngine::String),
              "host DLC flags must not overlap String storage");
#endif


void OnDestroyApplication(AbyssEngine::Engine * engine);

#endif
