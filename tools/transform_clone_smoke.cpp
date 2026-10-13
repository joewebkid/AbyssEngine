#include "engine/core/Array.h"
#include <cassert>
#include <cstdint>
#include <cstdio>

using uint = uint32_t;
using longlong = int64_t;

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
struct Mesh {
    static int copies;
    int marker;
    explicit Mesh(int value) : marker(value) {}
    explicit Mesh(Mesh *source) : marker(source->marker) { ++copies; }
};
int Mesh::copies = 0;

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

// PRODUCTION_BODIES
}

int main() {
    using namespace AbyssEngine;
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
        owner.meshes.data_[0] = new Mesh(10);
        owner.meshes.size_ = 1;
        Transform *child = new Transform;
        child->keyFrames.data_[0] = new KeyFrame;
        child->keyFrames.size_ = 1;
        child->meshes.data_[0] = new Mesh(20);
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
            assert(clone.meshes[0] != owner.meshes[0] && clone.meshes[0]->marker == 10);
            assert(clone.children[0] != child && clone.children[0]->keyFramesShared);
            assert(clone.children[0]->keyFrames[0] == child->keyFrames[0]);
            assert(clone.children[0]->meshes[0] != child->meshes[0]);
            assert(Mesh::copies == 2);

            delete clone.children[0]->meshes[0];
            delete clone.children[0];
            delete clone.meshes[0];
        }
        assert(KeyFrame::destroyed == 0);
        delete child->meshes[0];
        delete child;
        delete owner.meshes[0];
    }
    assert(KeyFrame::destroyed == 2);
    std::puts("Transform clone ownership smoke: OK");
}
