#include "engine/core/AbyssEngine.h"
#include "engine/file/AEFile.h"
#include "engine/core/GameText.h"
#include "engine/file/FileInterfaceAndroid.h"
#include "engine/math/AEMath.h"
#include "engine/math/Transform.h"
#include "game/core/String.h"
#include "engine/render/Mesh.h"
#include "engine/render/Engine.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/FBOContainer.h"
#include "engine/math/Quaternion.h"
#include <GLES2/gl2.h>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace {

// A single keyframe record (heap-allocated; the Curve::entries array holds
// one pointer per keyframe). Layout recovered from the original ARM
// disassembly of CurveGetValue / CurveRelease.
struct CurveKeyframe {    // size >= 0x1c
    uint8_t  tag;         // 0x00: 1=step, 2=linear, 3=hermite
    uint8_t  pad0x1[7];   // 0x01
    uint32_t timeLo;      // 0x08: keyframe time, low 32 bits
    uint32_t timeHi;      // 0x0c: keyframe time, high 32 bits
    int32_t  value;       // 0x10
    int32_t  c0;          // 0x14: hermite tangent in
    int32_t  c1;          // 0x18: hermite tangent out
};

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(offsetof(CurveKeyframe, tag) == 0x0, "CurveKeyframe.tag");
static_assert(offsetof(CurveKeyframe, timeLo) == 0x8, "CurveKeyframe.timeLo");
static_assert(offsetof(CurveKeyframe, timeHi) == 0xc, "CurveKeyframe.timeHi");
static_assert(offsetof(CurveKeyframe, value) == 0x10, "CurveKeyframe.value");
static_assert(offsetof(CurveKeyframe, c0) == 0x14, "CurveKeyframe.c0");
static_assert(offsetof(CurveKeyframe, c1) == 0x18, "CurveKeyframe.c1");
#endif

} // anonymous namespace

namespace AbyssEngine {
    // Defined in EngineFlags.cpp. These must have external linkage so the
    // flag-gated engine functions cannot be folded to local null-pointer stubs.
    extern char *g_Camera_frustumEnabledFlag;
    extern char *g_Engine_fboEnabledFlag;
    extern char *g_Engine_shaderModeFlag;
    extern char *g_GameText_arabicEnabledFlag;
    extern char *g_MeshIntersect_flipVFlag;
    extern char *g_SpriteSystem_tangentFlag;
    extern char *g_SpriteSystem_uvFlipFlag;
    extern void (*g_MeshRelease_freeFn)(AbyssEngine::Engine *, AbyssEngine::Mesh **);

    // ImageFont, SpriteSystem, Curve, Image, AELoadedTexture are defined in
    // AbyssEngine.h (layouts recovered from the original disassembly).

    int currentUsedShaderIndex;

    // Namespace-scope globals present in the original binary (defined for symbol parity).
    float currentFps;
    float debugTouch[4];
    int fpsCounter;
    int SwapCounter;
    unsigned char firstRender;
    int loadTexture;
    float currentLODBias;
    int CubeMapSetIndex;
    int debugModusIndex;
    unsigned char firstRenderGlow;
    unsigned char performanceTest;
    long long performanceTime;
    float timeBetweenFrames;
    unsigned char BiasErrorOutputFlag;
    unsigned char performanceTestEnded;
    float orientationChangeTimer;
    long long performanceFrameCounter;
    long long performanceModulSwitches;
    long long performanceOverallModulSwitches;
    int lauf;
    float i_fov;
    float i_zFar;
    float i_zNear;
    float fpsTimer;
    int loadMesh;
    long long triDrawn;
    unsigned char ShaderSet;

    Engine *AE_getInitGLThis();
    int AE_getInitGLWidth();
    int AE_getInitGLHeight();
    void AE_AEMath_matMul(AEMath::Matrix *out, const AEMath::Matrix *rhs);
    void AE_SpriteSystem_pushMatrix(unsigned int m0, unsigned int m1, unsigned int m2, unsigned int m3,
                                    unsigned int m4, unsigned int m5, unsigned int m6, unsigned int m7,
                                    unsigned int m8, unsigned int m9, unsigned int m10, unsigned int m11,
                                    unsigned int m12, unsigned int m13, unsigned int m14, int dst);
}

namespace {
    struct EngineArrayHeader {
        uint32_t count;
        void *data;
        uint32_t capacity;
    };

    template<class T>
    inline void ArrayAddCachedRaw(T item, void *arrayHeader) {
        EngineArrayHeader *a = (EngineArrayHeader *) arrayHeader;
        if (a->count >= a->capacity) {
            uint32_t oldCap = a->capacity;
            a->data = realloc(a->data, (size_t) oldCap * 2 * sizeof(T));
            memset((T *) a->data + oldCap, 0, oldCap * sizeof(T));
            a->capacity = oldCap * 2;
        }
        ((T *) a->data)[a->count] = item;
        a->count = a->count + 1;
    }
}

unsigned int AELabelObject(unsigned int glIdentifier, unsigned int name, const char *label) {
    (void) name;
    (void) label;
    return glIdentifier;
}

namespace AbyssEngine {
    void ImageFontSetYOffset(ImageFont *font, short yOffset) {
        if (font != 0)
            font->yOffset = yOffset;
    }
}

namespace AbyssEngine {
    int CameraIsPointinViewFrustum(const Vector &point, Matrix *extra, Camera *cam) {
        if (*g_Camera_frustumEnabledFlag == 0)
            return 1;

        Matrix local;
        Matrix transformed;
        Vector pos, dir, axis;
        Vector camPoint = {0.0f, 0.0f, 0.0f};

        Matrix *srcMatrix = &cam->projection;
        Matrix *dstMatrix;
        if (extra == 0) {
            dstMatrix = &local;
        } else {
            memcpy(&local, srcMatrix, 0x3c);
            local *= *extra;
            dstMatrix = &transformed;
            srcMatrix = &local;
        }

        *(Vector *) dstMatrix = AEMath::MatrixInverseTransformVector(*srcMatrix, *(Vector *) dstMatrix);
        camPoint = *(Vector *) dstMatrix;

        pos = AEMath::MatrixGetPosition(*dstMatrix);
        pos -= point;
        dir = AEMath::MatrixGetDir(*dstMatrix);
        axis -= dir;
        Vector normAxis = AEMath::VectorNormalize(axis);
        float fwd = AEMath::VectorDot(pos, normAxis);

        if (fwd > cam->position[2] || fwd < cam->position[1])
            return 0;

        axis = AEMath::MatrixGetUp(*dstMatrix);
        normAxis = AEMath::VectorNormalize(axis);
        float up = AEMath::VectorDot(pos, normAxis);

        float hLimit = fwd * cam->frustumTanHalfFov;
        if (up > hLimit || up < -hLimit)
            return 0;

        axis = AEMath::MatrixGetRight(*dstMatrix);
        normAxis = AEMath::VectorNormalize(axis);
        float right = AEMath::VectorDot(pos, normAxis);

        float vLimit = hLimit * cam->frustumAspect;
        return (right <= vLimit && right >= -vLimit) ? 1 : 0;
    }
}

namespace AbyssEngine {
    Quaternion operator*(const Quaternion &a, float s) {
        float yv = a.y * s;
        float wv = a.w * s;
        return Quaternion(wv, 0.0f, yv, 0.0f);
    }
}

namespace AbyssEngine {
    String operator+(const String &a, const String &b) {
        String result(a);
        result += b;
        return result;
    }
}

namespace AbyssEngine {
    void SpriteSystemSetAllUv(float u0, float v0, float u1, float v1, SpriteSystem *sys) {
        if (sys == 0)
            return;

        unsigned short count = sys->count;
        float *uvBase = (float *) sys->mesh->texCoords;

        for (unsigned short idx = 0; idx < (count << 3); idx += 8) {
            float *p = uvBase + idx;
            p[0] = u0;
            p[1] = 1.0f - v0;
            p[2] = u1;
            p[3] = 1.0f - v0;
            p[4] = u1;
            p[5] = 1.0f - v1;
            p[6] = u0;
            p[7] = 1.0f - v1;
        }
    }
}

namespace AbyssEngine {
    void getAppVersion() {
        Engine *self = AE_getInitGLThis();
        int width = AE_getInitGLWidth();
        int height = AE_getInitGLHeight();

        self->fboContainer = 0;
        self->framebufferWidth = width;
        self->framebufferHeight = height;
        self->viewportWidth = width;
        self->viewportHeight = height;

        FileInterfaceAndroid *fileIface = new FileInterfaceAndroid();
        self->fileInterface = fileIface;
        AEFile::SetInterface((FileInterface *) fileIface);

        self->lastGlError = 0;
        self->vibrationSupported = 0;
        self->hasVibration = false;

        char *shaderFlag = g_Engine_shaderModeFlag;
        self->field_0x40c = 0;
        self->ResetLightParam();
        glViewport(0, 0, self->viewportHeight, self->viewportWidth);

        if (*shaderFlag == 0) {
            glEnable(0x803a);
            glDisable(0xb50);
            glLineWidth(1.0f);
        } else {
            self->ShaderInit();
        }

        float zero3[3] = {0, 0, 0};
        self->field_0x468 = *(const Vector *) zero3;
        self->lightDirty[0] = 0;
        self->field_0x474 = *(const Vector *) zero3;
        self->lightDirty[1] = 0;

        glEnable(0xb71);
        self->GlEnable(0xde1, true);
        glDisable(0xbe2);
        glCullFace(0x405);
        glEnable(0xb44);
        self->AfterGLInit();
        ((PaintCanvas *) *(void **) self->paintCanvas)->Initialize(false);

        self->maxTextureSize = 0;
        glGetIntegerv(0xd33, reinterpret_cast<GLint *>(&self->maxTextureSize));

        if (*shaderFlag != 0 && *g_Engine_fboEnabledFlag != 0) {
            FBOContainer *fbo = new FBOContainer(self, String(""));
            self->fboContainer = fbo;
            fbo->Create((int) self->framebufferWidth, (int) self->framebufferHeight, true, false);
        }
    }
}

namespace AbyssEngine {
    void SpriteSystemSetUv(unsigned short idx, float a, float b, float c, float d, SpriteSystem *sys) {
        unsigned int count = 0;
        if (sys != 0)
            count = sys->count;
        if (sys == 0 || idx >= count)
            return;

        char flag = *g_SpriteSystem_uvFlipFlag;
        float *p = (float *) sys->mesh->texCoords + (unsigned int) idx * 8;

        if (flag == 0) {
            c = 1.0f - c;
            d = 1.0f - d;
        }
        p[0] = a;
        p[1] = c;
        p[2] = b;
        p[3] = c;
        p[4] = b;
        p[5] = d;
        p[6] = a;
        p[7] = d;
    }
}

namespace AbyssEngine {
    int ImageFontGetYOffset(ImageFont *font) {
        short v = (font == 0) ? (short) 0 : font->yOffset;
        return (int) v;
    }
}

namespace AbyssEngine {
    Quaternion operator-(const Quaternion &a, const Quaternion &b) {
        float yv = a.y - b.y;
        float wv = a.w - b.w;
        return Quaternion(wv, 0.0f, yv, 0.0f);
    }
}

namespace AbyssEngine {
    int MeshCreate(Engine *engine, unsigned short vertexCount, unsigned short triCount,
                   signed char vertexFormat, Mesh **out);

    void MeshRelease(Engine * engine, Mesh * *slot);

    int ImageCreateRegionFromFile(Engine *engine, const char *path, unsigned short index, Image2D *region) {
        if (engine == 0 || path == 0)
            return -4;

        unsigned int handle = 0;
        if (AEFile::OpenRead(path, (uint32_t *) (&handle)) == 0)
            return -1;

        char header[8] = "*******";
        if (AEFile::Read((uint32_t)(8), header, handle) == 0)
            return -1;
        static const char aeiMagic[8] = "AEimage";
        for (unsigned int k = 0; k < 8; ++k)
            if (aeiMagic[k] != header[k])
                return -1;

        unsigned short regionCount;
        if (AEFile::Skip((uint32_t)(1), handle) == 0) return -1;
        if (AEFile::Read((uint32_t)(2), &region->atlasW, handle) == 0) return -1;
        if (AEFile::Read((uint32_t)(2), &region->atlasH, handle) == 0) return -1;
        regionCount = 0;
        if (AEFile::Read((uint32_t)(2), &regionCount, handle) == 0) return -1;
        if (regionCount <= index) return -1;

        if (MeshCreate(engine, 4, 2, 0x13, &region->mesh) != 1)
            return -2;

        for (unsigned short i = 0; i < regionCount; ++i) {
            if (i == index) {
                if (AEFile::Read((uint32_t)(2), &region->offX, handle) == 0) goto fail;
                if (AEFile::Read((uint32_t)(2), &region->offY, handle) == 0) goto fail;
                if (AEFile::Read((uint32_t)(2), &region->sizeX, handle) == 0) goto fail;
                if (AEFile::Read((uint32_t)(2), &region->sizeY, handle) == 0) goto fail;

                Mesh *mesh = region->mesh;
                float *pos = (float *) mesh->positions;

                float width = (float) region->sizeX;
                pos[0] = 0;
                pos[1] = 0;
                pos[2] = 0;
                pos[4] = 0;
                pos[5] = 0;
                pos[3] = width;
                pos[6] = width;
                float height = (float) region->sizeY;
                pos[8] = 0;
                pos[9] = 0;
                pos[11] = 0;
                pos[7] = height;
                pos[10] = height;

                float invAtlasH = (float) (1.0 / (double) region->atlasH);
                float invAtlasW = (float) (1.0 / (double) region->atlasW);
                short signedOffX = (short) region->offX;
                short signedOffY = (short) region->offY;
                float offYs = (float) signedOffY;
                float offXs = (float) signedOffX;
                float offYu = (float) (unsigned short) signedOffY;
                float offXu = (float) (unsigned short) signedOffX;

                float *uv = (float *) mesh->texCoords;
                float u0 = offXu * invAtlasW;
                float v0 = offYu * invAtlasH;
                float v1 = (height + offYs) * invAtlasH;
                float u1 = (width + offXs) * invAtlasW;
                uv[0] = u0;
                uv[1] = v0;
                uv[2] = u1;
                uv[3] = v0;
                uv[4] = u1;
                uv[5] = v1;
                uv[6] = u0;
                uv[7] = v1;

                unsigned int *draw = (unsigned int *) mesh->indices;
                draw[0] = 0x20000;
                draw[1] = 1;
                draw[2] = 0x20003;
            } else {
                if (AEFile::Skip((uint32_t)(8), handle) == 0)
                    goto fail;
            }
        }

        AEFile::Close(handle);
        return 1;

    fail:
        MeshRelease(engine, &region->mesh);
        return -1;
    }
}

