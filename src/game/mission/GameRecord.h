#ifndef GOF2_GAMERECORD_H
#define GOF2_GAMERECORD_H
#include <cstddef>
#include <cstdint>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
class Ship;
class Station;
class Standing;
class BluePrint;
class PendingProduct;
class Agent;
// ARM layout is byte-faithful to the original 32-bit binary (449 bytes / 0x1c1).
// The on-disk save record is read/written almost entirely through raw offsets:
// GameRecord.cpp treats `this` as `uint32_t rec[]` (rec[N] == *(uint32_t*)(this
// + N*4)) and as a byte array, and RecordHandler.cpp casts a fresh GameRecord to
// `char *`. So most members are `field_0xNN` slots that sit at struct offset 0xNN
// and only exist to make later fields land at their true offsets on ARM.
// Named fields used by source code are kept. Filler in the byte-accessed
// regions (0x100..0x11b) is `uint8_t` so those
// single-byte reads/writes have a home. Locked in by the static_asserts at end.
// pack(1): the original struct has alignment 1 (no padding); without it the float
// and pointer members would force the ARM size up past 0x1c1 and shift offsets.
// Native 64-bit preview strings live after the existing fields to avoid overlap.
#pragma pack(push, 1)
class alignas(__SIZEOF_POINTER__ == 4 ? 1 : alignof(AbyssEngine::String)) GameRecord {
public:
    void *data;                  // 0x000  rec[0]
    uint32_t field_0x04;         // 0x004  rec[1]
    uint32_t field_0x08;         // 0x008  rec[2]
    uint32_t field_0x0c;         // 0x00c  rec[3]
    union {
        struct {
            void *playTimeObj;   // 0x010  rec[4] (String* play-time, used by createRecordButtons)
            uint32_t field_0x14; // 0x014  rec[5]
        };
        int64_t playTime64;      // 0x010  8-byte play-time (read/written as i64)
    };
    uint32_t field_0x18;         // 0x018  rec[6]
    uint32_t field_0x1c;         // 0x01c  rec[7]
    uint32_t killsText;          // 0x020  rec[8] (String at 0x20, accessed via &)
    uint32_t field_0x24;         // 0x024  rec[9]
    uint32_t field_0x28;         // 0x028  rec[0xa]
    uint32_t field_0x2c;         // 0x02c  rec[0xb]
    uint32_t field_0x30;         // 0x030  rec[0xc]
    uint32_t field_0x34;         // 0x034  rec[0xd]
    uint32_t field_0x38;         // 0x038  rec[0xe]
    uint32_t field_0x3c;         // 0x03c  rec[0xf]
    uint32_t field_0x40;         // 0x040  rec[0x10]
    uint32_t field_0x44;         // 0x044  rec[0x11]
    uint32_t field_0x48;         // 0x048  rec[0x12]
    uint32_t field_0x4c;         // 0x04c  rec[0x13]
    uint32_t field_0x50;         // 0x050  rec[0x14]
    uint32_t field_0x54;         // 0x054  rec[0x15]
    uint32_t field_0x58;         // 0x058  rec[0x16]
    uint32_t field_0x5c;         // 0x05c  rec[0x17]
    uint32_t field_0x60;         // 0x060  rec[0x18]
    uint32_t field_0x64;         // 0x064  rec[0x19]
    uint32_t field_0x68;         // 0x068  rec[0x1a]
    uint32_t field_0x6c;         // 0x06c  rec[0x1b]
    uint32_t field_0x70;         // 0x070  rec[0x1c]
    uint32_t field_0x74;         // 0x074  rec[0x1d]
    uint32_t field_0x78;         // 0x078  rec[0x1e]
    uint32_t field_0x7c;         // 0x07c  rec[0x1f]
    uint32_t field_0x80;         // 0x080  rec[0x20]
    uint32_t field_0x84;         // 0x084  rec[0x21]
    uint32_t field_0x88;         // 0x088  rec[0x22]
    uint32_t field_0x8c;         // 0x08c  rec[0x23]
    uint32_t field_0x90;         // 0x090  rec[0x24]
    uint32_t field_0x94;         // 0x094  rec[0x25]
    uint32_t field_0x98;         // 0x098  rec[0x26]
    uint32_t field_0x9c;         // 0x09c  rec[0x27]
    uint32_t field_0xa0;         // 0x0a0  rec[0x28]
    uint32_t field_0xa4;         // 0x0a4  rec[0x29]
    uint32_t field_0xa8;         // 0x0a8  rec[0x2a]
    uint32_t field_0xac;         // 0x0ac  rec[0x2b]
    uint32_t field_0xb0;         // 0x0b0  rec[0x2c]
    uint32_t field_0xb4;         // 0x0b4  rec[0x2d]
    uint32_t field_0xb8;         // 0x0b8  rec[0x2e]
    uint32_t field_0xbc;         // 0x0bc  rec[0x2f]
    uint32_t field_0xc0;         // 0x0c0  rec[0x30]
    uint32_t field_0xc4;         // 0x0c4  rec[0x31]
    uint32_t field_0xc8;         // 0x0c8  rec[0x32]
    uint32_t field_0xcc;         // 0x0cc  rec[0x33]
    uint32_t field_0xd0;         // 0x0d0  rec[0x34]
    uint8_t  field_0xd4;         // 0x0d4  byte (ctor: *(t+0xd4)=0)
    uint8_t  field_0xd5;         // 0x0d5  rec[0x35].b0 (read as byte)
    uint8_t  field_0xd6;         // 0x0d6
    uint8_t  field_0xd7;         // 0x0d7
    uint32_t field_0xd8;         // 0x0d8  rec[0x36]
    uint8_t  field_0xdc;         // 0x0dc  byte (ctor: *(t+0xdc)=0); rec[0x37].b0
    uint8_t  field_0xdd;         // 0x0dd
    uint8_t  field_0xde;         // 0x0de
    uint8_t  field_0xdf;         // 0x0df
    uint32_t field_0xe0;         // 0x0e0  rec[0x38]
    uint32_t field_0xe4;         // 0x0e4  rec[0x39]
    uint32_t field_0xe8;         // 0x0e8  rec[0x3a]
    uint32_t field_0xec;         // 0x0ec  rec[0x3b]
    uint32_t field_0xf0;         // 0x0f0  rec[0x3c]
    uint32_t field_0xf4;         // 0x0f4  rec[0x3d]
    uint32_t field_0xf8;         // 0x0f8  rec[0x3e]
    uint32_t field_0xfc;         // 0x0fc  rec[0x3f]
    // 0x100..0x11b: dense byte-accessed region (flags / packed bytes).
    uint8_t  field_0x100;        // 0x100  rec[0x40].b0
    uint8_t  field_0x101;        // 0x101
    uint8_t  field_0x102;        // 0x102  byte flag (load: *(rec+0x102))
    uint8_t  field_0x103;        // 0x103
    uint8_t  field_0x104;        // 0x104  rec[0x41].b0
    uint8_t  field_0x105;        // 0x105  byte
    uint8_t  field_0x106;        // 0x106  byte
    uint8_t  field_0x107;        // 0x107  byte
    uint8_t  field_0x108;        // 0x108  rec[0x42].b0
    uint8_t  field_0x109;        // 0x109  byte
    uint8_t  field_0x10a;        // 0x10a  byte
    uint8_t  field_0x10b;        // 0x10b  byte (ctor writes uint32 @0x10b)
    uint8_t  field_0x10c;        // 0x10c  rec[0x43].b0
    uint8_t  field_0x10d;        // 0x10d  byte
    uint8_t  field_0x10e;        // 0x10e  byte
    uint8_t  field_0x10f;        // 0x10f  byte (ctor writes uint32 @0x10f)
    uint8_t  field_0x110;        // 0x110  rec[0x44].b0
    uint8_t  field_0x111;        // 0x111  byte
    uint8_t  field_0x112;        // 0x112  byte
    uint8_t  field_0x113;        // 0x113  byte (ctor writes uint32 @0x113)
    uint8_t  field_0x114;        // 0x114  rec[0x45].b0
    uint8_t  dlcRequiredFlag;    // 0x115  (load: *(rec+0x115))
    uint8_t  field_0x116;        // 0x116
    uint8_t  versionMismatchFlag; // 0x117  (ctor writes u32 @0x117)
    uint8_t  field_0x118;        // 0x118  rec[0x46].b0
    uint8_t  field_0x119;        // 0x119  byte flag (load: *(rec+0x119))
    uint8_t  field_0x11a;        // 0x11a  byte
    uint8_t  field_0x11b;        // 0x11b
    union {
        float    rank;           // 0x11c  rec[0x47] (ctor: this->rank = 0)
        int32_t  rankBits;       // 0x11c  rank as raw bits (serialised via Write_f32)
    };
    uint32_t field_0x120;        // 0x120  rec[0x48]
    uint32_t field_0x124;        // 0x124  rec[0x49]
    uint32_t field_0x128;        // 0x128  rec[0x4a]
    uint32_t field_0x12c;        // 0x12c  rec[0x4b]
#if __SIZEOF_POINTER__ == 4
    Ship *ship;                  // 0x130  rec[0x4c]
#else
    uint32_t field_0x130;
#endif
    uint32_t field_0x134;        // 0x134  rec[0x4d]
#if __SIZEOF_POINTER__ == 4
    Station *station;            // 0x138  rec[0x4e]
#else
    uint32_t field_0x138;
#endif
#if __SIZEOF_POINTER__ == 4
    Standing *standing;          // 0x13c  rec[0x4f]
    Array<BluePrint *> *bluePrints; // 0x140  rec[0x50]
#else
    uint32_t field_0x13c;
    uint32_t field_0x140;
#endif
#if __SIZEOF_POINTER__ == 4
    Array<PendingProduct *> *pendingProducts; // 0x144  rec[0x51]
    Array<Agent *> *agents;                    // 0x148  rec[0x52]
#else
    uint32_t field_0x144;
    uint32_t field_0x148;
#endif
#if __SIZEOF_POINTER__ == 4
    Array<AbyssEngine::String *> *wingmen; // 0x14c  rec[0x53]
#else
    uint32_t field_0x14c;
#endif
    uint32_t field_0x150;        // 0x150  rec[0x54]
    uint32_t field_0x154;        // 0x154  rec[0x55]
#if __SIZEOF_POINTER__ == 4
    int *wingmenStats;           // 0x158  rec[0x56], five ints
#else
    uint32_t field_0x158;
#endif
    uint32_t field_0x15c;        // 0x15c  rec[0x57]
    uint32_t field_0x160;        // 0x160  rec[0x58]
    uint32_t field_0x164;        // 0x164  rec[0x59]
    uint32_t field_0x168;        // 0x168  rec[0x5a]
    uint32_t field_0x16c;        // 0x16c  rec[0x5b]
    uint32_t field_0x170;        // 0x170  rec[0x5c]
    uint32_t field_0x174;        // 0x174  rec[0x5d]
    uint32_t field_0x178;        // 0x178  rec[0x5e]
    uint32_t field_0x17c;        // 0x17c  rec[0x5f]
    uint32_t field_0x180;        // 0x180  rec[0x60]
    uint32_t field_0x184;        // 0x184  rec[0x61]
#if __SIZEOF_POINTER__ == 4
    AbyssEngine::String previewSystemName; // 0x188, 12-byte ARM String
    AbyssEngine::String previewStationName; // 0x194, 12-byte ARM String
#else
    uint32_t field_0x188;
    uint32_t field_0x18c;
    uint32_t field_0x190;
    uint32_t field_0x194;
    uint32_t field_0x198;
    uint32_t field_0x19c;
#endif
    uint32_t shipId;             // 0x1a0  rec[0x68] (ctor: this->shipId = 0xffffffff)
    uint32_t field_0x1a4;        // 0x1a4  rec[0x69]
    uint32_t field_0x1a8;        // 0x1a8  rec[0x6a]
    uint32_t field_0x1ac;        // 0x1ac  rec[0x6b]
    uint32_t field_0x1b0;        // 0x1b0  rec[0x6c]
    uint32_t field_0x1b4;        // 0x1b4  rec[0x6d]
    uint32_t field_0x1b8;        // 0x1b8  rec[0x6e]
    uint32_t field_0x1bc;        // 0x1bc  rec[0x6f] (kRecordExtendedTag check)
    uint8_t  field_0x1c0;        // 0x1c0  byte (load: *(rec+0x70))

#if __SIZEOF_POINTER__ != 4
    // pack(1) suppresses member alignment, so pad the native strings explicitly.
    uint8_t _nativePreviewPadding[7];
    alignas(AbyssEngine::String) AbyssEngine::String previewSystemName;
    alignas(AbyssEngine::String) AbyssEngine::String previewStationName;
    Ship *ship;
    Station *station;
    Standing *standing;
    Array<BluePrint *> *bluePrints;
    Array<PendingProduct *> *pendingProducts;
    Array<Agent *> *agents;
    Array<AbyssEngine::String *> *wingmen;
    int *wingmenStats;
#endif

