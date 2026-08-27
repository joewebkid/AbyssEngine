#ifndef GOF2_PARTICLESYSTEMMESH_H
#define GOF2_PARTICLESYSTEMMESH_H
#include "engine/core/Array.h"
#include "engine/render/RenderEnums.h"
#include "../core/AEString.h"
#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSettings.h"

#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"


namespace AbyssEngine {
}

namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;
using AbyssEngine::BlendMode;

using ParticleSet = ParticleSettings::ParticleSet;

class ParticleSystemMesh : public IParticleSystem {
public:
    uint32_t pointCount; // +0x70
    uint8_t wide;
    uint8_t field_0x75[3];
    uint32_t field_0x78;
    uint32_t field_0x7c;
    uint32_t field_0x80;
    uint32_t field_0x84;
    uint32_t field_0x88;
    uint32_t field_0x8c;
    uint8_t newSectionStarted;
    uint8_t field_0x91[3];
    uint32_t frameCounter;
    uint32_t edgeCount;
    uint32_t stride;

    ParticleSystemMesh(PaintCanvas *canvas, const Matrix *matrix, const Array<ParticleSet> &sets, bool a, bool b);

    ~ParticleSystemMesh();

    void emit(int delta) override;

    int getQuadCount() override;

    void reset() override;

    void release() override;

    int init(uint32_t mesh, uint16_t firstPoint) override;

    void setParticle(const Vector &pos, float scale, uint32_t color, float u0, float u1, float v0,
                     float v1, bool useMaskedColor, float upScale, float dirScale,
                     const Vector &delta) override;

    void updateSingle(int id, float delta) override;

    int getPrevId(int id);

    void incId();

    void startNewSection();

    uint8_t wasNewSectionStarted();

    void setQuadEdge(const Vector &edge, int point, const Vector &delta);

    void finishCurrentTrailParticle(ParticleSet set, int id, const Vector &first, const Vector &second);

    void setParticle(const Vector &pos, float scale, uint32_t color, float u0, float u1, float v0, float v1,
                     bool useMaskedColor, float upScale, float dirScale, const Vector &delta, bool finish);

    void updateUsualEdges(int id, int delta);

    void updateTrailEdges(int id, int delta);

    void updateSingleColor(int id);

    static void render(PaintCanvas *canvas, uint32_t transformId);

    static void render(PaintCanvas *canvas, uint32_t mesh, uint32_t texture, BlendMode blend);

    static void emitTrail(int self);
};

#if UINTPTR_MAX == 0xffffffffu
static_assert(sizeof(ParticleSystemMesh) == 0xa0, "ParticleSystemMesh ARM size");
#endif
#endif
