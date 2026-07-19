#include "engine/render/ParticleSystemSprite.h"

#define GOF2_ENUM_BlendMode
#include "engine/render/PaintCanvas.h"


void _pss_base_ctor(ParticleSystemSprite *self, PaintCanvas *canvas, const Matrix *matrix,
                    const Array<ParticleSettings::ParticleSet> &sets, bool mirror, bool alphaFade);

void _pss_base_dtor(ParticleSystemSprite * self);

void _pss_interpolateColor(ParticleSystemSprite *self, int index, float *alpha, float *red,
                           float *green, float *blue);

float *_pss_rotateUVs(ParticleSystemSprite *self, float *src, int index, float *dst);

void ParticleSystem_updateAreaExitParticleImpl(ParticleSystemSprite *self, int index, float dt);

using AbyssEngine::AEMath::VectorSignedToFloat;
using AbyssEngine::AEMath::VectorUnsignedToFloat;

static void *g_PaintCanvas = nullptr;

static char *g_particleSetBase = nullptr;

static float g_uvRoundBias = 0.0f;

static float g_colorScale = 0.0f;

ParticleSystemSprite::~ParticleSystemSprite() {
    this->release();
    _pss_base_dtor(this);
}

ParticleSystemSprite::ParticleSystemSprite(PaintCanvas *canvas, const Matrix *matrix,
                                           const Array<ParticleSettings::ParticleSet> &particleSets,
                                           bool mirror, bool alphaFade) {
    _pss_base_ctor(this, canvas, matrix, particleSets, mirror, alphaFade);

    uint32_t count = (uint32_t) this->particleCount;

    char *arr = new char[count * 0xc];
    if (count != 0)
        memset(arr, 0, (size_t) count * 0xc);
    this->spriteData = arr;

    this->cachedPow = AbyssEngine::AEMath::Pow(0.0f, 0.0f);
}

void ParticleSystemSprite::reset() {
    PaintCanvas *pc = (PaintCanvas *) g_PaintCanvas;
    for (int i = 0; i < this->particleCount; i++) {
        pc->SpriteSystemSetPosition(this->canvasHandle, (uint16_t)(this->idOffset + i),
                                    4294967296.0f, 4294967296.0f, 4294967296.0f);
        pc->SpriteSystemSetSize(this->canvasHandle, (uint16_t)(this->idOffset + i), 0);
        this->ages[i] = -1;
    }
    this->liveCount = 0;
    this->started = 1;
}

int ParticleSystemSprite::init(uint32_t spriteId, uint16_t idOffset) {
    this->spriteId = spriteId;
    this->idOffset = idOffset;
    this->initialized = 1;

    this->reset();
    return 0;
}

int ParticleSystemSprite::getQuadCount() {
    return this->particleCount;
}

void ParticleSystemSprite::release() {
    delete[] (char *) this->spriteData;
    this->spriteData = nullptr;
    delete[] this->ages;
    this->ages = nullptr;
    delete[] this->setIndices;
    this->setIndices = nullptr;
}

