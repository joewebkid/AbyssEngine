#include <cstdlib>
#include <cstddef>
static void *contractRealloc(void *, size_t);
// Observe the real Array allocator boundary, including buffers moved by realloc.
#define realloc contractRealloc
#include "engine/file/FileRead.h"
#undef realloc
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

// Allocation capture covers only the real loader's OpenRead/Close interval.
// The file boundary does not allocate while that interval is active.
struct Allocation { void *pointer; size_t size; bool live; };
static Allocation allocations[512];
static unsigned allocationCount, vectorsFreed, vectorsExpected;
static bool capture;
static void retire(void *p) {
    for (unsigned i = allocationCount; i > 0; --i) {
        auto &a = allocations[i - 1];
        if (a.pointer == p && a.live) {
            a.live = false;
            if (a.size == sizeof(Vector)) ++vectorsFreed;
            break;
        }
    }
}
static void *contractRealloc(void *p, size_t size) {
    // Retire old storage before realloc invalidates its pointer value.
    retire(p);
    void *result = std::realloc(p, size);
    assert(result);
    if (capture) {
        assert(allocationCount < 512);
        allocations[allocationCount++] = {result, size, true};
    }
    return result;
}
void *operator new(size_t size) {
    void *p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    if (capture) {
        assert(allocationCount < 512);
        allocations[allocationCount++] = {p, size, true};
    }
    return p;
}
void *operator new[](size_t size) { return ::operator new(size); }
void operator delete(void *p) noexcept {
    retire(p);
    std::free(p);
}
void operator delete(void *p, size_t) noexcept { ::operator delete(p); }
void operator delete[](void *p) noexcept { ::operator delete(p); }
void operator delete[](void *p, size_t) noexcept { ::operator delete(p); }

int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
String operator+(const String &, const String &) { std::abort(); }
}

struct Entry { int16_t type, x, y, z; uint32_t ex, ey, ez; };
struct Record { int ship; unsigned begin, count, end; };
#include "createplayer_oracle.inc"
static std::vector<unsigned char> original;
static size_t cursor;
static bool opened;
static unsigned opens, closes;
uint32_t AEFile::FileExist(const String &name) {
    static const char path[] = "data/bin/weapons_hd.bin";
    assert(name.length == int(std::strlen(path)));
    for (int i = 0; i <= name.length; ++i) assert(name.data[i] == path[i]);
    return 1;
}
uint32_t AEFile::OpenRead(const char *name, uint32_t *handle) {
    assert(!opened && std::strcmp(name, "data/bin/weapons_hd.bin") == 0);
    cursor = 0; opened = true; ++opens; *handle = 1; capture = true;
    return 1;
}
uint32_t AEFile::Read(uint32_t size, void *out, uint32_t handle) {
    assert(opened && handle == 1 && (size == 2 || size == 4));
    assert(cursor + size <= original.size());
    std::memcpy(out, original.data() + cursor, size);
    cursor += size;
    return size;
}
void AEFile::Close(uint32_t handle) {
    assert(opened && handle == 1);
    opened = false; ++closes; capture = false;
}
#include "createplayer_loader.inc"
static void released() {
    assert(!capture && !opened && vectorsFreed == vectorsExpected);
    for (unsigned i = 0; i < allocationCount; ++i) {
        if (allocations[i].live) std::fprintf(stderr, "loader allocation still live: %u size %zu\n", i, allocations[i].size);
        assert(!allocations[i].live);
    }
}
static uint32_t bits(float f) { uint32_t b; std::memcpy(&b, &f, 4); return b; }
static float fromBits(uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }
static void same(const Vector &v, float x, float y, float z) {
    assert(bits(v.x) == bits(x) && bits(v.y) == bits(y) && bits(v.z) == bits(z));
}

