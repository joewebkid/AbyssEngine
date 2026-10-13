#include "engine/core/Array.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>

using uint = uint32_t;
using longlong = int64_t;
void glDeleteBuffers(int count, const unsigned int *);

namespace AbyssEngine {
namespace AEMath {
struct Vector { float x = 0, y = 0, z = 0; };
struct Matrix { float m[15] = {}; };
struct BSphere { Vector center; float radius, radiusSq; };
}
struct Quaternion { float x = 0, y = 0, z = 0, w = 1; };
struct KeyFrame {
    static int destroyed;
    ~KeyFrame() { ++destroyed; }
};
int KeyFrame::destroyed = 0;
struct Engine { static bool enableShader; };
bool Engine::enableShader = false;
struct Transform;
struct Mesh {
    uint8_t vertexFormat;
    uint16_t vertexCount;
    void *positions, *texCoords, *colors, *normals, *tangents, *binormals;
    uint32_t materialId;
    float shaderAnimValue0;
    uint16_t indexCount, field_0x2a;
    void *indices, *material;
    Transform *animation;
    uint8_t shared;
    float boundsCenterX, boundsCenterY, boundsCenterZ, boundsRadius, boundsRadiusSq;
    float pivotX, pivotY, pivotZ;
    uint8_t uploaded;
    unsigned int positionVBO, indexVBO, texCoordVBO, normalVBO;
    unsigned int tangentVBO, binormalVBO, colorVBO;
    int vboByteSize;
    uint32_t enhancedData;
    uint8_t vboEligible, hasAnimation;

    Mesh() { std::memset(this, 0, sizeof(*this)); }
    explicit Mesh(Mesh *src);
};
int convertCalls = 0;
int MeshConvertToVBO(Mesh *) { ++convertCalls; return 1; }

struct Transform {
    AEMath::Matrix worldMatrix;
    Array<Mesh *> meshes;
    int color;
    Array<Transform *> children;
    int renderMode;
    AEMath::Matrix rotationMatrix, localMatrix;
    AEMath::Vector boundingCenter;
    float boundingRadius, boundingRadius2;
    int animationStart;
    bool visible, animating;
    float animationSpeed;
    int field_0xf4;
    longlong animationLength, rangeStart, rangeEnd, currentTime;
    int currentKeyFrameIndex;
    Array<KeyFrame *> keyFrames;
    Quaternion rotation;
    AEMath::Vector translation, scale;
    Quaternion localRotation;
    AEMath::Vector localTranslation, localScale;
    float alpha;
    bool vfcEnabled, keyFramesShared;

    AEMath::BSphere &bounds() { return *reinterpret_cast<AEMath::BSphere *>(&boundingCenter); }
    Transform();
    explicit Transform(Transform *other);
    ~Transform();
};

void MeshReleaseIntern(Engine *, Mesh **);
void MeshRelease(Engine *, Mesh **);
void TransformRelease(Engine *, Transform **);
// PRODUCTION_BODIES
}

int deletedBuffers = 0;
void glDeleteBuffers(int count, const unsigned int *) { deletedBuffers += count; }