void ParticleSystemSprite::render(PaintCanvas *canvas, uint32_t handle, uint32_t texture, BlendMode blend) {
    if (handle == 0xffffffffu)
        return;

    canvas->SetTexture(texture, 0);
    canvas->SetBlendMode(blend);

    float m[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    canvas->SetWorldViewMatrix(*(const AbyssEngine::AEMath::Matrix *) m);
    canvas->DrawSpriteSystem(handle);
}

void ParticleSystemSprite::updateSingle(int index, float dt) {
    if ((int) ((uint32_t) this->flags << 0x18) < 0)
        return;

    PaintCanvas *pc = (PaintCanvas *) g_PaintCanvas;
    uint32_t handle = this->canvasHandle;
    uint16_t id = (uint16_t) this->spriteId;

    int *ages = this->ages;
    int8_t *setIdx = this->setIndices;
    char *set = g_particleSetBase + (int) setIdx[index] * 0xa0;

    float age = VectorSignedToFloat(ages[index], 0);
    age = (float) (int) (age + dt);
    ages[index] = (int) age;

    int lifetime = *(int *) (set + 0x28);
    if ((int) age > lifetime) {
        ages[index] = -1;
        pc->SpriteSystemSetPosition(handle, id, age, 0.0f, 0.0f);
        return;
    }

    VectorSignedToFloat(*(int *) (set + 0x44), 0);
    pc->SpriteSystemAddSize(handle, id, (short) this->idOffset + (short) index);

    float ca, cr, cg, cb;
    _pss_interpolateColor(this, index, &ca, &cr, &cg, &cb);
    pc->SpriteSystemSetRGBA(handle, id, cg, 0.0f, cb, 0.0f);

    int frames = *(int *) (set + 0x9c);
    if (frames != 0) {
        int span = *(int *) (set + 0x28);
        int aged = ages[index] - 1;
        int cur = (aged * frames) / span;
        int prevAged = (int) ((float) aged - dt);
        int prev = (prevAged * frames) / span;
        if (prev < 0) prev = 0;

        if ((uint32_t) cur != (uint32_t) prev) {
            float fcur = VectorSignedToFloat(cur, 0);
            float du = *(float *) (set + 0x90) - *(float *) (set + 0x88);
            float dv = *(float *) (set + 0x94) - *(float *) (set + 0x8c);
            float u0 = *(float *) (set + 0x88) + du * fcur;
            float frac = VectorSignedToFloat((int) (u0 + g_uvRoundBias), 0);
            u0 = u0 - frac;
            float v0 = *(float *) (set + 0x8c) + dv * frac;
            float uv[4] = {u0, du + u0, v0, dv + v0};

            float *out = uv;
            float uvRot[4];

            if ((int) ((uint32_t) this->flags2 << 0x1e) < 0)
                out = _pss_rotateUVs(this, uv, index, uvRot);

            pc->SpriteSystemSetUv(handle, id, out[1], 0.0f, out[2], 0.0f);
        }
    }

    float pos[4] = {0, 0, 0, 0};
    pc->SpriteSystemAddPosition(handle, id, pos[1], 0.0f, pos[2]);
}

void ParticleSystemSprite::setAlpha(int index, uint32_t color, float alpha) {
    float c0 = (float) (color & 0xff);
    float c1 = (float) ((color >> 8) & 0xff);
    float c2 = (float) ((color >> 16) & 0xff);
    float c3 = (float) (color >> 24);

    c0 = c0 * (1.0f / 255.0f);
    c1 = c1 * (1.0f / 255.0f);
    c2 = c2 * (1.0f / 255.0f);
    c3 = c3 * (1.0f / 255.0f);

    if (this->cAlphaChannelMode == 0) {
        c0 = c0 * alpha;
    } else {
        c1 = c1 * alpha;
        c2 = c2 * alpha;
        c3 = c3 * alpha;
    }

    ((PaintCanvas *) g_PaintCanvas)->SpriteSystemSetRGBA(this->canvasHandle,
                                                         (uint16_t)(this->idOffset + index), c3, c2, c1, c0);
}

void ParticleSystemSprite::enable(bool enabled) {
    (void) enabled;
}

void ParticleSystemSprite::updateAreaExitParticle(int index, float dt) {
    ParticleSystem_updateAreaExitParticleImpl(this, index, dt);
}

void ParticleSystemSprite::setParticle(const Vector &pos, float p2, uint32_t color, float p4,
                                       float p5, float p6, float p7, bool clearColor, float p9,
                                       float p10, const Vector &uv) {
    (void) p2;
    (void) p5;
    (void) p6;
    (void) p7;
    (void) p9;
    (void) p10;

    PaintCanvas *pc = (PaintCanvas *) g_PaintCanvas;
    uint32_t handle = this->canvasHandle;
    uint16_t id = (uint16_t) this->spriteId;

    pc->SpriteSystemSetPosition(handle, id, pos.y, p4, pos.z);

    short size = (short) this->baseSize + (short) this->idOffset;
    pc->SpriteSystemSetSize(handle, id, size);

    pc->SpriteSystemSetUv(handle, id, uv.x, uv.y, uv.z, ((const float *) &uv)[3]);

    if (clearColor)
        color &= 0xffffff00;

    float a = VectorUnsignedToFloat(color >> 0x18, 0);
    float b = VectorUnsignedToFloat((color & 0xffff) >> 8, 0);
    float g = VectorUnsignedToFloat(color & 0xff, 0);
    float r = VectorUnsignedToFloat((color & 0xffffff) >> 0x10, 0);
    (void) a;
    (void) b;
    (void) g;

    pc->SpriteSystemSetRGBA(handle, id, r * g_colorScale, g, g_colorScale, b);
}

void ParticleSystemSprite::render(PaintCanvas *canvas, uint32_t handle) {
    if (handle == 0xffffffffu)
        return;

    float *a = (float *) canvas->CameraGetLocal(canvas->CameraGetCurrent());
    Matrix am;
    for (int i = 0; i < 15; ++i)
        am.m[i] = a[i];

    float *b = (float *) canvas->CameraGetLocal(canvas->CameraGetCurrent());
    Matrix bm;
    for (int i = 0; i < 15; ++i)
        bm.m[i] = b[i];

    canvas->DrawSpriteSystem(handle, am, bm);
}
