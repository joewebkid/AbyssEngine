#include <cassert>
#include <cstdint>

struct Vector { float x, y, z; };
struct AEGeometry {
    Vector position{1.25f, -2.5f, 3.75f};
    Vector getPosition() { return position; }
};
struct Explosion { bool playing = false; bool isPlaying() { return playing; } };
struct Player {
    bool active = true;
    uint8_t enemyFlagsLo = 7;
    uint8_t carriesFriendCargoFlag = 7;
    int damageDoneByPlayer = 0;
    int maxHitpoints = 100;
    int getMaxHitpoints() { return maxHitpoints; }
    void setActive(bool value) { active = value; }
};
struct Standing {
    bool enemy = false;
    bool friendly = false;
    int enemyCalls = 0;
    int friendCalls = 0;
    bool isEnemy(int) { ++enemyCalls; return enemy; }
    bool isFriend(int) { ++friendCalls; return friendly; }
};
struct Status {
    Standing *standing;
    Standing *getStanding() { return standing; }
};
struct Globals { static Status *status; };
Status *Globals::status = nullptr;

struct KIPlayer {
    Player *player = nullptr;
    AEGeometry *geometry = nullptr;
    Explosion *explosion = nullptr;
    int state = 0;
    uint8_t crateCaptured = 1;
    int deathTimer = 0;
    int field_0x1c0 = 0;
    int maneuverTimer = 0;
    int shipGroup = 0;
    int deltaTime = 0;
    int deltaTimeHi = 0;
    Vector renderPosition{};
    uint8_t field_0x43 = 0;
    bool wingman = false;
    void setActive(bool value) { player->setActive(value); }
    bool isWingMan() { return wingman; }
};
struct PlayerFighter : KIPlayer { void update(int dt); };
int bodyCalls = 0;
void PF_update_body(PlayerFighter *, int) { ++bodyCalls; }

// PRODUCTION_METHOD

int main() {
    AEGeometry geometry;
    Explosion explosion;
    Player player;
    Standing standing;
    Status status{&standing};
    Globals::status = &status;
    PlayerFighter fighter;
    fighter.geometry = &geometry;
    fighter.explosion = &explosion;
    fighter.player = &player;

    fighter.state = 4;
    fighter.crateCaptured = 0;
    fighter.update(7);
    assert(!player.active && bodyCalls == 0 && fighter.deltaTime == 0);

    player.active = true;
    fighter.crateCaptured = 1;
    fighter.deathTimer = 60000;
    fighter.shipGroup = 8;
    fighter.update(7);
    assert(player.active && bodyCalls == 1);
    assert(fighter.deltaTime == 7 && fighter.deltaTimeHi == 0);
    assert(fighter.renderPosition.x == 1.25f && fighter.renderPosition.y == -2.5f);
    assert(fighter.renderPosition.z == 3.75f);
    assert(fighter.field_0x1c0 == 7 && fighter.maneuverTimer == 7);
    assert(player.enemyFlagsLo == 1 && player.carriesFriendCargoFlag == 0);
    assert(standing.enemyCalls == 0 && standing.friendCalls == 0);

    fighter.shipGroup = 2;
    fighter.wingman = true;
    fighter.update(-3);
    assert(fighter.deltaTime == -3 && fighter.deltaTimeHi == -1);
    assert(player.enemyFlagsLo == 0 && player.carriesFriendCargoFlag == 1);

    fighter.wingman = false;
    standing.enemy = true;
    standing.friendly = true;
    fighter.update(1);
    assert(player.enemyFlagsLo == 1 && player.carriesFriendCargoFlag == 1);
    assert(standing.enemyCalls == 1 && standing.friendCalls == 1);

    fighter.field_0x43 = 1;
    player.enemyFlagsLo = 7;
    player.carriesFriendCargoFlag = 7;
    player.damageDoneByPlayer = 5;
    fighter.update(1);
    assert(player.enemyFlagsLo == 7 && player.carriesFriendCargoFlag == 7);
    player.damageDoneByPlayer = 6;
    fighter.update(1);
    assert(player.enemyFlagsLo == 1 && player.carriesFriendCargoFlag == 7);
    assert(standing.enemyCalls == 1 && standing.friendCalls == 1);

    fighter.deathTimer = 60001;
    fighter.update(1);
    assert(!player.active && bodyCalls == 5);
}
