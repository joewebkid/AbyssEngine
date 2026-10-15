#include <cassert>
#include <cmath>
#include <cstdint>

struct Player {
    float flShake = 0.0f;
};
struct PlayerEgo;
struct Hud {
    int events = 0;
    void hudEvent(int id, PlayerEgo *ego, int value) {
        assert(id == 26 && ego != nullptr && value == 0);
        ++events;
    }
};
struct PlayerEgo {
    Player *player = nullptr;
    int driveCharge = 0;
    int driveChargeMax = 39;
    uint8_t chargingDrive = 0;
    uint8_t volatileGoods = 0;
    void *gunMuzzleRoot = reinterpret_cast<void *>(static_cast<uintptr_t>(0x1234));
    void updateDrive(int dt, Hud *hud);
};

void PlayerEgo::updateDrive(int dt, Hud *hud) {
    Player *player = this->player;
// PRODUCTION_FRAGMENT
}

int main() {
    Player player;
    Hud hud;
    PlayerEgo ego;
    ego.player = &player;

    ego.volatileGoods = 1;
    ego.updateDrive(17, &hud);
    assert(ego.driveCharge == 0 && hud.events == 0 && player.flShake == 0.0f);

    ego.chargingDrive = 1;
    ego.volatileGoods = 0;
    ego.updateDrive(17, &hud);
    assert(ego.driveCharge == 17 && hud.events == 0);
    assert(reinterpret_cast<uintptr_t>(ego.gunMuzzleRoot) == 0x1234);

    ego.volatileGoods = 1;
    ego.updateDrive(17, &hud);
    assert(ego.driveCharge == 34 && hud.events == 0);
    assert(std::fabs(player.flShake - 0.0085f) < 0.000001f);

    ego.updateDrive(5, &hud);
    assert(ego.driveCharge == 39 && hud.events == 1 && ego.chargingDrive == 1);
    assert(reinterpret_cast<uintptr_t>(ego.gunMuzzleRoot) == 0x1200);
    assert(std::fabs(player.flShake - 0.011f) < 0.000001f);
}
