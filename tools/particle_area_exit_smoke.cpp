#include "engine/render/ParticleSystemSprite.h"
#include "engine/render/ParticleSettingsRef.h"
#define GOF2_ENUM_BlendMode
#include "engine/render/PaintCanvas.h"
#include "engine/math/AEMath.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace {
struct CanvasState {
    Vector position{};
    float alpha = -1;
    int adds = 0;
    int sets = 0;
} state;

bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }
}

ParticleSettings ParticleSettingsRef::cur;
ParticleSettings::ParticleSettings() {}
ParticleSettings::~ParticleSettings() {}
void IParticleSystem::emit(int) {}

ParticleSystemSprite::ParticleSystemSprite(PaintCanvas *pc, const Matrix *m,
        const Array<ParticleSettings::ParticleSet> &, bool, bool) : IParticleSystem() {
    canvas = pc; matrix = m; resource = 1; resourceOffset = 0; alphaFade = 0;
    particleVelocities = new Vector[1]{};
    particleSetIds = static_cast<int8_t *>(::operator new(1));
    particleSetIds[0] = 0;
    random.setSeed(1);
}
ParticleSystemSprite::~ParticleSystemSprite() {
    delete[] particleVelocities;
    ::operator delete(particleSetIds);
}
int ParticleSystemSprite::init(uint32_t, uint16_t) { return 0; }
void ParticleSystemSprite::reset() {}
void ParticleSystemSprite::release() {}
int ParticleSystemSprite::getQuadCount() { return 0; }
void ParticleSystemSprite::updateSingle(int, float) {}
void ParticleSystemSprite::setParticle(const Vector &, float, uint32_t,
        float, float, float, float, bool, float, float, const Vector &) {}

PaintCanvas::PaintCanvas(Engine *) {}
PaintCanvas::~PaintCanvas() {}
void PaintCanvas::SpriteSystemAddPosition(unsigned, unsigned short, float x,
                                         float y, float z) {
    state.position.x += x; state.position.y += y; state.position.z += z;
    ++state.adds;
}
void PaintCanvas::SpriteSystemGetPosition(unsigned, unsigned short, Vector &out) {
    out = state.position;
}
float PaintCanvas::SpriteSystemSetPosition(unsigned, unsigned short, float x,
                                          float y, float z) {
    state.position = {x, y, z}; ++state.sets; return z;
}
void PaintCanvas::SpriteSystemSetRGBA(unsigned, unsigned short, float,
                                     float, float, float alpha) {
    state.alpha = alpha;
}

namespace AbyssEngine { namespace AEMath {
Matrix::Matrix() {}
Vector &Vector::operator=(const Vector &v) { x=v.x; y=v.y; z=v.z; return *this; }
Vector &Vector::operator+=(const Vector &v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
Vector &Vector::operator*=(float scale) { x*=scale; y*=scale; z*=scale; return *this; }
Vector operator*(const Vector &v, float scale) {
    return {v.x*scale, v.y*scale, v.z*scale};
}
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x-b.x, a.y-b.y, a.z-b.z};
}
Vector MatrixGetPosition(const Matrix &m) { return {m.m[3], m.m[7], m.m[11]}; }
}}

int main() {
    PaintCanvas canvas(nullptr);
    Matrix matrix(1,0,0,100, 0,1,0,200, 0,0,1,300, 0,0,0);
    Array<ParticleSettings::ParticleSet> sets;
    ParticleSystemSprite particle(&canvas, &matrix, sets, false, false);
    auto &def = ParticleSettingsRef::cur.sets[0];
    def.color0 = 0xffffffffU;
    def.posRight = 10; def.posUp = 8; def.posDir = 3;
    float inner = 1;
    std::memcpy(&def.posBase, &inner, sizeof(inner));
    def.velBaseX = 7; def.velBaseY = 8; def.velBaseZ = 9;

    auto run = [&](float x, float expectedAlpha) {
        state = {}; state.position = {100+x, 200, 300};
        particle.particleVelocities[0] = {0,0,0};
        particle.random.setSeed(123);
        uint32_t before = particle.random.seedLow;
        particle.updateAreaExitParticle(0, 10);
        assert(state.adds == 1 && state.sets == 0);
        assert(near(state.alpha, expectedAlpha));
        assert(particle.random.seedLow == before);
    };
    run(0.5f, 0);
    run(2.0f, 3.0f/8.0f);
    run(5.0f, 1);
    run(9.0f, 19.0f/36.0f);
    run(10.02f, 0);

    state = {}; state.position = {105,200,300};
    particle.particleVelocities[0] = {100,0,0};
    particle.updateAreaExitParticle(0, 10);
    assert(near(state.position.x, 106) && state.adds == 1);

    bool sawRebirth = false, sawRejected = false;
    for (int seed = 0; seed < 1000 && !(sawRebirth && sawRejected); ++seed) {
        state = {}; state.position = {112,200,300};
        particle.particleVelocities[0] = {0,0,0};
        particle.random.setSeed(seed);
        AERandom expected(seed);
        float q[4];
        for (int i = 0; i < 4; ++i)
            q[i] = float(expected.nextInt(2000) - 1000) * 0.001f;
        float norm = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
        particle.updateAreaExitParticle(0, 10);
        assert(state.adds == 1 && near(state.alpha, 0));
        assert(particle.random.seedLow == expected.seedLow);
        assert(particle.random.seedHigh == expected.seedHigh);
        if (norm < 1.0f) {
            sawRejected = true;
            assert(state.sets == 0);
            assert(near(state.position.x, 112));
        } else {
            sawRebirth = true;
            assert(state.sets == 1);
            Vector delta = state.position - Vector{100,200,300};
            assert(std::fabs(delta.x - 20*(q[2]*q[3]-q[0]*q[1])/norm) < 0.001f);
            assert(std::fabs(delta.y - 20*(q[1]*q[3]+q[0]*q[2])/norm) < 0.001f);
            assert(std::fabs(delta.z - 10*(q[0]*q[0]+q[3]*q[3]-q[1]*q[1]-q[2]*q[2])/norm)
                   < 0.001f);
            assert(near(particle.particleVelocities[0].x, 7));
            assert(near(particle.particleVelocities[0].y, 8));
            assert(near(particle.particleVelocities[0].z, 9));
        }
    }
    assert(sawRebirth && sawRejected);
    std::puts("Particle area-exit production smoke: OK");
}
