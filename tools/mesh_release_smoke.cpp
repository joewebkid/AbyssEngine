#include <cassert>
#include <cstdint>
#include <cstdio>

static unsigned deletedBuffers[8] = {};
static int deleteCalls = 0;
void glDeleteBuffers(int count, const unsigned *buffers) {
    for (int i = 0; i < count; ++i) {
        assert(deleteCalls < 8);
        deletedBuffers[deleteCalls++] = buffers[i];
    }
}

namespace AbyssEngine {
struct Engine { static unsigned char enableShader; };
unsigned char Engine::enableShader = 0;
struct Transform {
    static int destructors;
    ~Transform() { ++destructors; }
};
int Transform::destructors = 0;
struct Mesh {
    unsigned char shared = 0, uploaded = 0, vertexFormat = 0;
    unsigned positionVBO = 0, indexVBO = 0, texCoordVBO = 0;
    unsigned normalVBO = 0, tangentVBO = 0, binormalVBO = 0, colorVBO = 0;
    void *indices = nullptr, *positions = nullptr, *texCoords = nullptr;
    void *colors = nullptr, *normals = nullptr, *tangents = nullptr, *binormals = nullptr;
    Transform *animation = nullptr;
};
struct SpriteSystem {
    float *posCpu = nullptr;
    int16_t *sizeCpu = nullptr;
    Mesh *mesh = nullptr;
};
static int transformReleases = 0;
static Engine *lastEngine = nullptr;
void TransformRelease(Engine *engine, Transform **slot) {
    ++transformReleases;
    lastEngine = engine;
    assert(slot != nullptr);
}
void MeshReleaseIntern(Engine *, Mesh **);
void MeshRelease(Engine *, Mesh **);

// PRODUCTION_RELEASE
}

int main() {
    using namespace AbyssEngine;
    Engine engine;
    Mesh *mesh = nullptr;
    MeshRelease(&engine, &mesh);
    assert(transformReleases == 0);

    mesh = new Mesh;
    MeshRelease(nullptr, &mesh);
    assert(mesh != nullptr && transformReleases == 0);
    MeshRelease(&engine, &mesh);
    assert(mesh == nullptr && transformReleases == 1 && lastEngine == &engine);

    Mesh *uploaded = new Mesh;
    uploaded->uploaded = 1;
    uploaded->vertexFormat = 15;
    uploaded->positionVBO = 11; uploaded->indexVBO = 12; uploaded->texCoordVBO = 13;
    uploaded->normalVBO = 14; uploaded->tangentVBO = 15;
    uploaded->binormalVBO = 16; uploaded->colorVBO = 17;
    uploaded->indices = new unsigned char[6];
    uploaded->positions = new unsigned char[24];
    uploaded->texCoords = new unsigned char[16];
    uploaded->colors = new unsigned char[32];
    uploaded->normals = new unsigned char[24];
    uploaded->tangents = new unsigned char[24];
    uploaded->binormals = new unsigned char[24];
    uploaded->animation = new Transform;
    Engine::enableShader = 1;
    MeshRelease(&engine, &uploaded);
    assert(uploaded == nullptr && transformReleases == 2);
    assert(Transform::destructors == 1 && deleteCalls == 7);
    for (int i = 0; i < 7; ++i) assert(deletedBuffers[i] == static_cast<unsigned>(11 + i));

    Mesh *shared = new Mesh;
    shared->shared = 1;
    shared->uploaded = 1;
    shared->positions = new unsigned char[4];
    void *sharedPositions = shared->positions;
    MeshRelease(&engine, &shared);
    assert(shared == nullptr && deleteCalls == 7);
    operator delete[](sharedPositions);

    SpriteSystem *sprite = new SpriteSystem;
    sprite->posCpu = new float[3];
    sprite->sizeCpu = new int16_t[1];
    sprite->mesh = new Mesh;
    SpriteSystemRelease(&engine, &sprite);
    assert(sprite == nullptr && transformReleases == 4);
    std::puts("MeshRelease and sprite release smoke: OK");
}
