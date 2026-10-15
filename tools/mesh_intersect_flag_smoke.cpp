#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>

namespace AbyssEngine {
struct Engine { static unsigned char enableShader; };
unsigned char Engine::enableShader = 0;
struct Mesh {
    void *positions;
    void *texCoords;
    uint16_t indexCount;
    void *indices;
};
struct Vec2 { float u, v; };

// PRODUCTION_MESH_INTERSECT
}

int main() {
    using namespace AbyssEngine;
    float positions[] = {0, 0, 0, 0, 0, 1, 1, 0, 0};
    float uvs[] = {0, 0, 0.5f, 0.25f, 1, 0.5f};
    uint16_t indices[] = {0, 1, 2};
    Mesh mesh{positions, uvs, 3, indices};

    Vec2 miss = MeshIntersect(2.0f, 2.0f, &mesh);
    assert(miss.u == -1.0f && miss.v == -1.0f);
    mesh.indexCount = 0;
    miss = MeshIntersect(0.25f, 0.25f, &mesh);
    assert(miss.u == -1.0f && miss.v == -1.0f);
    mesh.indexCount = 3;

    Engine::enableShader = 0;
    Vec2 hit = MeshIntersect(0.25f, 0.25f, &mesh);
    assert(hit.u == 0.5f && hit.v == 0.25f);
    Engine::enableShader = 1;
    hit = MeshIntersect(0.25f, 0.25f, &mesh);
    assert(hit.u == 0.5f && hit.v == 0.75f);
    std::puts("MeshIntersect shader-flag production smoke: OK");
}
