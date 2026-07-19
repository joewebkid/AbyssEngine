

#include <GLES2/gl2.h>
#include <GLES/gl.h>
#include "engine/core/AbyssEngine.h"
#include "engine/math/AEMath.h"
#include "engine/render/FBOContainer.h"
#include "engine/render/Engine.h"
AbyssEngine::Engine *gEngine = nullptr;

bool AbyssEngine::Engine::vboSupported = false;
bool AbyssEngine::Engine::clampTextures = false;
bool AbyssEngine::Engine::vfc = false;
float AbyssEngine::Engine::lodBiasDiffuse = 0.0f;
float AbyssEngine::Engine::lodBiasNormal = 0.0f;
unsigned int AbyssEngine::Engine::countryCode = 0;
bool AbyssEngine::Engine::EnablePostEffect = false;
bool AbyssEngine::Engine::CheckForOrientationChange;
bool AbyssEngine::Engine::fogEnabled = false;
bool AbyssEngine::PostEffectFlag;
#include "engine/core/ApplicationManager.h"
#include "engine/core/NFC.h"
#include "engine/file/AEFile.h"
#include "engine/file/FileInterfaceAndroid.h"
#include "game/core/String.h"
#include "engine/render/Mesh.h"
#include "engine/render/Material.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/ShaderBaseStruct.h"
#include <arm_neon.h>
#include <cstdarg>
#include <cstring>

// OpenGL ES 1.x fixed-function entry points. The binary provides these as
// internal no-op stubs (they are NOT imported from the GL driver), so they are
// absent from <GLES2/gl2.h> and are declared locally. All ES 2.0 functions used
// here (glColorMask, glDepthFunc, glGetString, glDrawElements, glDrawArrays,
// glGetIntegerv, glLineWidth, glCullFace, glGetError, ...) come from gl.h.
void FBOContainer_ActivateRender2Texture(AbyssEngine::FBOContainer * self);
void FBOContainer_ActivateTexture(AbyssEngine::FBOContainer * self);
void FBOContainer_DeactivateRender2Texture(AbyssEngine::FBOContainer * self);

void ShaderUpdateRimColor();

void ShaderUpdateMaterialColor();

void ShaderCtor_0(void *);

void ShaderCtor_1(void *);

void ShaderCtor_2(void *);

void ShaderCtor_3(void *);

void ShaderCtor_4(void *);

static String *g_Engine_vendorString = nullptr;
static String *g_Engine_rendererString = nullptr;

namespace {
    int g_Engine_useShaders;
    int g_Engine_supportsFBO;

    int g_Engine_defaultShader;
    int g_Engine_altShader;
    int g_Engine_lineShader;
    int g_Engine_cloakShader;
    int g_Engine_currentShader;
    int g_Engine_activeShader;
    int g_Engine_shaderDrew;
    int g_Engine_shaderDirty;

    int g_Engine_shaderPostA;
    int g_Engine_shaderPostB;
    int g_Engine_shaderPostC;

    float g_Engine_texEnv;
    int g_Engine_texEnvDirty;

    uint32_t g_Engine_postEffectBW;
    uint32_t g_Engine_postEffectBlur;
    int g_Engine_postEffectFlag;
    int g_Engine_postEffectCounter;
    int g_Engine_postEffectPending;

    // Layout of a PaintCanvas texture-table entry (pointed to by
    // PaintCanvas::field_0x14[index]). Only the fields touched here are named;
    // the gap preserves the original byte offsets.
    struct TextureEntry {
        uint32_t glTexture;      // 0x00 GL texture id
        unsigned char pad_04[0x10 - 0x04];
        float texEnv;            // 0x10 GL_TEXTURE_ENV blend value
        unsigned char isCube;    // 0x14 0 => GL_TEXTURE_2D, else cube map
    };
#if __SIZEOF_POINTER__ == 4
    static_assert(__builtin_offsetof(TextureEntry, glTexture) == 0x00, "TextureEntry::glTexture");
    static_assert(__builtin_offsetof(TextureEntry, texEnv) == 0x10, "TextureEntry::texEnv");
    static_assert(__builtin_offsetof(TextureEntry, isCube) == 0x14, "TextureEntry::isCube");
#endif
}

void MeshRelease(Engine *self, void *meshSlot);

void MeshCreate(Engine *self, int vertices, int faces, int flags, void *outMesh);

void esMatrixMultiply(void *out, const void *lhs, const void *rhs);

void glError() {
    glGetError();
}

double *Engine::GetAccelValue() {
    double x = this->accelRaw[0];
    double y;
    if (this->appManager->paintCanvas->gameOrientation == 1) {
        x = -x;
        y = -this->accelRaw[1];
    } else {
        y = this->accelRaw[1];
    }
    this->accelValue[0] = x;
    this->accelValue[1] = y;
    this->accelValue[2] = this->accelRaw[2];
    return (double *) this->accelValue;
}

void Engine::ActivateRender2FracFBO() {
    FBOContainer *fbo = this->refractFBO;
    if (fbo != 0) {
        return FBOContainer_ActivateRender2Texture(fbo);
    }
}

uint32_t Engine::Resume() {
    this->appManager->paintCanvas->Resume();
    for (int index = 0; index != 0x14; index += 1) {
        this->boundTextures[index] = -1;
    }
    return 1;
}

uint32_t Engine::Suspend() {
    this->appManager->paintCanvas->Suspend();
    return 1;
}

uint32_t Engine::GetDisplayWidth() {
    return this->displayWidth;
}

double *Engine::GetGravValue() {
    double x = this->gravRaw[0];
    double y;
    if (this->appManager->paintCanvas->gameOrientation == 1) {
        x = -x;
        y = -this->gravRaw[1];
    } else {
        y = this->gravRaw[1];
    }
    this->gravValue[0] = x;
    this->gravValue[1] = y;
    this->gravValue[2] = this->gravRaw[2];
    return (double *) this->gravValue;
}

uint32_t Engine::GetDisplayHeight() {
    return this->displayHeight;
}

void Engine::LightSetRimColor(float red, float green, float blue) {
    if (g_Engine_useShaders == 0) {
        return;
    }
    this->rimColor.x = red;
    this->rimColor.y = green;
    this->rimColor.z = blue;
    return ShaderUpdateRimColor();
}

bool Engine::IsPostEffectActivated() {
    return this->postEffectFlags != 0;
}