namespace AbyssEngine {
    int ImageFontGetWidth(ImageFont *font, const unsigned short *str, unsigned int count) {
        if (font == 0 || str == 0)
            return 0;

        int total = 0;
        for (unsigned short i = 0; i < count; ++i) {
            unsigned short glyphIndex = 0;
            while (glyphIndex < font->glyphCount) {
                unsigned int slot = glyphIndex++;
                unsigned short code = font->codes[slot];
                if (code == str[i]) {
                    int width = (int) ((float *) font->glyphMeshes[slot]->positions)[3];
                    int advance = (int) font->spacing + width;
                    int contribution = advance;
                    if (width == 11)
                        contribution = advance - 2;
                    if (code != 32)
                        contribution = advance;
                    total += contribution;
                    break;
                }
            }
        }
        return total;
    }
}

namespace AbyssEngine {
    struct Vec2 {
        float u, v;
    };

    Vec2 MeshIntersect(float qx, float qz, Mesh *mesh) {
        Vec2 out;
        unsigned int i = 0;

        for (;;) {
            if ((unsigned int) mesh->indexCount <= i) {
                out.u = -1.0f;
                out.v = -1.0f;
                return out;
            }

            float *pos = (float *) mesh->positions;
            unsigned short *idx = (unsigned short *) mesh->indices + i;
            float *a = pos + (unsigned int) idx[0] * 3;
            float *b = pos + (unsigned int) idx[1] * 3;
            float *c = pos + (unsigned int) idx[2] * 3;

            float ax = a[0], az = a[2];
            float bx = b[0], bz = b[2];
            float cx = c[0], cz = c[2];

            float minZ = az, maxZ = bz;
            if (bz < az) {
                minZ = bz;
                maxZ = az;
            }
            float minX = ax, maxX = bx;
            if (bx < ax) {
                minX = bx;
                maxX = ax;
            }
            if (cz > maxZ) maxZ = cz;
            if (cz < minZ) minZ = cz;
            if (cx > maxX) maxX = cx;
            if (cx < minX) minX = cx;

            if (maxZ >= qz && minX <= qx && maxX >= qx && minZ <= qz) {
                float ex = bx - ax, ez = bz - az;
                float len = sqrtf(ez * ez + ex * ex);
                float side = (ex / len) * qz + (-ez / len) * qx -
                             (az * (ex / len) + ax * (-ez / len));
                if (side <= 0.0f) {
                    ex = cx - bx;
                    ez = cz - bz;
                    len = sqrtf(ez * ez + ex * ex);
                    side = (ex / len) * qz + (-ez / len) * qx -
                           (bz * (ex / len) + bx * (-ez / len));
                    if (side <= 0.0f) {
                        ez = az - cz;
                        ex = ax - cx;
                        len = sqrtf(ez * ez + ex * ex);
                        side = (ex / len) * qz + (-ez / len) * qx -
                               (cz * (ex / len) + cx * (-ez / len));
                        if (side <= 0.0f) {
                            float *uv = (float *) mesh->texCoords;
                            unsigned int i0 = idx[0], i1 = idx[1], i2 = idx[2];
                            out.u = (uv[i0 * 2] + uv[i1 * 2] + uv[i2 * 2]) / 3.0f;
                            float vv = (uv[i0 * 2 + 1] + uv[i1 * 2 + 1] + uv[i2 * 2 + 1]) / 3.0f;
                            out.v = vv;
                            if (*g_MeshIntersect_flipVFlag != 0)
                                out.v = 1.0f - vv;
                            return out;
                        }
                    }
                }
            }
            i += 3;
        }
    }
}

// ES 1.x fixed-function client-array entry points (no-op stubs in the binary);
// not declared by <GLES2/gl2.h>.
extern "C" {
void glVertexPointer(int size, unsigned int type, int stride, const void *ptr);

void glTexCoordPointer(int size, unsigned int type, int stride, const void *ptr);

void glNormalPointer(unsigned int type, int stride, const void *ptr);

void glColorPointer(int size, unsigned int type, int stride, const void *ptr);
}

namespace AbyssEngine {
    int MeshDraw(Engine *engine, Mesh *mesh) {
        if (mesh == 0 || (short) mesh->vertexCount == 0 || (mesh->vertexFormat & 1) == 0)
            return -4;

        unsigned char flags = mesh->vertexFormat;
        if (Engine::enableShader == 0 && mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            engine->AEClientState(0x8074, true);
            glVertexPointer(3, 0x1406, 0, 0);
            glBindBuffer(0x8893, mesh->indexVBO);

            if (flags & 2) {
                glBindBuffer(0x8892, mesh->texCoordVBO);
                engine->AEClientState(0x8078, true);
                glTexCoordPointer(2, 0x1406, 0, 0);
            } else {
                engine->AEClientState(0x8078, false);
            }

            if (flags & 4) {
                glBindBuffer(0x8892, mesh->normalVBO);
                engine->AEClientState(0x8075, true);
                glNormalPointer(0x1406, 0, 0);
            } else {
                engine->AEClientState(0x8075, false);
            }

            if (flags & 8) {
                glBindBuffer(0x8892, mesh->colorVBO);
                engine->AEClientState(0x8076, true);
                glColorPointer(4, 0x1406, 0, 0);
            } else {
                engine->AEClientState(0x8076, false);
            }

            glDrawElements(4, (int) mesh->indexCount, 0x1403, 0);

            if (engine->statsEnabled != 0) {
                int tris = (int) ((unsigned) mesh->indexCount / (unsigned) 3);
                if (engine->statsBucketFlag == 0) {
                    engine->drawCallCountA += 1;
                    engine->triangleCountA += tris;
                } else {
                    engine->triangleCountB += tris;
                    engine->drawCallCountB += 1;
                }
            }

            glBindBuffer(0x8892, 0);
            glBindBuffer(0x8893, 0);
        } else {
            engine->RenderMesh(mesh);
        }
        return 1;
    }
}

namespace AbyssEngine {
    void SpriteSystemSetRGBA(unsigned short idx, float r, float g, float b, float a, SpriteSystem *sys) {
        unsigned int count = 0;
        if (sys != 0)
            count = sys->count;
        if (sys == 0 || idx >= count)
            return;

        float *p = (float *) sys->mesh->colors + (unsigned int) idx * 16;
        for (int v = 0; v < 4; ++v) {
            p[v * 4 + 0] = r;
            p[v * 4 + 1] = g;
            p[v * 4 + 2] = b;
            p[v * 4 + 3] = a;
        }
    }
}

namespace AbyssEngine {
    namespace AEMath {
    }

    float CameraSetPerspective(float fov, float aspectNum, float aspectDen, float near, Camera *cam) {
        float ret = fov;
        if (cam != 0) {
            float *f = (float *) cam;
            f[1] = fov;
            f[2] = aspectNum;

            float s = AbyssEngine::AEMath::Sinf(f[0] * 0.5f);
            float c = AbyssEngine::AEMath::Cosf(f[0] * 0.5f);
            float scale = s / c;
            f[0x12] = scale;
            f[0x13] = (aspectDen / near) * scale;
            f[0x14] = aspectDen / near;

            float at = AbyssEngine::AEMath::ATanf((aspectDen / near) * scale);
            float ca = AbyssEngine::AEMath::Cosf(at);
            ret = 1.0f / ca;
            f[0x15] = ret;
        }
        return ret;
    }
}

namespace AbyssEngine {
    int ImageFontGetWidth(ImageFont *font, const unsigned short *str, unsigned int start,
                          unsigned int len) {
        if (font == 0 || str == 0)
            return 0;

        unsigned int end = len + start;
        unsigned short textIndex = (unsigned short) start;
        int total = 0;
        while (end > textIndex) {
            unsigned short glyphIndex = 0;
            while (glyphIndex < font->glyphCount) {
                unsigned int slot = glyphIndex++;
                unsigned short code = font->codes[slot];
                if (code == str[textIndex]) {
                    int width = (int) ((float *) font->glyphMeshes[slot]->positions)[3];
                    int advance = (int) font->spacing + width;
                    int contribution = advance;
                    if (width == 11)
                        contribution = advance - 2;
                    if (code != 32)
                        contribution = advance;
                    total += contribution;
                    break;
                }
            }
            ++textIndex;
        }
        return total;
    }
}

namespace AbyssEngine {
    void CurveRelease(Curve **slot) {
        Curve *curve = (Curve *) *slot;
        if (curve == 0)
            return;

        CurveKeyframe **data = 0;
        for (unsigned int i = 0; (data = (CurveKeyframe **) curve->entries),
                                 i < (unsigned int) curve->count; ++i) {
            CurveKeyframe *entry = data[i];
            char tag = (char) entry->tag;
            if (tag == 3 || tag == 2)
                operator delete(entry);
            else if (tag == 1)
                operator delete(entry);
            curve = (Curve *) *slot;
        }

        if (data != 0)
            operator delete[](data);
        curve = (Curve *) *slot;
        curve->entries = 0;

        if (*slot != 0)
            operator delete((void *) *slot);
        *slot = 0;
    }
}

namespace AbyssEngine {
    int MeshDraw(Engine * engine, Mesh * mesh);

    void MaterialDraw(PaintCanvas *canvas, Engine *engine, Material *mat, bool setTextures) {
        if (canvas == 0 || mat == 0)
            return;

        if (setTextures) {
            engine->SetTexturesExt(mat->textures[0], mat->textures[1], mat->textures[2], 0xffffffff);
        }
        engine->SetAddData(mat->addData, (int) mat->addDataSize);

        float ambient = mat->ambientColor.x;
        if (ambient != -10.0f) {
            engine->LightSetGlobalSceneColorAmbient(ambient, 0.0f, 0.0f);
        }

        const float inv255 = 1.0f / 255.0f;
        for (unsigned int i = 0; i < mat->meshes.size_; ++i) {
            Matrix world;
            AE_AEMath_matMul(&world, &mat->arr_5c.data_[i]);
            ((PaintCanvas *) canvas)->SetWorldViewMatrix(canvas->worldViewMatrix);
            engine->SetModelMatrix(world);
            engine->SetUVMatrix(mat->arr_38.data_[i]);

            unsigned int packed = mat->arr_50.data_[i];
            float ca = (float) ((packed >> 24) & 0xff);
            float cr = (float) ((packed >> 16) & 0xff);
            float cg = (float) ((packed >> 8) & 0xff);
            float cb = (float) (packed & 0xff);
            engine->SetColor(ca * inv255, cr * inv255, cg * inv255, cb * inv255);

            MeshDraw(engine, mat->meshes.data_[i]);
        }

        if (mat->ambientColor.x != -10.0f) {
            engine->LightSetGlobalSceneColorAmbient(mat->ambientColor.x, 0.0f, 0.0f);
        }

        mat->arr_2c.size_ = 0;
        mat->arr_5c.size_ = 0;
        mat->arr_38.size_ = 0;
        mat->meshes.size_ = 0;
        mat->arr_50.size_ = 0;
    }
}

namespace AbyssEngine {
    void MeshRelease(Engine * engine, Mesh * *slot);

    void ImageFontRelease(Engine *engine, ImageFont **slot) {
        if (*slot == 0)
            return;

        void *table = (*slot)->codes;
        if (table != 0)
            operator delete[](table);
        (*slot)->codes = 0;

        unsigned int i = 0;
        Mesh **glyphs;
        for (;;) {
            ImageFont *f = *slot;
            glyphs = f->glyphMeshes;
            if (f->glyphCount <= i)
                break;
            MeshRelease(engine, &glyphs[i]);
            ++i;
        }

        if (glyphs != 0)
            operator delete[](glyphs);
        (*slot)->glyphMeshes = 0;

        if (*slot != 0)
            operator delete((void *) *slot);
        *slot = 0;
    }
}

