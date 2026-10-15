#include <cassert>
#include <cstdint>

struct Vector { float x, y, z; };

struct ParticleSystemManager {
    int emitCalls = 0, renderCalls = 0;
    void enableSystemEmit(int id, bool enabled) {
        assert(id == 0x123 && !enabled);
        ++emitCalls;
    }
    void enableSystemRender(int id, bool enabled) {
        assert(id == 0x123 && !enabled);
        ++renderCalls;
    }
};
struct Level { ParticleSystemManager *field_74; };
struct Explosion {
    static int destroyed;
    ~Explosion() { ++destroyed; }
};
int Explosion::destroyed = 0;
struct TargetFollowCamera {
    bool active = false;
    void setActive(bool value) { active = value; }
};
struct Player {
    bool active = false;
    int hp = 0, armor = 0;
    void setActive(bool value) { active = value; }
    int getMaxHitpoints() { return 91; }
    int getMaxArmorHP() { return 73; }
    void setHitpoints(int value) { hp = value; }
    void setArmorHP(int value) { armor = value; }
};
struct FModSound {
    int currentMusicEvent = 17;
    int ids[2] = {-1, -1};
    int count = 0;
    void play(int id, Vector *position, Vector *velocity, float pitch) {
        assert(position == nullptr && velocity == nullptr && pitch == 0.0f);
        assert(count < 2);
        ids[count++] = id;
    }
};
struct Globals { static FModSound *sound; };
FModSound *Globals::sound = nullptr;
struct AEGeometry {
    Vector forward{}, up{};
    void setDirection(const Vector &a, const Vector &b) { forward = a; up = b; }
};
struct PlayerEgo {
    Level *level = nullptr;
    int currentSystem = 0;
    Explosion *explosion = nullptr;
    void *targetFollowCamera = nullptr;
    Player *player = nullptr;
    int field_0x1c = 0;
    AEGeometry *geometry = nullptr;
    int explosionTimer = -1;
    bool exhaustVisible = false;
    Vector position{};
    int positionCalls = 0;
    void setExhaustVisible(bool value) { exhaustVisible = value; }
    void setPosition(float x, float y, float z) {
        position = {x, y, z};
        ++positionCalls;
    }
    void revive();
};

// PRODUCTION_METHOD

int main() {
    ParticleSystemManager particles;
    Level level{&particles};
    TargetFollowCamera camera;
    Player player;
    FModSound sound;
    AEGeometry geometry;
    Globals::sound = &sound;
    PlayerEgo ego;
    ego.level = &level;
    ego.currentSystem = 0x123;
    ego.explosion = new Explosion;
    ego.targetFollowCamera = &camera;
    ego.player = &player;
    ego.field_0x1c = 45;
    ego.geometry = &geometry;
    ego.revive();
    assert(particles.emitCalls == 1 && particles.renderCalls == 1);
    assert(Explosion::destroyed == 1 && ego.explosion == nullptr);
    assert(camera.active && player.active && ego.exhaustVisible);
    assert(sound.count == 2 && sound.ids[0] == 17 && sound.ids[1] == 45);
    assert(player.hp == 91 && player.armor == 73);
    assert(ego.positionCalls == 1 && ego.position.x == 0.0f
           && ego.position.y == 0.0f && ego.position.z == 10000.0f);
    assert(geometry.forward.x == 0.0f && geometry.forward.y == 0.0f
           && geometry.forward.z == 1.0f);
    assert(geometry.up.x == 0.0f && geometry.up.y == 1.0f && geometry.up.z == 0.0f);
    assert(ego.explosionTimer == 0);

    sound.count = 0;
    ego.position = {5.0f, 6.0f, 7.0f};
    ego.revive();
    assert(particles.emitCalls == 2 && particles.renderCalls == 2);
    assert(Explosion::destroyed == 1 && ego.explosion == nullptr);
    assert(sound.count == 2 && sound.ids[0] == 17 && sound.ids[1] == 45);
    assert(ego.positionCalls == 2 && ego.position.z == 10000.0f);
}
