#include "engine/render/Mesh.h"
#include "engine/math/AEMath.h"
#include "engine/math/Transform.h"
#include "engine/file/AEFile.h"

namespace AbyssEngine {
    int MeshConvertToVBO(Mesh * mesh);

    static unsigned char *g_hasNormalsFlag = nullptr;

    Mesh::Mesh(Mesh *src) {
        Mesh * self = this;

        self->boundsCenterX = 0.0f;
        self->boundsCenterY = 0.0f;
        self->boundsCenterZ = 1.0f;
        self->boundsRadius = 0.0f;
        self->boundsRadiusSq = 0.0f;
        self->pivotX = 0.0f;
        self->pivotY = 1.0f;
        self->pivotZ = 0.0f;

        if (src == 0)
            return;

        if (src->vboEligible != 0)
            MeshConvertToVBO(src);

        self->boundsCenterX = src->boundsCenterX;
        self->boundsCenterY = src->boundsCenterY;
        self->boundsCenterZ = src->boundsCenterZ;
        self->boundsRadius = src->boundsRadius;
        self->boundsRadiusSq = src->boundsRadiusSq;

        const bool hasTangents = (*g_hasNormalsFlag != 0);

        self->vboByteSize = 0;
        self->vertexFormat = src->vertexFormat;
        self->materialId = 0;
        self->shaderAnimValue0 = 0;
        self->vertexCount = src->vertexCount;
        self->positions = src->positions;
        self->texCoords = src->texCoords;
        self->colors = src->colors;
        self->normals = src->normals;
        if (hasTangents) {
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

        self->pivotX = src->pivotX;
        self->pivotY = src->pivotY;
        self->pivotZ = src->pivotZ;

        self->shared = 1;
        self->hasAnimation = src->hasAnimation;
        self->uploaded = src->uploaded;
        self->positionVBO = src->positionVBO;
        self->indexVBO = src->indexVBO;
        self->texCoordVBO = src->texCoordVBO;
        self->normalVBO = src->normalVBO;
        self->colorVBO = src->colorVBO;
        self->vboEligible = src->vboEligible;
        if (hasTangents) {
            self->tangentVBO = src->tangentVBO;
            self->binormalVBO = src->binormalVBO;
        }
        self->enhancedData = src->enhancedData;
    }

    static float *g_maxA = nullptr;
    static float *g_maxB = nullptr;
    static float *g_maxC = nullptr;
    static float *g_maxD = nullptr;
    static float *g_maxE = nullptr;
    static float *g_maxF = nullptr;
    static float *g_maxG = nullptr;
    static float *g_maxH = nullptr;
    static float *g_animRate = nullptr;
    static float g_uvDiv = 0.0f, g_uvMulA = 0.0f, g_uvDivB = 0.0f;

    namespace {
        inline void bumpMax(float *slot, float v) {
            if (v > 0.0f && *slot < v)
                *slot = v;
        }
    }

    int Mesh::ReadEnhancedDataFromFile(unsigned int file, unsigned int flags) {
        Mesh * self = this;
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
            unsigned short type;

            if (AEFile::Read(2, &type, file) == 0) goto fail;
            if (type == 1) {
                unsigned short count;
                if (AEFile::Read(2, &count, file) == 0) goto fail;
                for (unsigned int i = 0; i < count; ++i) {
                    float key;
                    if (AEFile::Read(4, &key, file) == 0) goto fail;
                    bumpMax(g_maxB, key);
                    unsigned char vec[12];
                    if (AEFile::Read(0xc, vec, file) == 0) goto fail;
                    anim->InsertKeyFrame(7, key);
                }
            } else if (type == 0) {
                for (unsigned int axis = 0; axis < 3; ++axis) {
                    unsigned short count;
                    if (AEFile::Read(2, &count, file) == 0) goto fail;
                    for (unsigned int j = 0; j < count; ++j) {
                        float value;
                        if (AEFile::Read(4, &value, file) == 0) goto fail;
                        bumpMax(g_maxA, value);
                        float key;
                        if (AEFile::Read(4, &key, file) == 0) goto fail;
                        if (axis == 2) {
                            anim->InsertKeyFrame(2, value);
                        } else if (axis == 1) {
                            value = -value;
                            anim->InsertKeyFrame(4, value);
                        } else {
                            anim->InsertKeyFrame(1, value);
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
                    bumpMax(g_maxD, key);
                    unsigned char vec[12];
                    if (AEFile::Read(0xc, vec, file) == 0) goto fail;
                    anim->InsertKeyFrame(0x1c0, key);
                }
            } else if (type == 0) {
                for (unsigned int axis = 0; axis < 3; ++axis) {
                    unsigned short count;
                    if (AEFile::Read(2, &count, file) == 0) goto fail;
                    for (unsigned int j = 0; j < count; ++j) {
                        float value;
                        if (AEFile::Read(4, &value, file) == 0) goto fail;
                        bumpMax(g_maxC, value);
                        float key;
                        if (AEFile::Read(4, &key, file) == 0) goto fail;
                        if (axis == 2) {
                            anim->InsertKeyFrame(0x100, value);
                        } else if (axis == 1) {
                            anim->InsertKeyFrame(0x80, value);
                        } else {
                            anim->InsertKeyFrame(0x40, value);
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
                    bumpMax(g_maxF, key);
                    unsigned char vec[12];
                    if (AEFile::Read(0xc, vec, file) == 0) goto fail;
                    anim->InsertKeyFrame(0x38, key);
                }
            } else if (type == 0) {
                for (unsigned int axis = 0; axis < 3; ++axis) {
                    unsigned short count;
                    if (AEFile::Read(2, &count, file) == 0) goto fail;
                    for (unsigned int j = 0; j < count; ++j) {
                        float value;
                        if (AEFile::Read(4, &value, file) == 0) goto fail;
                        bumpMax(g_maxE, value);
                        float key;
                        if (AEFile::Read(4, &key, file) == 0) goto fail;
                        if (axis == 2) {
                            anim->InsertKeyFrame(0x20, value);
                        } else if (axis == 1) {
                            anim->InsertKeyFrame(0x10, value);
                        } else {
                            anim->InsertKeyFrame(8, value);
                        }
                    }
                }
            }

            if ((flags & 0x18) != 0) {
                if (AEFile::Read(2, &type, file) == 0) goto fail;
                if (type == 2) {
                    unsigned short count;
                    if (AEFile::Read(2, &count, file) == 0) goto fail;
                    for (unsigned int i = 0; i < count; ++i) {
                        float value;
                        if (AEFile::Read(4, &value, file) == 0) goto fail;
                        bumpMax(g_maxG, value);
                        float key;
                        if (AEFile::Read(4, &key, file) == 0) goto fail;
                        anim->InsertKeyFrame(0x200, value);
                    }
                }
            }

            if ((flags & 0x10) != 0) {
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
                            bumpMax(g_maxH, value);
                            float key;
                            if (AEFile::Read(4, &key, file) == 0) goto fail;
                            key = key / g_uvDiv;
                            if (c == 6)
                                key = (key * g_uvMulA) / g_uvDivB;
                            anim->InsertKeyFrame(kChannels[c], value);
                            self->hasAnimation = 1;
                        }
                    }
                }
            }
        }

        if (anim->keyFrames.size() < 1) {
            delete anim;
        } else {
            self->animation = anim;
            float rate = *g_animRate;
            anim->animationStart = (int) rate;
            anim->SetAnimationRangeInTime((long long) rate, (long long) rate);
        }
        return 1;

    fail:
        delete anim;
        return -1;
    }
}
