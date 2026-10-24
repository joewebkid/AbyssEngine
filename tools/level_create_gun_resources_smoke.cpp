#include <cassert>
#include <cstdint>
#include <new>
#include <vector>

enum ItemSort : int {
    ITEM_SORT_LASER = 0, ITEM_SORT_BLASTER = 1, ITEM_SORT_AUTO_CANNON = 2,
    ITEM_SORT_THERMO = 3, ITEM_SORT_ROCKET = 4, ITEM_SORT_MISSILE = 5,
    ITEM_SORT_EMP_BOMB = 6, ITEM_SORT_NUKE = 7, ITEM_SORT_TURRET = 8,
    ITEM_SORT_MINE = 11, ITEM_SORT_SCATTER_GUN = 25,
    ITEM_SORT_IONIZING_MISSILE = 34, ITEM_SORT_PLASMA_COLLECTOR = 35,
    ITEM_SORT_SENTRY_GUN = 39, ITEM_SORT_CLUSTER_MISSILE = 40,
    ITEM_SORT_SHOCK_BLAST = 42,
};

struct Vector { float x, y, z; };
template<class T> struct Array {
    std::vector<T> entries;
    T *data() { return entries.data(); }
    unsigned size() const { return static_cast<unsigned>(entries.size()); }
};
template<class T> void ArrayAdd(T value, Array<T> &array) { array.entries.push_back(value); }
struct Item {
    int attribute = 0;
    int getAttribute(int index) { assert(index == 15); return attribute; }
};
struct Globals {
    static Globals *gGlobals;
    static void *items;
    std::vector<int> sounds;
    void addSoundResourceToList(int sound) { sounds.push_back(sound); }
};
Globals *Globals::gGlobals;
void *Globals::items;

struct Level;
struct Gun {
    int owner, damage, barrels, hitpoints, cooldown, rate, index = -1;
    float color;
    Vector offset, direction;
    ItemSort weaponType = ITEM_SORT_LASER;
    uint8_t field_0xa4 = 0, field_0xa5 = 0;
    int error = 0;
    Level *level = nullptr;
    Gun(int o, int d, int b, int h, int c, int r, float f, Vector v, Vector w)
        : owner(o), damage(d), barrels(b), hitpoints(h), cooldown(c), rate(r),
          color(f), offset(v), direction(w) {}
    void setIndex(int i) { index = i; }
    void setPlayerGun(int v) { assert(v == 1); }
    void setErrorMagnitudePercentage(int v) { error = v; }
    void setLevel(Level *v) { level = v; }
};
struct AbstractGun {};
static int lastClass, lastResource, lastMode;
struct ObjectGun : AbstractGun {
    ObjectGun() = default;
    ObjectGun(int, Gun *, int resource, int range, Level *) {
        assert(range == 1000); lastClass = 2; lastResource = resource;
    }
};
struct BeamGun : ObjectGun {
    BeamGun(int, Gun *, int resource, Level *) { lastClass = 1; lastResource = resource; }
};
struct RocketGun : ObjectGun {
    RocketGun(int, Gun *, int resource, int, int, int, int mode, Level *) {
        lastClass = 3; lastResource = resource; lastMode = mode;
    }
};
struct BombGun : ObjectGun {
    BombGun(Gun *, int resource, int, int, int mode, Level *) {
        lastClass = 4; lastResource = resource; lastMode = mode;
    }
};
struct MineGun : ObjectGun {
    MineGun(Gun *, int resource, int, int, Level *) { lastClass = 5; lastResource = resource; }
};
struct SentryGun : ObjectGun {
    SentryGun(Gun *, int resource, int, int, Level *) { lastClass = 6; lastResource = resource; }
};
struct Level {
    Array<AbstractGun *> *playerGuns = nullptr;
    Gun *createGun(int, int, int, int, int, int, int, int);
};
static_assert(sizeof(Gun) <= 0x114, "test Gun must fit original allocation");
static_assert(sizeof(BeamGun) <= 0x24, "test BeamGun must fit original allocation");

// SOURCE_TABLES
// SOURCE_METHOD

int main() {
    Globals globals;
    Globals::gGlobals = &globals;
    Array<Item *> items;
    items.entries.resize(233, nullptr);
    Item bombItem;
    bombItem.attribute = 1;
    items.entries[34] = &bombItem;
    Globals::items = &items;
    Level level;

    Gun *beam = level.createGun(9, 2, ITEM_SORT_LASER, 100, 4, 7, 10, 3);
    assert(beam->level == &level && beam->index == 9 && beam->barrels == 1
           && lastClass == 1 && lastResource == 9 && globals.sounds.empty());

    Gun *object = level.createGun(0, 2, ITEM_SORT_LASER, 100, 4, 7, 10, 3);
    assert(object->barrels == 20 && lastClass == 2 && lastResource == 6754);

    Gun *turret = level.createGun(48, 2, ITEM_SORT_TURRET, 100, 4, 7, 10, 3);
    assert(turret->offset.x == 80.0f && turret->offset.y == 0.0f
           && turret->offset.z == 0.0f && turret->field_0xa4 == 1
           && lastClass == 2 && lastResource == 6797);

    globals.sounds.clear();
    Gun *rocket = level.createGun(176, 2, ITEM_SORT_ROCKET, 100, 4, 7, 10, 3);
    assert(rocket->weaponType == ITEM_SORT_ROCKET && lastClass == 3
           && lastResource == 6900 && lastMode == 0
           && globals.sounds == std::vector<int>{1111});

    globals.sounds.clear();
    Gun *bomb = level.createGun(34, 2, ITEM_SORT_IONIZING_MISSILE, 100, 4, 7, 10, 3);
    assert(bomb->weaponType == ITEM_SORT_IONIZING_MISSILE && lastClass == 4
           && lastResource == 14247 && lastMode == 1
           && globals.sounds == std::vector<int>{85});

    globals.sounds.clear();
    Gun *cascade = level.createGun(41, 2, ITEM_SORT_LASER, 100, 4, 7, 10, 3);
    assert(cascade->index == 41 && lastResource == 14684
           && (globals.sounds == std::vector<int>{15, 16, 17, 14, 13, 12}));
    assert(level.playerGuns && level.playerGuns->size() == 6);
}
