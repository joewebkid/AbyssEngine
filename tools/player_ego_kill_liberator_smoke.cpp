#include <cassert>

struct Vector { float x, y, z; };

template <class T> struct Array {
    unsigned count;
    T *data_;
    T &operator[](unsigned i) { return data_[i]; }
};

struct Gun {
    Array<Vector> positionArray;
    int *lifetimes;
    int itemIndex;
    unsigned char active;
};

struct Player { Array<Array<Gun *> *> *guns; };

struct PlayerEgo {
    Player *player;
    int currentSecondaryWeaponIndex;
    int killLiberator();
};

// PRODUCTION_METHOD

int main() {
    Vector positions[2] = {{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}};
    int lifetimes[2] = {12, 34};
    Gun first{{1, &positions[0]}, &lifetimes[0], 0xb3, 1};
    Gun second{{1, &positions[1]}, &lifetimes[1], 1, 1};
    Gun *guns[2] = {&first, &second};
    Array<Gun *> bank{2, guns};
    Array<Gun *> *groups[2] = {nullptr, &bank};
    Array<Array<Gun *> *> rack{2, groups};
    Player player{&rack};
    PlayerEgo ego{&player, 0xb3};

    assert(ego.killLiberator() == 1);
    assert(lifetimes[0] == -1 && first.active == 0);
    assert(positions[0].x == 50000.0f && positions[0].y == 50000.0f
           && positions[0].z == 50000.0f);
    assert(lifetimes[1] == 34 && second.active == 1 && positions[1].x == 4.0f);

    first.active = 1;
    first.itemIndex = 2;
    assert(ego.killLiberator() == 1 && first.active == 1);
    ego.currentSecondaryWeaponIndex = 2;
    assert(ego.killLiberator() == 0 && first.active == 1);
    ego.currentSecondaryWeaponIndex = 0xb3;
    bank.count = 0;
    assert(ego.killLiberator() == 0);
    bank.count = 2;
    rack.count = 0;
    assert(ego.killLiberator() == 0);
    rack.count = 2;
    groups[1] = nullptr;
    assert(ego.killLiberator() == 0);
    player.guns = nullptr;
    assert(ego.killLiberator() == 0);
}
