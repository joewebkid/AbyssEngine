#ifndef GOF2_SHIPDEFTABLE_H
#define GOF2_SHIPDEFTABLE_H
#include <cstdint>

struct ShipDefTable {
    uint8_t _pad_0x00[0x8];

    int32_t itemDef_primary;

    uint8_t _pad_0x0c[0x8];

    union {
        int32_t shipDefId;
        int32_t itemDef_secondary;
    };

    uint8_t _pad_0x18[0x38];

    int32_t itemDef_0x50;

    uint8_t _pad_0x54[0x24];

    int32_t shipDefId_0x78;

    uint8_t _pad_0x7c[0x14];

    int32_t itemDef_missile;

    uint8_t _pad_0x94[0x1c];

    int32_t itemDef_0xb0;

    uint8_t _pad_0xb4[0x18];

    union {
        int32_t itemDef_equip0xcc;
        int32_t itemDef_0xcc;
    };

    uint8_t _pad_0xd0[0x10];

    int32_t itemDef_0xe0;

    uint8_t _pad_0xe4[0x2c];

    int32_t itemDef_0x110;

    uint8_t _pad_0x114[0x30];

    union {
        int32_t itemDef_equip0x144;
        int32_t itemDef_0x144;
    };

    uint8_t _pad_0x148[0xc];

    union {
        int32_t itemDef_equip0x154;
        int32_t itemDef_0x154;
    };

    union {
        int32_t itemDef_equip0x158;
        int32_t itemDef_0x158;
    };

    uint8_t _pad_0x15c[0x8c];

    int32_t itemDef_cargo0x1e8;

    uint8_t _pad_0x1ec[0xd4];

    int32_t itemDef_0x2c0;
};

static_assert(sizeof(struct ShipDefTable) == 0x2c4, "ShipDefTable row size");

#endif
