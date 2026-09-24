#include "engine/render/Mesh.h"
#include "engine/core/AbyssEngine.h"
#include "engine/math/AEMath.h"
#include "engine/math/BSphere.h"
#include "engine/math/Transform.h"
#include "engine/file/AEFile.h"
#include <cstring>

namespace AbyssEngine {
    int MeshConvertToVBO(Mesh * mesh);

    Mesh::Mesh(Mesh *src) {
        Mesh * self = this;

        self->boundsCenterX = 0.0f;
        self->boundsCenterY = 0.0f;
        self->boundsCenterZ = 0.0f;
        self->boundsRadius = 0.0f;
        self->boundsRadiusSq = 1.0f;
        self->pivotX = 0.0f;
        self->pivotY = 0.0f;
        self->pivotZ = 0.0f;

        if (src == 0)
            return;

        if (src->vboEligible != 0)
            MeshConvertToVBO(src);

        *reinterpret_cast<AEMath::BSphere *>(&self->boundsCenterX) =
            *reinterpret_cast<const AEMath::BSphere *>(&src->boundsCenterX);

        unsigned char vertexFormat = src->vertexFormat;
        self->vboByteSize = 0;
        self->vertexFormat = vertexFormat;
        self->materialId = 0;
        self->shaderAnimValue0 = 0;
        self->vertexCount = src->vertexCount;
        memcpy(&self->positions, &src->positions, sizeof(self->positions) * 4);
        if (Engine::enableShader) {
            self->tangents = src->tangents;
            self->binormals = src->binormals;
        }
        self->indexCount = src->indexCount;
        self->field_0x2a = src->field_0x2a;
        self->indices = src->indices;
        self->material = src->material;

        Transform *srcAnim = src->animation;
        if (srcAnim == nullptr) {
            self->animation = nullptr;
        } else {
            self->animation = new Transform(srcAnim);
        }

        *reinterpret_cast<AEMath::Vector *>(&self->pivotX) =
            *reinterpret_cast<const AEMath::Vector *>(&src->pivotX);

        unsigned char hasAnimation = src->hasAnimation;
        self->shared = 1;
        self->hasAnimation = hasAnimation;
        self->uploaded = src->uploaded;
        memcpy(&self->positionVBO, &src->positionVBO, sizeof(self->positionVBO) * 4);
        self->colorVBO = src->colorVBO;
        self->vboEligible = src->vboEligible;
        if (Engine::enableShader) {
            self->tangentVBO = src->tangentVBO;
            self->binormalVBO = src->binormalVBO;
        }
        self->enhancedData = src->enhancedData;
    }

    namespace {
        inline void updateTimeBetweenFrames(float value) {
            if (value > 0.0f) {
                if (timeBetweenFrames > value)
                    timeBetweenFrames = value;
            }
        }
    }

