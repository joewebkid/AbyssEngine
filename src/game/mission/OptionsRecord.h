#ifndef GOF2_OPTIONSRECORD_H
#define GOF2_OPTIONSRECORD_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "GameRecord.h"
#include "Mission.h"
#include "engine/render/Engine.h"
#include "game/world/Wanted.h"

struct OptionsRecord {
    uint8_t field_0x00[8];
    uint16_t flag_word_0x8;
    uint8_t flag_0xa;
    uint8_t field_0x0b[2];

    union {
        uint16_t flag_word_0xd;

        struct {
            uint8_t musicMasterVolumeFlag;
            uint8_t autoAdvanceEnabled;
        };
    };

    uint8_t flag_0xf;
    uint8_t field_0x10[3];
    uint8_t flag_0x13;
    uint8_t field_0x14;
    uint8_t flag_0x15;
    uint8_t field_0x16;
    uint8_t flag_0x17;
    uint8_t field_0x18[4];
    uint16_t flag_word_0x1c;
    uint8_t flag_0x1e;
    uint8_t field_0x1f;
    int32_t flag_dword_0x20;
    uint8_t flag_0x24;
    uint8_t field_0x25;
    uint8_t flag_0x26;
    uint8_t field_0x27[5];
    uint32_t fadeValue;
    uint8_t field_0x30;
    uint8_t flag_0x31;

    union {
        int32_t flag_dword_0x32;

        struct {
            uint8_t field_0x32[2];
            uint8_t flag_0x34;
            uint8_t flag_0x35;
        };
    };

    uint8_t flag_0x36;
    uint8_t flag_0x37;

    union {
        uint16_t flag_word_0x38;

        struct {
            uint8_t flag_0x38;
            uint8_t flag_0x39;
        };
    };

    uint8_t field_0x3a;
    uint8_t flag_0x3b;
    uint8_t field_0x3c[0xc];
    uint8_t firstRunPreviewChecked;
};

#endif
