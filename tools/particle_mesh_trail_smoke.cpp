#include <cassert>
#include <cmath>
#include <cstdint>
#include <vector>

namespace AbyssEngine { namespace AEMath {
struct Vector {
    float x, y, z;
    Vector &operator-=(const Vector &other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
};
struct Matrix {
    Vector position;
};
Vector operator+(Vector a, Vector b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
Vector operator-(Vector a, Vector b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Vector operator*(Vector a, float s) { return {a.x * s, a.y * s, a.z * s}; }
Vector operator*(float s, Vector a) { return a * s; }
Vector MatrixGetRight(const Matrix &) { return {1, 0, 0}; }
Vector MatrixGetUp(const Matrix &) { return {0, 1, 0}; }
Vector MatrixGetDir(const Matrix &) { return {0, 0, 1}; }
Vector MatrixTransformVector(const Matrix &m, const Vector &v) { return v + m.position; }
float VectorDot(const Vector &a, const Vector &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
}}
using AbyssEngine::AEMath::Vector;
using AbyssEngine::AEMath::Matrix;

struct ParticleSettings {
    enum ParticleSet { dummy = 0 };
    struct SetDefinition {
        float lifeBase = 2, colorFlag = 0, flLifetime = 100;
        float posRight = 1, posUp = 2, posDir = 3;
        float uvU0 = 0.1f, uvV0 = 0.3f, uvU1 = 0.2f, uvV1 = 0.4f;
        int posBase = 3;
        int lifetime = 100;
        uint32_t color0 = 0x11223344u;
    } sets[1];
};
using ParticleSet = ParticleSettings::ParticleSet;
struct ParticleSettingsRef { static ParticleSettings cur; };
ParticleSettings ParticleSettingsRef::cur;
const ParticleSettings::SetDefinition &currentParticleSet(int set) {
    return ParticleSettingsRef::cur.sets[set];
}

struct PaintCanvas {
    struct Uv { uint16_t point; float u, v; };
    struct Color { uint16_t point; uint32_t value; };
    std::vector<Uv> uv;
    std::vector<Color> colors;
    void MeshSetUv(uint32_t, uint16_t point, float u, float v) {
        uv.push_back({point, u, v});
    }
    void MeshSetColor(uint32_t, uint16_t point, uint32_t value) {
        colors.push_back({point, value});
    }
};

struct IParticleSystem {
    int baseEmitCalls = 0, lastBaseDelta = -1;
    void emit(int delta) { ++baseEmitCalls; lastBaseDelta = delta; }
};

struct ParticleSystemMesh : IParticleSystem {
    struct Sets { ParticleSet values[1]{ParticleSet::dummy}; ParticleSet operator[](int i) {
        return values[i];
    } } particleSets;
    int particleSetIndex = 0, field_0x78 = 1, field_0x7c = 2;
    int maxParticles = 2, currentParticle = 0, frameCounter = 0;
    uint32_t flags = 0x1000, edgeCount = 1, stride = 1;
    uint32_t resource = 1, resourceOffset = 100;
    uint8_t mirror = 0, newSectionStarted = 1, wide = 0, alphaFade = 0;
    uint8_t emitEnabled = 1, renderEnabled = 1;
    Matrix matrixValue{{10, 20, 30}};
    const Matrix *matrix = &matrixValue;
    Vector lastTrailPosition{};
    Vector velocities[8]{};
    Vector *particleVelocities = velocities;
    int ages[2]{-1, -1};
    int *particleAges = ages;
    int8_t ids[2]{0, 0};
    int8_t *particleSetIds = ids;
    PaintCanvas canvasValue;
    PaintCanvas *canvas = &canvasValue;
    int edgeCalls = 0, finishCalls = 0, extraCalls = 0;
    int trailUpdates = 0, usualUpdates = 0, colorUpdates = 0;
    int lastEdgePoint = -1, lastFinished = -1, extraSlot = -1;
    Vector lastEdge{}, lastEdgeOffset{}, lastFinishRight{}, lastFinishUp{}, extraPosition{};
    bool extraFinish = false;

    void setQuadEdge(const Vector &position, int point, const Vector &offset) {
        ++edgeCalls; lastEdge = position; lastEdgeOffset = offset; lastEdgePoint = point;
    }
    void setParticle(const Vector &position, float, uint32_t, float, float, float, float,
                     bool, float, float, const Vector &, bool finish) {
        ++extraCalls; extraPosition = position; extraSlot = currentParticle;
        extraFinish = finish;
    }
    void finishCurrentTrailParticle(ParticleSet, int id, const Vector &right, const Vector &up) {
        ++finishCalls; lastFinished = id; lastFinishRight = right; lastFinishUp = up;
        particleAges[id] = 0;
    }
    void incId() { currentParticle = (currentParticle + 1) % maxParticles; }
    void updateTrailEdges(int, int) { ++trailUpdates; }
    void updateUsualEdges(int, int) { ++usualUpdates; }
    void updateSingleColor(int) { ++colorUpdates; }
    void emit(int id);
    void emitTrail(int delta);
    void updateSingle(int id, float delta);
};

void _psm_matrixGetRight(Vector *out, const Matrix *m) { *out = AbyssEngine::AEMath::MatrixGetRight(*m); }
void _psm_matrixGetUp(Vector *out, const Matrix *m) { *out = AbyssEngine::AEMath::MatrixGetUp(*m); }
void _psm_vectorScale(Vector *out, const Vector *v, float scale) { *out = *v * scale; }
void _psm_finishCurrentTrailParticle(ParticleSystemMesh *mesh, ParticleSet set, int id,
                                     const Vector *right, const Vector *up) {
    mesh->finishCurrentTrailParticle(set, id, *right, *up);
}
void _psm_meshSetPoint(PaintCanvas *, uint32_t, uint16_t, float, float, float) {}

// PRODUCTION_METHODS

static bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }

int main() {
    ParticleSystemMesh ordinary;
    ordinary.emit(7);
    assert(ordinary.baseEmitCalls == 1 && ordinary.lastBaseDelta == 7);
    assert(ordinary.finishCalls == 0);
    ordinary.flags |= 0x80;
    ordinary.emit(8);
    assert(ordinary.baseEmitCalls == 1 && ordinary.finishCalls == 0);
    ordinary.emitEnabled = 0;
    ordinary.newSectionStarted = 0;
    ordinary.emit(9);
    assert(ordinary.newSectionStarted == 1 && ordinary.baseEmitCalls == 1);

    ParticleSystemMesh trailDispatch;
    trailDispatch.flags |= 0x8000;
    trailDispatch.emit(8);
    assert(trailDispatch.baseEmitCalls == 0 && trailDispatch.finishCalls == 1);

    ParticleSystemMesh mesh;
    mesh.emitTrail(10);
    assert(mesh.finishCalls == 1 && mesh.lastFinished == 0);
    assert(mesh.currentParticle == 1 && mesh.frameCounter == 0 && mesh.newSectionStarted == 0);
    assert(mesh.edgeCalls == 2 && mesh.lastEdgePoint == 106);
    assert(near(mesh.lastEdge.x, 12) && near(mesh.lastEdge.y, 22) && near(mesh.lastEdge.z, 35));
    assert(near(mesh.lastFinishRight.x, 3) && near(mesh.lastFinishUp.y, 3));
    assert(near(mesh.velocities[2].x, 3) && near(mesh.velocities[3].x, 0));
    assert(mesh.ages[1] == -2 && mesh.canvas->colors.size() == 4);
    assert(mesh.canvas->colors[0].value == 0x11223300u);
    assert(mesh.canvas->colors[2].value == 0x11223344u);
    assert(mesh.canvas->uv.size() == 4 && near(mesh.canvas->uv[3].v, 0.4f));

    mesh.emitTrail(20);
    assert(mesh.edgeCalls == 3 && mesh.lastEdgePoint == 106);
    assert(mesh.finishCalls == 1 && mesh.frameCounter == 20 && mesh.canvas->colors.size() == 4);
    mesh.flags |= 0x100000;
    mesh.emitTrail(10);
    assert(mesh.canvas->uv.size() == 6 && mesh.canvas->uv[4].point == 106);
    assert(near(mesh.canvas->uv[4].v, 0.33f) && mesh.frameCounter == 30);

    mesh.lastTrailPosition = {1000, 0, 0};
    mesh.frameCounter = 100;
    mesh.emitTrail(1);
    assert(mesh.finishCalls == 2 && mesh.currentParticle == 0);
    assert(mesh.frameCounter == 0 && mesh.newSectionStarted == 0 && mesh.ages[0] == -2);
    assert(mesh.canvas->colors.size() == 8);
    assert(mesh.canvas->colors[4].value == 0x11223344u);

    ParticleSystemMesh cap;
    ParticleSettingsRef::cur.sets[0].colorFlag = 4.0f;
    cap.emitTrail(1);
    assert(cap.extraCalls == 1 && cap.extraSlot == 2 && cap.extraFinish);
    assert(near(cap.extraPosition.z, 33.0f));
    assert(near(cap.lastTrailPosition.z, 31.0f));
    assert(cap.currentParticle == 1);

    ParticleSystemMesh rotated;
    rotated.flags = 0x1000 | 0x2000 | 0x4000 | 0x20000;
    rotated.edgeCount = 3;
    rotated.stride = 6;
    rotated.wide = 1;
    rotated.mirror = 1;
    rotated.alphaFade = 1;
    rotated.newSectionStarted = 0;
    rotated.emitTrail(1);
    assert(rotated.edgeCalls == 3 && rotated.lastEdgePoint == 118);
    assert(near(rotated.lastEdgeOffset.z, 2.0f));
    assert(rotated.finishCalls == 0 && rotated.frameCounter == 1);

    ParticleSystemMesh dormant;
    dormant.flags |= 0x8000;
    dormant.newSectionStarted = 0;
    dormant.ages[0] = -2;
    dormant.updateSingle(0, 10.0f);
    assert(dormant.trailUpdates == 1 && dormant.finishCalls == 0);
    assert(dormant.ages[0] == -2 && dormant.colorUpdates == 0);

    ParticleSystemMesh resumed;
    resumed.flags |= 0x8000;
    resumed.newSectionStarted = 1;
    resumed.ages[0] = -2;
    resumed.updateSingle(0, 10.0f);
    assert(resumed.trailUpdates == 1 && resumed.finishCalls == 1);
    assert(resumed.ages[0] == 10 && resumed.colorUpdates == 1);

    ParticleSystemMesh ongoing;
    ongoing.flags |= 0x8000;
    ongoing.newSectionStarted = 0;
    ongoing.ages[0] = 5;
    ongoing.updateSingle(0, 10.0f);
    assert(ongoing.trailUpdates == 1 && ongoing.finishCalls == 0);
    assert(ongoing.ages[0] == 15 && ongoing.colorUpdates == 1);
}
