#ifndef GOF2_IPARTICLESYSTEM_H
#define GOF2_IPARTICLESYSTEM_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
#include "engine/core/AERandom.h"
#include "engine/render/ParticleSettings.h"

#include "engine/math/AEMath.h"



namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;

class IParticleSystem {
public:
    // Android ARM layout: base ends at +0x70. Do not add host-only state here.
    uint8_t resetEmitterVelocityPending; // +0x04
    uint8_t emitterVelocityDirty;        // +0x05
    uint16_t field_0x6;
    PaintCanvas *canvas;
    uint8_t emitEnabled;
    uint8_t renderEnabled;
    uint8_t updateEnabled;
    uint8_t field_0x0f;
    AERandom random;
    AbyssEngine::AEMath::Matrix const *matrix;
    AbyssEngine::AEMath::Vector emitterVelocity;
    AbyssEngine::AEMath::Vector lastEmitterPosition;
    uint32_t flags;
    Array<ParticleSettings::ParticleSet> particleSets;
    uint8_t particleSetIndex;
    uint8_t alphaFade;
    uint16_t field_0x46;
    int32_t maxParticles;
    uint8_t mirror;
    uint8_t field_0x4d[3];
    int32_t currentParticle;
    uint32_t resource;
    uint32_t resourceOffset;
    uint8_t initialized;
    uint8_t field_0x5d[3];
    float emitTimer;
    AbyssEngine::AEMath::Vector *particleVelocities;
    int *particleAges;
    int8_t *particleSetIds;

    IParticleSystem(PaintCanvas *canvas, AbyssEngine::AEMath::Matrix const *matrix,
                    Array<ParticleSettings::ParticleSet> const &sets,
                    bool mirror, bool alphaFade);

    IParticleSystem() = default;

    ~IParticleSystem() = default;

    virtual int init(uint32_t resource, uint16_t idOffset) = 0;

    virtual void emit(int delta);

    virtual void reset() = 0;

    virtual void release() = 0;

    virtual int getQuadCount() = 0;

    virtual void updateSingle(int index, float delta) = 0;

    virtual void setParticle(AbyssEngine::AEMath::Vector const &pos, float scale, uint32_t color,
                             float u0, float u1, float v0, float v1, bool maskedColor,
                             float size0, float size1, AbyssEngine::AEMath::Vector const &velocity) = 0;

    int getParticleCount();

    void setParticleSet(ParticleSettings::ParticleSet set);

    void setParticleSetIndex(uint8_t index);

    void setMatrix(AbyssEngine::AEMath::Matrix const *matrix);

    void enableEmit(bool enabled);

    void enableRender(bool enabled);

    void enableUpdate(bool enabled);

    void update(int delta);

    void emitManual(AbyssEngine::AEMath::Vector position, int particleSet, AbyssEngine::AEMath::Vector const *velocity,
                    float lifetime);

    void interpolateColor(int index, float &alpha, float &red, float &green, float &blue);

    float *rotateUVs(float *src, int seed, float *dst);

    void calcEmitterVelocity(int delta);

    void resetEmitterVelocity();
};

#if UINTPTR_MAX == 0xffffffffu
static_assert(sizeof(IParticleSystem) == 0x70, "IParticleSystem ARM size");
#endif
#endif
