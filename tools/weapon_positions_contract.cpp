#include "engine/file/FileRead.h"
#include "engine/core/GameText.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <new>
#include <vector>

using Outer = Array<Array<Vector *> *>;
struct Entry { int16_t type, x, y, z; uint32_t ex, ey, ez; };
struct Record { int ship; unsigned begin, count, end, reads; };
#include "weapon_contract_oracle.inc"

// Real String.cpp is linked; unrelated language/float/print routes fail closed.
int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
String operator+(const String &, const String &) { std::abort(); }
}

struct OwnedVector { void *pointer; unsigned frees; };
static OwnedVector ownedVectors[128];
static unsigned ownedCount, allReleased;
static OwnedVector arrayObjects[5], arrayBuffers[5];
static unsigned arrayCount;
static void *pathData;
static bool pathReleased;
void *operator new(size_t size) {
    if (void *p = std::malloc(size)) return p;
    throw std::bad_alloc();
}
void *operator new[](size_t size) { return ::operator new(size); }
void operator delete(void *pointer) noexcept {
    for (unsigned i = 0; i < ownedCount; ++i)
        if (ownedVectors[i].pointer == pointer) assert(++ownedVectors[i].frees == 1);
    for (unsigned i = 0; i < arrayCount; ++i)
        if (arrayObjects[i].pointer == pointer) assert(++arrayObjects[i].frees == 1);
    std::free(pointer);
}
void operator delete(void *pointer, size_t) noexcept { ::operator delete(pointer); }
void operator delete[](void *pointer) noexcept {
    if (pointer == pathData) pathReleased = true;
    for (unsigned i = 0; i < arrayCount; ++i)
        if (arrayBuffers[i].pointer == pointer) assert(++arrayBuffers[i].frees == 1);
    std::free(pointer);
}
void operator delete[](void *pointer, size_t) noexcept { ::operator delete[](pointer); }

static std::vector<unsigned char> original;
static const std::vector<unsigned char> *active;
static size_t cursor;
static bool present, opened;
static unsigned opens, closes, failedReads;
struct ReadEvent { size_t offset; uint32_t size; };
static std::vector<ReadEvent> reads;
static const char path[] = "data/bin/weapons_hd.bin";

uint32_t AEFile::FileExist(const String &name) {
    assert(name.length == int(std::strlen(path)));
    for (int i = 0; i <= name.length; ++i) assert(name.data[i] == path[i]);
    pathData = name.data;
    pathReleased = false;
    return present;
}
uint32_t AEFile::OpenRead(const char *name, uint32_t *handle) {
    assert(pathReleased && std::strcmp(name, path) == 0 && active && !opened);
    cursor = 0;
    reads.clear();
    *handle = 1;
    opened = true;
    ++opens;
    return 1;
}
uint32_t AEFile::Read(uint32_t size, void *out, uint32_t handle) {
    assert(opened && handle == 1 && (size == 2 || size == 4));
    reads.push_back({cursor, size});
    if (size > active->size() - cursor) { ++failedReads; return 0; }
    std::memcpy(out, active->data() + cursor, size);
    cursor += size;
    return size;
}
void AEFile::Close(uint32_t handle) {
    assert(opened && handle == 1);
    opened = false;
    ++closes;
}
#include "weapon_contract_body.inc"

namespace Consumers {
struct Gun { Vector offset; void setOffset(Vector *); };
struct PlayerEgo { Vector turretOffsetVec; void setTurretPosition(Vector); };
}
#include "weapon_contract_consumers.inc"
static void releaseForCreatePlayer(Outer *wpos) {
#include "weapon_contract_cleanup.inc"
}

