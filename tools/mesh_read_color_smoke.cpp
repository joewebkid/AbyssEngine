// Exercise verbatim production MeshReadData: colors, bounds and tangent bases.
// Build/run through run_mesh_read_color_smoke.py; vector math is production code.
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
// Enhanced/child boundaries must stay unused by these reader tests.
int Mesh::ReadEnhancedDataFromFile(unsigned int, unsigned int) { std::abort(); }
void AEMath::BSphere::Merge(const AEMath::BSphere &) { std::abort(); }
}

#include "mesh_read_math_under_test.inc"
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
    assert(mesh.boundsCenterX == 2 && mesh.boundsCenterY == 1 && mesh.boundsCenterZ == 6);
    assert(std::fabs(mesh.boundsRadius - std::sqrt(24.0f)) < 0.000001f);
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

    Engine::enableShader = 1;
    for (int fixture = 0; fixture < 3; ++fixture) {
        input.clear();
        put<unsigned short>(3);
        for (unsigned short index = 0; index < 3; ++index) put(index);
        put<unsigned short>(3);
        const int vertices[3][9] = {{0,0,0, 1,0,0, 0,1,0}, {0,0,0, 3,3,0, 2,1,0},
                                    {0,0,0, 3,0,3, 2,0,1}};
        for (int coordinate : vertices[fixture]) put(coordinate);
        const short uv[3][6] = {{0,0, 4096,0, 0,4096}, {0,0, 4096,4096, 0,4096},
                                {0,0, 4096,4096, 0,4096}};
        for (short coordinate : uv[fixture]) put(coordinate);
        for (int vertex = 0; vertex < 3; ++vertex) {
            put<short>(0);
            put<short>(fixture == 2 ? 32767 : 0);
            put<short>(fixture == 2 ? 0 : 32767);
        }
        cursor = 0; limit = input.size();
        Mesh triangle;
        triangle.vertexFormat = 0x16;
        slot = &triangle;
        assert(MeshReadData(nullptr, handle, 1, &slot, nullptr) == 1);
        assert(cursor == input.size());
        const float tx = fixture == 0 ? 1.0f : 1.0f / std::sqrt(5.0f);
        const float ty = fixture == 1 ? 2.0f / std::sqrt(5.0f) : 0.0f;
        const float tz = fixture == 2 ? 2.0f / std::sqrt(5.0f) : 0.0f;
        const auto *tangent = static_cast<const float *>(triangle.tangents);
        const auto *binormal = static_cast<const float *>(triangle.binormals);
        for (int vertex = 0; vertex < 3; ++vertex) {
            assert(std::fabs(tangent[vertex*3] - tx) < 0.000001f);
            assert(std::fabs(tangent[vertex*3+1] - ty) < 0.000001f);
            assert(std::fabs(tangent[vertex*3+2] - tz) < 0.000001f);
            assert(std::fabs(binormal[vertex*3] - (fixture == 2 ? tz : -ty)) < 0.000001f);
            assert(std::fabs(binormal[vertex*3+1] - (fixture == 2 ? 0 : tx)) < 0.000001f);
            assert(std::fabs(binormal[vertex*3+2] - (fixture == 2 ? -tx : 0)) < 0.000001f);
        }
        ::operator delete[](triangle.indices);
        ::operator delete[](triangle.positions);
        ::operator delete[](triangle.texCoords);
        ::operator delete[](triangle.normals);
        ::operator delete[](triangle.tangents);
        ::operator delete[](triangle.binormals);
    }
}
