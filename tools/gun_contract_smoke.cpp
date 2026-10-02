#include "game/weapons/Gun.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>

// Focused host lifetime check, not an ARM executable certification.
int main() {
    for (int slots : {0, 1, 3}) {
        Gun gun(2, 40, slots, 5, 1000, 100, 2.0f,
                Vector{1, 2, 3}, Vector{0, 0, 4});
        assert(gun.count == static_cast<unsigned>(slots));
        assert(gun.directionCount == slots);
        assert(gun.field_0x20 == slots && gun.field_0x2c == slots);
        assert(gun.wobbleOffsets->size() == static_cast<unsigned>(slots));
        assert(gun.velocity.z == 4.0f * 0.15339000523090363f);
        for (int i = 0; i < slots; ++i) {
            assert(gun.lifetimes[i] == -1000000 && gun.hitFlags[i] == 0);
            assert(gun.wobbleOffsets->data_[i] == nullptr);
            assert(reinterpret_cast<Vector *>(gun.positions)[i].x == 50000.0f);
            gun.wobbleOffsets->data_[i] = new Vector{1, 2, 3};
        }
        Vector offset{10, 20, 30};
        gun.setOffset(&offset);
        assert(gun.offset.x == 10 && gun.offset.y == 20 && gun.offset.z == 130);
        gun.setOffset(0, 2);
        assert(gun.offset.x == 280 && gun.offset.y == 20 && gun.offset.z == 230);
        gun.setOffset(1, 2);
        assert(gun.offset.x == 10 && gun.offset.y == 20 && gun.offset.z == 330);
        // Fake Player pointers are stored, never dereferenced or deleted.
        auto *first = reinterpret_cast<Player *>(0x1000);
        auto *second = reinterpret_cast<Player *>(0x2000);
        gun.setEnemy(first);
        assert(gun.enemies->size() == 1 && (*gun.enemies)[0] == first);
        gun.setEnemy(second);
        assert(gun.enemies->size() == 1 && (*gun.enemies)[0] == second);
        auto *container = gun.enemies;
        gun.removeAllEnemies();
        assert(gun.enemies == nullptr);
        delete container; // Native Gun destructor does not own enemies.
    }
    std::puts("Gun host construction/ownership/offset smoke: OK");
}