    AbyssEngine::String &getPreviewSystemName() { return previewSystemName; }
    AbyssEngine::String &getPreviewStationName() { return previewStationName; }
    const AbyssEngine::String &getPreviewSystemName() const { return previewSystemName; }
    const AbyssEngine::String &getPreviewStationName() const { return previewStationName; }
    Ship *getShip() const { return ship; }
    Station *getStation() const { return station; }
    void setShip(Ship *value) { ship = value; }
    void setStation(Station *value) { station = value; }
    Standing *getStanding() const { return standing; }
    Array<BluePrint *> *getBluePrints() const { return bluePrints; }
    void setStanding(Standing *value) { standing = value; }
    void setBluePrints(Array<BluePrint *> *value) { bluePrints = value; }
    Array<PendingProduct *> *getPendingProducts() const { return pendingProducts; }
    Array<Agent *> *getAgents() const { return agents; }
    void setPendingProducts(Array<PendingProduct *> *value) { pendingProducts = value; }
    void setAgents(Array<Agent *> *value) { agents = value; }
    Array<AbyssEngine::String *> *getWingmen() const { return wingmen; }
    int *getWingmenStats() const { return wingmenStats; }
    void setWingmen(Array<AbyssEngine::String *> *value) { wingmen = value; }
    void setWingmenStats(int *value) { wingmenStats = value; }