namespace AbyssEngine {
    int MeshReadData(Engine *engine, const unsigned int &handleRef, unsigned int flags, Mesh **slot,
                     Material *mat);

    int MeshReadData(Engine *engine, const unsigned int &handleRef, unsigned int flags, Mesh **slot,
                     Material *mat) {
        unsigned int subBit = flags & 0x1a;
        if (subBit != 0) {
            if (AEFile::Read((uint32_t)(0xc), &(*slot)->pivotX, handleRef) == 0)
                return -1;
        }

        Mesh *m = *slot;

        if (m->vertexFormat & 0x10) {
            if (AEFile::Read((uint32_t)(2), &m->indexCount, handleRef) == 0)
                return -1;
            void *idx = ::operator new[]((unsigned int) m->indexCount << 1);
            (*slot)->indices = idx;
            if (AEFile::Read((uint32_t)((unsigned int) (*slot)->indexCount << 1), (*slot)->indices, handleRef) == 0)
                return -1;
            m = *slot;
        }

        if (AEFile::Read((uint32_t)(2), &m->vertexCount, handleRef) == 0)
            return -1;

        unsigned int compressedData = flags << 0x1d;
        unsigned int vcount = (*slot)->vertexCount;

        {
            float minv[3] = {10000000.0f, 10000000.0f, 10000000.0f};
            float maxv[3] = {-10000000.0f, -10000000.0f, -10000000.0f};

            if ((flags & 4) != 0) {
                void *raw = ::operator new[](vcount * 6);
                if (AEFile::Read((uint32_t)(vcount * 6), raw, handleRef) == 0) {
                    ::operator delete[](raw);
                    return -1;
                }
                m = *slot;
                void *pos = ::operator new[](vcount * 0xc);
                m->positions = pos;
                m = *slot;
                short *rawShorts = (short *) raw;
                float *posFloats = (float *) m->positions;
                for (unsigned int i = 0; i < vcount * 3; ++i) {
                    float v = (float) rawShorts[i];
                    posFloats[i] = v;
                    int axis = i % 3;
                    if (v < minv[axis]) minv[axis] = v;
                    if (v > maxv[axis]) maxv[axis] = v;
                }
                ::operator delete[](raw);
            } else if ((flags << 0x1e) != 0) {
                void *raw = ::operator new[](vcount * 0xc);
                if (AEFile::Read((uint32_t)(vcount * 0xc), raw, handleRef) == 0) {
                    ::operator delete[](raw);
                    return -1;
                }
                m = *slot;
                void *pos = ::operator new[](vcount * 0xc);
                m->positions = pos;
                m = *slot;
                int *rawInts = (int *) raw;
                float *posFloats = (float *) m->positions;
                for (unsigned int i = 0; i < vcount * 3; ++i) {
                    float v = (float) rawInts[i];
                    posFloats[i] = v;
                    int axis = i % 3;
                    if (v < minv[axis]) minv[axis] = v;
                    if (v > maxv[axis]) maxv[axis] = v;
                }
                ::operator delete[](raw);
            } else if ((flags & 0x18) != 0) {
                m = *slot;
                void *pos = ::operator new[](vcount * 0xc);
                m->positions = pos;
                if (AEFile::Read((uint32_t)((*slot)->vertexCount * 0xc), (*slot)->positions, handleRef) == 0)
                    return -1;
            }

            float center[3];
            center[0] = (maxv[0] + minv[0]) * 0.5f;
            center[1] = (maxv[1] + minv[1]) * 0.5f;
            center[2] = (maxv[2] + minv[2]) * 0.5f;
            *(Vector *) &(*slot)->boundsCenterX = *(const Vector *) center;
            float halfDiag[3] = {minv[0], minv[1], minv[2]};
            *(Vector *) center -= *(const Vector *) halfDiag;
            (*slot)->boundsRadius = AEMath::VectorLength(*(const Vector *) center);
        }

        m = *slot;

        if (m->vertexFormat & 2) {
            if (compressedData != 0) {
                void *raw = ::operator new[](vcount << 2);
                if (AEFile::Read((uint32_t)(vcount << 2), raw, handleRef) == 0) {
                    ::operator delete[](raw);
                    return -1;
                }
                m = *slot;
                void *uv = ::operator new[](vcount << 3);
                m->texCoords = uv;
                m = *slot;
                const bool flip = Engine::enableShader != 0;
                short *rawShorts = (short *) raw;
                float *uvFloats = (float *) m->texCoords;
                const double scale = 1.0 / 4096.0;
                for (unsigned int i = 0; i < (vcount << 1); i += 2) {
                    double u = (double) rawShorts[i] * scale;
                    float *p = uvFloats + i;
                    p[0] = (float) u;
                    double v = (double) rawShorts[i + 1] * scale;
                    double vv = flip ? (1.0 - v) : v;
                    p[1] = (float) vv;
                }
                ::operator delete[](raw);
            } else if ((flags & 0x18) != 0) {
                void *uv = ::operator new[](vcount << 3);
                m->texCoords = uv;
                if (AEFile::Read((uint32_t)((*slot)->vertexCount << 3), (*slot)->texCoords, handleRef) == 0)
                    return -1;
                if (Engine::enableShader != 0) {
                    m = *slot;
                    float *uvFloats = (float *) m->texCoords;
                    for (unsigned int i = 0; i < (unsigned int) (m->vertexCount << 1); i += 2) {
                        float *p = uvFloats + (i + 1);
                        *p = 1.0f - *p;
                    }
                }
            }
        }

        m = *slot;

        if (m->vertexFormat & 4) {
            if (compressedData != 0) {
                void *raw = ::operator new[](vcount * 6);
                if (AEFile::Read((uint32_t)(vcount * 6), raw, handleRef) == 0) {
                    ::operator delete[](raw);
                    return -1;
                }
                m = *slot;
                void *nrm = ::operator new[](vcount * 0xc);
                m->normals = nrm;
                m = *slot;
                const double scale = 1.0 / 32768.0;
                short *s = (short *) raw;
                Vector *normals = (Vector *) m->normals;
                for (unsigned int i = 0; i < vcount; ++i) {
                    float nx = (float) ((double) s[0] * scale);
                    float ny = (float) ((double) s[1] * scale);
                    float nz = (float) ((double) s[2] * scale);
                    float len2 = nx * nx + ny * ny + nz * nz;
                    float len = sqrtf(len2);
                    if (len != 0.0f) {
                        float dxf = nx / len;
                        float dyf = ny / len;
                        float dzf = nz / len;

                        double dx = (double)dxf;
                        if (dx < -1.0) dx = -1.0;
                        if (dx > 1.0) dx = 1.0;

                        double dy = (double)dyf;
                        if (dy < -1.0) dy = -1.0;
                        if (dy > 1.0) dy = 1.0;

                        double dz = (double)dzf;
                        if (dz < -1.0) dz = -1.0;
                        if (dz > 1.0) dz = 1.0;

                        normals[i].x = (float)dx;
                        normals[i].y = (float)dy;
                        normals[i].z = (float)dz;
                    } else {
                        normals[i].x = 0.0f;
                        normals[i].y = 1.0f;
                        normals[i].z = 0.0f;
                    }
                    s += 3;
                }
                ::operator delete[](raw);
            } else if ((flags & 0x18) != 0) {
                void *nrm = ::operator new[](vcount * 0xc);
                m->normals = nrm;
                if (AEFile::Read((uint32_t)((*slot)->vertexCount * 0xc), (*slot)->normals, handleRef) == 0)
                    return -1;
            }

            if (Engine::enableShader != 0) {
                m = *slot;
                void *tan = ::operator new[](vcount * 0xc);
                m->tangents = tan;
                m = *slot;
                void *bin = ::operator new[](vcount * 0xc);
                m->binormals = bin;
                m = *slot;

                unsigned int triCount = (unsigned int) ((unsigned) (unsigned short) m->indexCount / (unsigned) 3);
                float *accum = (float *) ::operator new[](vcount * 0xc);

                if (vcount != 0)
                    memset(accum, 0, vcount * 0xc);

                for (unsigned int t = 0; t < triCount; ++t) {
                    m = *slot;
                    float *posBase = (float *) m->positions;
                    float *uvBase = (float *) m->texCoords;
                    unsigned short *idxBase = (unsigned short *) m->indices + t * 3;
                    unsigned int i0 = (unsigned int) idxBase[0];
                    unsigned int i2 = (unsigned int) idxBase[2];
                    unsigned int i1 = (unsigned int) idxBase[1];

                    float uv0v = uvBase[i0 * 2 + 1];
                    float uv0u = uvBase[i0 * 2];
                    float dv1 = uvBase[i2 * 2 + 1] - uv0v;
                    float dv2 = uvBase[i1 * 2 + 1] - uv0v;
                    float *p0 = posBase + i0 * 3;
                    float *p2 = posBase + i2 * 3;
                    float *p1 = posBase + i1 * 3;
                    float denom = (uvBase[i1 * 2] - uv0u) * dv1 -
                                  (uvBase[i2 * 2] - uv0u) * dv2;
                    float r = 1.0f / denom;
                    float tng[3];
                    tng[1] = ((p1[1] - p0[1]) * dv1 - (p2[1] - p0[1]) * dv2) * r;
                    tng[0] = ((p1[0] - p0[0]) * dv1 - (p2[0] - p0[0]) * dv2) * r;
                    tng[2] = ((p1[2] - p0[2]) * dv1 - (p2[2] - p0[2]) * dv2) * r;
                    float *a = accum + i0 * 3;
                    *(Vector *) a += *(const Vector *) tng;
                    a = accum + i1 * 3;
                    *(Vector *) a += *(const Vector *) tng;
                    a = accum + i2 * 3;
                    *(Vector *) a += *(const Vector *) tng;
                }

                for (unsigned int v = 0; v < vcount; ++v) {
                    m = *slot;
                    float *nrmBase = (float *) m->normals;
                    float nrm[3];
                    nrm[0] = nrmBase[v * 3 + 0];
                    nrm[1] = nrmBase[v * 3 + 1];
                    nrm[2] = nrmBase[v * 3 + 2];
                    float tg[3];
                    tg[0] = accum[v * 3 + 0];
                    tg[1] = accum[v * 3 + 1];
                    tg[2] = accum[v * 3 + 2];
                    float d = AEMath::VectorDot(*(const Vector *) nrm, *(const Vector *) tg);
                    float scaled[3] = {nrm[0], nrm[1], nrm[2]};
                    *(Vector *) scaled *= d;
                    *(Vector *) tg -= *(const Vector *) scaled;
                    float tanOut[3];
                    *(Vector *) tanOut = AEMath::VectorNormalize(*(const Vector *) tg);
                    float *tb = (float *) (*slot)->tangents + v * 3;
                    tb[0] = tanOut[0];
                    tb[1] = tanOut[1];
                    tb[2] = tanOut[2];
                    float binOut[3];
                    *(Vector *) binOut = AEMath::VectorCross(*(const Vector *) nrm, *(const Vector *) tanOut);
                    float *bb = (float *) (*slot)->binormals + v * 3;
                    bb[0] = binOut[0];
                    bb[1] = binOut[1];
                    bb[2] = binOut[2];
                }
                ::operator delete[](accum);
            }
        }

        m = *slot;

        if (m->vertexFormat & 8) {
            if (compressedData != 0) {
                void *raw = ::operator new[](vcount << 2);
                if (AEFile::Read((uint32_t)(vcount << 2), raw, handleRef) == 0) {
                    ::operator delete[](raw);
                    return -1;
                }
                m = *slot;
                void *col = ::operator new[](vcount << 4);
                m->colors = col;
                m = *slot;
                const float inv = 255.0f;
                unsigned char *rawBytes = (unsigned char *) raw;
                float *colorFloats = (float *) m->colors;
                for (unsigned int i = 0; i < (vcount << 2); ++i) {
                    colorFloats[i] = (float) rawBytes[i] / inv;
                }
                ::operator delete[](raw);
            } else if ((flags & 0x18) != 0) {
                void *col = ::operator new[](vcount << 4);
                m->colors = col;
                if (AEFile::Read((uint32_t)((*slot)->vertexCount << 4), (*slot)->colors, handleRef) == 0)
                    return -1;
            }
        }

        if (subBit != 0) {
            if ((*slot)->ReadEnhancedDataFromFile(handleRef, flags) == 0)
                return -1;
            unsigned short childCount;
            if (AEFile::Read((uint32_t)(2), &childCount, handleRef) == 0)
                return -1;
            Transform *xf = (*slot)->animation;
            if (xf != 0)
                ((AEMath::BSphere *) &(*slot)->boundsCenterX)->Merge(xf->bounds());
            for (unsigned int c = 0; c < childCount; ++c) {
                Mesh *childPtr = new Mesh();
                childPtr->vboEligible = 1;
                childPtr->vertexFormat = (*slot)->vertexFormat;
                childPtr->material = (*slot)->material;
                if (MeshReadData(engine, handleRef, flags, &childPtr, mat) == -1)
                    return -1;
                ((AEMath::BSphere *) &(*slot)->boundsCenterX)->Merge(
                    *(const AEMath::BSphere *) &childPtr->boundsCenterX);
                ArrayAdd<Mesh *>(childPtr, (*slot)->animation->meshes);
            }
        }

        return 1;
    }
}