void Engine::SetUVMatrix(const Matrix &matrix) {
    if (g_Engine_useShaders == 0) {
        glMatrixMode(0x1702);
        MatrixGetGL(matrix, this->uvMatrixGL);
        glLoadMatrixf(this->uvMatrixGL);
        return glMatrixMode(0x1700);
    }

    const float *m = matrix;
    float m0 = m[0];
    float m1 = m[1];
    float m2 = m[2];
    float m3 = m[3];
    float m4 = m[4];
    float m5 = m[5];
    float m6 = m[6];
    float m7 = m[7];
    float m8 = m[8];
    float m9 = m[9];
    float m10 = m[10];
    float m11 = m[11];

    float *uv = this->uvMatrix;
    uv[0] = m0;
    uv[1] = m4;
    uv[2] = m8;
    uv[3] = 0.0f;
    uv[4] = m1;
    uv[5] = m5;
    uv[6] = m9;
    uv[7] = 0.0f;
    uv[8] = m2;
    uv[9] = m6;
    uv[10] = m10;
    uv[11] = 0.0f;
    uv[12] = m3;
    uv[13] = m7;
    uv[14] = m11;
    uv[15] = 1.0f;
}

void Engine::ActivateRender2TextureFBO() {
    FBOContainer *fbo = this->postEffectFBO;
    if (fbo != 0) {
        return FBOContainer_ActivateRender2Texture(fbo);
    }
}

typedef void Materialfv(unsigned int face, unsigned int pname, const void *params);

static Materialfv *volatile g_Engine_glMaterialfv;

void Engine::LightSetMaterialColorAlpha(float alpha) {
    if (this->lightingEnabled == 0) {
        return;
    }

    this->materialAmbient[3] = alpha;
    this->materialAlpha = alpha;
    Materialfv *materialfv = g_Engine_glMaterialfv;
    materialfv(0x408, 0x1200, this->materialAmbient);
    this->materialSpecular[3] = this->materialAlpha;
    materialfv(0x408, 0x1202, this->materialSpecular);
    this->materialDiffuse[3] = this->materialAlpha;
    return materialfv(0x408, 0x1201, this->materialDiffuse);
}

void Engine::SetAccelValue(double x, double y, double z) {
    this->accelRaw[0] = x;
    this->accelRaw[1] = y;
    this->accelRaw[2] = z;
}

void Engine::ResetUVMatrix() {
    if (g_Engine_useShaders != 0) {
        uint32_t one = 0x3f800000;
        uint32x4_t zero = vdupq_n_u32(0);
        uint32_t *uv = (uint32_t *) this->uvMatrix;
        uv[0] = one;
        uv[5] = one;
        uv[10] = one;
        uv[15] = one;
        vst1q_u32(uv + 1, zero);
        vst1q_u32(uv + 6, zero);
        vst1q_u32(uv + 11, zero);
        return;
    }

    glMatrixMode(0x1702);
    glLoadIdentity();
    glScalef(1.0f, -1.0f, 1.0f);
    return glMatrixMode(0x1700);
}

void Engine::ActivateTextureFBO() {
    FBOContainer *fbo = this->postEffectFBO;
    if (fbo != 0) {
        return FBOContainer_ActivateTexture(fbo);
    }
}

void Engine::GlowEndGlow() {
    if (g_Engine_useShaders == 0) {
        return;
    }
    this->glowActive = 0;
    glColorMask(1, 1, 1, 1);
    return glDepthFunc(0x201);
}

void Engine::ActivateViewBuffer() {
    glBindFramebuffer(0x8d40, this->viewFramebuffer);
    return glViewport(0, 0, this->viewportWidth, this->viewportHeight);
}

