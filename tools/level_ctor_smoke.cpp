#include "game/world/Level.h"

#include <cstdio>
#include <initializer_list>
#include <new>

int main() {
    // The constructor owns no heap allocations; the unrelated Level destructor
    // is outside this focused entry test.
    alignas(Level) unsigned char storage[sizeof(Level)];
    Level *level = new (storage) Level(3);
    if (level->missionPtr != 3 || level->skyboxMesh != -1 ||
        level->skyboxTexture != -1) {
        std::fputs("Level constructor state mismatch\n", stderr);
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
