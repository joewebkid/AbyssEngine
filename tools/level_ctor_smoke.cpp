#include "game/world/Level.h"

#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <new>

int main() {
    Level::initStreamOutPosition = 0;
    Level::setInitStreamOut();
    if (Level::initStreamOutPosition != 1) {
        std::fputs("Level stream-out flag mismatch\n", stderr);
        return 1;
    }
    // The constructor owns no heap allocations; the unrelated Level destructor
    // is outside this focused entry test.
    alignas(Level) unsigned char storage[sizeof(Level)];
    std::memset(storage, 0xa5, sizeof(storage));
    Level *level = new (storage) Level(3);
    if (level->missionPtr != 3 || level->skyboxMesh != -1 ||
        level->skyboxTexture != -1) {
        std::fputs("Level constructor state mismatch\n", stderr);
        return 1;
    }
    if (level->field_134 != 0 || level->playerRoute || level->friendRoute ||
        level->enemyRoute || level->starSystem || level->player ||
        level->field_74 || level->particleEmitBoolPtr || level->particleSystemMgr ||
        level->field_80 || level->particleRenderBoolPtr || level->skybox2Mesh ||
        level->field_8c || level->field_90 || level->field_94 || level->field_98 ||
        level->field_9c || level->friendCargoStolen || level->alarmRequested ||
        level->supernovaFlareActive || level->field_29d) {
        std::fputs("Level constructor left entry state uninitialized\n", stderr);
        return 1;
    }
    for (const auto *matrix : {&level->skyMatrix, &level->cloudMatrix,
                               &level->reversalMatrix}) {
        if (matrix->m[0] != 1.0f || matrix->m[5] != 1.0f ||
            matrix->m[12] != 1.0f || matrix->m[14] != 1.0f) {
            std::fputs("Level matrix initialization mismatch\n", stderr);
            return 1;
        }
    }
    std::puts("Level constructor: ok");
    return 0;
}
