#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdint>

namespace FMOD { struct Event {}; }
namespace AbyssEngine { namespace AEMath {
struct Vector { float x, y, z; };
struct Matrix {
    float m[15]{};
    Matrix() { m[0] = m[5] = m[10] = m[12] = m[13] = m[14] = 1.0f; }
    template<class... Floats> explicit Matrix(Floats... values) : m{static_cast<float>(values)...} {
        static_assert(sizeof...(values) == 15);
    }
    Matrix &operator*=(const Matrix &) { ++multiplies; return *this; }
    static int multiplies;
};
int Matrix::multiplies = 0;
Matrix operator*(const Matrix &a, const Matrix &b) {
    Matrix result = a;
    result.m[3] += b.m[3];
    result.m[7] += b.m[7];
    result.m[11] += b.m[11];
    return result;
}
static float rotationX = 0.0f, rotationY = 0.0f;
Matrix MatrixSetRotation(Matrix &matrix, float x, float y, float z) {
    assert(z == 0.0f);
    rotationX = x;
    rotationY = y;
    return matrix;
}
Vector VectorNormalize(const Vector &v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / length, v.y / length, v.z / length};
}
Vector VectorCross(const Vector &a, const Vector &b) {
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}
Vector MatrixRotateVector(const Matrix &m, const Vector &v) {
    return {v.x * m.m[0] + v.y * m.m[1] + v.z * m.m[2],
            v.x * m.m[4] + v.y * m.m[5] + v.z * m.m[6],
            v.x * m.m[8] + v.y * m.m[9] + v.z * m.m[10]};
}
}}
using AbyssEngine::AEMath::Matrix;
using AbyssEngine::AEMath::Vector;

struct FModSound {
    int count = 0, indices[4]{};
    float values[4]{};
    void setParamValue(FMOD::Event *event, int index, float value) {
        assert(reinterpret_cast<intptr_t>(event) == 1);
        indices[count] = index;
        values[count++] = value;
    }
};
namespace AbyssEngine {
struct PaintCanvas {
    Matrix ship{}, body{};
    int sets = 0, gets = 0;
    void *TransformGetLocal(unsigned id) {
        ++gets;
        assert(id == 11 || id == 12);
        return id == 11 ? &ship : &body;
    }
    void TransformSetLocal(unsigned id, const Matrix &matrix) {
        assert(id == 11);
        ++sets;
        ship = matrix;
    }
};
}
struct Globals {
    static FModSound *sound;
    static AbyssEngine::PaintCanvas *Canvas;
};
FModSound *Globals::sound = nullptr;
AbyssEngine::PaintCanvas *Globals::Canvas = nullptr;
struct AEGeometry { unsigned transform = 0; };
struct TargetFollowCamera {
    int translations = 0;
    Vector last{};
    void translateNoUpdate(float x, float y, float z) {
        ++translations;
        last = {x, y, z};
    }
};
struct Player {
    Matrix transformMatrix{};
    int eventCalls = 0;
    int GetEngineEvent() { ++eventCalls; return 1; }
};
static const float g_PE_hs_throttleBias = 0.2f;
struct PlayerEgo {
    void *player = nullptr;
    AEGeometry *geometry = nullptr, *field_0x4 = nullptr;
    double yawAccumD = 0.0, rollAccum = 0.0, pitchAccumD = 0.0;
    float yawAccumF = 0.0f;
    float thrust = 2.0f, speedValue = 3.0f;
    float strafeAccel = 0.0f, strafeFactor = 0.0f;
    int shakeIntensity = 10;
    unsigned char rolling = 0;
    Matrix rollMatrix{};
    void *targetFollowCamera = nullptr;
    int pitchRamp = 1, yawRamp = 1, pitchAccumDir = 1, yawAccumDir = 1;
    float yawAccumulator = 1.0f, pitchAccumulator = 1.0f;
    void handleShip(int dt);
};

// PRODUCTION_METHOD

static bool near(float a, float b) { return std::fabs(a - b) < 0.00001f; }

int main() {
    Player player;
    AEGeometry ship{11}, body{12};
    TargetFollowCamera camera;
    FModSound sound;
    AbyssEngine::PaintCanvas canvas;
    Globals::sound = &sound;
    Globals::Canvas = &canvas;
    PlayerEgo ego;
    ego.player = &player;
    ego.geometry = &ship;
    ego.field_0x4 = &body;
    ego.targetFollowCamera = &camera;
    ego.strafeAccel = 0.5f;
    reinterpret_cast<float *>(&ego.yawAccumD)[0] = -0.6f;
    reinterpret_cast<float *>(&ego.rollAccum)[0] = 0.3f;
    reinterpret_cast<float *>(&ego.pitchAccumD)[0] = 1.0f;
    reinterpret_cast<float *>(&ego.pitchAccumD)[1] = -2.0f;
    ego.handleShip(10);
    assert(player.eventCalls == 2 && sound.count == 2);
    assert(sound.indices[0] == 0 && near(sound.values[0], 0.6f));
    assert(sound.indices[1] == 1 && near(sound.values[1], 0.56f));
    float expectedRotation = 0.0000152587890625f * 6.2831855f * 10.0f * 0.033f;
    assert(std::fabs(AbyssEngine::AEMath::rotationX - expectedRotation) < 1e-9f);
    assert(std::fabs(AbyssEngine::AEMath::rotationY + 2.0f * expectedRotation) < 1e-9f);
    assert(near(ego.yawAccumF, -2.0f));
    assert(Matrix::multiplies == 1);
    assert(canvas.sets == 1 && canvas.gets == 3);
    assert(near(canvas.ship.m[3], 5.0f) && near(canvas.ship.m[11], 60.0f));
    assert(near(player.transformMatrix.m[3], 5.0f));
    assert(camera.translations == 1 && near(camera.last.x, 5.0f));
    assert(near(ego.strafeAccel, 0.35f));
    assert(ego.pitchRamp == 0 && ego.yawRamp == 0);
    assert(ego.yawAccumulator == 0.0f && ego.pitchAccumulator == 0.0f);
    assert(ego.pitchAccumDir == 1 && ego.yawAccumDir == 1);
    assert(reinterpret_cast<float *>(&ego.yawAccumD)[0] == 0.0f);
    assert(reinterpret_cast<float *>(&ego.rollAccum)[0] == 0.0f);

    ego.rolling = 1;
    ego.strafeAccel = 0.005f;
    ego.handleShip(0);
    assert(Matrix::multiplies == 3);
    assert(camera.translations == 1 && near(ego.strafeFactor, 0.1f));
    assert(sound.count == 4 && near(sound.values[2], 0.0f));
    std::puts("PlayerEgo handleShip smoke: OK");
}
