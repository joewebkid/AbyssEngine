#include "engine/render/ParticleSystemSprite.h"
#include "engine/render/ParticleSettingsRef.h"
#define GOF2_ENUM_BlendMode
#include "engine/render/PaintCanvas.h"
#include "engine/math/AEMath.h"
#include <cassert>
#include <cmath>
#include <cstdio>

namespace {
struct CanvasState {
    Vector position{};
    float alpha = -1;
    float rgba[4]{};
    float uv[4]{};
    short sizeDelta = 0;
    short sizeValue = -1;
    int adds = 0;
    int sets = 0;
    int sizeAdds = 0;
    int sizeSets = 0;
    int uvSets = 0;
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
    particleAges = new int[1]{0};
    particleSetIds = static_cast<int8_t *>(::operator new(1));
    particleSetIds[0] = 0;
    random.setSeed(1);
}
ParticleSystemSprite::~ParticleSystemSprite() {
    delete[] particleVelocities;
    delete[] particleAges;
    ::operator delete(particleSetIds);
}
int ParticleSystemSprite::init(uint32_t, uint16_t) { return 0; }
void ParticleSystemSprite::reset() {}
void ParticleSystemSprite::release() {}
int ParticleSystemSprite::getQuadCount() { return 0; }
void ParticleSystemSprite::setParticle(const Vector &, float, uint32_t,
        float, float, float, float, bool, float, float, const Vector &) {}

void IParticleSystem::interpolateColor(int, float &a, float &r, float &g, float &b) {
    a=0.2f; r=0.4f; g=0.6f; b=0.8f;
}
float *IParticleSystem::rotateUVs(float *src, int, float *) { return src; }

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
void PaintCanvas::SpriteSystemSetRGBA(unsigned, unsigned short, float a,
                                     float r, float g, float b) {
    state.alpha = b;
    state.rgba[0]=a; state.rgba[1]=r; state.rgba[2]=g; state.rgba[3]=b;
}
void PaintCanvas::SpriteSystemAddSize(unsigned, unsigned short, short value) {
    state.sizeDelta = value; ++state.sizeAdds;
}
void PaintCanvas::SpriteSystemSetSize(unsigned, unsigned short, short value) {
    state.sizeValue = value; ++state.sizeSets;
}
void PaintCanvas::SpriteSystemSetUv(unsigned, unsigned short, float a, float b,
                                    float c, float d) {
    state.uv[0]=a; state.uv[1]=b; state.uv[2]=c; state.uv[3]=d;
    ++state.uvSets;
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
    def.colorFlag = 1;
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

    state = {}; state.position = {105,200,300};
    particle.flags = 0x80;
    particle.particleAges[0] = 24;
    particle.particleVelocities[0] = {0,0,0};
    particle.updateSingle(0, 10);
    assert(particle.particleAges[0] == 24 && near(state.alpha, 1));

    particle.flags = 0;
    def.lifetime = 100; def.posBase = 200; def.posSpread = 7;
    def.frames = 4;
    def.uvU0 = 0.8995f; def.uvU1 = 0.9995f;
    def.uvV0 = 0.2f; def.uvV1 = 0.4f;
    state = {}; state.position = {100,200,300};
    particle.particleAges[0] = 24;
    particle.particleVelocities[0] = {100,0,0};
    particle.updateSingle(0, 10);
    assert(particle.particleAges[0] == 34);
    assert(state.sizeAdds == 1 && state.sizeDelta == 2);
    assert(state.sizeSets == 0 && state.uvSets == 1);
    assert(near(state.position.x, 101) && state.adds == 1);
    assert(near(state.rgba[0], 0.2f) && near(state.rgba[3], 0.8f));
    assert(near(state.uv[0], -0.0005f));
    assert(near(state.uv[1], 0.4f));
    assert(near(state.uv[2], 0.0995f));
    assert(near(state.uv[3], 0.6f));

    state = {}; state.position = {100,200,300};
    particle.particleAges[0] = 26;
    particle.updateSingle(0, 10.9f);
    assert(particle.particleAges[0] == 36);
    assert(state.uvSets == 0); // Prior frame uses int(dt), not int(aged-dt).

    state = {}; state.position = {100,200,300};
    def.lifetime = 30;
    particle.particleAges[0] = 25;
    particle.updateSingle(0, 10);
    assert(particle.particleAges[0] == -1);
    assert(state.sets == 1 && state.sizeSets == 1 && state.sizeValue == 0);
    assert(state.position.x == 4294967296.0f);
    assert(state.position.y == 4294967296.0f);
    assert(state.position.z == 4294967296.0f);
    assert(state.sizeAdds == 0 && state.adds == 0);
    std::puts("Particle sprite update production smoke: OK");
}
