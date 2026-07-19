#ifndef GOF2_PARTICLESYSTEMSPRITE_H
#define GOF2_PARTICLESYSTEMSPRITE_H
#include "engine/core/Array.h"
#include "engine/render/RenderEnums.h"
#include "../core/AEString.h"
#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSettings.h"

#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"


namespace AbyssEngine {
#ifndef GOF2_ENUM_BlendMode
#define GOF2_ENUM_BlendMode
#endif
}

namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;
using AbyssEngine::BlendMode;

using ParticleSet = ParticleSettings::ParticleSet;

class ParticleSystemSprite : public IParticleSystem {
public:
    uint8_t started;
    uint32_t canvasHandle;
    uint8_t flags;
    uint8_t flags2;
    char cAlphaChannelMode;
    int particleCount;
    int baseSize;
    uint32_t spriteId;
    uint32_t idOffset;
    uint8_t initialized;
    int liveCount;
    void *spriteData;
    int *ages;
    int8_t *setIndices;
    float cachedPow;

    ParticleSystemSprite(PaintCanvas *canvas, const Matrix *matrix,
                         const Array<ParticleSettings::ParticleSet> &particleSets,
                         bool mirror, bool alphaFade);

    ~ParticleSystemSprite();

    int init(uint32_t spriteId, uint16_t idOffset) override;

    void reset() override;

    void release() override;

    int getQuadCount() override;

    void updateSingle(int index, float dt) override;

    void setParticle(const Vector &pos, float p2, uint32_t color, float p4, float p5,
                     float p6, float p7, bool clearColor, float p9, float p10,
                     const Vector &uv) override;

    void setAlpha(int index, uint32_t color, float alpha);

    void updateAreaExitParticle(int index, float dt);

    void enable(bool enabled);

    static void render(PaintCanvas *canvas, uint32_t handle, uint32_t texture, BlendMode blend);

    static void render(PaintCanvas *canvas, uint32_t handle);
};
#endif
