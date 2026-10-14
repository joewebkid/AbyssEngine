#include <cassert>
#include <cstdint>

struct PaintCanvas {};
struct AEGeometry {
    static int constructed;
    static int destroyed;
    unsigned transform = 0;
    unsigned children[2]{};
    int childCount = 0;
    AEGeometry(PaintCanvas *) : transform(1) { ++constructed; }
    AEGeometry(uint16_t resource, PaintCanvas *, bool visible)
        : transform(resource == 0x4973 ? 73 : 72) {
        assert((resource == 0x4973 || resource == 0x4972) && !visible);
        ++constructed;
    }
    ~AEGeometry() { ++destroyed; }
    void addChild(unsigned child) { assert(childCount < 2); children[childCount++] = child; }
};
int AEGeometry::constructed = 0;
int AEGeometry::destroyed = 0;
struct Item {
    int index = 0;
    int getIndex() { return index; }
};
struct Ship {
    Item *protection = nullptr;
    int queries = 0;
    Item *getFirstEquipmentOfSort(int sort) {
        assert(sort == 38);
        ++queries;
        return protection;
    }
};
struct Status {
    Ship ship;
    bool supernovaSystem = false;
    bool supernovaOrbit = false;
    bool inSupernovaSystem() { return supernovaSystem; }
    bool inSupernovaOrbit() { return supernovaOrbit; }
    Ship *getShip() { return &ship; }
};
namespace Globals {
    Status *status;
    PaintCanvas *Canvas;
}
struct PlayerEgo {
    void *gunYawGeo = reinterpret_cast<void *>(1);
    void *gunMuzzleRoot = reinterpret_cast<void *>(1);
    int engineSoundId = 0;
    void initializeSupernova();
};
void PlayerEgo::initializeSupernova() {
// PRODUCTION_SUPERNOVA_TAIL
}

int main() {
    PaintCanvas canvas;
    Status status;
    Globals::Canvas = &canvas;
    Globals::status = &status;

    PlayerEgo ordinary;
    ordinary.initializeSupernova();
    assert(ordinary.gunYawGeo == nullptr && ordinary.engineSoundId == -1);
    assert(*reinterpret_cast<uint8_t *>(&ordinary.gunMuzzleRoot) == 0);
    assert(AEGeometry::constructed == 0 && status.ship.queries == 0);

    status.supernovaSystem = true;
    PlayerEgo unprotected;
    unprotected.initializeSupernova();
    assert(unprotected.gunYawGeo == nullptr && unprotected.engineSoundId == -1);
    assert(*reinterpret_cast<uint8_t *>(&unprotected.gunMuzzleRoot) == 0);
    assert(AEGeometry::constructed == 0 && status.ship.queries == 1);

    Item standard{204};
    status.ship.protection = &standard;
    PlayerEgo protectedEgo;
    protectedEgo.initializeSupernova();
    assert(protectedEgo.engineSoundId == 2260);
    assert(*reinterpret_cast<uint8_t *>(&protectedEgo.gunMuzzleRoot) == 1);
    AEGeometry *effect = static_cast<AEGeometry *>(protectedEgo.gunYawGeo);
    assert(effect && effect->childCount == 2 && effect->children[0] == 73 && effect->children[1] == 72);
    assert(AEGeometry::constructed == 3 && AEGeometry::destroyed == 2);
    delete effect;

    status.supernovaSystem = false;
    status.supernovaOrbit = true;
    Item enhanced{205};
    status.ship.protection = &enhanced;
    PlayerEgo enhancedEgo;
    enhancedEgo.initializeSupernova();
    assert(enhancedEgo.engineSoundId == 2261);
    effect = static_cast<AEGeometry *>(enhancedEgo.gunYawGeo);
    assert(effect && effect->children[0] == 73 && effect->children[1] == 72);
    assert(AEGeometry::constructed == 6 && AEGeometry::destroyed == 5);
    delete effect;
    assert(AEGeometry::destroyed == 6);
}
