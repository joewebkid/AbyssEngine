#ifndef GOF2_AEPAKFILEENTRY_H
#define GOF2_AEPAKFILEENTRY_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "FileInterface.h"
#include "game/core/String.h"
#include "AELowLevelFile.h"
#include "AENormalFile.h"
#include "AEPakFile.h"

struct AEPakFileEntry {
    uint32_t crc;
    String name;
    uint32_t offset;
    uint32_t packedSize;
    uint32_t size;
};
#endif
