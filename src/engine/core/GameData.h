#ifndef GOF2_GAMEDATA_H
#define GOF2_GAMEDATA_H
#include "engine/core/Array.h"
#include "AEString.h"
class Globals;
namespace AbyssEngine { class Engine; }

class GameData {
public:
    Globals *globals;
    uint16_t field_0x04;
    int32_t field_0x08;
    uint16_t field_0x0c;
    uint8_t field_0x0e;
    int32_t field_0x10;
    uint16_t field_0x14;
    AbyssEngine::String field_0x18;
    AbyssEngine::String field_0x24;
    AbyssEngine::String field_0x30;

    int32_t field_0x3c;
    int32_t field_0x40;
    uint8_t field_0x44;
    int32_t field_0x48;
    uint8_t field_0x4c;
    int32_t field_0x50;
    int32_t field_0x54;
    int32_t field_0x58;
    int32_t field_0x5c;
    int32_t field_0x60;
    int32_t field_0x64;
    int32_t field_0x68;
    int32_t field_0x6c;
    int32_t field_0x6d;
    int32_t field_0x71;
    uint16_t field_0x75;
    uint8_t field_0x77;
    uint16_t field_0x78;
    uint8_t field_0x7a;
    AbyssEngine::String field_0x7c;
    AbyssEngine::String field_0x88;
    AbyssEngine::String field_0x94;
    int32_t field_0xa0;
    uint8_t field_0xa4;
    uint8_t field_0xa5;
    AbyssEngine::String field_0xa8;
    uint8_t field_0xb4;
    AbyssEngine::String field_0xb8;
    uint8_t field_0xc4;

    GameData();

    ~GameData();
};


void OnDestroyApplication(AbyssEngine::Engine * engine);

#endif