static float fromBits(uint32_t bits) { float f; std::memcpy(&f, &bits, sizeof(f)); return f; }
static uint32_t bits(float f) { uint32_t b; std::memcpy(&b, &f, sizeof(b)); return b; }
static void same(const Vector *v, float x, float y, float z) {
    assert(v && bits(v->x) == bits(x) && bits(v->y) == bits(y) && bits(v->z) == bits(z));
}
static void markAndRelease(Outer *positions) {
    assert(positions && positions->size() == 4 && positions->capacity_ == 4 && !ownedCount);
    Consumers::Gun gun;
    Consumers::PlayerEgo ego;
    bool usedGun = false, usedTurret = false;
    Vector gunInput{}, turretInput{};
    assert(!arrayCount);
    arrayObjects[0] = {positions, 0};
    arrayBuffers[0] = {positions->data(), 0};
    arrayCount = 1;
    for (unsigned type = 0; type < 4; ++type) {
        auto *slot = (*positions)[type];
        if (!slot) continue;
        assert(arrayCount < 5);
        arrayObjects[arrayCount] = {slot, 0};
        arrayBuffers[arrayCount++] = {slot->data(), 0};
        assert(slot->size() && slot->capacity_ == slot->size());
        if (type == 3) assert(slot->size() % 2 == 0);
        if (type < 2 && !usedGun) {
            gunInput = *(*slot)[0]; gun.setOffset((*slot)[0]); usedGun = true;
        }
        if (type == 2) {
            turretInput = *(*slot)[0]; ego.setTurretPosition(*(*slot)[0]); usedTurret = true;
        }
        for (auto *v : *slot) {
            assert(v && ownedCount < 128);
            for (unsigned i = 0; i < ownedCount; ++i) assert(ownedVectors[i].pointer != v);
            ownedVectors[ownedCount++] = {v, 0};
        }
    }
    releaseForCreatePlayer(positions);
    for (unsigned i = 0; i < ownedCount; ++i) assert(ownedVectors[i].frees == 1);
    for (unsigned i = 0; i < arrayCount; ++i)
        assert(arrayObjects[i].frees == 1 && arrayBuffers[i].frees == 1);
    allReleased += ownedCount;
    ownedCount = 0;
    arrayCount = 0;
    if (usedGun) same(&gun.offset, gunInput.x, gunInput.y, gunInput.z + 100.0f);
    if (usedTurret) same(&ego.turretOffsetVec, turretInput.x, turretInput.y, turretInput.z);
}
template<class T> static void append(std::vector<unsigned char> &bytes, T value) {
    auto *p = reinterpret_cast<unsigned char *>(&value);
    bytes.insert(bytes.end(), p, p + sizeof(value));
}

int main(int argc, char **argv) {
    assert(argc == 2 && sizeof(Vector) == 12 && sizeof(float) == 4);
    std::ifstream stream(argv[1], std::ios::binary);
    assert(stream.good());
    original.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    active = &original;
    {
        FileRead reader;
        assert(reader.loadWeaponPositions(63) == nullptr);
        assert(pathReleased && !opens && !closes && reads.empty());
    }
    present = true;
    for (const auto &record : expectedRecords) {
        Outer *positions;
        { FileRead reader; positions = reader.loadWeaponPositions(record.ship); }
        assert(!opened && cursor == record.end && reads.size() == record.reads);
        size_t expectedOffset = 0;
        for (unsigned i = 0; i < reads.size(); ++i) {
            assert(reads[i].offset == expectedOffset && reads[i].size == expectedReadSizes[i]);
            expectedOffset += expectedReadSizes[i];
        }
        assert(expectedOffset == record.end && positions && positions->size() == 4);
        unsigned counts[4] = {}, indices[4] = {};
        for (unsigned i = 0; i < record.count; ++i) {
            const auto &e = expectedEntries[record.begin + i];
            counts[e.type] += e.type == 3 ? 2 : 1;
        }
        for (unsigned type = 0; type < 4; ++type) {
            auto *slot = (*positions)[type];
            if (!counts[type]) assert(slot == nullptr);
            else assert(slot && slot->size() == counts[type]);
        }
        for (unsigned i = 0; i < record.count; ++i) {
            const auto &e = expectedEntries[record.begin + i];
            auto *slot = (*positions)[e.type];
            same((*slot)[indices[e.type]++], float(e.x), float(e.z), float(-int(e.y)));
            if (e.type == 3)
                same((*slot)[indices[3]++], fromBits(e.ex), fromBits(e.ez), fromBits(e.ey));
        }
        markAndRelease(positions);
    }
    assert(opens == 59 && closes == 59 && failedReads == 0 && allReleased == 788);
    std::puts("Original asset phase: all 59 records and createPlayer cleanup passed");
    std::fflush(stdout);

    // Read slots retain previous values on a zero-byte read, including after a
    // skipped record. No new EOF fallback or file format is installed in production.
    std::vector<unsigned char> fault;
    append<int16_t>(fault, 0); append<int16_t>(fault, 1);
    for (int16_t value : {3, 7, -2, 3}) append(fault, value);
    append(fault, 1.25f); append(fault, 2.5f); append(fault, -3.5f);
    append<int16_t>(fault, 1); append<int16_t>(fault, 1);
    for (int16_t value : {3, 11, -12, 13}) append(fault, value);
    active = &fault;
    Outer *retained;
    { FileRead reader; retained = reader.loadWeaponPositions(1); }
    assert(failedReads == 3 && opens == 60 && closes == 60 && !opened);
    assert(retained && (*retained)[0] == nullptr && (*retained)[1] == nullptr && (*retained)[2] == nullptr);
    assert((*retained)[3] && (*retained)[3]->size() == 2);
    same((*(*retained)[3])[0], 11, 13, 12);
    same((*(*retained)[3])[1], 1.25f, -3.5f, 2.5f);
    markAndRelease(retained);
    assert(allReleased == 790);
    std::puts("Original weapons_hd.bin: 59 IDs, 552 entries, four ordered slots, 236 extra vectors OK");
    std::puts("Every scalar read/close, path lifetime, 788 vector releases and copied consumer values OK");
    std::puts("Missing file returns null; controlled zero-byte float reads retain prior record values; no game/GPU claim");
}
