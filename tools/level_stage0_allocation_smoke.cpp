#include "engine/render/LODManager.h"
#include "engine/render/ParticleSystemManager.h"
#include "engine/render/PaintCanvas.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <new>

struct Allocation { unsigned char *data; std::size_t size; };
static Allocation allocations[256];
static unsigned allocationCount;

void *operator new(std::size_t size) {
    assert(allocationCount < 256);
    auto *data = static_cast<unsigned char *>(std::malloc(size + 16));
    if (!data) throw std::bad_alloc();
    std::memset(data + size, 0xa5, 16);
    allocations[allocationCount++] = {data, size};
    return data;
}
void operator delete(void *data) noexcept {
    for (unsigned i = 0; i < allocationCount; ++i) {
        if (allocations[i].data == data) allocations[i].data = nullptr;
    }
    std::free(data);
}

struct SolarSystem {
    int textureIndex = 0;
    int getTextureIndex() { return textureIndex; }
};
struct Status {
    static Status *gStatus;
    SolarSystem system;
    bool alien = false;
    int inAlienOrbit() { return alien; }
    SolarSystem *getSystem() { return &system; }
};
Status *Status::gStatus;
struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas;

struct Stage0Fixture {
    LODManager *lodManager = nullptr;
    ParticleSystemManager *particleEmitBoolPtr = nullptr;
    ParticleSystemManager *particleRenderBoolPtr = nullptr;
    ParticleSystemManager *field_74 = nullptr;
    ParticleSystemManager *field_80 = nullptr;
    ParticleSystemManager *field_98 = nullptr;
    ParticleSystemManager *particleSystemMgr = nullptr;
    ParticleSystemManager *skybox2Mesh = nullptr;
    ParticleSystemManager *field_8c = nullptr;
    ParticleSystemManager *field_9c = nullptr;
    ParticleSystemManager *field_94 = nullptr;
    void run();
};

#include "level_stage0_under_test.inc"

static void check(const Stage0Fixture &level, int dustTexture) {
    assert(level.lodManager && level.lodManager->objects);
    assert(level.lodManager->timer == 1001);
    const ParticleSystemManager *systems[] = {
        level.particleEmitBoolPtr, level.particleRenderBoolPtr, level.field_74,
        level.field_80, level.field_98, level.particleSystemMgr,
        level.skybox2Mesh, level.field_8c, level.field_9c, level.field_94,
    };
    const int textures[] = {
        0x4e85, 0x6a72, 0x4e83, 0x4e7a, 0x5e20,
        dustTexture, 0x4e7c, 0x6a7c, 0x6ab9, 0x6aaf,
    };
    for (unsigned i = 0; i < 10; ++i) {
        assert(systems[i]);
        assert(systems[i]->spriteTextureId == textures[i]);
        assert(systems[i]->cameraSet == ParticleSettings::CameraSet_1);
        assert(systems[i]->flags == 0x101);
    }
}

int main() {
    static_assert(sizeof(LODManager) > 0x14, "host LODManager must exceed ARM allocation");
    static_assert(sizeof(ParticleSystemManager) > 100, "host particle manager must exceed ARM allocation");
    PaintCanvas *canvas = reinterpret_cast<PaintCanvas *>(0x1234);
    Globals::Canvas = canvas;
    Status status;
    Status::gStatus = &status;
    Stage0Fixture normal;
    normal.run();
    check(normal, 0x4e7f);
    status.system.textureIndex = 12;
    Stage0Fixture dust;
    dust.run();
    check(dust, 0x4ea9);
    for (unsigned i = 0; i < allocationCount; ++i) {
        if (!allocations[i].data) continue;
        for (unsigned j = 0; j < 16; ++j) {
            assert(allocations[i].data[allocations[i].size + j] == 0xa5);
        }
    }
}