namespace AbyssEngine {
    String operator+(const String &a, const long long &b) {
        String result(a);
        String num;
        num.ctor_longlong(b);
        result += num;
        return result;
    }
}

namespace AbyssEngine {
    void TransformRelease(Engine * engine, Transform * *slot);

    void MeshRelease(Engine *engine, Mesh **slot) {
        if (engine != 0 && *slot != 0) {
            TransformRelease(engine, &(*slot)->animation);

            typedef void (*FreeFn)(Engine *, Mesh **);
            ((FreeFn) g_MeshRelease_freeFn)(engine, slot);
        }
    }
}

namespace AbyssEngine {
    String operator+(const float &a, const String &b) {
        String result;
        result.ctor_float(a);
        result += b;
        return result;
    }
}

namespace AbyssEngine {
    int MeshCreate(Engine *engine, unsigned short vertexCount, unsigned short triCount,
                   unsigned int fmt, void **out);

    void MeshRelease(Engine * engine, Mesh * *slot);

    int SpriteSystemCreate(Engine *engine, unsigned short count, bool sharedSize, SpriteSystem **out) {
        unsigned int n = count;
        if (n == 0)
            return -4;

        SpriteSystem *sys = (SpriteSystem *) operator new(0x14);
        sys->posCpu = 0;
        sys->sizeCpu = 0;
        sys->mesh = 0;
        *out = sys;
        sys->count = count;

        unsigned int triCount = (n << 1) & 0xffff;
        int rc = MeshCreate(engine, (unsigned short) ((n & 0x3fff) << 2), (unsigned short) triCount,
                            0x1f, (void **) &sys->mesh);
        SpriteSystem *s = *out;
        if (rc != 1) {
            MeshRelease(engine, &s->mesh);
            if (s->posCpu != 0)
                operator delete[]((*out)->posCpu);
            (*out)->posCpu = 0;
            if ((*out)->sizeCpu != 0)
                operator delete[]((*out)->sizeCpu);
            (*out)->sizeCpu = 0;
            if (*out != 0)
                operator delete((void *) *out);
            *out = 0;
            return -1;
        }

        Mesh *mesh = s->mesh;
        int *indexArr = (int *) mesh->indices;

        void *posCpu = operator new[](n * 0xc);
        s->posCpu = (float *) posCpu;
        memset((*out)->posCpu, 0, n * 0xc);

        s = *out;
        s->sharedSize = sharedSize ? 1 : 0;
        if (sharedSize) {
            void *sz = operator new[](2);
            s->sizeCpu = (int16_t *) sz;
            *(unsigned short *) (*out)->sizeCpu = 0;
        } else {
            void *sz = operator new[]((n << 1));
            s->sizeCpu = (int16_t *) sz;
            memset((*out)->sizeCpu, 0, (n << 1));
        }

        unsigned short base = 0;
        unsigned short *idx = (unsigned short *) indexArr;
        for (unsigned short off = 0; off < n * 6; off += 6) {
            unsigned short *p = idx + off;
            p[0] = base;
            p[1] = base | 1;
            p[2] = base | 2;
            p[3] = base;
            p[4] = base | 2;
            p[5] = base | 3;
            base = base + 4;
        }

        Mesh *m = (*out)->mesh;
        unsigned int vcount = (unsigned int) (unsigned short) m->vertexCount;

        float *colors = (float *) m->colors;
        for (unsigned int i = 0; i <= vcount * 4 && vcount * 4 - i != 0; ++i)
            colors[i] = 1.0f;

        char tangent = *g_SpriteSystem_tangentFlag;
        float *normals = (float *) m->normals;
        float *tangents = (float *) m->tangents;
        float *binormals = (float *) m->binormals;
        unsigned int vi = 0;
        for (unsigned int k = vcount; k != 0; --k) {
            float *nrm = normals + vi;
            nrm[0] = 0.0f;
            nrm[1] = 0.0f;
            nrm[2] = 1.0f;
            if (tangent != 0) {
                float *tan = tangents + vi;
                tan[0] = 1.0f;
                tan[1] = 0.0f;
                tan[2] = 0.0f;
                float *bin = binormals + vi;
                bin[0] = 0.0f;
                bin[1] = 1.0f;
                bin[2] = 0.0f;
            }
            vi += 3;
        }

        return 1;
    }
}

namespace AbyssEngine {
    void ImageRelease(Image * *slot);

    int ImageCreateFromFile(Engine *engine, const char *path, Image **out) {
        if (engine == 0 || path == 0)
            return -4;

        Image *image = (Image *) ::operator new(0x14);
        image->hasMipmaps = 0;
        *(uint32_t *) &image->width = 0;
        image->format = 0;
        image->data = 0;
        *out = image;

        unsigned int dataLen;
        char header[12];
        unsigned short regionCount;
        unsigned char type;
        unsigned int handle;
        handle = 0;
        type = 0;
        regionCount = 0;
        if (AEFile::OpenRead(path, (uint32_t *) (&handle)) == 0) {
            if (*out != 0)
                ::operator delete((void *) *out);
            *out = 0;
            return -1;
        }

        static const char aeiMagic[8] = "AEimage";
        strcpy(header, "*******");
        if (AEFile::Read((uint32_t)(8), header, handle) == 0)
            goto fail;
        for (unsigned int k = 0; k < 8; ++k) {
            if (aeiMagic[k] != header[k])
                goto fail;
        }

        if (AEFile::Read((uint32_t)(1), &type, handle) == 0) goto fail;
        if (AEFile::Read((uint32_t)(2), &(*out)->width, handle) == 0) goto fail;
        if (AEFile::Read((uint32_t)(2), &(*out)->height, handle) == 0) goto fail;
        if (AEFile::Read((uint32_t)(2), &regionCount, handle) == 0) goto fail;

        AEFile::Skip((uint32_t)((unsigned int) regionCount << 3), handle);

        if (type & 2)
            (*out)->hasMipmaps = 1;

        switch (type) {
                case 1:
                case 3:
                case 0x81: {
                    (*out)->data = ::operator new[](
                        4 * (unsigned int) (*out)->height * (unsigned int) (*out)->width);
                    unsigned int size =
                        4 * (unsigned int) (*out)->width * (unsigned int) (*out)->height;
                    if (AEFile::Read((uint32_t)(size), (*out)->data, handle) == 0) goto fail;
                    (*out)->dataLen =
                        4 * (unsigned int) (*out)->height * (unsigned int) (*out)->width;
                    (*out)->format = ((signed char) type > -1) ? 3u : 6u;
                    break;
                }
                case 0xd:
                case 0xf:
                    if (AEFile::Read((uint32_t)(4), &dataLen, handle) == 0) goto fail;
                    (*out)->data = ::operator new[](dataLen);
                    if (AEFile::Read((uint32_t)(dataLen), (*out)->data, handle) == 0) goto fail;
                    (*out)->format = 4;
                    (*out)->dataLen = dataLen;
                    break;
                case 0x10:
                case 0x12:
                    if (AEFile::Read((uint32_t)(4), &dataLen, handle) == 0) goto fail;
                    (*out)->data = ::operator new[](dataLen);
                    if (AEFile::Read((uint32_t)(dataLen), (*out)->data, handle) == 0) goto fail;
                    (*out)->format = 5;
                    (*out)->dataLen = dataLen;
                    break;
                case 0x11:
                case 0x13:
                    if (AEFile::Read((uint32_t)(4), &(*out)->dataLen, handle) == 0) goto fail;
                    (*out)->format = 7;
                    (*out)->data = ::operator new[]((*out)->dataLen);
                    if (AEFile::Read((uint32_t)((*out)->dataLen), (*out)->data, handle) == 0) goto fail;
                    break;
                case 0x14:
                case 0x16:
                case 0x17:
                case 0x40:
                case 0x42:
                    if (type == 0x40)
                        (*out)->hasMipmaps = 0;
                    if (AEFile::Read((uint32_t)(4), &dataLen, handle) == 0) goto fail;
                    (*out)->data = ::operator new[](dataLen);
                    if (AEFile::Read((uint32_t)(dataLen), (*out)->data, handle) == 0) goto fail;
                    (*out)->format = 0xb;
                    (*out)->dataLen = dataLen;
                    if (type == 0x17)
                        (*out)->hasMipmaps = 0;
                    break;
                case 0x20:
                case 0x22:
                    if (AEFile::Read((uint32_t)(4), &(*out)->dataLen, handle) == 0) goto fail;
                    (*out)->format = 8;
                    (*out)->data = ::operator new[]((*out)->dataLen);
                    if (AEFile::Read((uint32_t)((*out)->dataLen), (*out)->data, handle) == 0) goto fail;
                    break;
                case 0x21:
                case 0x23:
                    if (AEFile::Read((uint32_t)(4), &(*out)->dataLen, handle) == 0) goto fail;
                    (*out)->format = 9;
                    (*out)->data = ::operator new[]((*out)->dataLen);
                    if (AEFile::Read((uint32_t)((*out)->dataLen), (*out)->data, handle) == 0) goto fail;
                    break;
                case 0x24:
                case 0x26:
                    if (AEFile::Read((uint32_t)(4), &(*out)->dataLen, handle) == 0) goto fail;
                    (*out)->format = 10;
                    (*out)->data = ::operator new[]((*out)->dataLen);
                    if (AEFile::Read((uint32_t)((*out)->dataLen), (*out)->data, handle) == 0) goto fail;
                    break;
                default:
                    break;
        }

        AEFile::Close(handle);
        return 1;

    fail:
        ImageRelease(out);
        AEFile::Close(handle);
        return -1;
    }
}

namespace AbyssEngine {
    void ImageRelease(Image **slot) {
        if (*slot != 0) {
            operator delete[]((*slot)->data);
            (*slot)->data = 0;
            if (*slot != 0)
                operator delete((void *) *slot);
            *slot = 0;
        }
    }
}

namespace AbyssEngine {
    void MeshRelease(Engine * engine, Mesh * *slot);

    void Image2DRelease(Engine *engine, Image2D **slot) {
        if (*slot != 0) {
            MeshRelease(engine, (Mesh **) *slot);
            if (*slot != 0)
                operator delete((void *) *slot);
            *slot = 0;
        }
    }
}

namespace AbyssEngine {
    int MeshCreate(Engine * /*engine*/, unsigned short vertexCount, unsigned short triCount,
                   signed char vertexFormat, Mesh **out) {
        if (vertexCount < 4 || triCount == 0 || (vertexFormat & 1) == 0)
            return -4;

        Mesh *m = new Mesh();

        *out = m;
        m->vertexCount = (short) vertexCount;
        m->vertexFormat = vertexFormat;
        m->indexCount = (short) (triCount + (triCount << 1));
        m->field_0x2a = (short) triCount;

        unsigned int posBytes = (unsigned int) vertexCount * 0xc;
        void *p = operator new[](posBytes);
        m->positions = p;
        memset(p, 0, posBytes);

        if (vertexFormat & 0x10) {
            p = operator new[]((unsigned int) triCount * 6);
            m->indices = p;
            memset(p, 0, (unsigned int) triCount * 6);
        }
        if (vertexFormat & 2) {
            p = operator new[]((unsigned int) vertexCount << 3);
            m->texCoords = p;
            memset(p, 0, (unsigned int) vertexCount << 3);
        }
        if (vertexFormat & 4) {
            p = operator new[](posBytes);
            m->normals = p;
            memset(p, 0, posBytes);
            if (Engine::enableShader != 0) {
                p = operator new[](posBytes);
                m->tangents = p;
                memset(p, 0, posBytes);
                p = operator new[](posBytes);
                m->binormals = p;
                memset(p, 0, posBytes);
            }
        }
        if (vertexFormat & 8) {
            p = operator new[]((unsigned int) vertexCount << 4);
            m->colors = p;
            memset(p, 0, (unsigned int) vertexCount << 4);
        }

        return 1;
    }
}

namespace AbyssEngine {
    namespace AEMath {
    }

    float CameraSetPerspective(float p1, float aspectNum, float fov, float aspectDen, float near,
                               Camera *cam) {
        float ret = p1;
        if (cam != 0) {
            float *f = (float *) cam;
            f[0] = p1;
            f[1] = aspectNum;
            f[2] = fov;

            float s = AbyssEngine::AEMath::Sinf(f[0] * 0.5f);
            float c = AbyssEngine::AEMath::Cosf(f[0] * 0.5f);
            f[0x12] = s / c;
            f[0x13] = (aspectDen / near) * (s / c);
            f[0x14] = aspectDen / near;

            float c2 = AbyssEngine::AEMath::Cosf(f[0] * 0.5f);
            f[0x16] = 1.0f / c2;

            AbyssEngine::AEMath::ATanf(f[0x12] * f[0x14]);
            float c3 = AbyssEngine::AEMath::Cosf(f[0] * 0.5f);
            ret = 1.0f / c3;
            f[0x15] = ret;
        }
        return ret;
    }
}

namespace AbyssEngine {
    void ImageFontSetSpacing(ImageFont *font, short spacing) {
        if (font != 0)
            font->spacing = spacing;
    }
}

namespace AbyssEngine {
    int ImageFontGetHeight(ImageFont *font) {
        if (font != 0) {
            Mesh **p = font->glyphMeshes;
            Mesh *q = p[0];
            float *r = (float *) q->positions;
            int v = (int) r[7];
            if (v == 0x18)
                v = 0x13;
            return v;
        }
        return 0;
    }
}

