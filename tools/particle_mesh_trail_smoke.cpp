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
        uint32_t color1 = 0xa1b2c3d4u;
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
    struct Move { uint16_t point; float x, y, z; };
    struct FloatColor { uint16_t point; float a, r, g, b; };
    struct Point { uint16_t point; float x, y, z; };
    std::vector<Uv> uv;
    std::vector<Color> colors;
    std::vector<Move> moves;
    std::vector<FloatColor> floatColors;
    std::vector<Point> points;
    std::vector<char> order;
    void MeshSetUv(uint32_t, uint16_t point, float u, float v) {
        uv.push_back({point, u, v});
    }
    void MeshSetColor(uint32_t, uint16_t point, uint32_t value) {
        colors.push_back({point, value});
    }
};

struct IParticleSystem {
    int baseEmitCalls = 0, lastBaseDelta = -1;
    int interpolateCalls = 0;
    void emit(int delta) { ++baseEmitCalls; lastBaseDelta = delta; }
    void interpolateColor(int, float &a, float &r, float &g, float &b) {
        ++interpolateCalls;
        a = 0.1f; r = 0.2f; g = 0.3f; b = 0.4f;
    }
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
    int usualUpdates = 0;
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
    void updateTrailEdges(int, int);
    void updateUsualEdges(int, int) { ++usualUpdates; }
    void updateSingleColor(int);
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
void _psm_meshSetPoint(PaintCanvas *canvas, uint32_t, uint16_t point,
                       float x, float y, float z) {
    canvas->points.push_back({point, x, y, z});
    canvas->order.push_back('P');
}
void _psm_meshSetColor(PaintCanvas *canvas, uint32_t, uint16_t point,
                       float a, float r, float g, float b) {
    canvas->floatColors.push_back({point, a, r, g, b});
    canvas->order.push_back('C');
}
void _psm_meshTranslatePoint(PaintCanvas *canvas, uint32_t, uint16_t point,
                             float x, float y, float z) {
    canvas->moves.push_back({point, x, y, z});
}

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
    assert(dormant.canvas->moves.size() == 2 && dormant.finishCalls == 0);
    assert(dormant.ages[0] == -2 && dormant.canvas->floatColors.empty());

    ParticleSystemMesh resumed;
    resumed.flags |= 0x8000;
    resumed.newSectionStarted = 1;
    resumed.ages[0] = -2;
    resumed.updateSingle(0, 10.0f);
    assert(resumed.canvas->moves.size() == 2 && resumed.finishCalls == 1);
    assert(resumed.ages[0] == 10 && resumed.canvas->floatColors.size() == 2);

    ParticleSystemMesh ongoing;
    ongoing.flags |= 0x8000;
    ongoing.newSectionStarted = 0;
    ongoing.ages[0] = 5;
    ongoing.updateSingle(0, 10.0f);
    assert(ongoing.canvas->moves.size() == 4 && ongoing.finishCalls == 0);
    assert(ongoing.ages[0] == 15 && ongoing.canvas->floatColors.size() == 2);

    ParticleSystemMesh edgeSign;
    edgeSign.flags |= 0x8000;
    edgeSign.ages[0] = 5;
    edgeSign.velocities[0] = {2, 3, 4};
    edgeSign.velocities[1] = {5, 6, 7};
    edgeSign.updateTrailEdges(0, 1000);
    const auto &moves = edgeSign.canvas->moves;
    assert(moves.size() == 4);
    assert(moves[0].point == 100 && near(moves[0].x, -2) && near(moves[0].y, -3) && near(moves[0].z, -4));
    assert(moves[1].point == 101 && near(moves[1].x, 2) && near(moves[1].y, 3) && near(moves[1].z, 4));
    assert(moves[2].point == 102 && near(moves[2].x, -5) && near(moves[2].y, -6) && near(moves[2].z, -7));
    assert(moves[3].point == 103 && near(moves[3].x, 5) && near(moves[3].y, 6) && near(moves[3].z, 7));

    ParticleSystemMesh colorStart;
    colorStart.flags |= 0x8000;
    colorStart.updateSingleColor(0);
    const auto &front = colorStart.canvas->floatColors;
    assert(front.size() == 2 && front[0].point == 102 && front[1].point == 103);
    assert(colorStart.interpolateCalls == 0);
    assert(near(front[0].a, 0xa1 / 255.0f) && near(front[0].r, 0xb2 / 255.0f));
    assert(near(front[0].g, 0xc3 / 255.0f) && front[0].b == 0.0f);

    ParticleSystemMesh alphaStart;
    alphaStart.flags |= 0x8000;
    alphaStart.alphaFade = 1;
    alphaStart.updateSingleColor(0);
    const auto &alphaFront = alphaStart.canvas->floatColors;
    assert(alphaFront.size() == 2 && alphaFront[0].a == 0.0f);
    assert(alphaFront[0].r == 0.0f && alphaFront[0].g == 0.0f);
    assert(near(alphaFront[0].b, 0xd4 / 255.0f));

    ParticleSystemMesh linkedColor;
    linkedColor.flags |= 0x8000;
    linkedColor.ages[1] = 5;
    linkedColor.updateSingleColor(0);
    const auto &linked = linkedColor.canvas->floatColors;
    assert(linkedColor.interpolateCalls == 1 && linked.size() == 4);
    assert(linked[0].point == 102 && linked[1].point == 103);
    assert(linked[2].point == 104 && linked[3].point == 105);
    assert(near(linked[0].a, 0.1f) && near(linked[2].b, 0.4f));

    ParticleSystemMesh wrapColor;
    wrapColor.flags |= 0x8000;
    wrapColor.ages[0] = 5;
    wrapColor.ages[1] = 5;
    wrapColor.updateSingleColor(1);
    const auto &wrapped = wrapColor.canvas->floatColors;
    assert(wrapColor.interpolateCalls == 1 && wrapped.size() == 4);
    assert(wrapped[0].point == 106 && wrapped[1].point == 107);
    assert(wrapped[2].point == 100 && wrapped[3].point == 101);

    ParticleSystemMesh expired;
    expired.flags |= 0x8000;
    expired.ages[0] = 95;
    expired.updateSingle(0, 10.0f);
    assert(expired.ages[0] == -1);
    assert(expired.canvas->floatColors.size() == 2 && expired.canvas->points.size() == 4);
    assert(expired.canvas->order == std::vector<char>({'C', 'C', 'P', 'P', 'P', 'P'}));
    for (int i = 0; i < 4; ++i) {
        const auto &point = expired.canvas->points[i];
        assert(point.point == 100 + i && point.x == 0 && point.y == 0 && point.z == 0);
    }

    ParticleSystemMesh boundary;
    boundary.flags |= 0x8000;
    boundary.ages[0] = 90;
    boundary.updateSingle(0, 10.0f);
    assert(boundary.ages[0] == 100 && boundary.canvas->points.empty());
    boundary.updateSingle(0, 1.0f);
    assert(boundary.ages[0] == -1 && boundary.canvas->points.size() == 4);
}