int main() {
    using namespace AbyssEngine;
    int rawPositions[3] = {11, 22, 33};
    Engine engine;
    {
        Transform owner;
        owner.animationStart = 7;
        owner.animationLength = 31;
        ::operator delete[](owner.keyFrames.data_);
        owner.keyFrames.data_ = static_cast<KeyFrame **>(::operator new[](2 * sizeof(KeyFrame *)));
        owner.keyFrames.capacity_ = 2;
        owner.keyFrames.data_[0] = new KeyFrame;
        owner.keyFrames.data_[1] = nullptr;
        owner.keyFrames.size_ = 1;
        owner.meshes.data_[0] = new Mesh;
        owner.meshes[0]->positions = rawPositions;
        owner.meshes[0]->vertexCount = 3;
        owner.meshes[0]->boundsCenterX = 10;
        owner.meshes[0]->pivotZ = 13;
        owner.meshes[0]->positionVBO = 101;
        owner.meshes[0]->indexVBO = 102;
        owner.meshes[0]->texCoordVBO = 103;
        owner.meshes[0]->normalVBO = 104;
        owner.meshes[0]->colorVBO = 105;
        owner.meshes[0]->vboEligible = 1;
        owner.meshes[0]->uploaded = 1;
        owner.meshes[0]->animation = new Transform;
        owner.meshes[0]->animation->animationStart = 19;
        owner.meshes.size_ = 1;
        Transform *child = new Transform;
        child->keyFrames.data_[0] = new KeyFrame;
        child->keyFrames.size_ = 1;
        child->meshes.data_[0] = new Mesh;
        child->meshes[0]->materialId = 20;
        child->meshes.size_ = 1;
        owner.children.data_[0] = child;
        owner.children.size_ = 1;

        {
            Transform clone(&owner);
            assert(clone.color == -1 && clone.animationStart == 7);
            assert(clone.animationLength == 31 && clone.keyFramesShared);
            assert(clone.keyFrames.data_ != owner.keyFrames.data_);
            assert(clone.keyFrames[0] == owner.keyFrames[0]);
            assert(clone.keyFrames.capacity_ == clone.keyFrames.size_);
            assert(clone.meshes[0] != owner.meshes[0]);
            assert(clone.meshes[0]->positions == rawPositions);
            assert(clone.meshes[0]->vertexCount == 3);
            assert(clone.meshes[0]->boundsCenterX == 10 && clone.meshes[0]->pivotZ == 13);
            assert(clone.meshes[0]->shared == 1 && clone.meshes[0]->uploaded == 1);
            assert(clone.meshes[0]->positionVBO == 101 && clone.meshes[0]->indexVBO == 102);
            assert(clone.meshes[0]->texCoordVBO == 103 && clone.meshes[0]->normalVBO == 104);
            assert(clone.meshes[0]->colorVBO == 105 && clone.meshes[0]->vboEligible == 1);
            assert(clone.meshes[0]->animation != owner.meshes[0]->animation);
            assert(clone.meshes[0]->animation->animationStart == 19);
            assert(clone.children[0] != child && clone.children[0]->keyFramesShared);
            assert(clone.children[0]->keyFrames[0] == child->keyFrames[0]);
            assert(clone.children[0]->meshes[0] != child->meshes[0]);
            assert(clone.children[0]->meshes[0]->materialId == 0);

            Transform *cloneSlot = &clone;
            TransformRelease(&engine, &cloneSlot);
            assert(clone.meshes[0] == nullptr && clone.children[0]->meshes[0] == nullptr);
            assert(clone.children[0] != nullptr);
            assert(rawPositions[0] == 11 && deletedBuffers == 0);
            delete clone.children[0];
        }
        assert(convertCalls == 1);
        assert(KeyFrame::destroyed == 0);
        delete child->meshes[0];
        delete child;
        delete owner.meshes[0]->animation;
        delete owner.meshes[0];
    }
    assert(KeyFrame::destroyed == 2);
    {
        Mesh source;
        int tangents[3] = {1, 2, 3};
        int binormals[3] = {4, 5, 6};
        source.tangents = tangents;
        source.binormals = binormals;
        source.tangentVBO = 201;
        source.binormalVBO = 202;
        Engine::enableShader = true;
        Mesh *clone = new Mesh(&source);
        assert(clone->shared == 1 && clone->tangents == tangents);
        assert(clone->binormals == binormals);
        assert(clone->tangentVBO == 201 && clone->binormalVBO == 202);
        MeshReleaseIntern(&engine, &clone);
        assert(clone == nullptr && deletedBuffers == 0);
    }
    std::puts("Transform/Mesh clone and recursive release smoke: OK");
}
