#include <cassert>
#include <vector>

struct Matrix {};

struct AEGeometry {
    Matrix reference;
    Matrix &getReferenceMatrix() { return reference; }
};

struct ParticleSettings {
    enum ParticleSet { ParticleSet_9 = 9, ParticleSet_0xf = 15, ParticleSet_0x2a = 42 };
};

struct ParticleSystemManager {
    struct Add { const Matrix *matrix; int set; bool flag; };
    struct Emit { int handle; bool enabled; };
    int base;
    std::vector<Add> adds;
    std::vector<Emit> emits;

    int addSystem(const Matrix *matrix, ParticleSettings::ParticleSet set, bool flag) {
        adds.push_back({matrix, set, flag});
        return base + static_cast<int>(adds.size());
    }
    void enableSystemEmit(int handle, bool enabled) { emits.push_back({handle, enabled}); }
};

struct Level {
    ParticleSystemManager *field_74;
    ParticleSystemManager *particleEmitBoolPtr;
    ParticleSystemManager *particleRenderBoolPtr;
    ParticleSystemManager *field_8c;
};

struct KIPlayer {
    AEGeometry *geometry = nullptr;
    Level *level = nullptr;
    int field_0x80 = -1;
    int field_0x84 = -1;
    void setLevel(Level *value) { level = value; }
};

struct PlayerFighter : KIPlayer {
    int engineTrailSystem = -1;
    int field_0x134 = -1;
    int field_0x138 = -1;
    void setLevel(Level *lvl);
};

// PRODUCTION_METHOD

int main() {
    AEGeometry geometry;
    ParticleSystemManager trail{100}, emit{200}, render{300}, misc{400};
    Level level{&trail, &emit, &render, &misc};
    PlayerFighter fighter;
    fighter.geometry = &geometry;
    fighter.setLevel(&level);

    assert(fighter.level == &level);
    assert(fighter.engineTrailSystem == 101);
    assert(fighter.field_0x80 == 201);
    assert(fighter.field_0x84 == 301);
    assert(fighter.field_0x134 == 401);
    assert(fighter.field_0x138 == 402);
    const ParticleSystemManager *managers[] = {&trail, &emit, &render, &misc};
    const int expected_sets[][2] = {{9, -1}, {15, -1}, {42, -1}, {17, 18}};
    for (int i = 0; i != 4; ++i) {
        assert(managers[i]->adds.size() == managers[i]->emits.size());
        assert(managers[i]->adds.size() == (i == 3 ? 2u : 1u));
        for (size_t j = 0; j != managers[i]->adds.size(); ++j) {
            assert(managers[i]->adds[j].matrix == &geometry.reference);
            assert(managers[i]->adds[j].set == expected_sets[i][j]);
            assert(!managers[i]->adds[j].flag);
            assert(managers[i]->emits[j].handle == managers[i]->base + static_cast<int>(j) + 1);
            assert(!managers[i]->emits[j].enabled);
        }
    }
}