namespace AbyssEngine {
    String operator+(const String &a, const int &b) {
        String result(a);
        String num;
        num.Set((long long) (b));
        result += num;
        return result;
    }
}

namespace AbyssEngine {
    int ImageFontGetSpacing(ImageFont *font) {
        if (font != 0)
            return (int) font->spacing;
        return 0;
    }
}

namespace AbyssEngine {
    String operator+(const String &a) {
        return String(a);
    }
}

namespace AbyssEngine {
    void ImageFontDrawString(ImageFont *font, const unsigned short *str, unsigned int len, int x,
                             int y, PaintCanvas *canvas, Engine *engine, bool flag);

    void ImageFontDrawString(ImageFont *font, const unsigned short *str, int x, int y,
                             PaintCanvas *canvas, Engine *engine, bool flag) {
        if (font != 0 && str != 0) {
            unsigned short i = 0;
            unsigned int len;
            do {
                len = (unsigned int) i;
                i = i + 1;
            } while (str[len] != 0);
            ImageFontDrawString(font, str, len, x, y, canvas, engine, flag);
        }
    }
}

namespace AbyssEngine {
    int ImageFontGetWidth(ImageFont *font, const unsigned short *text, unsigned int len);

    int ImageFontGetHeight(ImageFont * font);
    int MeshDraw(Engine * engine, Mesh * mesh);

    void ImageFontDrawString(ImageFont *font, const unsigned short *text, unsigned int len, int x, int y,
                             PaintCanvas *canvas, Engine *engine, bool rtl) {
        int textValid = text != 0 ? 1 : 0;
        int fontValid = font != 0 ? 1 : 0;
        int valid = textValid & fontValid;

        if (engine->shaderModeFlag != 0) {
            if (valid == 0)
                return;
            if (ImageFontGetWidth(font, text, len) + x < 0)
                return;

            int drawY = (int) font->yOffset;
            int height = ImageFontGetHeight(font);
            int displayWidth = engine->GetDisplayWidth();
            bool outside = displayWidth < x;
            if (displayWidth >= x) {
                drawY += y;
                outside = height + drawY < 0;
            }
            if (outside || drawY > engine->GetDisplayHeight())
                return;

            int step = -1;
            int textIndex = (int) len - 1;
            if (canvas->field_0x1c != 0 || rtl) {
                step = 1;
                textIndex = 0;
            }
            if (*g_GameText_arabicEnabledFlag != 0 && GameText::getLanguage() == 9 &&
                GameText::isNonArabicString(text, len) != 0) {
                textIndex = 0;
                step = 1;
            }

            float baseY = (float) (drawY - 2);
            for (unsigned short i = 0; i < len; ++i) {
                unsigned short glyphIndex = 0;
                while (glyphIndex < font->glyphCount) {
                    unsigned int slot = glyphIndex++;
                    if (font->codes[slot] != text[textIndex])
                        continue;

                    Mesh *glyphMesh = font->glyphMeshes[slot];
                    int advance = (int) ((float *) glyphMesh->positions)[3];
                    if (x + advance >= 0 && x <= engine->GetDisplayWidth()) {
                        Mesh *batchMesh = (Mesh *) canvas->quad2dMesh;
                        int batchIndex = canvas->field_0xc;
                        float *sourcePositions = (float *) glyphMesh->positions;
                        float *positions = (float *) batchMesh->positions + batchIndex * 12;
                        positions[0] = sourcePositions[0] + (float) x;
                        positions[1] = sourcePositions[1] + baseY;
                        positions[3] = sourcePositions[3] + (float) x;
                        positions[4] = sourcePositions[4] + baseY;
                        positions[6] = sourcePositions[6] + (float) x;
                        positions[7] = sourcePositions[7] + baseY;
                        positions[9] = sourcePositions[9] + (float) x;
                        positions[10] = sourcePositions[10] + baseY;

                        uint32_t *sourceUv = (uint32_t *) glyphMesh->texCoords;
                        uint32_t *uv = (uint32_t *) batchMesh->texCoords + batchIndex * 8;
                        for (unsigned int word = 0; word < 8; ++word)
                            uv[word] = sourceUv[word];

                        uint32_t *colors = (uint32_t *) batchMesh->colors + batchIndex * 16;
                        for (unsigned int vertex = 0; vertex < 4; ++vertex) {
                            colors[vertex * 4] = *(uint32_t *) &engine->flCurrentColorR;
                            colors[vertex * 4 + 1] = *(uint32_t *) &engine->field_0xd4;
                            colors[vertex * 4 + 2] = *(uint32_t *) &engine->field_0xd8;
                            colors[vertex * 4 + 3] = *(uint32_t *) &engine->field_0xdc;
                        }

                        canvas->field_0xc = batchIndex + 1;
                        if (batchIndex >= 99) {
                            batchMesh->indexCount = (unsigned short) (6 * (batchIndex + 1));
                            float identity[15] = {
                                1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                1.0f, 1.0f, 1.0f,
                            };
                            canvas->SetWorldViewMatrix(
                                *reinterpret_cast<const AEMath::Matrix *>(identity));
                            MeshDraw(engine, batchMesh);
                            canvas->field_0xc = 0;
                        }
                    }

                    int effectiveAdvance = (int) font->spacing + advance;
                    if (text[textIndex] == 32 && advance == 11)
                        effectiveAdvance -= 2;
                    x += effectiveAdvance;
                    break;
                }
                textIndex += step;
            }

            int batchCount = canvas->field_0xc;
            if (batchCount > 0) {
                Mesh *batchMesh = (Mesh *) canvas->quad2dMesh;
                batchMesh->indexCount = (unsigned short) (6 * batchCount);
                float identity[15] = {
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f,
                };
                canvas->SetWorldViewMatrix(
                    *reinterpret_cast<const AEMath::Matrix *>(identity));
                MeshDraw(engine, batchMesh);
                canvas->field_0xc = 0;
            }
            return;
        }

        if (valid == 0)
            return;
        if (ImageFontGetWidth(font, text, len) + x < 0)
            return;

        int drawY = (int) font->yOffset;
        int height = ImageFontGetHeight(font);
        int displayWidth = engine->GetDisplayWidth();
        bool outside = displayWidth < x;
        if (displayWidth >= x) {
            drawY += y;
            outside = height + drawY < 0;
        }
        if (outside || drawY > engine->GetDisplayHeight())
            return;

        int step = -1;
        int textIndex = (int) len - 1;
        if (canvas->field_0x1c != 0 || rtl) {
            step = 1;
            textIndex = 0;
        }
        float baseY = (float) (drawY - 2);
        float drawMatrix[15] = {
            1.0f, 0.0f, 0.0f, (float) x,
            0.0f, 1.0f, 0.0f, baseY,
            0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f,
        };

        for (unsigned short i = 0; i < len; ++i) {
            unsigned short glyphIndex = 0;
            while (glyphIndex < font->glyphCount) {
                unsigned int slot = glyphIndex++;
                if (font->codes[slot] != text[textIndex])
                    continue;

                drawMatrix[7] = baseY;
                drawMatrix[3] = (float) x;
                Mesh *glyphMesh = font->glyphMeshes[slot];
                int advance = (int) ((float *) glyphMesh->positions)[3];
                if (x + advance >= 0 && x <= engine->GetDisplayWidth()) {
                    canvas->SetWorldViewMatrix(
                        *reinterpret_cast<const AEMath::Matrix *>(drawMatrix));
                    MeshDraw(engine, glyphMesh);
                }

                int effectiveAdvance = (int) font->spacing + advance;
                if (text[textIndex] == 32 && advance == 11)
                    effectiveAdvance -= 2;
                x += effectiveAdvance;
                break;
            }
            textIndex += step;
        }
    }
}

namespace AbyssEngine {
    String operator+(const int &a, const String &b) {
        String result;
        result.Set((long long) (a));
        result += b;
        return result;
    }
}

namespace AbyssEngine {
    int MeshCreate(Engine *engine, unsigned short vertexCount, unsigned short triCount,
                   signed char vertexFormat, Mesh **out);

    void ImageFontRelease(Engine * engine, ImageFont * *slot);

    int ImageCreateFontFromFile(Engine *engine, const char *path, unsigned short index, ImageFont **out) {
        if (engine == 0 || path == 0)
            return -4;

        unsigned int payloadBytes = 0;
        char header[15];
        unsigned char type = 0;
        unsigned short atlasH = 0;
        unsigned short atlasW = 0;
        unsigned short regionCount = 0;
        unsigned short fontCount = 0;
        unsigned int handle = 0;
        if (AEFile::OpenRead(path, (uint32_t *) (&handle)) == 0)
            return -1;

        strcpy(header, "*******");
        if (AEFile::Read((uint32_t)(8), header, handle) == 0)
            goto fail;
        static const char aeiMagic[8] = "AEimage";
        for (unsigned int k = 0; k < 8; ++k)
            if (aeiMagic[k] != header[k])
                return -1;

        if (AEFile::Read((uint32_t)(1), &type, handle) == 0 ||
            AEFile::Read((uint32_t)(2), &atlasW, handle) == 0 ||
            AEFile::Read((uint32_t)(2), &atlasH, handle) == 0 ||
            AEFile::Read((uint32_t)(2), &regionCount, handle) == 0 ||
            AEFile::Skip((uint32_t)((unsigned int) regionCount << 3), handle) == 0)
            goto fail;

        {
            unsigned int formatIndex = (unsigned int) type - 3;
            if (formatIndex <= 0x1e &&
                ((1u << formatIndex) & 0x601bf400u) != 0)
                goto read_payload_length;
            if (type == 3)
                goto skip_raw_payload;
            formatIndex = (unsigned int) type - 36;
            if (formatIndex <= 0x1e &&
                ((1u << formatIndex) & 0x50000001u) != 0)
                goto read_payload_length;
            if (type == 1)
                goto skip_raw_payload;
            goto payload_skipped;
        }

    read_payload_length:
        if (AEFile::Read((uint32_t)(4), &payloadBytes, handle) == 0)
            goto fail;
        if (AEFile::Skip((uint32_t)(payloadBytes), handle) == 0)
            goto fail;
        goto payload_skipped;

    skip_raw_payload:
        if (AEFile::Skip((uint32_t)(4 * (unsigned int) atlasH * (unsigned int) atlasW),
                         handle) == 0)
            goto fail;

    payload_skipped:
        if (AEFile::Read((uint32_t)(2), &fontCount, handle) == 0)
            goto fail;
        if (fontCount <= index)
            goto fail;

        {
            ImageFont *font = (ImageFont *) ::operator new(sizeof(ImageFont));
            font->glyphCount = 0;
            font->codes = 0;
            font->field_0x8 = 0;
            font->glyphMeshes = 0;
            font->spacing = 0;
            font->yOffset = 0;
            *out = font;
        }

        for (unsigned short fontIndex = 0; fontIndex < fontCount; ++fontIndex) {
            if (fontIndex == index) {
                ImageFont *font = *out;
                if (AEFile::Read((uint32_t)(2), &font->glyphCount, handle) == 0)
                    goto fail;
                font->codes = (uint16_t *) ::operator new[](
                    (unsigned int) font->glyphCount * sizeof(uint16_t));
                if (AEFile::Read((uint32_t)((unsigned int) (*out)->glyphCount * sizeof(uint16_t)),
                                 (*out)->codes, handle) == 0)
                    goto fail;
                font = *out;
                font->glyphMeshes = (Mesh **) ::operator new[](
                    (unsigned int) font->glyphCount * sizeof(Mesh *));

                for (unsigned int glyphIndex = 0;
                     glyphIndex < (unsigned int) (*out)->glyphCount; ++glyphIndex) {
                    if (MeshCreate(engine, 4, 2, 0x13,
                                   &(*out)->glyphMeshes[glyphIndex]) != 1)
                        goto fail;

                    unsigned short rectX = 0;
                    unsigned short rectY = 0;
                    unsigned short rectW = 0;
                    unsigned short rectH = 0;
                    if (AEFile::Read((uint32_t)(2), &rectX, handle) == 0 ||
                        AEFile::Read((uint32_t)(2), &rectY, handle) == 0 ||
                        AEFile::Read((uint32_t)(2), &rectW, handle) == 0 ||
                        AEFile::Read((uint32_t)(2), &rectH, handle) == 0)
                        goto fail;

                    Mesh *mesh = (*out)->glyphMeshes[glyphIndex];
                    float *positions = (float *) mesh->positions;
                    float width = (float) rectW;
                    float height = (float) rectH;
                    positions[0] = 0.0f;
                    positions[1] = 0.0f;
                    positions[2] = 0.0f;
                    positions[3] = width;
                    positions[4] = 0.0f;
                    positions[5] = 0.0f;
                    positions[6] = width;
                    positions[7] = height;
                    positions[8] = 0.0f;
                    positions[9] = 0.0f;
                    positions[10] = height;
                    positions[11] = 0.0f;

                    float x = (float) rectX;
                    float y = (float) rectY;
                    float invAtlasH = (float) (1.0 / (double) atlasH);
                    float invAtlasW = (float) (1.0 / (double) atlasW);
                    float *uv = (float *) mesh->texCoords;
                    float u0 = x * invAtlasW;
                    float v0 = y * invAtlasH;
                    float u1 = (width + x) * invAtlasW;
                    float v1 = (height + y) * invAtlasH;
                    uv[0] = u0;
                    uv[1] = v0;
                    uv[2] = u1;
                    uv[3] = v0;
                    uv[4] = u1;
                    uv[5] = v1;
                    uv[6] = u0;
                    uv[7] = v1;

                    unsigned int *indices = (unsigned int *) mesh->indices;
                    indices[0] = 0x20000;
                    indices[1] = 1;
                    indices[2] = 0x20003;
                }
            } else {
                unsigned short skippedGlyphCount = 0;
                if (AEFile::Read((uint32_t)(2), &skippedGlyphCount, handle) == 0 ||
                    AEFile::Skip((uint32_t)((unsigned int) skippedGlyphCount * 10), handle) == 0)
                    goto fail;
            }
        }

        AEFile::Close(handle);
        return 1;

    fail:
        ImageFontRelease(engine, out);
        return -1;
    }
}

