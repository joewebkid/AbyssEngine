#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

using GLuint = unsigned;
static GLuint deleted[4] = {};
static unsigned deletedCount = 0;
static unsigned activeUnits[8] = {};
static unsigned activeCount = 0;
static unsigned boundIds[4] = {};
static unsigned boundCount = 0;

extern "C" void glDeleteTextures(int count, const GLuint *ids) {
    for (int i = 0; i < count; ++i) deleted[deletedCount++] = ids[i];
}
extern "C" void glActiveTexture(unsigned unit) {
    activeUnits[activeCount++] = unit;
}
extern "C" void glBindTexture(unsigned target, unsigned id) {
    assert(target == 0x8513);
    boundIds[boundCount++] = id;
}

template<class T> struct Array {
    unsigned count;
    T *data_;
    unsigned capacity;
};

namespace AbyssEngine {
int CubeMapSetIndex = 0;
struct Engine { static int enableShader; };
int Engine::enableShader = 1;
struct String {
    const char *text;
    static unsigned reads;
    char *GetAEChar() const {
        ++reads;
        const std::size_t n = std::strlen(text);
        char *copy = new char[n + 1];
        std::memcpy(copy, text, n + 1);
        return copy;
    }
};
unsigned String::reads = 0;
struct AELoadedTexture {
    std::uint32_t glId;
    String name;
    float scale;
    std::uint8_t isCube;
};

static unsigned createCount = 0;
static std::string createdPaths[4];
static float createdScales[4] = {};
int TextureCreateFromFile(Engine *, const char *path, void *, void *,
                          unsigned *out, bool managed, float scale) {
    assert(!managed);
    createdPaths[createCount] = path;
    createdScales[createCount] = scale;
    *out = 201 + createCount++;
    return 1;
}

struct PaintCanvas {
    Engine *engine;
    Array<AELoadedTexture *> cubeTextures;
    explicit PaintCanvas(Engine *value) : engine(value) {}
    void ChangeCubeTexture(unsigned int idx);
    void Suspend();
    void Resume();
    void ReloadTextures();
};
}

using namespace AbyssEngine;

// PRODUCTION_METHODS

int main() {
    Engine engine;
    AELoadedTexture *slots[] = {
        new AELoadedTexture{11, {"flat"}, 1.5f, 0},
        new AELoadedTexture{12, {"cube"}, 2.5f, 1}
    };
#if INTPTR_MAX > INT32_MAX
    assert(reinterpret_cast<std::uintptr_t>(slots[0]) > UINT32_MAX);
#endif
    PaintCanvas canvas(&engine);
    canvas.cubeTextures = {2, slots, 2};

    canvas.ChangeCubeTexture(0);
    assert(String::reads == 1 && boundCount == 0 && CubeMapSetIndex == 0);
    canvas.ChangeCubeTexture(1);
    assert(CubeMapSetIndex == 1 && boundCount == 1 && boundIds[0] == 12);
    assert(activeCount == 2 && activeUnits[0] == 0x84c7 && activeUnits[1] == 0x84c0);

    canvas.Suspend();
    assert(deletedCount == 2 && deleted[0] == 11 && deleted[1] == 12);
    assert(slots[0]->glId == UINT32_MAX && slots[1]->glId == UINT32_MAX);
    canvas.Suspend();
    assert(deletedCount == 2);

    canvas.Resume();
    assert(createCount == 2 && createdPaths[0] == "flat" && createdPaths[1] == "cube");
    assert(createdScales[0] == 1.5f && createdScales[1] == 2.5f);
    assert(slots[0]->glId == 201 && slots[1]->glId == 202);
    assert(boundCount == 2 && boundIds[1] == 202);
    assert(activeCount == 4 && activeUnits[2] == 0x84c7 && activeUnits[3] == 0x84c0);

    slots[0]->glId = UINT32_MAX;
    canvas.ReloadTextures();
    assert(createCount == 3 && createdPaths[2] == "flat" && createdScales[2] == 1.5f);
    assert(slots[0]->glId == 203 && slots[1]->glId == 202);
    canvas.ReloadTextures();
    assert(createCount == 3);

    delete slots[0];
    delete slots[1];
}
