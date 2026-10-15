#include <cassert>

struct ParticleSystemManager {
    int calls = 0;
    void enableSystemEmit(int id, bool enabled) {
        assert(id == 0x123 && enabled);
        ++calls;
    }
};
struct Level { ParticleSystemManager *field_74; };
struct TargetFollowCamera {
    bool active = true;
    int calls = 0;
    void setActive(bool value) { active = value; ++calls; }
};
struct Explosion {
    static int constructed;
    int type;
    explicit Explosion(int value) : type(value) { ++constructed; }
};
int Explosion::constructed = 0;
struct Player {
    bool active = true;
    int calls = 0;
    void setActive(bool value) { active = value; ++calls; }
};
struct FModSound {
    int currentMusicEvent = 17;
    int ids[10]{};
    int count = 0;
    void stop(int id) {
        assert(count < 10);
        ids[count++] = id;
    }
};
struct Globals { static FModSound *sound; };
FModSound *Globals::sound = nullptr;
struct PlayerEgo {
    Level *level = nullptr;
    int currentSystem = 0;
    void *explosion = nullptr;
    void *targetFollowCamera = nullptr;
    void *player = nullptr;
    int field_0x1c = 0;
    bool exhaustVisible = true;
    int exhaustCalls = 0;
    void setExhaustVisible(bool value) { exhaustVisible = value; ++exhaustCalls; }
    void explode();
};

// PRODUCTION_METHOD

int main() {
    ParticleSystemManager particles;
    Level level{&particles};
    TargetFollowCamera camera;
    Player player;
    FModSound sound;
    Globals::sound = &sound;
    PlayerEgo ego;
    ego.level = &level;
    ego.currentSystem = 0x123;
    ego.targetFollowCamera = &camera;
    ego.player = &player;
    ego.field_0x1c = 45;

    ego.explode();
    assert(particles.calls == 1 && Explosion::constructed == 1);
    assert(ego.explosion != nullptr && static_cast<Explosion *>(ego.explosion)->type == 0);
    assert(!camera.active && camera.calls == 1);
    assert(!player.active && player.calls == 1);
    assert(!ego.exhaustVisible && ego.exhaustCalls == 1);
    const int expected[10] = {17, 45, 27, 35, 2261, 2260, 2252, 1095, 1096, 1097};
    assert(sound.count == 10);
    for (int i = 0; i < 10; ++i) assert(sound.ids[i] == expected[i]);

    void *firstExplosion = ego.explosion;
    ego.explode();
    assert(particles.calls == 2);
    assert(Explosion::constructed == 1 && ego.explosion == firstExplosion);
    assert(camera.calls == 1 && player.calls == 1 && ego.exhaustCalls == 1);
    assert(sound.count == 10);
    delete static_cast<Explosion *>(ego.explosion);
}