namespace AbyssEngine {
    Quaternion operator+(const Quaternion &a, const Quaternion &b) {
        float yv = a.y + b.y;
        float wv = a.w + b.w;
        return Quaternion(wv, 0.0f, yv, 0.0f);
    }
}

namespace AbyssEngine {
    void SpriteSystemSetAllSize(short size, SpriteSystem *sys) {
        if (sys == 0)
            return;

        short *sizes = sys->sizeCpu;
        if (sys->sharedSize != 0) {
            sizes[0] = size;
            return;
        }
        unsigned int count = sys->count;
        for (unsigned int i = 0; i < count; ++i)
            sizes[i] = size;
    }
}

namespace AbyssEngine {
    float MODF(float value, float *intPart) {
        float ip = (float) (int) value;
        *intPart = ip;
        return value - ip;
    }
}

namespace AbyssEngine {
    int MeshDraw(Engine * engine, Mesh * mesh);

    static inline unsigned int f2u(float f) {
        union {
            float f;
            unsigned int u;
        } c;
        c.f = f;
        return c.u;
    }

    void SpriteSystemDraw(Engine *engine, const Matrix &view, const Matrix &world, SpriteSystem *sys) {
        if (sys == 0)
            return;

        Mesh *mesh = sys->mesh;
        float *vbuf = (float *) mesh->positions;

        Matrix mv;
        AE_AEMath_matMul(&mv, &view);

        unsigned short count = sys->count;
        unsigned char sharedSize = sys->sharedSize;
        float *posCpu = sys->posCpu;
        short *sizeCpu = sys->sizeCpu;
        const float *W = world;

        unsigned short sizeIdx = 0;
        unsigned short posIdx = 0;
        for (unsigned short q = 0; q < count * 0xc; q += 0xc) {
            float *p = posCpu + (unsigned int) posIdx;
            posIdx = posIdx + 3;
            float px = p[0], py = p[1], pz = p[2];

            unsigned int si = sizeIdx;
            sizeIdx = (unsigned short) (((sharedSize ^ 1) & 0xff) + sizeIdx);
            float half = (float) ((int) sizeCpu[si] >> 1);

            float cx = W[0xc / 4 + 0] + W[4 / 4] * py + W[0] * px + W[8 / 4] * pz;
            float cy = W[0x1c / 4] + py * W[0x14 / 4] + px * W[0x10 / 4] + pz * W[0x18 / 4];
            float cz = W[0x2c / 4] + py * W[0x24 / 4] + px * W[0x20 / 4] + pz * W[0x28 / 4];

            float left = cx - half, right = cx + half;
            float bottom = cy - half, top = cy + half;

            float *out = vbuf + q;
            out[0] = left;
            out[1] = bottom;
            out[2] = cz;
            out[3] = right;
            out[4] = bottom;
            out[5] = cz;
            out[6] = right;
            out[7] = top;
            out[8] = cz;
            out[9] = left;
            out[10] = top;
            out[11] = cz;
        }

        if (mesh->material == 0) {
            MeshDraw(engine, mesh);
        } else {
            Material *batch = (Material *) mesh->material;
            ArrayAddCachedRaw<Mesh *>(mesh, &batch->meshes);

            const float *v = view;
            AE_SpriteSystem_pushMatrix(f2u(v[0]), f2u(v[1]), f2u(v[2]), f2u(v[3]), f2u(v[4]), f2u(v[5]),
                                       f2u(v[6]), f2u(v[7]), f2u(v[8]), f2u(v[9]), f2u(v[10]), f2u(v[11]),
                                       f2u(v[12]), f2u(v[13]), f2u(v[14]), (int) (intptr_t) &batch->arr_2c);
            unsigned int one = 0x3f800000;
            unsigned int negOne = 0xbf800000;
            AE_SpriteSystem_pushMatrix(one, 0, 0, 0, 0, negOne, 0, 0, 0, 0, one, 0,
                                       one, one, one, (int) (intptr_t) &batch->arr_38);
            const float *w = world;
            AE_SpriteSystem_pushMatrix(f2u(w[0]), f2u(w[1]), f2u(w[2]), f2u(w[3]), f2u(w[4]), f2u(w[5]),
                                       f2u(w[6]), f2u(w[7]), f2u(w[8]), f2u(w[9]), f2u(w[10]), f2u(w[11]),
                                       f2u(w[12]), f2u(w[13]), f2u(w[14]), (int) (intptr_t) &batch->arr_5c);
            ArrayAddCachedRaw<unsigned int>(0xffffffff, &batch->arr_50);
        }
    }
}

namespace AbyssEngine {
    int MeshConvertToVBOIntern(Mesh *m) {
        if (m == 0 || Engine::vboSupported == 0)
            return -4;
        if (m->uploaded != 0 || (short) m->indexCount == 0)
            return -4;

        unsigned int vcount = m->vertexCount;

        void *colArr = m->colors;
        void *tanArr = m->tangents;
        void *binArr = m->binormals;

        glGenBuffers(1, &m->positionVBO);
        glBindBuffer(0x8892, m->positionVBO);
        glBufferData(0x8892, vcount * 0xc, m->positions, 0x88e4);

        unsigned char flags = m->vertexFormat;
        if (flags & 2) {
            glGenBuffers(1, &m->texCoordVBO);
            glBindBuffer(0x8892, m->texCoordVBO);
            glBufferData(0x8892, vcount << 3, m->texCoords, 0x88e4);
            m->vboByteSize += (int) (vcount * 8);

            glGenBuffers(1, &m->indexVBO);
            glBindBuffer(0x8893, m->indexVBO);
            glBufferData(0x8893, (unsigned int) m->indexCount << 1, m->indices, 0x88e4);
            m->vboByteSize += (int) (m->indexCount * 2);
            flags = m->vertexFormat;
        }

        if (flags & 4) {
            glGenBuffers(1, &m->normalVBO);
            glBindBuffer(0x8892, m->normalVBO);
            glBufferData(0x8892, vcount * 0xc, m->normals, 0x88e4);
            m->vboByteSize += (int) (vcount * 0xc);
            if (Engine::enableShader != 0) {
                glGenBuffers(1, &m->tangentVBO);
                glBindBuffer(0x8892, m->tangentVBO);
                glBufferData(0x8892, vcount * 0xc, tanArr, 0x88e4);
                m->vboByteSize += (int) (vcount * 0xc);
                glGenBuffers(1, &m->binormalVBO);
                glBindBuffer(0x8892, m->binormalVBO);
                glBufferData(0x8892, vcount * 0xc, binArr, 0x88e4);
                m->vboByteSize += (int) (vcount * 0xc);
            }
        }

        if (m->vertexFormat & 8) {
            glGenBuffers(1, &m->colorVBO);
            glBindBuffer(0x8892, m->colorVBO);
            glBufferData(0x8892, vcount << 4, colArr, 0x88e4);
            m->vboByteSize += (int) (vcount * 0x10);
        }

        glBindBuffer(0x8892, 0);
        glBindBuffer(0x8893, 0);

        if (glGetError() == 0) {
            if (Engine::KeepRawMeshData == 0) {
                if (m->positions != 0) operator delete[](m->positions);
                m->positions = 0;
                if (m->texCoords != 0) operator delete[](m->texCoords);
                m->texCoords = 0;
                if (m->indices != 0) operator delete[](m->indices);
                m->indices = 0;
                if (m->normals != 0) operator delete[](m->normals);
                m->normals = 0;
                if (m->colors != 0) operator delete[](m->colors);
                m->colors = 0;
                if (m->tangents != 0) operator delete[](m->tangents);
                m->tangents = 0;
                if (m->binormals != 0) operator delete[](m->binormals);
                m->binormals = 0;
            }
            m->uploaded = 1;
            Engine::vboSize += m->vboByteSize;
            return 1;
        }

        if (m->uploaded != 0) {
            glDeleteBuffers(1, &m->positionVBO);
            glDeleteBuffers(1, &m->indexVBO);
            unsigned char f = m->vertexFormat;
            if (f & 2) {
                glDeleteBuffers(1, &m->texCoordVBO);
                f = m->vertexFormat;
            }
            if (f & 4) {
                glDeleteBuffers(1, &m->normalVBO);
                if (Engine::enableShader != 0) {
                    glDeleteBuffers(1, &m->tangentVBO);
                    glDeleteBuffers(1, &m->binormalVBO);
                }
            }
            if (m->vertexFormat & 8)
                glDeleteBuffers(1, &m->colorVBO);
        }
        m->vboByteSize = 0;
        return -1;
    }
}

namespace AbyssEngine {
    int MeshConvertToVBO(Mesh * mesh);

    int TransformConvertToVBO(Transform *t) {
        if (t != 0) {
            for (unsigned int i = 0; i < t->meshes.size_; ++i)
                MeshConvertToVBO(t->meshes.data_[i]);
            for (unsigned int i = 0; i < t->children.size_; ++i)
                TransformConvertToVBO(t->children.data_[i]);
        }
        return 1;
    }
}

namespace AbyssEngine {
    void ImageFontCheckString(ImageFont * /*font*/, const unsigned short * /*str*/, unsigned int count) {
        unsigned short i = 0;
        unsigned int v;
        do {
            v = (unsigned int) i;
            i = i + 1;
        } while (v < count);
    }
}

namespace AbyssEngine {
    int TextureCreateFromFile(Engine *engine, const char *path, ImageCallback cb, void *user,
                              unsigned int *outIds, bool flag, float scale) {
        TextureCreateFromFileIntern(engine, path, cb, user, outIds, scale,
                                    (AELoadedTexture *) 0, flag);
        return 1;
    }
}

namespace AbyssEngine {
    // ES 1.x fixed-function texture-environment entry point (no-op stub in the
    // binary); not declared by <GLES2/gl2.h>.
    extern "C" {
    void glTexEnvi(unsigned int t, unsigned int p, int v);
    }

    int ImageCreateFromFile(Engine *engine, const char *path, Image **out);

    void ImageRelease(Image * *slot);

    void SetFXMaterial(BlendMode /*mode*/) {
    }

    int GenerateCompressedTexture(Image * /*image*/) {
        return 1;
    }