// Explicit CPU boundaries: no native constructor layout or graphics is emulated.
namespace CPU {
class Level;
enum ItemSort { SortZero };
struct Item {
    int type, index, slot; bool weapon;
    int getType() { return type; }
    int getIndex() { return index; }
    int getSort() { return 10 + type; }
    int getAmount() { return 19; }
    int getAttribute(int a) { return a + 10; }
    bool isWeapon() { return weapon; }
};
struct Ship {
    int index, counts[3] = {}, selections[2] = {};
    Array<Item *> *equipment;
    float damageFactor = 1.5f, fireFactor = 2.0f;
    Array<Item *> *getEquipment() { return equipment; }
    int getUsedSlots(int t) { return counts[t]; }
    int getMaxHP() { return 300; }
    int getMaxShieldHP() { return 80; }
    int getMaxArmorHP() { return 90; }
    int getIndex() { return index; }
    int getRace() { return 7; }
    float getDamageFactor() { return damageFactor; }
    float getFireRateFactor() { return fireFactor; }
    int getSlotPos(Item *it) {
        if (it->type == 2) {
            std::fputs("turret unexpectedly requested getSlotPos\n", stderr);
            std::fflush(stderr); std::exit(31);
        }
        assert(it->type == 0 || it->type == 1);
        ++selections[it->type]; return it->slot;
    }
};
struct Status { static Status *gStatus; Ship *ship; Ship *getShip() { return ship; } };
Status *Status::gStatus;
struct PaintCanvas { static PaintCanvas *gCanvas; };
PaintCanvas *PaintCanvas::gCanvas;
struct AEGeometry {
    Vector position{}, scaling{}, rotation{};
    explicit AEGeometry(PaintCanvas *) {}
    void setPosition(Vector v) { position = v; }
    void setScaling(Vector v) { scaling = v; }
    void setRotation(Vector v) { rotation = v; }
};
struct Gun {
    int itemIndex, equipmentIndex, magnitude, amount, damage, rate, range, speed;
    ItemSort weaponType;
    Vector offset{};
    void setOffset(Vector *);
    void setMagnitude(int v) { magnitude = v; }
};
struct Player {
    int field_69 = 0;
    Array<Array<Gun *> *> *guns;
    bool playShootSoundFlag = false;
    int playShootSoundId = 0;
    Player(int radius, int hp, int, int, int) {
        assert(radius == 1200 && hp == 300);
        guns = new Array<Array<Gun *> *>(); ArraySetLength(4, *guns);
    }
    void setMaxShieldHP(int v) { assert(v == 80); }
    void setMaxArmorHP(int v) { assert(v == 90); }
    void addGun(Array<Gun *> *, int);
    void calcWeaponSounds(int) { std::abort(); }
};
struct PlayerEgo {
    Player *player; AEGeometry *geometry;
    Vector turretOffsetVec{};
    explicit PlayerEgo(Player *p) : player(p), geometry(new AEGeometry(nullptr)) {}
    void setShip(int, int race) { assert(race == 7); }
    void setLevel(Level *) {}
    void setTurretPosition(Vector);
    void addGun(Array<Gun *> *, int);
    void checkForTurret() { released(); }
};
struct Level {
    PlayerEgo *player = nullptr;
    Array<AEGeometry *> *field_a4 = nullptr;
    int field_138 = 0;
    void createPlayer();
    Gun *createGun(int index, int equipmentIndex, int, int amount, int damage,
                   int rate, int range, int speed) {
        auto *g = new Gun(); g->itemIndex = index; g->equipmentIndex = equipmentIndex;
        g->amount = amount; g->damage = damage; g->rate = rate; g->range = range; g->speed = speed;
        return g;
    }
};
#include "createplayer_body.inc"
}

