// Verbatim Create/Read bodies and production Mesh.cpp; file/release/Transform boundaries are mocked.
// Run through run_mesh_read_color_smoke.py --create-smoke. This does not validate native ABI.
#include "engine/core/AbyssEngine.h"
#include "engine/render/Mesh.h"
#include "engine/math/Transform.h"
#include "engine/math/AEMath.h"
#include "engine/file/AEFile.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {
std::vector<unsigned char> input;
size_t cursor, limit;
int opened, closed, released;
bool openSucceeds;

template<class T> void put(T value) {
    const auto *bytes = reinterpret_cast<const unsigned char *>(&value);
    input.insert(input.end(), bytes, bytes + sizeof(value));
}
void reset(size_t bytes) {
    cursor = 0; limit = bytes; opened = closed = released = 0; openSucceeds = true;
}
void dispose(AbyssEngine::Mesh *mesh) {
    ::operator delete[](mesh->positions);
    ::operator delete[](mesh->texCoords);
    ::operator delete[](mesh->normals);
    ::operator delete[](mesh->colors);
    ::operator delete[](mesh->indices);
    delete mesh->animation;
    delete mesh;
}
void fixture(bool enhanced, unsigned short subCount) {
    input.clear();
    const char plain[] = {'A', 'E', 'M', 'e', 's', 'h', 0};
    const char v4[] = {'V', '4', 'A', 'E', 'M', 'e', 's'};
    const char *magic = enhanced ? v4 : plain;
    for (int i = 0; i < 7; ++i) put(magic[i]);
    if (enhanced) put<unsigned short>(0);
    put<unsigned char>(1);
    if (enhanced) {
        put(subCount);
        for (int i = 0; i < 3; ++i) put(0.0f); // Pivot.
    }
    put<unsigned short>(1);
    if (enhanced) {
        put(1.0f); put(2.0f); put(3.0f);
        put(1.0f); put(2.0f); put(3.0f); put(4.0f); // Enhanced bounds.
        for (int i = 0; i < 4; ++i) put<short>(9); // Unknown groups: no keys.
        put<unsigned short>(0); // Children.
    } else {
        put<short>(1); put<short>(2); put<short>(3);
    }
}
}

uint32_t AEFile::OpenRead(const char *, uint32_t *handle) {
    ++opened;
    if (!openSucceeds) return 0;
    *handle = 1;
    return 1;
}
void AEFile::Close(uint32_t handle) { assert(handle == 1); ++closed; }
uint32_t AEFile::Read(uint32_t bytes, void *out, uint32_t handle) {
    assert(handle == 1);
    if (cursor + bytes > limit) return 0;
    std::memcpy(out, input.data() + cursor, bytes);
    cursor += bytes;
    return bytes;
}

namespace AbyssEngine {
int MeshReadData(Engine *, const unsigned int &, unsigned int, Mesh **, Material *);
float timeBetweenFrames;
unsigned char Engine::enableShader;
void MeshRelease(Engine *, Mesh **slot) { ++released; dispose(*slot); *slot = nullptr; }
// These fixtures have neither children, nonempty animation nor cloned meshes.
int MeshConvertToVBO(Mesh *) { std::abort(); }
AEMath::BSphere &AEMath::BSphere::operator=(const AEMath::BSphere &) { std::abort(); }
void AEMath::BSphere::Merge(const AEMath::BSphere &) { std::abort(); }
Transform::Transform(Transform *) { std::abort(); }
AEMath::Matrix::Matrix() { std::memset(m, 0, sizeof(m)); }
Quaternion::Quaternion() : x(0), y(0), z(0), w(1) {}
Quaternion::~Quaternion() {}
Transform::Transform() {}
Transform::~Transform() {}
void Transform::InsertKeyFrame(const float *, longlong, int) { std::abort(); }
void Transform::CollectAnimationData() { std::abort(); }
void Transform::SetAnimationRangeInTime(longlong, longlong) { std::abort(); }
}

#include "mesh_read_math_under_test.inc"
#include "mesh_read_under_test.inc"
#include "mesh_create_under_test.inc"

int main() {
    using namespace AbyssEngine;
    // Engine is an opaque nonnull boundary token; these paths never dereference it.
    Engine *engine = reinterpret_cast<Engine *>(uintptr_t(1));
    Mesh *slot = nullptr;
    reset(0);
    assert(MeshCreateFromFile(nullptr, "fixture", &slot, nullptr) == -4);
    assert(MeshCreateFromFile(engine, nullptr, &slot, nullptr) == -4);
    assert(slot == nullptr && opened == 0 && closed == 0 && released == 0);
    openSucceeds = false;
    assert(MeshCreateFromFile(engine, "fixture", &slot, nullptr) == -1);
    assert(slot == nullptr && opened == 1 && closed == 0 && released == 0);

    for (int kind = 0; kind < 3; ++kind) {
        const bool enhanced = kind != 0;
        fixture(enhanced, kind == 2 ? 1 : 0);
        reset(input.size());
        assert(MeshCreateFromFile(engine, "fixture", &slot, nullptr) == 1);
        assert(slot != nullptr && slot->vertexCount == 1 && slot->animation == nullptr);
        assert(opened == 1 && closed == 1 && released == 0 && cursor == input.size());
        const auto *positions = static_cast<const float *>(slot->positions);
        assert(positions[0] == 1 && positions[1] == 2 && positions[2] == 3);
        if (enhanced)
            assert(slot->boundsCenterX == 1 && slot->boundsCenterY == 3 && slot->boundsCenterZ == -2 && slot->boundsRadius == 4);
        dispose(slot); slot = nullptr;

        // Fail inside the real reader's position read, before enhanced parsing.
        reset(enhanced ? 37 : input.size() - 1);
        assert(MeshCreateFromFile(engine, "fixture", &slot, nullptr) == -1);
        assert(slot == nullptr && opened == 1 && closed == 1 && released == 1);
        if (enhanced) {
            reset(input.size() - 1); // Real enhanced parsing succeeds, then child-count read fails.
            assert(MeshCreateFromFile(engine, "fixture", &slot, nullptr) == -1);
            assert(slot == nullptr && opened == 1 && closed == 1 && released == 1);
        }
    }
}
