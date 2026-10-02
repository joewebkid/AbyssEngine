// Exercise the verbatim production MeshReadData body and short color reads.
// Build/run through run_mesh_read_color_smoke.py; other math/engine paths abort.
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
template<class T> void put(T value) {
    const auto *bytes = reinterpret_cast<const unsigned char *>(&value);
    input.insert(input.end(), bytes, bytes + sizeof(value));
}
}

uint32_t AEFile::Read(uint32_t bytes, void *out, uint32_t) {
    if (cursor + bytes > limit) return 0;
    std::memcpy(out, input.data() + cursor, bytes);
    cursor += bytes;
    return bytes;
}

namespace AbyssEngine {
int MeshReadData(Engine *, const unsigned int &, unsigned int, Mesh **, Material *);
unsigned char Engine::enableShader;
// These enhanced/child boundaries must stay unused by this color-only test.
int Mesh::ReadEnhancedDataFromFile(unsigned int, unsigned int) { std::abort(); }
void AEMath::BSphere::Merge(const AEMath::BSphere &) { std::abort(); }
namespace AEMath {
Vector &Vector::operator+=(const Vector &) { std::abort(); }
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x-b.x, a.y-b.y, a.z-b.z};
}
float VectorLength(const Vector &v) { return std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z); }
float VectorDot(const Vector &, const Vector &) { std::abort(); }
Vector operator*(const Vector &, float) { std::abort(); }
Vector VectorNormalize(const Vector &) { std::abort(); }
Vector VectorCross(const Vector &, const Vector &) { std::abort(); }
}
}

#include "mesh_read_under_test.inc"

int main() {
    using namespace AbyssEngine;
    const unsigned int handle = 1;
    const unsigned char rgba[] = {0, 1, 127, 255, 64, 128, 192, 254};
    input.clear();
    put<unsigned short>(2);
    // flags=1 selects integer positions before compressed colors.
    const int positions[] = {-2, 3, 4, 6, -1, 8};
    for (int value : positions) put(value);
    for (unsigned char value : rgba) put(value);
    const auto complete = input;
    cursor = 0; limit = input.size();
    Mesh mesh;
    mesh.vertexFormat = 8;
    Mesh *slot = &mesh;
    assert(MeshReadData(nullptr, handle, 1, &slot, nullptr) == 1);
    assert(slot == &mesh && cursor == input.size() && mesh.vertexCount == 2);
    const auto *colors = static_cast<const float *>(mesh.colors);
    for (size_t i = 0; i < sizeof(rgba); ++i)
        assert(std::fabs(colors[i] - static_cast<float>(rgba[i]) / 255.0f) < 0.000001f);
    const auto *converted = static_cast<const float *>(mesh.positions);
    for (size_t i = 0; i < 6; ++i) assert(converted[i] == positions[i]);
    ::operator delete[](mesh.positions);
    ::operator delete[](mesh.colors);

    // Every shorter byte prefix fails before the complete color allocation.
    for (size_t cut = 0; cut < complete.size(); ++cut) {
        input = complete; cursor = 0; limit = cut;
        Mesh truncated;
        truncated.vertexFormat = 8;
        slot = &truncated;
        assert(MeshReadData(nullptr, handle, 1, &slot, nullptr) == -1);
        assert(truncated.colors == nullptr);
        ::operator delete[](truncated.positions);
    }

    input.clear(); put<unsigned short>(0);
    cursor = 0; limit = input.size();
    Mesh empty;
    empty.vertexFormat = 8;
    slot = &empty;
    // This file boundary returns zero for a zero-byte read; the parser rejects it.
    assert(MeshReadData(nullptr, handle, 1, &slot, nullptr) == -1);
    assert(cursor == input.size() && empty.vertexCount == 0);
    assert(empty.positions == nullptr && empty.colors == nullptr);
    ::operator delete[](empty.positions);
    ::operator delete[](empty.colors);
}