    int Mesh::ReadEnhancedDataFromFile(unsigned int file, unsigned int flags) {
        Mesh * self = this;
        unsigned char format = (unsigned char) flags;
        Transform *anim = new Transform();

        if (AEFile::Read(4, &self->boundsCenterX, file) == 0) goto fail;
        if (AEFile::Read(4, &self->boundsCenterY, file) == 0) goto fail;
        if (AEFile::Read(4, &self->boundsCenterZ, file) == 0) goto fail;
        if (AEFile::Read(4, &self->boundsRadius, file) == 0) goto fail;
        {
            float y = self->boundsCenterY;
            self->boundsCenterY = self->boundsCenterZ;
            self->boundsCenterZ = -y;
        }

        {
            short type = -1;

            if (AEFile::Read(2, &type, file) == 0) goto fail;
            if (type == 1) {
                unsigned short count;
                if (AEFile::Read(2, &count, file) == 0) goto fail;
                for (unsigned int i = 0; i < count; ++i) {
                    float key;
                    if (AEFile::Read(4, &key, file) == 0) goto fail;
                    updateTimeBetweenFrames(key);
                    float vec[3];
                    if (AEFile::Read(0xc, vec, file) == 0) goto fail;
                    anim->InsertKeyFrame(vec, 7, (int) key);
                }
            } else if (type == 0) {
                for (unsigned int axis = 0; axis < 3; ++axis) {
                    unsigned short count;
                    if (AEFile::Read(2, &count, file) == 0) goto fail;
                    for (unsigned int j = 0; j < count; ++j) {
                        float value;
                        if (AEFile::Read(4, &value, file) == 0) goto fail;
                        updateTimeBetweenFrames(value);
                        float key;
                        if (AEFile::Read(4, &key, file) == 0) goto fail;
                        if (axis == 2) {
                            anim->InsertKeyFrame(&key, 2, (int) value);
                        } else if (axis == 1) {
                            key = -key;
                            anim->InsertKeyFrame(&key, 4, (int) value);
                        } else {
                            anim->InsertKeyFrame(&key, 1, (int) value);
                        }
                    }
                }
            }

            if (AEFile::Read(2, &type, file) == 0) goto fail;
            if (type == 1) {
                unsigned short count;
                if (AEFile::Read(2, &count, file) == 0) goto fail;
                for (unsigned int i = 0; i < count; ++i) {
                    float key;
                    if (AEFile::Read(4, &key, file) == 0) goto fail;
                    updateTimeBetweenFrames(key);
                    float vec[3];
                    if (AEFile::Read(0xc, vec, file) == 0) goto fail;
                    anim->InsertKeyFrame(vec, 0x1c0, (int) key);
                }
            } else if (type == 0) {
                for (unsigned int axis = 0; axis < 3; ++axis) {
                    unsigned short count;
                    if (AEFile::Read(2, &count, file) == 0) goto fail;
                    for (unsigned int j = 0; j < count; ++j) {
                        float value;
                        if (AEFile::Read(4, &value, file) == 0) goto fail;
                        updateTimeBetweenFrames(value);
                        float key;
                        if (AEFile::Read(4, &key, file) == 0) goto fail;
                        if (axis == 2) {
                            anim->InsertKeyFrame(&key, 0x100, (int) value);
                        } else if (axis == 1) {
                            anim->InsertKeyFrame(&key, 0x80, (int) value);
                        } else {
                            anim->InsertKeyFrame(&key, 0x40, (int) value);
                        }
                    }
                }
            }

            if (AEFile::Read(2, &type, file) == 0) goto fail;
            if (type == 1) {
                unsigned short count;
                if (AEFile::Read(2, &count, file) == 0) goto fail;
                for (unsigned int i = 0; i < count; ++i) {
                    float key;
                    if (AEFile::Read(4, &key, file) == 0) goto fail;
                    updateTimeBetweenFrames(key);
                    float vec[3];
                    if (AEFile::Read(0xc, vec, file) == 0) goto fail;
                    anim->InsertKeyFrame(vec, 0x38, (int) key);
                }
            } else if (type == 0) {
                for (unsigned int axis = 0; axis < 3; ++axis) {
                    unsigned short count;
                    if (AEFile::Read(2, &count, file) == 0) goto fail;
                    for (unsigned int j = 0; j < count; ++j) {
                        float value;
                        if (AEFile::Read(4, &value, file) == 0) goto fail;
                        updateTimeBetweenFrames(value);
                        float key;
                        if (AEFile::Read(4, &key, file) == 0) goto fail;
                        if (axis == 2) {
                            anim->InsertKeyFrame(&key, 0x20, (int) value);
                        } else if (axis == 1) {
                            anim->InsertKeyFrame(&key, 0x10, (int) value);
                        } else {
                            anim->InsertKeyFrame(&key, 8, (int) value);
                        }
                    }
                }
            }

            if ((format & 0x18) != 0) {
                if (AEFile::Read(2, &type, file) == 0) goto fail;
                if (type == 2) {
                    unsigned short count;
                    if (AEFile::Read(2, &count, file) == 0) goto fail;
                    for (unsigned int i = 0; i < count; ++i) {
                        float value;
                        if (AEFile::Read(4, &value, file) == 0) goto fail;
                        updateTimeBetweenFrames(value);
                        float key;
                        if (AEFile::Read(4, &key, file) == 0) goto fail;
                        anim->InsertKeyFrame(&key, 0x200, (int) value);
                    }
                }
            }

            if ((format & 0x10) != 0) {
                unsigned short present;
                if (AEFile::Read(2, &present, file) == 0) goto fail;
                if (present != 0) {
                    static const unsigned int kChannels[7] = {
                        0x400, 0x800, 0x2000, 0x4000, 0, 0, 0x40000
                    };
                    for (unsigned int c = 0; c < 7; ++c) {
                        unsigned short count;
                        if (AEFile::Read(2, &count, file) == 0) goto fail;
                        for (int j = 0; j < (short) count; ++j) {
                            float value;
                            if (AEFile::Read(4, &value, file) == 0) goto fail;
                            updateTimeBetweenFrames(value);
                            float key;
                            if (AEFile::Read(4, &key, file) == 0) goto fail;
                            key = key / 100.0f;
                            if (c == 6)
                                key = (key * 6.2832f) / 360.0f;
                            anim->InsertKeyFrame(&key, kChannels[c], (int) value);
                            self->hasAnimation = 1;
                        }
                    }
                }
            }
        }

        if (anim->keyFrames.size() < 1) {
            if (anim) {
                anim->~Transform();
                ::operator delete((void *) anim);
            }
        } else {
            self->animation = anim;
            float rate = timeBetweenFrames;
            anim->animationStart = (int) rate;
            // Android uses a fixed upper bound and lets Transform clamp it to animationLength.
            anim->SetAnimationRangeInTime((long long) rate, 10000000LL);
        }
        return 1;

    fail:
        if (anim) {
            anim->~Transform();
            ::operator delete((void *) anim);
        }
        return -1;
    }
}
