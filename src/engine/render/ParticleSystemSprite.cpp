#include "engine/render/ParticleSystemSprite.h"

#define GOF2_ENUM_BlendMode
#include "engine/render/PaintCanvas.h"
#include "engine/render/ParticleSettingsRef.h"


void ParticleSystem_updateAreaExitParticleImpl(ParticleSystemSprite *self, int index, float dt);

using AbyssEngine::AEMath::VectorSignedToFloat;

static float g_uvRoundBias = 0.0f;

namespace {

constexpr float kMillisecondsToSeconds = 0.001f;
constexpr float kColorChannelScale = 1.0f / 255.0f;

const ParticleSettings::SetDefinition &currentParticleSet(int set) {
    return ParticleSettingsRef::cur.sets[set];
}

} // namespace

ParticleSystemSprite::~ParticleSystemSprite() {
    this->release();
}

ParticleSystemSprite::ParticleSystemSprite(PaintCanvas *canvas, const Matrix *matrix,
                                           const Array<ParticleSettings::ParticleSet> &particleSets,
                                           bool mirror, bool alphaFade)
    : IParticleSystem(canvas, matrix, particleSets, mirror, alphaFade) {
    uint32_t count = (uint32_t) this->maxParticles;
    char *arr = static_cast<char *>(::operator new(count * 0xc));
    if (count != 0)
        memset(arr, 0, (size_t) count * 0xc);
    this->particleVelocities = reinterpret_cast<Vector *>(arr);

    this->cachedPow = AbyssEngine::AEMath::Pow(0.0f, 0.0f);
}

void ParticleSystemSprite::reset() {
    PaintCanvas *pc = this->canvas;
    for (int i = 0; i < this->maxParticles; i++) {
        pc->SpriteSystemSetPosition(this->resource, (uint16_t)(this->resourceOffset + i),
                                    4294967296.0f, 4294967296.0f, 4294967296.0f);
        pc->SpriteSystemSetSize(this->resource, (uint16_t)(this->resourceOffset + i), 0);
        this->particleAges[i] = -1;
    }
    this->emitTimer = 0.0f;
    this->resetEmitterVelocityPending = 1;
}

int ParticleSystemSprite::init(uint32_t spriteId, uint16_t idOffset) {
    this->resource = spriteId;
    this->resourceOffset = idOffset;
    this->initialized = 1;

    this->reset();
    return 0;
}

int ParticleSystemSprite::getQuadCount() {
    return this->maxParticles;
}

void ParticleSystemSprite::release() {
    if (this->particleVelocities != nullptr)
        ::operator delete(this->particleVelocities);
    this->particleVelocities = nullptr;
    if (this->particleAges != nullptr)
        ::operator delete(this->particleAges);
    this->particleAges = nullptr;
    ::operator delete(this->particleSetIds);
    this->particleSetIds = nullptr;
}

void ParticleSystemSprite::render(PaintCanvas *canvas, uint32_t handle, uint32_t texture, BlendMode blend) {
    if (handle == 0xffffffffu)
        return;

    canvas->SetTexture(texture, 0xffffffffu);
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
    if ((this->flags & 0x00000080u) != 0) {
        this->updateAreaExitParticle(index, dt);
        return;
    }

    PaintCanvas *pc = this->canvas;
    uint32_t handle = this->resource;
    uint16_t id = (uint16_t) (this->resourceOffset + index);

    int *ages = this->particleAges;
    int8_t *setIdx = this->particleSetIds;
    const ParticleSettings::SetDefinition &set = currentParticleSet((int) setIdx[index]);

    float age = VectorSignedToFloat(ages[index], 0);
    age = (float) (int) (age + dt);
    ages[index] = (int) age;

    int lifetime = set.lifetime;
    if ((int) age > lifetime) {
        ages[index] = -1;
        pc->SpriteSystemSetPosition(handle, id, age, 0.0f, 0.0f);
        return;
    }

    short sizeDelta = (short) (int) ((float) set.posBase * dt * kMillisecondsToSeconds);
    pc->SpriteSystemAddSize(handle, id, sizeDelta);

    float ca, cr, cg, cb;
    this->interpolateColor(index, ca, cr, cg, cb);
    pc->SpriteSystemSetRGBA(handle, id, ca, cr, cg, cb);

    int frames = set.frames;
    if (frames != 0) {
        int span = set.lifetime;
        int aged = ages[index] - 1;
        int cur = (aged * frames) / span;
        int prevAged = (int) ((float) aged - dt);
        int prev = (prevAged * frames) / span;
        if (prev < 0) prev = 0;

        if ((uint32_t) cur != (uint32_t) prev) {
            float fcur = VectorSignedToFloat(cur, 0);
            float du = set.uvU1 - set.uvU0;
            float dv = set.uvV1 - set.uvV0;
            float u0 = set.uvU0 + du * fcur;
            float frac = VectorSignedToFloat((int) (u0 + g_uvRoundBias), 0);
            u0 = u0 - frac;
            float v0 = set.uvV0 + dv * frac;
            float uv[4] = {u0, du + u0, v0, dv + v0};

            float *out = uv;
            float uvRot[4];

            if ((this->flags & 0x02000000u) != 0)
                out = this->rotateUVs(uv, index, uvRot);

            pc->SpriteSystemSetUv(handle, id, out[0], out[2], out[1], out[3]);
        }
    }

    Vector movement = this->particleVelocities[index] * dt;
    pc->SpriteSystemAddPosition(handle, id, movement.x, movement.y, movement.z);
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

    if (this->alphaFade == 0) {
        c0 = c0 * alpha;
    } else {
        c1 = c1 * alpha;
        c2 = c2 * alpha;
        c3 = c3 * alpha;
    }

    this->canvas->SpriteSystemSetRGBA(this->resource,
                                      (uint16_t)(this->resourceOffset + index), c3, c2, c1, c0);
}

void ParticleSystemSprite::enable(bool enabled) {
    (void) enabled;
}

void ParticleSystemSprite::updateAreaExitParticle(int index, float dt) {
    ParticleSystem_updateAreaExitParticleImpl(this, index, dt);
}

void ParticleSystemSprite::setParticle(const Vector &pos, float p2, uint32_t color, float p4,
                                       float p5, float p6, float p7, bool clearColor, float p9,
                                       float p10, const Vector &velocity) {
    (void) p2;
    (void) p10;
    (void) velocity;

    PaintCanvas *pc = this->canvas;
    uint32_t handle = this->resource;
    uint16_t id = (uint16_t) (this->resourceOffset + this->currentParticle);

    pc->SpriteSystemSetPosition(handle, id, pos.x, pos.y, pos.z);

    pc->SpriteSystemSetSize(handle, id, (short) (int) p9);

    pc->SpriteSystemSetUv(handle, id, p4, p5, p6, p7);

    if (clearColor)
        color &= 0xffffff00;

    float alpha = (float) (color >> 24) * kColorChannelScale;
    float red = (float) ((color >> 16) & 0xff) * kColorChannelScale;
    float green = (float) ((color >> 8) & 0xff) * kColorChannelScale;
    float blue = (float) (color & 0xff) * kColorChannelScale;
    pc->SpriteSystemSetRGBA(handle, id, alpha, red, green, blue);
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
