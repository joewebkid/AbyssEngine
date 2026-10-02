// Native parser smoke: production Mesh.cpp with mocked file/Transform boundaries.
// g++ -std=c++14 -Isrc -Ithird_party/gl -ffunction-sections -fdata-sections
// tools/mesh_loader_smoke.cpp src/engine/render/Mesh.cpp -Wl,--gc-sections -o mesh_loader_smoke.exe
#include "engine/render/Mesh.h"
#include "engine/core/AbyssEngine.h"
#include "engine/math/Transform.h"
#include "engine/file/AEFile.h"
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>

namespace {
std::vector<unsigned char> stream;
size_t cursor, limit;
int destroyed;
struct Key { longlong channel; int time; float value; };
std::vector<Key> keys;
longlong rangeStart, rangeEnd;
template<class T> void put(T value) {
    const auto *bytes = reinterpret_cast<const unsigned char *>(&value);
    stream.insert(stream.end(), bytes, bytes + sizeof(value));
}
void header() {
    stream.clear();
    put(1.0f); put(2.0f); put(3.0f); put(4.0f);
}
void unknownGroups() { put<short>(-1); put<short>(9); put<short>(42); }
void reset(size_t bytes) {
    cursor = 0; limit = bytes; destroyed = 0; keys.clear();
    AbyssEngine::timeBetweenFrames = 1000000.0f;
}
void dispose(AbyssEngine::Mesh &mesh) {
    delete mesh.animation;
    mesh.animation = nullptr;
}
}

uint32_t AEFile::Read(uint32_t bytes, void *out, uint32_t) {
    if (cursor + bytes > limit) return 0;
    std::memcpy(out, stream.data() + cursor, bytes);
    cursor += bytes;
    return bytes;
}
namespace AbyssEngine {
float timeBetweenFrames;
unsigned char Engine::enableShader;
// Unexercised Mesh clone dependencies must never be called in this parser test.
int MeshConvertToVBO(Mesh *) { std::abort(); }
AEMath::BSphere &AEMath::BSphere::operator=(const AEMath::BSphere &) { std::abort(); }
AEMath::Vector &AEMath::Vector::operator=(const AEMath::Vector &) { std::abort(); }
Transform::Transform(Transform *) { std::abort(); }
AEMath::Matrix::Matrix() { std::memset(m, 0, sizeof(m)); }
Quaternion::Quaternion() : x(0), y(0), z(0), w(1) {}
Quaternion::~Quaternion() {}
Transform::Transform() {}
Transform::~Transform() { ++destroyed; }
void Transform::InsertKeyFrame(const float *values, longlong channel, int time) {
    keys.push_back({channel, time, values[0]});
    ArrayAdd<KeyFrame *>(nullptr, keyFrames);
}
void Transform::SetAnimationRangeInTime(longlong first, longlong last) {
    ::rangeStart = first; ::rangeEnd = last;
}
}

int main() {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
    using namespace AbyssEngine;
    header(); unknownGroups(); reset(stream.size());
    Mesh empty;
    assert(empty.ReadEnhancedDataFromFile(1, 0) == 1);
    assert(empty.animation == nullptr && destroyed == 1 && keys.empty());
    assert(empty.boundsCenterX == 1 && empty.boundsCenterY == 3 && empty.boundsCenterZ == -2);
    assert(empty.boundsRadius == 4 && cursor == stream.size());

    header(); put<short>(0);
    const float times[] = {8, 0, -4};
    for (int axis = 0; axis < 3; ++axis) {
        put<unsigned short>(1); put(times[axis]); put(float(-2 - axis));
    }
    put<short>(1); put<unsigned short>(1); put(4.0f);
    put(10.0f); put(20.0f); put(30.0f);
    put<short>(9);
    reset(stream.size()); Mesh axes;
    assert(axes.ReadEnhancedDataFromFile(1, 0) == 1 && keys.size() == 4);
    assert(keys[0].channel == 1 && keys[0].value == -2 && keys[0].time == 8);
    assert(keys[1].channel == 4 && keys[1].value == 3 && keys[1].time == 0);
    assert(keys[2].channel == 2 && keys[2].value == -4 && keys[2].time == -4);
    assert(keys[3].channel == 0x1c0 && keys[3].time == 4);
    assert(timeBetweenFrames == 4 && axes.animation->animationStart == 4);
    assert(rangeStart == 4 && rangeEnd == 10000000);
    dispose(axes);

    header(); unknownGroups();
    put<short>(2); put<unsigned short>(1); put(2.0f); put(4.0f);
    put<unsigned short>(1);
    for (int c = 0; c < 7; ++c) {
        put<short>(1); put(c == 0 ? 0.0f : 6.0f);
        put(100.0f);
    }
    const auto complete = stream;
    reset(stream.size()); Mesh v5;
    assert(v5.ReadEnhancedDataFromFile(1, 0x10) == 1 && keys.size() == 8);
    const longlong channels[] = {0x200, 0x400, 0x800, 0x2000, 0x4000, 0, 0, 0x40000};
    for (size_t i = 0; i < keys.size(); ++i) assert(keys[i].channel == channels[i]);
    assert(std::fabs(keys.back().value - 6.2832f / 360.0f) < 0.000001f);
    assert(v5.hasAnimation == 1 && timeBetweenFrames == 2 && cursor == stream.size());
    dispose(v5);
    // Every truncated input must take the real parser's cleanup path.
    for (size_t cut = 0; cut < complete.size(); ++cut) {
        stream = complete; reset(cut); Mesh truncated;
        assert(truncated.ReadEnhancedDataFromFile(1, 0x10) == -1);
        assert(truncated.animation == nullptr && destroyed == 1);
    }

    header(); unknownGroups(); put<short>(9); put<unsigned short>(1);
    put<short>(-1);
    for (int c = 1; c < 7; ++c) put<short>(0);
    reset(stream.size()); Mesh signedCount;
    assert(signedCount.ReadEnhancedDataFromFile(1, 0x10) == 1);
    assert(keys.empty() && destroyed == 1 && cursor == stream.size());

    // A populated optional group must not make an absent V5 block look present.
    for (unsigned int flags : {0x8u, 0x10u}) {
        header(); unknownGroups(); put<short>(2); put<unsigned short>(2);
        put(8.0f); put(5.0f); put(2.0f); put(7.0f);
        if (flags & 0x10) put<unsigned short>(0);
        reset(stream.size()); Mesh optionalOnly;
        assert(optionalOnly.ReadEnhancedDataFromFile(1, flags) == 1);
        assert(keys.size() == 2 && cursor == stream.size());
        assert(keys[0].channel == 0x200 && keys[0].time == 8 && keys[0].value == 5);
        assert(keys[1].channel == 0x200 && keys[1].time == 2 && keys[1].value == 7);
        assert(optionalOnly.hasAnimation == 0 && timeBetweenFrames == 2);
        dispose(optionalOnly);
    }
}
