#ifndef GOF2_IPARTICLESYSTEM_H
#define GOF2_IPARTICLESYSTEM_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
#include "engine/render/ParticleSettings.h"

#include "engine/math/AEMath.h"



namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;

void AERandom_dtor(void *self);

class IParticleSystem {
public:
    volatile uint16_t field_0x4;
    volatile uint8_t emitterVelocityDirty;
    PaintCanvas *canvas;
    uint8_t emitEnabled;
    uint8_t renderEnabled;
    uint8_t updateEnabled;
    uint8_t random[8];
    AbyssEngine::AEMath::Matrix const *matrix;
    AbyssEngine::AEMath::Vector emitterVelocity;
    AbyssEngine::AEMath::Vector lastEmitterPosition;
    int32_t field_0x2c;
    int32_t field_0x30;
    uint32_t flags;
    uint8_t particleSetIndex;
    uint8_t alphaFade;
    int32_t maxParticles;
    uint8_t mirror;
    int32_t currentParticle;
    int32_t field_0x54;
    int32_t field_0x58;
    uint8_t field_0x5c;
    float emitTimer;
    AbyssEngine::AEMath::Vector *particleVelocities;
    int *particleAges;
    int8_t *particleSetIds;
    Array<ParticleSettings::ParticleSet> *particleSets;

    IParticleSystem(PaintCanvas *canvas, AbyssEngine::AEMath::Matrix const *matrix,
                    Array<ParticleSettings::ParticleSet> const &sets,
                    bool mirror, bool alphaFade);

    IParticleSystem() {
    }

    ~IParticleSystem() {
        delete this->particleSets;
        AERandom_dtor(reinterpret_cast<void *>(this->random));
    }

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
#endif