    GameRecord();

    ~GameRecord();

    void load();
};
#pragma pack(pop)

#if __SIZEOF_POINTER__ == 4  // live in 32-bit MATCH build (was #ifdef GOF2_MATCH, which is never defined)
static_assert(sizeof(GameRecord) == 449, "GameRecord must be 449 bytes (0x1c1)");
static_assert(offsetof(GameRecord, data) == 0x0, "");
static_assert(offsetof(GameRecord, field_0x98) == 0x98, "");
static_assert(offsetof(GameRecord, field_0xa8) == 0xa8, "");
static_assert(offsetof(GameRecord, field_0xc8) == 0xc8, "");
static_assert(offsetof(GameRecord, field_0xd4) == 0xd4, "");
static_assert(offsetof(GameRecord, field_0xd8) == 0xd8, "");
static_assert(offsetof(GameRecord, field_0xdc) == 0xdc, "");
static_assert(offsetof(GameRecord, field_0xe0) == 0xe0, "");
static_assert(offsetof(GameRecord, field_0x100) == 0x100, "");
static_assert(offsetof(GameRecord, field_0x102) == 0x102, "");
static_assert(offsetof(GameRecord, dlcRequiredFlag) == 0x115, "dlcRequiredFlag");
static_assert(offsetof(GameRecord, versionMismatchFlag) == 0x117, "versionMismatchFlag");
static_assert(offsetof(GameRecord, playTimeObj) == 0x10, "playTimeObj");
static_assert(offsetof(GameRecord, killsText) == 0x20, "killsText");
static_assert(sizeof(AbyssEngine::String) == 12, "ARM String layout");
static_assert(offsetof(GameRecord, previewSystemName) == 0x188, "previewSystemName");
static_assert(offsetof(GameRecord, previewStationName) == 0x194, "previewStationName");
static_assert(offsetof(GameRecord, field_0x119) == 0x119, "");
static_assert(offsetof(GameRecord, rank) == 0x11c, "");
static_assert(offsetof(GameRecord, ship) == 0x130, "ship");
static_assert(offsetof(GameRecord, station) == 0x138, "station");
static_assert(offsetof(GameRecord, standing) == 0x13c, "standing");
static_assert(offsetof(GameRecord, bluePrints) == 0x140, "bluePrints");
static_assert(offsetof(GameRecord, pendingProducts) == 0x144, "pendingProducts");
static_assert(offsetof(GameRecord, agents) == 0x148, "agents");
static_assert(offsetof(GameRecord, wingmen) == 0x14c, "wingmen");
static_assert(offsetof(GameRecord, wingmenStats) == 0x158, "wingmenStats");
static_assert(offsetof(GameRecord, shipId) == 0x1a0, "");
static_assert(offsetof(GameRecord, field_0x1b4) == 0x1b4, "");
static_assert(offsetof(GameRecord, field_0x1b8) == 0x1b8, "");
static_assert(offsetof(GameRecord, field_0x1c0) == 0x1c0, "");
#endif // 32-bit ARM layout

