// Focused host runtime check: real camera, AEMath operations and AERandom;
// geometry/Matrix construction are stubs, renderer is an unexecuted trap.
// This is not an ARM32 runtime or renderer integration test.
#include "game/ship/TargetFollowCamera.h"
#include "engine/math/AEMath.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"
#include <cassert>
#include <cmath>
#include <cstdio>

AEGeometry::AEGeometry(PaintCanvas *service) : canvas(service) {}
AEGeometry::~AEGeometry() = default;
Matrix &AEGeometry::getMatrix() { return referenceMatrix; }
void *Globals::rnd = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
void PaintCanvas::CameraSetLocal(unsigned int, const Matrix &) {
    assert(false && "renderer is outside this focused smoke");
}

// The repository has duplicate Matrix operators in its two math units.
// Use the real AEMath unit and supply only the standalone default constructor.
AbyssEngine::AEMath::Matrix::Matrix() {
    for (float &value : m) value = 0.0f;
    m[0] = m[5] = m[10] = m[12] = m[13] = m[14] = 1.0f;
}

static bool near(float a, float b) { return std::fabs(a - b) < 0.00001f; }

int main() {
    AEGeometry geometry(nullptr);
    geometry.referenceMatrix.m[10] = 1.0f;
    geometry.referenceMatrix.m[3] = 100.0f;
    geometry.referenceMatrix.m[7] = 200.0f;
    geometry.referenceMatrix.m[11] = 300.0f;
    const Vector targetOffset{1.0f, 2.0f, 3.0f};
    const Vector cameraOffset{4.0f, 5.0f, 6.0f};
    TargetFollowCamera camera(7, &geometry, targetOffset, cameraOffset);
    assert(camera.getTarget() == &geometry && camera.id == 7);
    assert(near(camera.posX, 104) && near(camera.posY, 205) && near(camera.posZ, 306));
    assert(near(camera.targetX, 101) && near(camera.targetY, 202) && near(camera.targetZ, 303));
    assert(camera.active == 1 && camera.lookAtCam == 0 && camera.rumbleActive == 0);
    assert(near(camera.shipHandling, 1) && near(camera.rollAngle, 0));

    camera.fastForward = 0; // Android constructor leaves this byte unspecified.
    camera.setShipHandling(50.0f);
    assert(near(camera.handlingDampingA, 0.0105f));
    assert(near(camera.handlingDampingB, 0.0065f));
    double before[5];
    for (int i = 0; i < 5; ++i) before[i] = camera.dampCoeffA[i];
    camera.setFastForwardMode(true);
    assert(camera.fastForward == 1);
    for (int i = 0; i < 5; ++i) assert(camera.dampCoeffA[i] == before[i]);

    camera.translateNoUpdate(10, 20, 30);
    assert(near(camera.posX, 114) && near(camera.targetZ, 333));
    camera.setPosition(Vector{7, 8, 9});
    assert(camera.posX == 7 && camera.posY == 8 && camera.posZ == 9);
    camera.setBoostPercentage(0.4f, 99);
    assert(camera.shakeFrequency == 8);
    camera.setBoostPercentage(0.4f, -1);
    assert(camera.shakeFrequency == 2);
    camera.hitSmall();
    assert(camera.rumbleTimer == 50 && camera.rumbleStrength == 2 && camera.smallRumble == 1);
    camera.hit(); // An active small hit must not be overwritten.
    assert(camera.rumbleTimer == 50 && camera.rumbleStrength == 2);
    camera.lookAtCam = 0xfe;
    assert(camera.isInLookAtMode() == 0xfe);
    std::puts("TargetFollowCamera host smoke: OK");
}