int main(int argc, char **argv) {
    assert(argc == 2 && sizeof(Vector) == 12 && sizeof(void *) == 8);
    std::ifstream stream(argv[1], std::ios::binary); assert(stream.good());
    original.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    unsigned cases = 0, totalVectors = 0, engines = 0, turrets = 0;
    for (const auto &r : expectedRecords) for (int reverse = 0; reverse < 2; ++reverse) {
        allocationCount = vectorsFreed = vectorsExpected = 0;
        unsigned counts[4] = {}, positions[3] = {};
        for (unsigned i = 0; i < r.count; ++i) ++counts[expectedEntries[r.begin + i].type];
        vectorsExpected = counts[0] + counts[1] + counts[2] + 2 * counts[3];
        CPU::Ship ship; ship.index = r.ship;
        for (int t = 0; t < 3; ++t) ship.counts[t] = int(counts[t]);
        Array<CPU::Item *> equipment;
        std::vector<CPU::Item> items; items.reserve(r.count + 1);
        items.push_back({0, -1, 0, false});
        ArrayAdd(&items.back(), equipment); ArrayAdd((CPU::Item *)nullptr, equipment);
        for (unsigned n = 0; n < r.count; ++n) {
            unsigned i = reverse ? r.count - 1 - n : n;
            const auto &e = expectedEntries[r.begin + i];
            if (e.type == 3) continue;
            unsigned slot = 0;
            for (unsigned j = 0; j < i; ++j) if (expectedEntries[r.begin + j].type == e.type) ++slot;
            items.push_back({e.type, int(i + 100), int(slot), true});
            ArrayAdd(&items.back(), equipment);
        }
        ship.equipment = &equipment;
        CPU::Status status; status.ship = &ship; CPU::Status::gStatus = &status;
        CPU::Level level; level.createPlayer();
        assert(cursor == r.end && opens == closes); released();
        assert(level.player && level.player->player->field_69 == 1);
        for (int t = 0; t < 3; ++t) {
            auto *arr = (*level.player->player->guns)[t];
            assert(bool(arr) == bool(counts[t]));
            if (!arr) continue;
            assert(arr->size() == counts[t]);
            if (t < 2) assert(ship.selections[t] == int(counts[t]));
            for (unsigned i = 0; i < arr->size(); ++i) {
                auto *g = (*arr)[i]; assert(g);
                auto *it = equipment[g->equipmentIndex]; assert(it && it->weapon && it->type == t);
                assert(g->itemIndex == it->index && int(g->weaponType) == 10 + t);
                assert(g->magnitude == 24 && g->range == 22 && g->speed == 23);
                assert(g->amount == (t == 1 ? 19 : -1));
                assert(g->damage == (t == 0 ? 28 : 19) && g->rate == (t == 0 ? 42 : 21));
                // Buckets fill backwards in equipment order, independently of mount index.
                unsigned expected = 0;
                for (unsigned j = g->equipmentIndex + 1; j < equipment.size(); ++j)
                    if (equipment[j] && equipment[j]->weapon && equipment[j]->type == t) ++expected;
                assert(i == expected);
                if (t < 2) {
                    unsigned slot = 0; const Entry *mount = nullptr;
                    for (unsigned j = 0; j < r.count; ++j) {
                        const auto &e = expectedEntries[r.begin + j];
                        if (e.type == t && slot++ == unsigned(it->slot)) { mount = &e; break; }
                    }
                    assert(mount); same(g->offset, float(mount->x), float(mount->z), float(-int(mount->y)) + 100.0f);
                }
                delete g;
            }
            delete arr;
        }
        for (unsigned i = 0; i < r.count; ++i) {
            const auto &e = expectedEntries[r.begin + i];
            if (e.type == 2 && positions[2]++ == 0) {
                same(level.player->turretOffsetVec, float(e.x), float(e.z), float(-int(e.y))); ++turrets;
            }
            if (e.type != 3) continue;
            assert(level.field_a4);
            auto *g = (*level.field_a4)[positions[0]++];
            same(g->position, float(e.x), float(e.z), float(-int(e.y)));
            same(g->scaling, fromBits(e.ex), fromBits(e.ez), fromBits(e.ey));
            delete g; ++engines;
        }
        assert(bool(level.field_a4) == bool(counts[3]));
        if (level.field_a4) { assert(level.field_a4->size() == counts[3]); delete level.field_a4; }
        assert((*level.player->player->guns)[3] == nullptr);
        delete level.player->player->guns;
        delete level.player->geometry; delete level.player->player; delete level.player;
        totalVectors += vectorsFreed; ++cases;
        // The original leaves temporary bucket Arrays allocated; this gate makes no
        // whole-function leak-free claim. Loader ownership is checked separately.
    }
    assert(cases == 118 && totalVectors == 1576 && engines == 472 && turrets > 0);
    std::printf("PASS full createPlayer: %u cases, %u freed loader vectors, %u engine pairs, %u turret cases\n",
                cases, totalVectors, engines, turrets);
}