    int TextureCreateFromFileIntern(Engine *engine, const char *path, void (*cb)(Image *, void *),
                                    void *user, unsigned int *outIds, float scale,
                                    AELoadedTexture *outTex, bool flag) {
        Image *imgPtr = 0;
        *outIds = 0;
        engine->lastGlError = glGetError();

        int result = ImageCreateFromFile(engine, path, &imgPtr);
        if (result != 1)
            return result;
        if (cb != 0)
            cb(imgPtr, user);

        glGenTextures(1, outIds);
        Image *img = imgPtr;
        int format = (int) img->format;

        if (format != 6) {
            glBindTexture(0xde1, *outIds);
            glPixelStorei(0xcf5, 1);
            if (Engine::enableShader == 0)
                glTexEnvi(0x2300, 0x2200, 0x2100);

            int wrap;
            if (Engine::clampTextures) {
                glTexParameteri(0xde1, 0x2802, 0x812f);
                wrap = 0x812f;
            } else {
                glTexParameteri(0xde1, 0x2802, 0x2901);
                wrap = 0x2901;
            }
            glTexParameteri(0xde1, 0x2803, wrap);

            if (img->hasMipmaps != 0) {
                if (Engine::AnisotropyValue > 0.0f) {
                    glTexParameterf(0xde1, 0x84fe, Engine::AnisotropyValue);
                    glTexParameteri(0xde1, 0x2801, 0x2703);
                } else {
                    glTexParameteri(0xde1, 0x2801, 0x2703);
                    glTexParameteri(0xde1, 0x2800, 0x2601);
                }
            } else {
                if (engine->linearFilterFlag != 0) {
                    glTexParameteri(0xde1, 0x2800, 0x2601);
                    glTexParameteri(0xde1, 0x2801, 0x2601);
                } else {
                    glTexParameteri(0xde1, 0x2800, 0x2600);
                    glTexParameteri(0xde1, 0x2801, 0x2600);
                }
            }

            if (Engine::clampTextures) {
                glTexParameteri(0xde1, 0x2800, 0x2601);
                glTexParameteri(0xde1, 0x2801, 0x2601);
            }

            int w = (int) img->width;
            int h = (int) img->height;
            switch (format) {
                case 1:
                    glTexImage2D(0xde1, 0, 0x1909, w, h, 0, 0x1909, 0x1401, img->data);
                    if (img->hasMipmaps != 0) glGenerateMipmap(0xde1);
                    break;
                case 2:
                    glTexImage2D(0xde1, 0, 0x1907, w, h, 0, 0x1907, 0x1401, img->data);
                    if (img->hasMipmaps != 0) glGenerateMipmap(0xde1);
                    break;
                case 3:
                    glTexImage2D(0xde1, 0, 0x1908, w, h, 0, 0x1908, 0x1401, img->data);
                    if (img->hasMipmaps != 0) glGenerateMipmap(0xde1);
                    break;
                case 4:
                    if (img->hasMipmaps == 0) {
                        glCompressedTexImage2D(0xde1, 0, 0x8c03, w, h, 0, (int) img->dataLen, img->data);
                    } else {
                        unsigned int ch = img->height;
                        unsigned int cw = img->width;
                        int level = 0;
                        for (unsigned int off = 0; off < img->dataLen;) {
                            int blocksWide = (int) (cw >> 3);
                            unsigned int rowSize = (2 * ch) & 0xfffffff8;
                            if (cw < 0x10)
                                blocksWide = 2;
                            if (ch < 8)
                                rowSize = 0x10;
                            unsigned int blockSz = rowSize * (unsigned int) blocksWide;
                            glCompressedTexImage2D(0xde1, level, 0x8c03, (int) cw, (int) ch, 0, (int) blockSz,
                                                   (unsigned char *) img->data + off);
                            off += blockSz;
                            unsigned int nh = 1;
                            unsigned int nw = 1;
                            if ((ch >> 1) > 1) nh = ch >> 1;
                            if ((cw >> 1) > 1) nw = cw >> 1;
                            ++level;
                            ch = nh;
                            cw = nw;
                        }
                    }
                    break;
                case 5:
                    if (img->hasMipmaps != 0) {
                        unsigned int ch = img->height;
                        unsigned int cw = img->width;
                        int level = 0;
                        for (unsigned int off = 0; off < img->dataLen;) {
                            int blockRows = (int) (ch >> 2);
                            unsigned int rowSize = (2 * cw) & 0xfffffff8;
                            if (ch < 8)
                                blockRows = 2;
                            if (cw < 8)
                                rowSize = 0x10;
                            unsigned int blockSz = rowSize * (unsigned int) blockRows;
                            glCompressedTexImage2D(0xde1, level, 0x8c02, (int) cw, (int) ch, 0, (int) blockSz,
                                                   (unsigned char *) img->data + off);
                            off += blockSz;
                            unsigned int nh = 1;
                            unsigned int nw = 1;
                            if ((ch >> 1) > 1) nh = ch >> 1;
                            if ((cw >> 1) > 1) nw = cw >> 1;
                            ++level;
                            ch = nh;
                            cw = nw;
                        }
                    } else {
                        glCompressedTexImage2D(0xde1, 0, 0x8c02, w, h, 0, (int) img->dataLen, img->data);
                    }
                    break;
                case 7:
                    if (img->hasMipmaps != 0) {
                        unsigned int ch = img->height;
                        unsigned int cw = img->width;
                        int level = 0;
                        for (unsigned int off = 0; off < img->dataLen;) {
                            unsigned int blockSz = ch * cw;
                            if (blockSz <= 0x10)
                                blockSz = 0x10;
                            glCompressedTexImage2D(0xde1, level, 0x8c93, (int) cw, (int) ch, 0, (int) blockSz,
                                                   (unsigned char *) img->data + off);
                            off += blockSz;
                            unsigned int nh = 1;
                            unsigned int nw = 1;
                            if ((ch >> 1) > 1) nh = ch >> 1;
                            if ((cw >> 1) > 1) nw = cw >> 1;
                            ++level;
                            cw = nw;
                            ch = nh;
                        }
                    } else {
                        glCompressedTexImage2D(0xde1, 0, 0x8c93, w, h, 0, (int) img->dataLen, img->data);
                    }
                    break;
                case 8:
                case 9:
                case 10: {
                    unsigned int glFmt = 0x83f0;
                    if (format == 9) glFmt = 0x83f2;
                    if (format == 10) glFmt = 0x83f3;
                    if (img->hasMipmaps != 0) {
                        unsigned int ch = img->height;
                        unsigned int cw = img->width;
                        int level = 0;
                        for (unsigned int off = 0; off < img->dataLen;) {
                            unsigned int blockSz = ch * cw;
                            if ((unsigned int) (format - 9) > 1) {
                                if (blockSz > 0xf)
                                    blockSz >>= 1;
                                else
                                    blockSz = (format == 8) ? 8U : 0x10U;
                            } else if (blockSz <= 0x10) {
                                blockSz = 0x10;
                            }
                            glCompressedTexImage2D(0xde1, level, glFmt, (int) cw, (int) ch, 0, (int) blockSz,
                                                   (unsigned char *) img->data + off);
                            off += blockSz;
                            unsigned int nh = 1;
                            unsigned int nw = 1;
                            if ((ch >> 1) > 1) nh = ch >> 1;
                            if ((cw >> 1) > 1) nw = cw >> 1;
                            ++level;
                            cw = nw;
                            ch = nh;
                        }
                    } else {
                        glCompressedTexImage2D(0xde1, 0, glFmt, w, h, 0, (int) img->dataLen, img->data);
                    }
                    break;
                }
                case 0xb:
                    if (img->hasMipmaps != 0) {
                        unsigned int ch = img->height;
                        unsigned int cw = img->width;
                        int level = 0;
                        for (unsigned int off = 0; off < img->dataLen;) {
                            unsigned int blockSz = (cw * ch) >> 1;
                            if (cw < 8)
                                blockSz = 8;
                            glCompressedTexImage2D(0xde1, level, 0x8d64, (int) cw, (int) ch, 0, (int) blockSz,
                                                   (unsigned char *) img->data + off);
                            off += blockSz;
                            unsigned int nh = 1;
                            unsigned int nw = 1;
                            if ((ch >> 1) > 1) nh = ch >> 1;
                            if ((cw >> 1) > 1) nw = cw >> 1;
                            ++level;
                            cw = nw;
                            ch = nh;
                        }
                    } else {
                        glCompressedTexImage2D(0xde1, 0, 0x8d64, w, h, 0, (int) img->dataLen, img->data);
                    }
                    break;
                default:
                    break;
            }
        } else if (Engine::enableShader != 0) {
            glBindTexture(0x8513, *outIds);
            glTexParameteri(0x8513, 0x2800, 0x2601);
            glTexParameteri(0x8513, 0x2801, 0x2601);
            int faceH = (int) ((unsigned int) img->height / 6U);
            int faceW = img->width;
            int faceBytes = 4 * faceH * faceW;
            unsigned char *pixels = (unsigned char *) img->data;
            glTexImage2D(0x8517, 0, 0x1908, faceW, faceH, 0, 0x1908, 0x1401, pixels);
            glTexImage2D(0x8516, 0, 0x1908, faceW, faceH, 0, 0x1908, 0x1401, pixels + faceBytes);
            glTexImage2D(0x8519, 0, 0x1908, faceW, faceH, 0, 0x1908, 0x1401, pixels + faceBytes * 2);
            glTexImage2D(0x8515, 0, 0x1908, faceW, faceH, 0, 0x1908, 0x1401, pixels + faceBytes * 3);
            glTexImage2D(0x851a, 0, 0x1908, faceW, faceH, 0, 0x1908, 0x1401, pixels + faceBytes * 4);
            glTexImage2D(0x8518, 0, 0x1908, faceW, faceH, 0, 0x1908, 0x1401, pixels + faceBytes * 5);
        }

        if (!flag) {
            int err = (int) glGetError();
            engine->lastGlError = (unsigned int) err;
            if (err != 0) {
                String tmp(path);
                engine->lastErrorPath = tmp;
                glDeleteTextures(1, outIds);
                ImageRelease(&imgPtr);
                return -4;
            }
            AELabelObject(0x1702, *outIds, path);
        } else if (outTex != 0) {
            outTex->valid = 1;
            outTex->isCube = (format == 6) ? 1 : 0;
            unsigned int byteSize;
            if (glGetError() != 0) {
                outTex->glId = 0xffffffff;
                outTex->byteSize = 0;
                glDeleteTextures(1, outIds);
                byteSize = outTex->byteSize;
            } else {
                AELabelObject(0x1702, *outIds, path);
                outTex->glId = *outIds;
                byteSize = img->dataLen;
                outTex->byteSize = byteSize;
                ++Engine::ImageCount;
            }
            engine->textureByteCounter += (int) byteSize;
        } else {
            AELoadedTexture *tex = new AELoadedTexture;
            tex->scale = scale;
            tex->valid = 1;
            tex->isCube = (format == 6) ? 1 : 0;
            tex->name.Set(path);
            unsigned int byteSize;
            if (glGetError() != 0) {
                glDeleteTextures(1, outIds);
                byteSize = 0;
                tex->byteSize = 0;
                tex->glId = 0xffffffff;
            } else {
                AELabelObject(0x1702, *outIds, path);
                tex->glId = *outIds;
                byteSize = img->dataLen;
                tex->byteSize = byteSize;
                ++Engine::ImageCount;
            }
            engine->textureByteCounter += (int) byteSize;
            PaintCanvas *canvas = (PaintCanvas *) engine->paintCanvas;
            ArrayAdd<AELoadedTexture *>(tex, canvas->cubeTextures);
            *outIds = canvas->cubeTextures.count - 1;
        }

        ImageRelease(&imgPtr);
        return 1;
    }
}

namespace AbyssEngine {
    String operator+(const long long &a, const String &b) {
        String result;
        result.ctor_longlong(a);
        result += b;
        return result;
    }
}

namespace AbyssEngine {
    void MeshRelease(Engine * engine, Mesh * *slot);

    void SpriteSystemRelease(Engine *engine, SpriteSystem **slot) {
        if (*slot == 0)
            return;

        void *p = (*slot)->posCpu;
        if (p != 0)
            operator delete[](p);
        (*slot)->posCpu = 0;

        p = (*slot)->sizeCpu;
        if (p != 0)
            operator delete[](p);
        (*slot)->sizeCpu = 0;

        MeshRelease(engine, &(*slot)->mesh);

        if (*slot != 0)
            operator delete((void *) *slot);
        *slot = 0;
    }
}

namespace AbyssEngine {
    int CurveCreate(void **src, unsigned short count, Curve **out) {
        Curve *curve = (Curve *) operator new(8);
        curve->entries = 0;
        *out = curve;
        curve->count = count;

        void *data = operator new[]((unsigned int) count << 2);
        curve->entries = data;
        memcpy(data, src, (unsigned int) count << 2);
        return 1;
    }
}

namespace AbyssEngine {
    bool operator==(const String &a, const String &b) {
        String tmp(a);
        int cmp = tmp.Compare_str(const_cast<String *>(&b));
        return cmp != 0;
    }
}

namespace AbyssEngine {
    void MeshReleaseIntern(Engine * /*engine*/, Mesh **slot) {
        if (*slot == 0)
            return;

        if ((*slot)->shared == 0) {
            if ((*slot)->uploaded != 0) {
                glDeleteBuffers(1, &(*slot)->positionVBO);
                glDeleteBuffers(1, &(*slot)->indexVBO);
                unsigned char flags = (*slot)->vertexFormat;
                if (flags & 2) {
                    glDeleteBuffers(1, &(*slot)->texCoordVBO);
                    flags = (*slot)->vertexFormat;
                }
                if (flags & 4) {
                    glDeleteBuffers(1, &(*slot)->normalVBO);
                    if (Engine::enableShader != 0) {
                        glDeleteBuffers(1, &(*slot)->tangentVBO);
                        glDeleteBuffers(1, &(*slot)->binormalVBO);
                    }
                }
                if ((*slot)->vertexFormat & 8)
                    glDeleteBuffers(1, &(*slot)->colorVBO);
            }

            if ((*slot)->indices != 0) operator delete[]((*slot)->indices);
            (*slot)->indices = 0;
            if ((*slot)->positions != 0) operator delete[]((*slot)->positions);
            (*slot)->positions = 0;
            if ((*slot)->texCoords != 0) operator delete[]((*slot)->texCoords);
            (*slot)->texCoords = 0;
            if ((*slot)->colors != 0) operator delete[]((*slot)->colors);
            (*slot)->colors = 0;
            if ((*slot)->normals != 0) operator delete[]((*slot)->normals);
            (*slot)->normals = 0;

            if (Engine::enableShader != 0) {
                if ((*slot)->tangents != 0) operator delete[]((*slot)->tangents);
                (*slot)->tangents = 0;
                if ((*slot)->binormals != 0) operator delete[]((*slot)->binormals);
                (*slot)->binormals = 0;
            }
        }

        if ((*slot)->animation != 0) {
            (*slot)->animation->~Transform();
            operator delete((void *) (*slot)->animation);
        }
        (*slot)->animation = 0;
        if (*slot != 0)
            operator delete((void *) *slot);
        *slot = 0;
    }
}

