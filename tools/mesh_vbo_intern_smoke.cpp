#include <cassert>
#include <cstddef>
#include <cstdio>
#include <vector>

struct Upload {
    unsigned target, usage;
    std::size_t size;
    const void *data;
};
static std::vector<Upload> uploads;
static std::vector<unsigned> bindings;
static unsigned nextBuffer = 1, glError = 0;
static int generated = 0, deleted = 0;
static void **positionSlotToChange = nullptr;
static void *replacementPosition = nullptr;
static unsigned short *countSlotToChange = nullptr;

void glGenBuffers(int count, unsigned *out) {
    for (int i = 0; i < count; ++i) out[i] = nextBuffer++;
    generated += count;
    if (positionSlotToChange) {
        *positionSlotToChange = replacementPosition;
        positionSlotToChange = nullptr;
    }
}
void glBindBuffer(unsigned target, unsigned buffer) {
    bindings.push_back(target);
    bindings.push_back(buffer);
}
void glBufferData(unsigned target, std::size_t size, const void *data, unsigned usage) {
    uploads.push_back({target, usage, size, data});
    if (countSlotToChange) {
        *countSlotToChange = 4;
        countSlotToChange = nullptr;
    }
}
unsigned glGetError() { return glError; }
void glDeleteBuffers(int count, const unsigned *) { deleted += count; }

namespace AbyssEngine {
struct Engine {
    static bool vboSupported;
    static unsigned char enableShader, KeepRawMeshData;
    static int vboSize;
};
bool Engine::vboSupported = true;
unsigned char Engine::enableShader = 0, Engine::KeepRawMeshData = 1;
int Engine::vboSize = 0;

struct Mesh {
    unsigned char vertexFormat = 0, uploaded = 0;
    unsigned short vertexCount = 0, indexCount = 0;
    void *positions = nullptr, *texCoords = nullptr, *colors = nullptr;
    void *normals = nullptr, *tangents = nullptr, *binormals = nullptr;
    void *indices = nullptr;
    unsigned positionVBO = 0, indexVBO = 0, texCoordVBO = 0;
    unsigned normalVBO = 0, tangentVBO = 0, binormalVBO = 0, colorVBO = 0;
    int vboByteSize = 0;
};

// PRODUCTION_METHOD
}

int main() {
    using namespace AbyssEngine;
    Mesh mesh;
    assert(MeshConvertToVBOIntern(nullptr) == -4);
    Engine::vboSupported = false;
    mesh.indexCount = 6;
    assert(MeshConvertToVBOIntern(&mesh) == -4);
    Engine::vboSupported = true;
    mesh.indexCount = 0;
    assert(MeshConvertToVBOIntern(&mesh) == -4);
    mesh.indexCount = 6;
    mesh.uploaded = 1;
    assert(MeshConvertToVBOIntern(&mesh) == -4);
    assert(generated == 0 && uploads.empty());

    unsigned char pos[36] = {}, tex[24] = {}, idx[12] = {};
    unsigned char norm[36] = {}, tan[36] = {}, bin[36] = {}, col[48] = {};
    mesh.uploaded = 0;
    mesh.vertexFormat = 15;
    mesh.vertexCount = 3;
    mesh.positions = pos; mesh.texCoords = tex; mesh.indices = idx;
    mesh.normals = norm; mesh.tangents = tan; mesh.binormals = bin; mesh.colors = col;
    Engine::enableShader = 1;
    Engine::KeepRawMeshData = 1;
    assert(MeshConvertToVBOIntern(&mesh) == 1);
    assert(generated == 7 && uploads.size() == 7 && deleted == 0);
    assert(uploads[0].target == 0x8892 && uploads[0].size == 36 && uploads[0].data == pos);
    assert(uploads[1].size == 24 && uploads[1].data == tex);
    assert(uploads[2].target == 0x8893 && uploads[2].size == 12 && uploads[2].data == idx);
    assert(uploads[3].size == 36 && uploads[3].data == norm);
    assert(uploads[4].size == 36 && uploads[4].data == tan);
    assert(uploads[5].size == 36 && uploads[5].data == bin);
    assert(uploads[6].size == 48 && uploads[6].data == col);
    for (const Upload &u : uploads) assert(u.usage == 0x88e4);
    assert(mesh.vboByteSize == 192 && Engine::vboSize == 192 && mesh.uploaded == 1);
    assert(mesh.positions == pos && mesh.indices == idx);
    assert(bindings[bindings.size() - 4] == 0x8892 && bindings[bindings.size() - 3] == 0);
    assert(bindings[bindings.size() - 2] == 0x8893 && bindings[bindings.size() - 1] == 0);
    assert(MeshConvertToVBOIntern(&mesh) == -4 && generated == 7);

    Mesh failed;
    failed.vertexFormat = 3;
    failed.vertexCount = 2;
    failed.indexCount = 3;
    failed.positions = pos; failed.texCoords = tex; failed.indices = idx;
    glError = 0x0502;
    assert(MeshConvertToVBOIntern(&failed) == -1);
    assert(failed.uploaded == 0 && failed.vboByteSize == 0);
    assert(failed.positions == pos && failed.texCoords == tex && failed.indices == idx);
    assert(Engine::vboSize == 192 && deleted == 0);

    Mesh released;
    released.vertexFormat = 1;
    released.vertexCount = 2;
    released.indexCount = 3;
    released.positions = new unsigned char[24];
    released.texCoords = new unsigned char[16];
    released.indices = new unsigned char[6];
    released.normals = new unsigned char[24];
    released.colors = new unsigned char[32];
    released.tangents = new unsigned char[24];
    released.binormals = new unsigned char[24];
    glError = 0;
    Engine::KeepRawMeshData = 0;
    assert(MeshConvertToVBOIntern(&released) == 1);
    assert(released.uploaded == 1 && released.positions == nullptr);
    assert(released.texCoords == nullptr && released.indices == nullptr);
    assert(released.normals == nullptr && released.colors == nullptr);
    assert(released.tangents == nullptr && released.binormals == nullptr);
    assert(released.vboByteSize == 0 && Engine::vboSize == 192);

    Mesh changedDuringCall;
    unsigned char alternatePos[36] = {};
    changedDuringCall.vertexFormat = 3;
    changedDuringCall.vertexCount = 3;
    changedDuringCall.indexCount = 3;
    changedDuringCall.positions = pos;
    changedDuringCall.texCoords = tex;
    changedDuringCall.indices = idx;
    Engine::KeepRawMeshData = 1;
    positionSlotToChange = &changedDuringCall.positions;
    replacementPosition = alternatePos;
    countSlotToChange = &changedDuringCall.vertexCount;
    assert(MeshConvertToVBOIntern(&changedDuringCall) == 1);
    assert(uploads[uploads.size() - 3].size == 36);
    assert(uploads[uploads.size() - 3].data == pos);
    assert(uploads[uploads.size() - 2].size == 32);
    assert(uploads[uploads.size() - 1].size == 6);
    assert(changedDuringCall.positions == alternatePos && changedDuringCall.vertexCount == 4);
    assert(changedDuringCall.vboByteSize == 38 && Engine::vboSize == 230);
    std::puts("MeshConvertToVBOIntern GL-fixture smoke: OK");
}
