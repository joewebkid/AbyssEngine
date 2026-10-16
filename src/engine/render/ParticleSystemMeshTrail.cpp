#include "engine/render/ParticleSystemMesh.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/ParticleSettingsRef.h"

namespace {

const ParticleSettings::SetDefinition &currentParticleSet(int set) {
    return ParticleSettingsRef::cur.sets[set];
}

} // namespace

void ParticleSystemMesh::emitTrail(int delta) {
    using namespace AbyssEngine::AEMath;

    int set = this->particleSets[this->particleSetIndex];
    const ParticleSettings::SetDefinition &settings = currentParticleSet(set);
    Vector right = MatrixGetRight(*this->matrix);
    right = right * (this->mirror ? -1.0f : 1.0f);
    Vector up = MatrixGetUp(*this->matrix);
    Vector dir = MatrixGetDir(*this->matrix);

    Vector rightEdge = right * settings.lifeBase;
    Vector upEdge = up * settings.lifeBase;
    if ((this->flags & 0x20000u) != 0) {
        rightEdge = (right + up) * (settings.lifeBase * 0.70711f);
        upEdge = (up - right) * (settings.lifeBase * 0.70711f);
    }
    Vector dirEdge = dir * settings.lifeBase;

    Vector origin{static_cast<float>(this->field_0x78), 0.0f,
                  static_cast<float>(this->field_0x7c)};
    Vector position = MatrixTransformVector(*this->matrix, origin);
    position = position + settings.posRight * right;
    position = position + settings.posUp * up;
    position = position + settings.posDir * dir;

    if (settings.colorFlag > 0.0f) {
        int previous = this->currentParticle ? this->currentParticle : this->maxParticles;
        bool finish = this->particleAges[previous - 1] == -1;
        position -= (settings.colorFlag * 0.5f) * dir;
        int savedParticle = this->currentParticle;
        this->currentParticle = this->maxParticles;
        Vector zero{0.0f, 0.0f, 0.0f};
        this->setParticle(position, settings.lifeBase, settings.color0,
                          settings.uvU0, settings.uvV0, settings.uvU1,
                          settings.uvV0 + (settings.uvV1 - settings.uvV0) * 0.05f,
                          true, 0.0f, settings.colorFlag * 0.5f, zero, finish);
        this->currentParticle = savedParticle;
        position -= (settings.colorFlag * 0.5f) * dir;
    }

    if (this->newSectionStarted == 0) {
        int point = static_cast<int>(this->resourceOffset) +
                    4 * this->currentParticle * static_cast<int>(this->stride);
        if ((this->flags & 0x1000u) != 0) {
            this->setQuadEdge(position, point + 2, rightEdge);
            point += this->wide ? 8 : 4;
        }
        if ((this->flags & 0x2000u) != 0) {
            this->setQuadEdge(position, point + 2, upEdge);
            point += this->wide ? 8 : 4;
        }
        if ((this->flags & 0x4000u) != 0)
            this->setQuadEdge(position, point + 2, dirEdge);

        if ((this->flags & 0x100000u) != 0) {
            float time = static_cast<float>(this->frameCounter + delta) /
                         settings.flLifetime;
            float v = settings.uvV0 +
                      (time < 1.0f ? time : 1.0f) * (settings.uvV1 - settings.uvV0);
            point = static_cast<int>(this->resourceOffset) +
                    4 * this->currentParticle * static_cast<int>(this->stride);
            for (int i = 0; i < static_cast<int>(this->stride); ++i) {
                this->canvas->MeshSetUv(this->resource, static_cast<uint16_t>(point + 2),
                                        settings.uvU0, v);
                this->canvas->MeshSetUv(this->resource, static_cast<uint16_t>(point + 3),
                                        settings.uvU1, v);
                point += 4;
            }
        }
    }

    this->frameCounter += delta;
    if (settings.flLifetime < static_cast<float>(this->frameCounter)) {
        Vector distance = this->lastTrailPosition - position;
        if (VectorDot(distance, distance) > 6000.0f)
            this->newSectionStarted = 1;
    }
    if (this->newSectionStarted == 0)
        return;

    int previous = this->currentParticle ? this->currentParticle : this->maxParticles;
    bool previousDead = this->particleAges[previous - 1] == -1;
    this->frameCounter = 0;
    float edgeSize = static_cast<float>(settings.posBase);
    this->finishCurrentTrailParticle(static_cast<ParticleSet>(set), this->currentParticle,
                                     right * edgeSize, up * edgeSize);
    this->incId();

    int point = static_cast<int>(this->resourceOffset) +
                4 * this->currentParticle * static_cast<int>(this->stride);
    Vector *velocity = this->particleVelocities +
                       2 * this->currentParticle * this->edgeCount;
    if ((this->flags & 0x1000u) != 0) {
        this->setQuadEdge(position, point, rightEdge);
        this->setQuadEdge(position, point + 2, rightEdge);
        velocity[0] = right * edgeSize;
        velocity[1] = {0.0f, 0.0f, 0.0f};
        ++velocity;
        velocity += 1;
        point += this->wide ? 8 : 4;
    }
    if ((this->flags & 0x2000u) != 0) {
        this->setQuadEdge(position, point, upEdge);
        this->setQuadEdge(position, point + 2, upEdge);
        velocity[0] = up * edgeSize;
        velocity[1] = {0.0f, 0.0f, 0.0f};
    }
    this->lastTrailPosition = position;

    uint32_t frontColor = settings.color0;
    if (previousDead)
        frontColor &= this->alphaFade ? 0xffu : 0xffffff00u;
    point = static_cast<int>(this->resourceOffset) +
            4 * this->currentParticle * static_cast<int>(this->stride);
    for (int i = 0; i < static_cast<int>(this->stride); ++i) {
        this->canvas->MeshSetColor(this->resource, static_cast<uint16_t>(point), frontColor);
        this->canvas->MeshSetColor(this->resource, static_cast<uint16_t>(point + 1), frontColor);
        this->canvas->MeshSetColor(this->resource, static_cast<uint16_t>(point + 2), settings.color0);
        this->canvas->MeshSetColor(this->resource, static_cast<uint16_t>(point + 3), settings.color0);
        this->canvas->MeshSetUv(this->resource, static_cast<uint16_t>(point),
                                settings.uvU0, settings.uvV0);
        this->canvas->MeshSetUv(this->resource, static_cast<uint16_t>(point + 1),
                                settings.uvU1, settings.uvV0);
        this->canvas->MeshSetUv(this->resource, static_cast<uint16_t>(point + 2),
                                settings.uvU0, settings.uvV1);
        this->canvas->MeshSetUv(this->resource, static_cast<uint16_t>(point + 3),
                                settings.uvU1, settings.uvV1);
        point += 4;
    }
    this->particleAges[this->currentParticle] = -2;
    this->newSectionStarted = 0;
}