namespace AbyssEngine {
    String operator+(const String &a, const float &b) {
        String result(a);
        String num;
        num.ctor_float(b);
        result += num;
        return result;
    }
}

namespace AbyssEngine {
    int MeshConvertToVBOIntern(Mesh * mesh);
    int TransformConvertToVBO(Transform * t);

    int MeshConvertToVBO(Mesh *mesh) {
        int result = -4;
        if (mesh != 0 && Engine::vboSupported != 0) {
            if (mesh->uploaded != 0 || mesh->vboEligible == 0)
                return -4;
            MeshConvertToVBOIntern(mesh);
            TransformConvertToVBO(mesh->animation);
            result = 1;
        }
        return result;
    }
}

namespace AbyssEngine {
    void esMatrixMultiply(ESMatrix *out, ESMatrix *a, ESMatrix *b) {
        float local[4][4];
        const float (*A)[4] = a->m;
        const float (*B)[4] = b->m;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                local[i][j] = A[i][0] * B[0][j] + A[i][1] * B[1][j] + A[i][2] * B[2][j] +
                              A[i][3] * B[3][j];
            }
        }

        memcpy(out, local, 0x40);
    }
}

namespace AbyssEngine {
    int MeshReadData(Engine *engine, const unsigned int &handle, unsigned int flags, Mesh **slot,
                     Material *mat);

    void MeshRelease(Engine * engine, Mesh * *slot);

    int MeshCreateFromFile(Engine *engine, const char *path, Mesh **out, Material *mat) {
        int result = -4;
        if (engine && path) {

        Mesh *m = new Mesh();
        *out = m;
        m->material = mat;

        unsigned int handle = 0;
        if (AEFile::OpenRead(path, &handle)) {
            char magic[8] = "*******";
            if (AEFile::Read((uint32_t)(7), magic, handle)) {
                static const char sigMesh[7] = {'A', 'E', 'M', 'e', 's', 'h', 0};
                static const char sigV2[7] = {'V', '2', 'A', 'E', 'M', 'e', 's'};
                static const char sigV3[7] = {'V', '3', 'A', 'E', 'M', 'e', 's'};
                static const char sigV4[7] = {'V', '4', 'A', 'E', 'M', 'e', 's'};
                static const char sigV5[7] = {'V', '5', 'A', 'E', 'M', 'e', 's'};
                unsigned int fmt = 0x1f;
                for (int i = 0; i < 7; ++i) {
                    char ch = magic[i];
                    if (sigMesh[i] != ch) fmt &= ~4u;
                    if (sigV2[i] != ch) fmt &= ~1u;
                    if (sigV3[i] != ch) fmt &= ~2u;
                    if (sigV4[i] != ch) fmt &= ~8u;
                    if (sigV5[i] != ch) fmt &= ~0x10u;
                }

                if (fmt) {
                    timeBetweenFrames = 1000000.0f;
                    if (((fmt & 0x1b) == 0 || AEFile::Read((uint32_t)(2), magic, handle)) &&
                        AEFile::Read((uint32_t)(1), &(*out)->vertexFormat, handle) &&
                        (*out)->vertexFormat) {
                        
                        if ((fmt & 0x1a) != 0) {
                            unsigned short subCount;
                            if (AEFile::Read((uint32_t)(2), &subCount, handle)) {
                                if (subCount >= 2) {
                                    (*out)->animation = new Transform();
                                    for (unsigned int s = 0; s < subCount; ++s) {
                                        Mesh *childPtr = new Mesh();
                                        childPtr->vboEligible = 1;
                                        childPtr->vertexFormat = (*out)->vertexFormat;
                                        childPtr->material = mat;
                                        if (MeshReadData(engine, handle, fmt, &childPtr, mat) == -1) {
                                            goto failed;
                                        }
                                        ((AEMath::BSphere *) &(*out)->boundsCenterX)->Merge(
                                            *(const AEMath::BSphere *) &childPtr->boundsCenterX);
                                        ArrayAdd<Mesh *>(childPtr, (*out)->animation->meshes);
                                    }
                                    goto loaded;
                                }
                                if (MeshReadData(engine, handle, fmt, out, mat) != -1)
                                    goto loaded;
                            }
                        } else if (MeshReadData(engine, handle, fmt, out, mat) != -1) {
                            goto loaded;
                        }
                    }
                }
            }
        failed:
            MeshRelease(engine, out);
            AEFile::Close(handle);
        } else {
            if (*out != 0)
                ::operator delete((void *) *out);
            *out = 0;
        }
        return -1;

    loaded:
        AEFile::Close(handle);
        Transform *xf = (*out)->animation;
        if (xf != 0) {
            xf->CollectAnimationData();
            xf->SetAnimationRangeInTime((long long) timeBetweenFrames, 10000000LL);
        }
        return 1;
        }
        return result;
    }
}

namespace AbyssEngine {
    int CameraIsSphereinViewFrustum(const Vector &center, float radius, Matrix *extra, Camera *cam) {
        if (!(radius != 0.0f && *g_Camera_frustumEnabledFlag != 0))
            return 1;

        Matrix local;
        Matrix transformed;
        Vector pos, dir, axis;
        Vector camPoint = {0.0f, 0.0f, 0.0f};

        Matrix *srcMatrix = &cam->projection;
        Matrix *dstMatrix;
        if (extra == 0) {
            dstMatrix = &local;
        } else {
            memcpy(&local, srcMatrix, 0x3c);
            local *= *extra;
            dstMatrix = &transformed;
            srcMatrix = &local;
        }

        *(Vector *) dstMatrix = AEMath::MatrixInverseTransformVector(*srcMatrix, *(Vector *) dstMatrix);
        camPoint = *(Vector *) dstMatrix;

        pos = AEMath::MatrixGetPosition(*dstMatrix);
        pos -= center;
        dir = AEMath::MatrixGetDir(*dstMatrix);
        axis -= dir;
        Vector normAxis = AEMath::VectorNormalize(axis);
        float fwd = AEMath::VectorDot(pos, normAxis);

        if (fwd > cam->position[2] + radius)
            return 0;
        if (fwd < cam->position[1] - radius)
            return 0;

        axis = AEMath::MatrixGetRight(*dstMatrix);
        normAxis = AEMath::VectorNormalize(axis);
        float right = AEMath::VectorDot(pos, normAxis);

        float rightPad = cam->frustumInvCosX * radius;
        float rightLimit = fwd * cam->frustumTanHalfFov * cam->frustumAspect;
        if (right > rightLimit + rightPad)
            return 0;
        if (right < -rightLimit - rightPad)
            return 0;

        axis = AEMath::MatrixGetUp(*dstMatrix);
        normAxis = AEMath::VectorNormalize(axis);
        float up = AEMath::VectorDot(pos, normAxis);

        float upPad = cam->frustumInvCosY * radius;
        float upLimit = fwd * cam->frustumTanHalfFov;
        if (up > upLimit + upPad)
            return 0;
        if (up < -upLimit - upPad)
            return 0;

        return 1;
    }
}

namespace AbyssEngine {
    struct CurveRec {
        Curve *curve;
    };

    long long CurveGetValue(unsigned long long time, Curve *curve) {
        CurveKeyframe **entries = (CurveKeyframe **) curve->entries;
        unsigned short count = curve->count;

        unsigned int qlo = (unsigned int) time;
        unsigned int qhi = (unsigned int) (time >> 32);

        auto keyTimeHi = [&](CurveKeyframe *kf) -> unsigned int { return kf->timeHi; };
        auto keyTimeLo = [&](CurveKeyframe *kf) -> unsigned int { return kf->timeLo; };
        auto keyVal = [&](CurveKeyframe *kf) -> int { return kf->value; };

        if (count == 0)
            return 0;

        CurveKeyframe *first = entries[0];
        if ((unsigned long long) time <
            (((unsigned long long) keyTimeHi(first) << 32) | keyTimeLo(first))) {
            return keyVal(first);
        }

        unsigned int idx = (unsigned int) (unsigned short) (count - 1);
        CurveKeyframe *last = entries[idx];
        if ((unsigned long long) time >
            (((unsigned long long) keyTimeHi(last) << 32) | keyTimeLo(last))) {
            return keyVal(last);
        }

        CurveKeyframe *seg = last;
        unsigned int segIdx = idx;
        while (segIdx > 0) {
            seg = entries[segIdx];
            unsigned long long st = ((unsigned long long) keyTimeHi(seg) << 32) | keyTimeLo(seg);
            if (st <= (unsigned long long) time)
                break;
            --segIdx;
        }

        char tag = (char) seg->tag;
        unsigned long long segStart =
                ((unsigned long long) keyTimeHi(seg) << 32) | keyTimeLo(seg);
        int v0 = keyVal(seg);

        if (tag == 1) {
            return v0;
        }

        CurveKeyframe *nextKf = entries[segIdx + 1];
        unsigned long long segEnd =
                ((unsigned long long) keyTimeHi(nextKf) << 32) | keyTimeLo(nextKf);
        int v1 = keyVal(nextKf);

        unsigned int num = (unsigned int) ((time - segStart)) << 12;
        unsigned int numHi = (unsigned int) (((time - segStart)) >> 20);
        unsigned int den = (unsigned int) (segEnd - segStart);
        long long tq = (((unsigned long long) (numHi) << 32 | (unsigned) (num)) /
                        ((unsigned long long) ((unsigned int) ((segEnd - segStart) >> 32)) << 32 |
                         (unsigned) (den)));
        unsigned int t = (unsigned int) tq;

        if (tag == 2) {
            long long d = (long long) (v1 - v0) * (unsigned int) t;
            return (int) ((unsigned int) (d >> 12)) + v0;
        }

        int c0 = seg->c0;
        int c1 = seg->c1;
        unsigned long long t2_64 = (unsigned long long) t * t;
        unsigned int t2 = (unsigned int) (t2_64 >> 12);
        unsigned long long t3_64 = (unsigned long long) t2 * t;
        unsigned int t3 = (unsigned int) (t3_64 >> 12);

        long long term0 = (long long) c0 * (int) (t - 2 * t2 + t3);
        long long term1 = (long long) c1 * (int) (t3 - t2);
        long long term2 = (long long) (v1 - v0) * (int) (3 * t2 - 2 * t3);
        int value = v0 + (int) (term0 >> 12) + (int) (term1 >> 12) + (int) (term2 >> 12);
        return value;
    }
}

namespace AbyssEngine {
    void computeFloatString(float value, int intValue, int *precision, int *exponentOut, int extra) {
        unsigned short *buf = (unsigned short *) operator new[](0x42);

        int prec = *precision & ~(*precision >> 31);
        if (prec > 0x1d)
            prec = 0x1e;

        int sign = 0;
        float v = (float) intValue;
        if (v < 0.0f) {
            v = -v;
            intValue = (int) v;
            sign = 1;
        }

        (void) sign;

        float ip = (float) (int) v;
        float frac = (float) intValue - ip;

        int digitCount = 0;
        unsigned short *w = buf;
        if ((int) v != 0) {
            unsigned short tmp[40];
            int n = 0;
            float cur = ip;
            while (cur != 0.0f) {
                float q = cur / 10.0f;
                float qi = (float) (int) q;
                int digit = (int) (((double) (q - qi)) * 10.0 + 0.5);
                tmp[n++] = (unsigned short) (0x30 + digit);
                cur = qi;
                ++digitCount;
            }
            for (int i = n - 1; i >= 0; --i)
                *w++ = tmp[i];
        } else {
            digitCount = 0;
            if (frac != 0.0f) {
                float f = frac;
                while (f * 10.0f < 1.0f) {
                    --digitCount;
                    f = f * 10.0f;
                }
            }
        }

        *exponentOut = digitCount;

        int limit = (extra == 0) ? (prec + (digitCount > 0 ? digitCount : 0)) : prec;
        float f = frac;
        int produced = 0;
        while (produced < limit && (w - buf) < 0x20) {
            float scaled = f * 10.0f;
            float di = (float) (int) scaled;
            f = scaled - di;
            *w++ = (unsigned short) (0x30 + (int) di);
            ++produced;
        }

        if (w > buf) {
            unsigned short *p = w - 1;
            unsigned short d = (unsigned short) (*p + 5);
            *p = d;
            while (d > 0x39) {
                *p = 0x30;
                if (p > buf) {
                    --p;
                    d = (unsigned short) (*p + 1);
                    *p = d;
                } else {
                    *p = 0x31;
                    ++digitCount;
                    *exponentOut = digitCount;
                    d = 0x31;
                    break;
                }
            }
        }

        *w = 0;
    }
}

namespace AbyssEngine {
    void MeshRelease(Engine * engine, Mesh * *slot);

    void TransformRelease(Engine *engine, Transform **slot) {
        Transform *t = (Transform *) *slot;
        if (t == 0)
            return;

        for (unsigned int i = 0; i < t->children.size_; ++i) {
            TransformRelease(engine, &t->children.data_[i]);
            t = (Transform *) *slot;
        }

        for (unsigned int i = 0; i < t->meshes.size_; ++i) {
            MeshRelease(engine, &t->meshes.data_[i]);
            t = (Transform *) *slot;
        }
    }
}

int GetStringLength(AbyssEngine::String str) {
    const unsigned short *p = str.GetAEWChar();
    while (*p)
        ++p;
    return static_cast<int>(p - str.GetAEWChar());
}