void Engine::GlowEnableGlow() {
    if (this->glowActive != 0) {
        return;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(0x4000);
    this->glowActive = 1;
}

void Engine::SetOnDestroyApp(DestroyCallback *callback) {
    this->onDestroyCallback = callback;
}

void Engine::SetGravValue(double x, double y, double z) {
    this->gravRaw[0] = x;
    this->gravRaw[1] = y;
    this->gravRaw[2] = z;
}

void Engine::SwapBuffer() {
    uint32_t index = 0;
    while (index < this->triangleCounts->size()) {
        (*this->triangleCounts)[index] = 0;
        index += 1;
    }
}

void Engine::ReloadShaders() {
    uint32_t index = 0;
    while (index < this->shaders->size()) {
        ShaderBaseStruct *shader = (*this->shaders)[index];
        shader->DeleteShader();

        shader = (*this->shaders)[index];
        shader->Init(this);
        index += 1;
    }
}

void Engine::DeactivateRender2TextureFBO() {
    FBOContainer *fbo = this->postEffectFBO;
    if (fbo != 0) {
        return FBOContainer_DeactivateRender2Texture(fbo);
    }
}

DeviceInfo Engine::GetDeviceInfo() {
    DeviceInfo info;
    info.isPad = NFC().isPad();
    info.width = NFC().getWidth();
    info.height = NFC().getHeight();
    return info;
}

void Engine::CopyFBO() {
    if (g_Engine_useShaders == 0) {
        return;
    }

    if (this->IsPostEffectActivated()) {
        this->DeactivateRender2TextureFBO();
        this->DrawCloakFBO(this->postEffectFBO);
        this->ActivateRender2TextureFBO();
    } else {
        this->DeactivateRender2FracFBO();
        this->DrawCloakFBO(this->refractFBO);
        this->ActivateViewBuffer();
    }

    glEnable(0xb71);
    glDepthMask(1);
    glDisable(0xbe2);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return glClear(0x100);
}

bool Engine::HasVibration() {
    if (this->hasVibration) {
        return this->vibrationSupported != 0;
    }
    return false;
}

void Engine::LightSetLightCount(int count) {
    if (count >= 8) {
        count = 8;
    }
    count &= ~(count >> 31);
    this->lightCount = count;
}

void Engine::SetAddData(void *data, int size) {
    this->addData = data;
    this->addDataSize = size;
}

void Engine::ShaderUpdate() {
    uint32_t index = 0;
    while (index < this->shaders->size()) {
        (*this->shaders)[index]->Update();
        index += 1;
    }
}

bool Engine::IsExtensionSupported(const char *extension) {
    const char *extensions = (const char *) glGetString(0x1f03);

    uint32_t allLength = 0;
    while (extensions[allLength] != 0) {
        allLength += 1;
    }

    uint32_t extLength = 0;
    while (extension[extLength] != 0) {
        extLength += 1;
    }

    uint32_t index = 0;
    while (index < allLength) {
        if (extensions[index] == extension[0]) {
            const char *cur = extensions + index;
            uint32_t offset = 0;
            while (offset < extLength && index + offset < allLength &&
                   extension[offset] == cur[offset]) {
                offset += 1;
                if (offset == extLength) {
                    return true;
                }
            }
        }
        index += 1;
    }
    return false;
}

void Engine::LightSetMaterialColorShininess(float shininess) {
    this->materialShininess = shininess;
    if (g_Engine_useShaders == 0) {
        return glMaterialf(0x408, 0x1601, shininess);
    }
}

void Engine::Initialize(InitializeCallback *callback) {
    if (callback != 0) {
        callback(this);
    }
}

void Engine::ActivateRefractFBO() {
    FBOContainer *fbo = this->refractFBO;
    if (fbo != 0) {
        return FBOContainer_ActivateTexture(fbo);
    }
}

void Engine::LightSetParticleAmbient(float red, float green, float blue) {
    this->particleAmbient.x = red;
    this->particleAmbient.y = green;
    this->particleAmbient.z = blue;
}

void Engine::DeactivateRender2FracFBO() {
    FBOContainer *fbo = this->refractFBO;
    if (fbo != 0) {
        return FBOContainer_DeactivateRender2Texture(fbo);
    }
}

void Engine::SetPerspMatrix(float *matrix) {
    if (g_Engine_useShaders == 0) {
        return;
    }
    float *proj = this->projMatrix;
    for (int i = 0; i < 16; i += 1) {
        proj[i] = matrix[i];
    }
}

void Engine::SetFrameBufferTexture(int slot0, int slot1) {
    int firstValue = this->frameBufferTextures[slot0];
    if (firstValue != -1) {
        glActiveTexture(0x84c0);
        glBindTexture(0xde1, firstValue);
    }

    if (slot1 == -1) {
        return;
    }
    int secondValue = this->frameBufferTextures[slot1];
    if (secondValue == -1) {
        return;
    }
    glActiveTexture(0x84c1);
    return glBindTexture(0xde1, secondValue);
}

void Engine::LightSetLightDirection(float x, float y, float z, unsigned int light) {
    unsigned int index = light - 0x4000;
    if (index < 8) {
        int count = light - 0x3fff;
        int current = this->lightCount;
        if (current > count) {
            count = current;
        }
        this->lightCount = count;

        Vector input;
        input.x = x;
        input.y = y;
        input.z = z;
        Vector normalized = AbyssEngine::AEMath::VectorNormalize(input);
        (&this->field_0x468)[index] = normalized;
        this->lightDirty[index] = 0.0f;
    }
    return;
}

void Engine::RenderMesh(Mesh *mesh) {
    if (mesh == 0 || mesh->indexCount == 0) {
        goto done;
    }

    if (g_Engine_useShaders == 0) {
        glVertexPointer(3, 0x1406, 0, mesh->positions);
        this->AEClientState(0x8074, true);
        bool tex = ((uint32_t) mesh->vertexFormat << 30) < 0;

        if (tex && (mesh->material == 0 ||
                    ((Material *) mesh->material)->textures[1] == -1)) {
            glTexCoordPointer(2, 0x1406, 0, mesh->texCoords);
        }
        this->AEClientState(0x8078, tex);
        bool normals = ((uint32_t) mesh->vertexFormat << 29) < 0;
        if (normals) {
            glNormalPointer(0x1406, 0, mesh->normals);
        }
        this->AEClientState(0x8075, normals);
        bool colors = ((uint32_t) mesh->vertexFormat << 28) < 0;
        if (colors) {
            glColorPointer(4, 0x1406, 0, (void *) (uintptr_t) mesh->colors);
        }
        this->AEClientState(0x8076, colors);
        if (((uint32_t) mesh->vertexFormat << 27) < 0) {
            glDrawElements(4, mesh->indexCount, 0x1403,
                           mesh->indices);
        } else {
            glDrawArrays(4, 0, mesh->vertexCount);
        }

        if (tex && mesh->material != 0 &&
            ((Material *) mesh->material)->textures[1] != -1) {
            this->AEClientState(0x8078, false);
        }
    } else {
        g_Engine_shaderDrew = 0;
        this->ShaderSetActive(g_Engine_defaultShader, mesh);
        if (g_Engine_shaderDrew != 0) {
            int oldBuffer = 0;
            glGetIntegerv(0x8ca6, &oldBuffer);
            if (((uint32_t) mesh->vertexFormat << 27) < 0) {
                if (mesh->uploaded == 0) {
                    glDrawElements(4, mesh->indexCount, 0x1403,
                                   mesh->indices);
                } else {
                    glBindBuffer(0x8893, mesh->indexVBO);
                    glDrawElements(4, mesh->indexCount, 0x1403, 0);
                    glBindBuffer(0x8892, 0);
                    glBindBuffer(0x8893, 0);
                }
            } else {
                glDrawArrays(4, 0, mesh->vertexCount);
            }
            this->ShaderSetInActive();
        }
    }

done:
    return;
}

void Engine::DrawQuad(int x, int y, int width, int height) {
    float fx = (float) x;
    float fy = (float) y;
    float right = (float) (x + width);
    float bottom = (float) (height + y);

    Mesh *mesh = this->quadMesh;
    float *positions = (float *) mesh->positions;
    positions[0] = fx;
    positions[1] = fy;
    positions[3] = right;
    positions[4] = fy;
    positions[6] = right;
    positions[7] = bottom;
    positions[9] = fx;
    positions[10] = bottom;

    static const float uvs[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    float *uv = (float *) mesh->texCoords;
    float32x4_t uv0 = vld1q_f32(uvs);
    float32x4_t uv1 = vld1q_f32(uvs + 4);
    vst1q_f32(uv, uv0);
    vst1q_f32(uv + 4, uv1);

    return glDrawElements(4, mesh->indexCount, 0x1403,
                          mesh->indices);
}

void Engine::SetColor(float red, float green, float blue, float alpha) {
    Engine *self = this;
    if (self->glColor[0] == red &&
        self->glColor[1] == green &&
        self->glColor[2] == blue &&
        self->glColor[3] == alpha) {
        return;
    }
    self->glColor[1] = green;
    self->glColor[0] = red;
    self->glColor[2] = blue;
    self->glColor[3] = alpha;
    self->packedColor =
            (int) (green * 255.0f) * 0x10000 + (int) (red * 255.0f) * 0x1000000 +
            (int) (blue * 255.0f) * 0x100 + (int) (alpha * 255.0f);
    if (g_Engine_useShaders != 0) {
        return ShaderUpdateMaterialColor();
    }
    self->LightSetMaterialColorAlpha(alpha);
    return glColor4f(red, green, blue, alpha);
}

Engine::~Engine() {
    DestroyCallback *destroy = this->onDestroyCallback;
    if (destroy != 0) {
        destroy(this);
    }

    delete this->appManager;
    this->appManager = nullptr;

    delete (FileInterface *) this->fileInterface;
    this->fileInterface = 0;

    AEFile::Release();
    for (uint32_t i = 0; i < this->shaders->size(); ++i) {
        delete (*this->shaders)[i];
    }
    ArrayRemoveAll(*(this->shaders));

    delete this->postEffectFBO;
    this->postEffectFBO = 0;

    delete this->refractFBO;
    this->refractFBO = 0;

    MeshRelease(this, &this->quadMesh);
    this->ReleaseGL();
    delete this->shaders;
    delete this->triangleCounts;
}

void Engine::ReleaseGL() {
}

void Engine::AfterGLInit() {
    this->ResetLightParam();
    MeshCreate(this, 4, 2, 0x13, &this->quadMesh);

    uint32_t *indices = (uint32_t *) this->quadMesh->indices;
    indices[0] = 0x20000;
    indices[1] = 1;
    indices[2] = 0x30002;

    String vendor((const char *) glGetString(0x1f00));
    *g_Engine_vendorString = vendor;
    String renderer((const char *) glGetString(0x1f01));
    *g_Engine_rendererString = renderer;
}

void Engine::DrawCloakFBO(FBOContainer *fbo) {
    if (g_Engine_useShaders != 0) {
        ShaderBaseStruct *shader = (*this->shaders)[g_Engine_cloakShader];
        shader->RenderEffect(fbo, this);
    }
}

void Engine::ShaderRegister(ShaderBaseStruct *shader) {
    if (shader != 0) {
        String name = shader->GetShaderName();
        char *text = name.GetAEChar();

        shader->Init(this);
        ArrayAdd(shader, *(this->shaders));
        ArrayAdd(0, *(this->triangleCounts));
        ::operator delete(text);
    }
}

void Engine::SetTextureSlot(uint32_t textureIndex, uint32_t slot) {
    PaintCanvas *manager = this->appManager->paintCanvas;
    uint32_t count = manager->cubeTextures.count;
    if (count == 0 || slot >= 8 || textureIndex > count - 1) {
        return;
    }
    uint32_t *bound = (uint32_t *) &this->boundTextures[slot];

    TextureEntry *textureEntry = (TextureEntry *) manager->cubeTextures.data_[textureIndex];
    uint32_t texture = textureEntry->glTexture;
    if (*bound == texture) {
        return;
    }
    glActiveTexture(slot + 0x84c0);
    float env = textureEntry->texEnv;
    if (g_Engine_texEnv != env) {
        g_Engine_texEnv = env;
        if (g_Engine_useShaders == 0) {
            glTexEnvf(0x8500, 0x8501, env);
            textureEntry = (TextureEntry *) manager->cubeTextures.data_[textureIndex];
        } else if (g_Engine_texEnvDirty != 0) {
            g_Engine_texEnvDirty = 0;
        }
    }

    glBindTexture(textureEntry->isCube == 0 ? 0xde1 : 0x8513, texture);
    *bound = texture;
}

void Engine::AEClientState(unsigned int state, bool enable) {
    uint32_t bits = this->clientStateFlagsAE;
    uint32_t mask = 0;
    switch (state) {
        case 0x8074: mask = 2;
            break;
        case 0x8075: mask = 1;
            break;
        case 0x8076: mask = 8;
            break;
        case 0x8078: mask = 4;
            break;
        default: return;
    }
    if (enable) {
        if ((bits & mask) != 0) {
            return;
        }
        glEnableClientState(state);
        bits |= mask;
    } else {
        if ((bits & mask) == 0) {
            return;
        }
        glDisableClientState(state);
        bits &= ~mask;
    }
    this->clientStateFlagsAE = bits;
}

void Engine::GlowBeginGlow(int depthFunc) {
    if (this->glowActive != 0) {
        return;
    }
    if (g_Engine_useShaders == 0) {
        return;
    }
    glColorMask(0, 0, 0, 1);
    this->GlowEnableGlow();
    if (this->glowActive != 0) {
        return glDepthFunc(depthFunc);
    }
}

void Engine::DrawLine2D(float *verts, int count, bool strip) {
    this->lineVertexBase = count;
    this->ShaderSetActive(g_Engine_lineShader, 0);
    unsigned int mode = strip != 0 ? 2 : 1;
    return glDrawArrays(mode, 0, count);
}

void Engine::ShaderSetActive(int shaderIndex, Mesh *mesh) {
    while (shaderIndex == -1) {
        shaderIndex = g_Engine_defaultShader;
        if ((((uint32_t) mesh->vertexFormat) << 30) >= 0) {
            shaderIndex = g_Engine_altShader;
        }
    }

    bool hasExtra = mesh != 0 && mesh->hasAnimation != 0;
    ShaderBaseStruct *shader = (*this->shaders)[shaderIndex];
    if (shader == 0) {
        return;
    }
    g_Engine_shaderDirty = 1;

    if (shader->program != this->currentProgram) {
        shader->UseShader(hasExtra);
        shader = (*this->shaders)[shaderIndex];
        this->currentProgram = shader->program;
        g_Engine_currentShader = shaderIndex;
    }
    shader->UseShader(hasExtra);
    shader = (*this->shaders)[shaderIndex];
    shader->UpdateMeshData(mesh, this);
    if (mesh != 0) {
        uint32_t triangles = ((unsigned) (mesh->indexCount) / (unsigned) (3));
        (*this->triangleCounts)[shaderIndex] += triangles;
    }
}

void Engine::DoPostEffect() {
    uint32_t flags = this->postEffectFlags;
    FBOContainer *current = this->postEffectFBO;
    FBOContainer *other = this->refractFBO;
    if (g_Engine_useShaders != 0) {
        FBOContainer *slot = other;
        if ((flags & 2) != 0) {
            ShaderBaseStruct *shader = (*this->shaders)[g_Engine_shaderPostA];
            if ((flags & ~2u) == 0) {
                shader->RenderEffect(current, this);
                flags = 0;
            } else {
                shader->RenderEffect(current, other, this);
                slot = other;
                other = current;
            }
        }
        if ((this->postEffectFlags & 1) != 0) {
            ShaderBaseStruct *shader = (*this->shaders)[g_Engine_shaderPostB];
            if ((flags & ~1u) == 0) {
                shader->RenderEffect(slot, this);
                flags = 0;
            } else {
                shader->RenderEffect(slot, other, this);
                slot = other;
            }
        }
        if ((this->postEffectFlags & 4) != 0) {
            ShaderBaseStruct *shader = (*this->shaders)[g_Engine_shaderPostC];
            if ((flags & ~4u) == 0) {
                shader->RenderEffect(slot, this);
            } else {
                shader->RenderEffect(slot, other, this);
            }
        }
        if (g_Engine_postEffectFlag == 1) {
            this->SetPostEffect(g_Engine_postEffectBW, false);
        }
    }
    return;
}

void Engine::LightSetMaterialColorSpecular(float red, float green, float blue) {
    this->materialSpecular[0] = red;
    this->materialSpecular[1] = green;
    this->materialSpecular[2] = blue;
    this->materialSpecular[3] = this->materialAlpha;

    if (g_Engine_useShaders == 0) {
        return glMaterialfv(0x408, 0x1202, this->materialSpecular);
    }

    int count = this->lightCount;
    for (int index = 0; index < count; index += 1) {
        const LightColor &src = (&this->lightSpecular)[index];
        Vector &dst = (&this->lightSpecularShaded)[index];
        dst.x = src.r * red;
        dst.y = src.g * green;
        dst.z = src.b * blue;
    }
    return ShaderUpdateMaterialColor();
}

void Engine::LightSetGlobalSceneColorAmbient(float red, float green, float blue) {
    this->sceneAmbient[0] = red;
    this->sceneAmbient[1] = green;
    this->sceneAmbient[2] = blue;
    this->sceneAmbient[3] = 0x3f800000;

    if (g_Engine_useShaders == 0) {
        return glLightModelfv(0xb53, this->sceneAmbient);
    }

    int count = this->lightCount;
    for (int index = 0; index < count; index += 1) {
        const LightColor &src = (&this->lightAmbient)[index];
        Vector &dst = (&this->lightAmbientShaded)[index];
        dst.x = (src.r + red) * this->materialAmbient[0];
        dst.y = (src.g + green) * this->materialAmbient[1];
        dst.z = (src.b + blue) * this->materialAmbient[2];
    }
    return ShaderUpdateMaterialColor();
}

void Engine::SetPostEffect(uint32_t effect, bool enable) {
    if (this->postEffectFBO == 0 && enable) {
        this->postEffectFBO = new FBOContainer(this, String("posteffect"));
        int width;
        int height;
        if (this->appManager->paintCanvas->gameOrientation == 2) {
            width = this->displayWidth;
            height = this->displayHeight;
        } else {
            width = this->displayHeight;
            height = this->displayWidth;
        }
        this->postEffectFBO->Create(width, height, false, true);
    }

    uint32_t flags = this->postEffectFlags;
    if (effect == (uint32_t) g_Engine_postEffectBW) {
        if (enable) {
            if (g_Engine_postEffectCounter > 0) {
                g_Engine_postEffectCounter -= 1;
            } else {
                flags |= 4;
            }
        } else if (g_Engine_postEffectFlag == 1) {
            flags &= ~4u;
            g_Engine_postEffectFlag = 0;
            g_Engine_postEffectPending = 1;
        } else {
            g_Engine_postEffectFlag = g_Engine_postEffectCounter < 1;
        }
    } else if (effect == (uint32_t) g_Engine_postEffectBlur) {
        g_Engine_postEffectFlag = enable;
        flags = enable ? (flags | 2) : (flags & ~2u);
    } else if (effect == 0x1400000) {
        flags = enable ? (flags | 1) : (flags & ~1u);
    }
    this->postEffectFlags = flags;
    return;
}

void Engine::LightSetMaterialColorDiffuse(float red, float green, float blue) {
    this->materialDiffuse[0] = red;
    this->materialDiffuse[1] = green;
    this->materialDiffuse[2] = blue;
    this->materialDiffuse[3] = this->materialAlpha;

    if (g_Engine_useShaders == 0) {
        return glMaterialfv(0x408, 0x1201, this->materialDiffuse);
    }

    int lightCount = this->lightCount;
    for (int i = 0; i < lightCount; i += 1) {
        const LightColor &src = (&this->lightDiffuse)[i];
        Vector &dst = (&this->lightDiffuseShaded)[i];
        dst.x = src.r * red;
        dst.y = src.g * green;
        dst.z = src.b * blue;
    }
    return ShaderUpdateMaterialColor();
}

void Engine::initFileInterface() {
    void *fileInterface = new (operator new(0x38)) FileInterfaceAndroid();
    this->fileInterface = fileInterface;
    return AEFile::SetInterface((FileInterface *) fileInterface);
}

void Engine::SetOrthoMatrix(float *projection, float *view, bool multiply) {
    if (g_Engine_useShaders != 0) {
        float *proj = this->projMatrix;
        for (int i = 0; i < 16; i += 1) {
            proj[i] = projection[i];
        }
        if (multiply) {
            float local[16];
            memcpy(local, view, 0x40);
            esMatrixMultiply(this->projMatrix, local, this->projMatrix);
        }
    }
    return;
}

int Engine::InitGL(bool shaders, int width, int height) {
    this->refractFBO = 0;
    this->displayWidth = width;
    this->displayHeight = height;
    this->viewportWidth = width;
    this->viewportHeight = height;

    void *fileInterface = new (operator new(0x38)) FileInterfaceAndroid();
    this->fileInterface = fileInterface;
    AEFile::SetInterface((FileInterface *) fileInterface);

    this->field_0x10 = 0;
    this->vibrationSupported = 0;
    this->hasVibration = 0;
    g_Engine_useShaders = shaders;
    this->viewFramebuffer = 0;

    this->ResetLightParam();
    glViewport(0, 0, this->viewportHeight, this->viewportWidth);
    if (g_Engine_useShaders != 0) {
        this->ShaderInit();
    } else {
        glEnable(0x803a);
        glDisable(0xb50);
        glLineWidth(1.0f);
    }

    Vector value;
    value.x = 0.0f;
    value.y = 1.0f;
    value.z = 0.0f;
    this->field_0x468 = value;
    this->lightDirty[0] = 0.0f;
    this->field_0x474 = value;
    this->lightDirty[1] = 0.0f;

    glEnable(0xb71);
    this->GlEnable(0xde1, true);
    glDisable(0xbe2);
    glCullFace(0x405);
    glEnable(0xb44);
    this->AfterGLInit();
    this->appManager->paintCanvas->Initialize(false);
    this->depthBits = 0;
    glGetIntegerv(0xd33, (GLint *) &this->depthBits);

    if (g_Engine_useShaders != 0 && g_Engine_supportsFBO != 0) {
        FBOContainer *fbo = new FBOContainer(this, String("refract"));
        this->refractFBO = fbo;
        fbo->Create(this->displayWidth, this->displayHeight, false, true);
    }

    return 1;
}

void Engine::ClearBuffer(uint32_t color) {
    const double scale = 255.0;
    double red = (double) (color >> 24) / scale;
    double green = (double) ((color >> 16) & 0xff) / scale;
    double blue = (double) ((color >> 8) & 0xff) / scale;
    double alpha = (double) (color & 0xff) / scale;
    glClearColor((float) red, (float) green, (float) blue, (float) alpha);
    return glClear(0x4100);
}

void Engine::LightSetLightPosition(float x, float y, float z, unsigned int light) {
    unsigned int index = light - 0x4000;
    if (index < 8) {
        int count = light - 0x3fff;
        int current = this->lightCount;
        if (current > count) {
            count = current;
        }
        this->lightCount = count;

        Vector value;
        value.x = x;
        value.y = y;
        value.z = z;
        (&this->field_0x468)[index] = value;
        this->lightDirty[index] = 1.0f;
    }
    return;
}

void Engine::LightSetLightColorAmbient(float red, float green, float blue, unsigned int light) {
    unsigned int index = light - 0x4000;
    if (index > 7) {
        return;
    }
    int count = light - 0x3fff;
    int current = this->lightCount;
    if (current > count) {
        count = current;
    }
    this->lightCount = count;

    LightColor &src = (&this->lightAmbient)[index];
    src.r = red;
    src.g = green;
    src.b = blue;
    src.a = 1.0f;
    if (g_Engine_useShaders == 0) {
        return glLightfv(light, 0x1200, &src.r);
    }
    Vector &dst = (&this->lightAmbientShaded)[index];
    dst.x = (this->sceneAmbient[0] + red) * this->materialAmbient[0];
    dst.y = (this->sceneAmbient[1] + src.g) * this->materialAmbient[1];
    dst.z = (this->sceneAmbient[2] + src.b) * this->materialAmbient[2];
    return ShaderUpdateMaterialColor();
}

void Engine::ShaderSetInActive() {
    ShaderBaseStruct *shader = (*this->shaders)[g_Engine_activeShader];
    shader->SetInActive();
}

void Engine::LightSetLightColorDiffuse(float red, float green, float blue, unsigned int light) {
    unsigned int index = light - 0x4000;
    if (index > 7) {
        return;
    }
    int count = light - 0x3fff;
    int current = this->lightCount;
    if (current > count) {
        count = current;
    }
    this->lightCount = count;

    LightColor &src = (&this->lightDiffuse)[index];
    src.r = red;
    src.g = green;
    src.b = blue;
    src.a = 1.0f;
    if (g_Engine_useShaders == 0) {
        return glLightfv(light, 0x1201, &src.r);
    }
    Vector &dst = (&this->lightDiffuseShaded)[index];
    dst.x = this->materialDiffuse[0] * red;
    dst.y = src.g * this->materialDiffuse[1];
    dst.z = src.b * this->materialDiffuse[2];
    return ShaderUpdateMaterialColor();
}

Engine::Engine() {
    Engine * self = this;

    self->field_0x340 = 0;
    self->lightColor.x = 0;
    self->lightColor.z = 0;
    self->field_0x3cc = 0;
    self->field_0x3d4 = 0;
    Vector up;
    up.x = 0.0f;
    up.y = 0.0f;
    up.z = 0.0f;
    self->lightDir = up;
    self->triangleCounts = new Array<int>();
    self->field_0x478 = 0;
    self->field_0x400 = 0;
    self->field_0x468 = up;
    self->fogColor = up;
    self->shaders = new Array<ShaderBaseStruct *>();
    self->quadMesh = nullptr;
    self->viewFramebuffer = 0;
    self->postEffectFlags = 0;
    self->field_0x3c4 = 0;
    self->field_0x3c8 = 0;
    up.x = 0.5f;
    up.y = 0.0f;
    up.z = 0.0f;
    *(Vector *) &self->field_0x3cc = up;
    self->addData = 0;
    self->postEffectFBO = 0;
    self->refractFBO = 0;
    self->glowActive = 0;
    self->field_0x360 = 0;
    self->field_0x4a8 = 0;
    self->field_0x70 = 0;
    self->field_0x100 = 0;
    self->currentProgram = -1;
    for (int i = 0; i != 0x14; i += 1) {
        self->boundTextures[i] = -1;
    }
    self->frameBufferTextures[0] = -1;
    self->frameBufferTextures[1] = -1;
    self->clientStateFlagsAE = 0;
    self->field_0xfd = 0x100;
    self->field_0x78 = -1;
    self->glEnableFlags = 0;
    self->lightingEnabled = 0;
    self->onDestroyCallback = 0;
    self->rimColor.x = 0;
    self->rimColor.z = 0;
    self->field_0x34 = 0;
    self->accelRaw[2] = 0;
    self->displayWidth = 0;
    self->viewportWidth = 0;
    self->accelRaw[0] = 0;
    self->accelRaw[1] = 0;
    self->gravRaw[0] = 0;
    self->gravRaw[1] = 0;
    self->gravRaw[2] = 0;
    self->field_0x28 = 0x14;
    self->field_0x20 = 1;
    self->appManager = new ApplicationManager(self);
    self->fogMinDist = 0;
    self->fogMaxDist = 0;
    self->glColor[0] = -1.0f;
    self->glColor[1] = -1.0f;
    self->glColor[2] = -1.0f;
    self->glColor[3] = -1.0f;
    up.x = 1.0f;
    up.y = 0.0f;
    up.z = 0.0f;
    self->fogColor = up;
    self->initFileInterface();
    return;
}

void Engine::SetTextures(uint32_t first, uint32_t second) {
    PaintCanvas *manager = this->appManager->paintCanvas;
    uint32_t count = manager->cubeTextures.count;
    if (count == 0 || first > count - 1) {
        return;
    }
    this->SetTextureSlot(first, 0);
    if (second > count - 1) {
        if (this->boundTextures[1] != -1) {
            if (g_Engine_useShaders == 0) {
                glActiveTexture(0x84c1);
                glDisable(0xde1);
                glActiveTexture(0x84c0);
            }
            this->boundTextures[1] = -1;
        }
        return;
    }

    uint32_t texture = **(uint32_t **) (manager->cubeTextures.data_ + second);
    if (this->boundTextures[1] != texture) {
        glActiveTexture(0x84c1);
        this->GlEnable(0xde1, true);
        unsigned int target = (g_Engine_useShaders != 0 &&
                               (this->glEnableFlags & 0x80008) != 0)
                                  ? 0x8513
                                  : 0xde1;
        glBindTexture(target, texture);
        this->boundTextures[1] = texture;
    }
}

typedef void ShaderCtor(void *);

uint32_t Engine::ShaderInit() {
    static const uint32_t sizes[] = {
        0x8c, 0x2c, 0x94, 0x2c, 0x30, 0x74, 0x5c, 0x68, 0x44, 0x50,
        0x64, 0x58, 0x58, 0x30, 0x60, 0x84, 0x5c, 0x5c, 0x5c, 0x84,
        0x5c, 0xa0, 0x64, 0x34, 0x70, 0x98, 0x98, 0x5c, 0x5c, 0x60,
        0x58, 0x34, 0xa8, 0x60, 0x40, 0x6c, 0x64, 0x4c, 0x98,
    };
    ShaderCtor *ctors[] = {ShaderCtor_0, ShaderCtor_1, ShaderCtor_2, ShaderCtor_3, ShaderCtor_4};
    for (uint32_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i += 1) {
        void *shader = operator new(sizes[i]);
        ctors[i % 5](shader);
        this->ShaderRegister((ShaderBaseStruct *) shader);
    }
    glGetError();
    return 1;
}

uint64_t Engine::SetEyePosition(float x, float y, float z) {
    this->eyePosition.x = x;
    this->eyePosition.y = y;
    this->eyePosition.z = z;
    union {
        float f;
        uint32_t u;
    } cx, cy;
    cx.f = x;
    cy.f = y;
    return (uint64_t) cx.u | ((uint64_t) cy.u << 32);
}

void Engine::SetModelMatrix(const Matrix &matrix) {
    Engine * self = this;
    const float *m = matrix;
    if (g_Engine_useShaders != 0) {
        self->normalMatrix[0] = m[0];
        self->normalMatrix[1] = m[4];
        self->normalMatrix[2] = m[8];
        self->normalMatrix[3] = m[1];
        self->normalMatrix[4] = m[5];
        self->normalMatrix[5] = m[9];
        self->normalMatrix[6] = m[2];
        self->normalMatrix[7] = m[6];
        self->normalMatrix[8] = m[10];
        float gl[16] = {
            m[0], m[4], m[8], 0.0f,
            m[1], m[5], m[9], 0.0f,
            m[2], m[6], m[10], 0.0f,
            m[3], m[7], m[11], 1.0f,
        };
        memcpy(self->modelMatrixGL, gl, 0x40);
        Vector tmp;
        if (self->lightDirty[0] == 0.0f) {
            tmp = AbyssEngine::AEMath::MatrixInverseRotateVector(
                matrix, self->field_0x468);
            tmp = AbyssEngine::AEMath::VectorNormalize(tmp);
            self->lightDir = tmp;
        } else {
            self->lightDir = self->field_0x468;
        }
        if (self->lightCount > 1) {
            if (self->lightDirty[1] == 0.0f) {
                tmp = AbyssEngine::AEMath::MatrixInverseRotateVector(
                    matrix, self->field_0x474);
                tmp = AbyssEngine::AEMath::VectorNormalize(tmp);
                self->field_0x33c = tmp;
            } else {
                self->field_0x33c = self->field_0x474;
            }
        }
        self->ShaderUpdate();
        tmp = AbyssEngine::AEMath::MatrixInverseTransformVector(
            matrix, self->eyePosition);
        self->lightColor = tmp;
        self->lightColor.x /= m[12];
        self->lightColor.y /= m[13];
        self->lightColor.z /= m[14];
    }
    return;
}

void Engine::LightSetLight(unsigned int light) {
    uint32_t values[4] = {0, 0, 0, 0};
    unsigned int index = light - 0x4000;
    if (index < 8) {
        int count = light - 0x3fff;
        int current = this->lightCount;
        if (current > count) {
            count = current;
        }
        this->lightCount = count;

        const Vector &dir = (&this->field_0x468)[index];
        values[0] = *(const uint32_t *) &dir.x;
        values[1] = *(const uint32_t *) &dir.y;
        values[2] = *(const uint32_t *) &dir.z;
        values[3] = *(const uint32_t *) &this->lightDirty[index];
        if (g_Engine_useShaders == 0) {
            glLightfv(light, 0x1203, reinterpret_cast<const float *>(values));
        }
    }
    return;
}

void Engine::SetTexturesExt(uint32_t first, ...) {
    PaintCanvas *manager = this->appManager->paintCanvas;
    if (manager->cubeTextures.count != 0) {
        va_list args;
        va_start(args, first);
        uint32_t slot = 0;
        uint32_t textureIndex = first;
        while (textureIndex != 0xffffffff) {
            this->SetTextureSlot(textureIndex, slot);
            slot += 1;
            textureIndex = va_arg(args, uint32_t);
        }
        va_end(args);
        for (uint32_t i = slot; i < 0x14; i += 1) {
            this->boundTextures[i] = -1;
        }
        glActiveTexture(0x84c0);
    }
    return;
}

void Engine::PreUpdate() {
}

void Engine::Release() {
}

void Engine::ActivateFrameBuffer(int slot) {
}

void Engine::SetFrameBufferScaleFactor(float factor, int slot) {
}

void Engine::GrabFrameBuffer() {
}

void *Engine::GetJPEGImageData(float quality) {
    return nullptr;
}

void Engine::SaveImageToPhotosAlbum() {
}

void Engine::SetScreenOrientation(AbyssEngine::LandscapeMode orientation) {
}

bool Engine::IsRefractActivated() {
    return true;
}

void Engine::Vibrate(unsigned short duration) {
}

void Engine::SetWorldViewMatrix(const Matrix &matrix) {
    const float *m = matrix;
    if (g_Engine_useShaders != 0) {
        float gl[16] = {
            m[0], m[4], m[8], 0.0f,
            m[1], m[5], m[9], 0.0f,
            m[2], m[6], m[10], 0.0f,
            m[3], m[7], m[11], 1.0f,
        };
        memcpy(this->worldViewMatrixGL, gl, 0x40);
        esMatrixMultiply(this->worldViewProjMatrix, gl, this->projMatrix);
    } else {
        MatrixGetGL(matrix, this->uvMatrixGL);
        return glLoadMatrixf(this->uvMatrixGL);
    }
    return;
}

void Engine::ResetLightParam() {
    this->materialAlpha = 1.0f;
    this->lightCount = 1;
    this->materialDiffuse[0] = 0.8f;
    this->materialDiffuse[1] = 0.8f;
    this->materialDiffuse[2] = 0.8f;
    this->materialDiffuse[3] = 0x3f800000;
    this->materialAmbient[0] = 0.2f;
    this->materialAmbient[1] = 0.2f;
    this->materialAmbient[2] = 0.2f;
    this->materialAmbient[3] = 0x3f800000;
    this->materialSpecular[0] = 0;
    this->materialSpecular[2] = 0x3f800000ULL;
    this->materialShininess = 0;
    this->lightAmbient.r = 0.0f;
    this->lightAmbient.g = 0.0f;
    this->lightAmbient.b = 0.0f;
    this->lightAmbient.a = 1.0f;

    Vector up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    this->field_0x468 = up;
    this->field_0x474 = up;
    this->lightDirty[0] = 0.0f;
    this->lightDirty[1] = 0.0f;

    if (g_Engine_useShaders == 0) {
        glLightfv(0x4000, 0x1200, &this->lightAmbient.r);
        glLightfv(0x4000, 0x1201, &this->lightDiffuse.r);
        glLightfv(0x4000, 0x1202, &this->lightSpecular.r);
        glMaterialfv(0x408, 0x1200, this->materialAmbient);
        glMaterialfv(0x408, 0x1201, this->materialDiffuse);
        glMaterialfv(0x408, 0x1202, this->materialSpecular);
        glMaterialf(0x408, 0x1601, this->materialShininess);
    }
    return;
}

void Engine::LightSetLightColorSpecular(float red, float green, float blue, unsigned int light) {
    unsigned int index = light - 0x4000;
    if (index > 7) {
        return;
    }
    int count = light - 0x3fff;
    int current = this->lightCount;
    if (current > count) {
        count = current;
    }
    this->lightCount = count;

    LightColor &src = (&this->lightSpecular)[index];
    src.r = red;
    src.g = green;
    src.b = blue;
    src.a = 1.0f;
    if (g_Engine_useShaders == 0) {
        return glLightfv(light, 0x1202, &src.r);
    }
    Vector &dst = (&this->lightSpecularShaded)[index];
    dst.x = this->materialSpecular[0] * red;
    dst.y = src.g * this->materialSpecular[1];
    dst.z = src.b * this->materialSpecular[2];
    return ShaderUpdateMaterialColor();
}

void Engine::GlEnable(unsigned int cap, bool enable) {
    if (g_Engine_useShaders == 0) {
        unsigned int glCap = cap == 0x1000000 ? 0xbc0 : cap;
        if (enable) {
            return glEnable(glCap);
        }
        return glDisable(glCap);
    }

    uint32_t bit = 0;
    if (cap == 0xde1) {
        bit = 1;
    } else if (cap == 0x1000000) {
        bit = 2;
    } else if (cap >= 0x1100000 && cap <= 0x1100024) {
        static const uint32_t bits[] = {
            4, 8, 0x10, 0x20, 0x80, 0x40, 0x100, 0x200, 0x400, 0x1000,
            0, 0, 0, 0, 0, 0, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000,
            0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0, 0, 0, 0, 0,
            0, 0x800000, 0x1000000, 0x2000000, 0x4000000, 0x8000000,
        };
        bit = bits[cap - 0x1100000];
    }
    if (bit == 0) {
        return;
    }
    uint32_t flags = this->glEnableFlags;
    flags = enable ? (flags | bit) : (flags & ~bit);
    this->glEnableFlags = flags;
}

void _ZN11AbyssEngine6Engine11LightEnableEb(Engine *self, bool enabled);

void Engine::LightEnable(bool enabled) {
    _ZN11AbyssEngine6Engine11LightEnableEb(this, enabled);
}

void Engine::LightSetMaterialColorAmbient(float red, float green, float blue) {
    this->materialAmbient[0] = red;
    this->materialAmbient[1] = green;
    this->materialAmbient[2] = blue;
    this->materialAmbient[3] = this->materialAlpha;

    if (g_Engine_useShaders == 0) {
        return glMaterialfv(0x408, 0x1200, this->materialAmbient);
    }

    int count = this->lightCount;
    for (int index = 0; index < count; index += 1) {
        const LightColor &src = (&this->lightAmbient)[index];
        Vector &dst = (&this->lightAmbientShaded)[index];
        dst.x = (this->sceneAmbient[0] + src.r) * red;
        dst.y = (this->sceneAmbient[1] + src.g) * green;
        dst.z = (this->sceneAmbient[2] + src.b) * blue;
    }
    return ShaderUpdateMaterialColor();
}

// Static data members present in the original binary (defined for symbol parity).
unsigned char AbyssEngine::Engine::EnableGlow;
int AbyssEngine::Engine::ImageCount;
int AbyssEngine::Engine::switchGlow;
int AbyssEngine::Engine::SwapCounter;
int AbyssEngine::Engine::switchBloom;
unsigned char AbyssEngine::Engine::enableShader;
unsigned char AbyssEngine::Engine::EnableRefract;
void *AbyssEngine::Engine::LodDistShader;
unsigned char AbyssEngine::Engine::DisableRefract;
int AbyssEngine::Engine::AnisotropyValue;
unsigned char AbyssEngine::Engine::KeepRawMeshData;
unsigned char AbyssEngine::Engine::backupSaveGames;
int AbyssEngine::Engine::MultiSampleValue;
unsigned char AbyssEngine::Engine::UseAdvancedShader;
unsigned char AbyssEngine::Engine::enableReverseFlag;
int AbyssEngine::Engine::tv_h;
int AbyssEngine::Engine::tv_w;
AbyssEngine::AEMath::Vector AbyssEngine::Engine::vendor;
unsigned char AbyssEngine::Engine::DrawFBO;
int AbyssEngine::Engine::vboSize;
unsigned char AbyssEngine::Engine::DEBUGKEY;
unsigned char AbyssEngine::Engine::TVEnable;
float AbyssEngine::Engine::farPlane;
AbyssEngine::AEMath::Vector AbyssEngine::Engine::renderer;
AbyssEngine::AEMath::Vector AbyssEngine::Engine::tv_modes;
unsigned char AbyssEngine::Engine::WireFrame;
float AbyssEngine::Engine::nearPlane;
