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
#include <fstream>
#include <iterator>
#include <vector>

namespace {
std::vector<unsigned char> input;
size_t cursor, limit;
int opened, closed, released;
bool openSucceeds;
bool changeTimeOnCollect;
int collected, rangeCalls;
AbyssEngine::Transform *rangeReceiver;
longlong lastRangeStart, lastRangeEnd;
std::vector<size_t> childStarts;
AbyssEngine::Mesh *failedChild;

template<class T> void put(T value) {
    const auto *bytes = reinterpret_cast<const unsigned char *>(&value);
    input.insert(input.end(), bytes, bytes + sizeof(value));
}
void reset(size_t bytes) {
    cursor = 0; limit = bytes; opened = closed = released = 0; openSucceeds = true;
    failedChild = nullptr;
    changeTimeOnCollect = false; collected = rangeCalls = 0; rangeReceiver = nullptr;
}
void dispose(AbyssEngine::Mesh *mesh) {
    if (mesh->animation) {
        for (auto *child : mesh->animation->meshes) dispose(child);
    }
    ::operator delete[](mesh->positions);
    ::operator delete[](mesh->texCoords);
    ::operator delete[](mesh->normals);
    ::operator delete[](mesh->colors);
    ::operator delete[](mesh->indices);
    delete mesh->animation;
    delete mesh;
}
void childNode(float x, float radius, bool animated, unsigned short children) {
    for (int i = 0; i < 3; ++i) put(0.0f); // Pivot.
    put<unsigned short>(1);
    put(x); put(2.0f); put(3.0f);
    put(1.0f); put(2.0f); put(3.0f); put(radius);
    if (animated) {
        put<short>(1); put<unsigned short>(1); put(2.0f);
        put(0.0f); put(0.0f); put(0.0f);
    } else {
        put<short>(9);
    }
    for (int i = 0; i < 3; ++i) put<short>(9);
    put(children);
}
void childrenFixture(unsigned short count) {
    input.clear();
    childNode(1.0f, 4.0f, true, count);
    childStarts.clear();
    for (unsigned int i = 0; i < count; ++i) {
        childStarts.push_back(input.size());
        childNode(float(4 + i), float(5 + i), false, 0);
    }
}
void fixture(bool enhanced, unsigned short subCount, bool uv = false, unsigned short vertices = 1) {
    input.clear();
    const char plain[] = {'A', 'E', 'M', 'e', 's', 'h', 0};
    const char v4[] = {'V', '4', 'A', 'E', 'M', 'e', 's'};
    const char *magic = enhanced ? v4 : plain;
    for (int i = 0; i < 7; ++i) put(magic[i]);
    if (enhanced) put<unsigned short>(0);
    put<unsigned char>(uv ? 3 : 1);
    if (enhanced) {
        put(subCount);
        for (int i = 0; i < 3; ++i) put(0.0f); // Pivot.
    }
    put(vertices);
    if (enhanced) {
        for (unsigned int i = 0; i < vertices; ++i) { put(1.0f); put(2.0f); put(3.0f); }
        if (uv) {
            for (unsigned int i = 0; i < vertices; ++i) {
                put(i == 0 ? 0.25f : 0.5f); put(i == 0 ? 0.75f : 0.125f);
            }
        }
        put(1.0f); put(2.0f); put(3.0f); put(4.0f); // Enhanced bounds.
        for (int i = 0; i < 4; ++i) put<short>(9); // Unknown groups: no keys.
        put<unsigned short>(0); // Children.
    } else {
        for (unsigned int i = 0; i < vertices; ++i) { put<short>(1); put<short>(2); put<short>(3); }
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
    if (cursor + bytes > limit) {
        // Original reader leaves an unattached failed child allocated. Capture
        // its first pivot destination so this test can explicitly free it.
        for (size_t start : childStarts) {
            if (cursor == start && bytes == 12)
                failedChild = reinterpret_cast<AbyssEngine::Mesh *>(
                    static_cast<unsigned char *>(out) - offsetof(AbyssEngine::Mesh, pivotX));
        }
        return 0;
    }
    std::memcpy(out, input.data() + cursor, bytes);
    cursor += bytes;
    return bytes;
}

namespace AbyssEngine {
int MeshReadData(Engine *, const unsigned int &, unsigned int, Mesh **, Material *);
float timeBetweenFrames;
unsigned char Engine::enableShader;
void MeshRelease(Engine *, Mesh **slot) { ++released; dispose(*slot); *slot = nullptr; }
// Mesh clone dependencies remain outside these fixtures.
int MeshConvertToVBO(Mesh *) { std::abort(); }
Transform::Transform(Transform *) { std::abort(); }
AEMath::Matrix::Matrix() { std::memset(m, 0, sizeof(m)); }
Quaternion::Quaternion() : x(0), y(0), z(0), w(1) {}
Quaternion::~Quaternion() {}
Transform::Transform() : boundingCenter{0, 0, 0}, boundingRadius(0), boundingRadius2(0) {}
Transform::~Transform() {}
void Transform::InsertKeyFrame(const float *, longlong, int) { ArrayAdd<KeyFrame *>(nullptr, keyFrames); }
void Transform::CollectAnimationData() {
    ++collected;
    assert(closed == 1);
    if (changeTimeOnCollect) timeBetweenFrames = 3.75f;
}
void Transform::SetAnimationRangeInTime(longlong first, longlong last) {
    ++rangeCalls; rangeReceiver = this; lastRangeStart = first; lastRangeEnd = last;
}
}

#include "mesh_read_math_under_test.inc"
#include "mesh_read_bounds_under_test.inc"
#include "mesh_read_under_test.inc"
#include "mesh_create_under_test.inc"

int main(int argc, char **argv) {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
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
    // Float V4 UVs exercise the actual reader's shader-dependent V flip.
    for (int shader = 0; shader < 2; ++shader) {
        Engine::enableShader = static_cast<unsigned char>(shader);
        fixture(true, 0, true, 2); reset(input.size());
        assert(MeshCreateFromFile(engine, "fixture", &slot, nullptr) == 1);
        assert(slot != nullptr && slot->vertexCount == 2 && slot->animation == nullptr);
        const auto *uv = static_cast<const float *>(slot->texCoords);
        assert(uv[0] == 0.25f && uv[1] == (shader ? 0.25f : 0.75f));
        assert(uv[2] == 0.5f && uv[3] == (shader ? 0.875f : 0.125f));
        assert(cursor == input.size() && opened == 1 && closed == 1 && released == 0);
        dispose(slot); slot = nullptr;
    }
    Engine::enableShader = 0;
    // Exercise caller completion with nonempty animation. The boundary changes
    // time during Collect, so a premature time load fails the final assertion.
    input.clear();
    for (char ch : {'V', '4', 'A', 'E', 'M', 'e', 's'}) put(ch);
    put<unsigned short>(0); put<unsigned char>(1); put<unsigned short>(0);
    childNode(1.0f, 4.0f, true, 0);
    reset(input.size()); changeTimeOnCollect = true;
    assert(MeshCreateFromFile(engine, "fixture", &slot, nullptr) == 1);
    assert(slot->animation != nullptr && collected == 1 && rangeCalls == 2);
    assert(rangeReceiver == slot->animation && lastRangeStart == 3 && lastRangeEnd == 10000000);
    assert(cursor == input.size() && opened == 1 && closed == 1 && released == 0);
    dispose(slot); slot = nullptr;
    const unsigned int handle = 1;
    auto *material = reinterpret_cast<Material *>(uintptr_t(0x1234));
    for (unsigned short count : {0, 1, 2}) {
        childrenFixture(count); reset(input.size());
        slot = new Mesh(); slot->vertexFormat = 1; slot->material = material;
        assert(MeshReadData(engine, handle, 8, &slot, material) == 1);
        assert(cursor == input.size() && slot->animation && slot->animation->meshes.size() == count);
        assert(slot->boundsCenterX == 1 && slot->boundsCenterY == 3 && slot->boundsCenterZ == -2);
        assert(slot->boundsRadius == (count ? float(4 + count) : 4.0f));
        for (unsigned int i = 0; i < count; ++i) {
            auto *child = slot->animation->meshes[i];
            assert(child->vboEligible == 1 && child->vertexFormat == 1 && child->material == material);
            assert(child->vertexCount == 1 && static_cast<float *>(child->positions)[0] == float(4 + i));
        }
        dispose(slot); slot = nullptr;
    }
    childrenFixture(2);
    for (unsigned int failedIndex = 0; failedIndex < 2; ++failedIndex) {
        reset(childStarts[failedIndex]);
        slot = new Mesh(); slot->vertexFormat = 1; slot->material = material;
        assert(MeshReadData(engine, handle, 8, &slot, material) == -1);
        assert(cursor == childStarts[failedIndex] && slot->animation && slot->animation->meshes.size() == failedIndex);
        assert(failedChild != nullptr);
        dispose(failedChild); failedChild = nullptr;
        dispose(slot); slot = nullptr;
    }
    for (int i = 1; i < argc; ++i) {
        std::ifstream file(argv[i], std::ios::binary);
        assert(file.good());
        input.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        assert(!input.empty());
        reset(input.size());
        assert(MeshCreateFromFile(engine, argv[i], &slot, nullptr) == 1);
        assert(slot && slot->vertexCount && cursor == input.size());
        assert(opened == 1 && closed == 1 && released == 0);
        dispose(slot); slot = nullptr;
    }
}