#if __SIZEOF_POINTER__ != 4
static_assert(offsetof(GameRecord, previewSystemName) % alignof(AbyssEngine::String) == 0,
              "native previewSystemName alignment");
static_assert(offsetof(GameRecord, previewStationName) % alignof(AbyssEngine::String) == 0,
              "native previewStationName alignment");
static_assert(offsetof(GameRecord, ship) % alignof(Ship *) == 0, "native ship alignment");
static_assert(offsetof(GameRecord, station) % alignof(Station *) == 0,
              "native station alignment");
static_assert(offsetof(GameRecord, standing) % alignof(Standing *) == 0,
              "native standing alignment");
static_assert(offsetof(GameRecord, bluePrints) % alignof(Array<BluePrint *> *) == 0,
              "native bluePrints alignment");
static_assert(offsetof(GameRecord, pendingProducts) % alignof(Array<PendingProduct *> *) == 0,
              "native pendingProducts alignment");
static_assert(offsetof(GameRecord, agents) % alignof(Array<Agent *> *) == 0,
              "native agents alignment");
static_assert(offsetof(GameRecord, wingmen) % alignof(Array<AbyssEngine::String *> *) == 0,
              "native wingmen alignment");
static_assert(offsetof(GameRecord, wingmenStats) % alignof(int *) == 0,
              "native wingmenStats alignment");
#endif

#endif
