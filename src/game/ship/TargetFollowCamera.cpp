#include "game/ship/TargetFollowCamera.h"
#include "engine/core/AERandom.h"
#include "engine/math/AEMath.h"
#include "engine/render/AEGeometry.h"
#include "engine/render/PaintCanvas.h"

#include <cstdint>
#include <cstring>

static const float g_TFC_hiddenShipDistance = 800.0f;
static const float g_TFC_firstPersonRumbleScale = 0.001f;
static const float g_TFC_firstPersonShakeScale = 0.0015f;

static inline float bitsToFloat(uint32_t u) {
    float f;
    memcpy(&f, &u, 4);
    return f;
}

static inline int tfcRandom(int bound) {
    if (bound <= 0)
        return 0;
    return AERandom::gRandom ? AERandom::gRandom->nextInt(bound) : 0;
}

static inline float tfcDampingFactor(const double coeffs[5], int dt) {
    const double t = static_cast<double>(dt);
    const double t2 = t * t;
    const double t3 = t2 * t;
    const double t4 = t3 * t;
    return static_cast<float>((coeffs[4] + coeffs[1] * t2 + coeffs[0] * t +
                               coeffs[2] * t3 + coeffs[3] * t4) /
                              static_cast<float>(dt));
}

TargetFollowCamera::TargetFollowCamera(unsigned id, AEGeometry *target,
                                       Vector camOffset, Vector targetOffset) {
    this->id = id;
    this->target = target;

    this->firstPersonMatrix = AbyssEngine::AEMath::Matrix();
    this->fpOffsetX = this->fpOffsetY = this->fpOffsetZ = 0;
    this->localMatrix = AbyssEngine::AEMath::Matrix();

    *getTargetOffset() = camOffset;
    *getCamOffset() = targetOffset;

    Vector zero = {0.0f, 0.0f, 0.0f};
    *getPosition() = zero;
    *getTargetPos() = zero;

    Matrix m = target->getMatrix();
    *getTargetPos() = MatrixGetPosition(m);
    *getPosition() = MatrixGetPosition(m);

    Vector up = {0.0f, 1.0f, 0.0f};
    *getUp() = up;

    this->locked = 0;
    this->rumbleTimer = 0;
    this->rotateAroundTargetEnabled = 0;
    this->firstPerson = 0;
    this->hideShip = 0;
    this->useTargetsUpVec = 1;
    this->shakeAmount = 0.0f;
    this->shakeFrequency = 5;

    float zoom = VectorLength(targetOffset);

    this->handlingDampingA = 0.005f;
    this->handlingDampingB = 0.006f;
    this->zoom = zoom;
    this->fixed = 0;

    aproximateCooefficientsForAproximationOfDampingFunktion(
        zoom, dampCoeffA[0], dampCoeffA[1], dampCoeffA[2], dampCoeffA[3], dampCoeffA[4]);
    aproximateCooefficientsForAproximationOfDampingFunktion(
        this->handlingDampingB,
        dampCoeffB[0], dampCoeffB[1], dampCoeffB[2], dampCoeffB[3], dampCoeffB[4]);
}

AEGeometry *TargetFollowCamera::getTarget() { return this->target; }
Vector *TargetFollowCamera::getPosition() { return reinterpret_cast<Vector *>(&this->posX); }
Vector *TargetFollowCamera::getTargetOffset() { return reinterpret_cast<Vector *>(&this->targetOffsetX); }
Vector *TargetFollowCamera::getCamOffset() { return reinterpret_cast<Vector *>(&this->camOffsetX); }

Matrix TargetFollowCamera::getLocal() { return this->localMatrix; }
void TargetFollowCamera::setLocal(Matrix m) { this->localMatrix = m; }
void TargetFollowCamera::setTarget(AEGeometry *t) { this->target = t; }
// IDA 0x15b498: _ZN18TargetFollowCamera10zoomTargetEf
void TargetFollowCamera::zoomTarget(float zoom) { this->zoom = zoom; }
void TargetFollowCamera::setRoll(float roll) { this->rollAngle = roll; }
void TargetFollowCamera::roll(float delta) { this->rollAngle += delta; }

TargetFollowCamera::~TargetFollowCamera() {
}

void TargetFollowCamera::setLookAtCam(bool e) { this->lookAtCam = e; }
void TargetFollowCamera::setActive(bool e) { this->active = e; }
void TargetFollowCamera::setFixed(bool e) { this->fixed = e; }
void TargetFollowCamera::setRotationAroundTarget(bool e) { this->rotateAroundTargetEnabled = e; }
void TargetFollowCamera::useTargetsUpVector(bool e) { this->useTargetsUpVec = e; }

bool TargetFollowCamera::isInLookAtMode() { return this->lookAtCam != 0; }
bool TargetFollowCamera::isInFastForwardMode() { return this->fastForward != 0; }
bool TargetFollowCamera::hideShipForFirstPersonCam() { return this->hideShip != 0; }

void TargetFollowCamera::setFirstPersonMatrix(Matrix &m) { this->firstPersonMatrix = m; }

void TargetFollowCamera::setCamOffset(const Vector &offset) {
    *getCamOffset() = offset;
    this->zoom = VectorLength(offset);
}

void TargetFollowCamera::setTargetOffset(const Vector &offset) {
    *getTargetOffset() = offset;
}

void TargetFollowCamera::setPosition(const Vector &position) {
    this->posX = position.x;
    this->posY = position.y;
    this->posZ = position.z;
}

void TargetFollowCamera::setPosition(float x, float y, float z) {
    this->posX = x;
    this->posY = y;
    this->posZ = z;
}

// IDA 0x15b34a: _ZN18TargetFollowCamera19setRumblePercentageEfi
void TargetFollowCamera::setRumblePercentage(float pct, int frequency) {
    this->shakeAmount = pct;
    this->shakeFrequency = frequency;
}

// IDA 0x15b338: _ZN18TargetFollowCamera18setBoostPercentageEfi
void TargetFollowCamera::setBoostPercentage(float pct, int frequency) {
    if (frequency > 8) frequency = 8;
    if (frequency < 2) frequency = 2;
    this->shakeAmount = pct;
    this->shakeFrequency = frequency;
}

void TargetFollowCamera::enableFirstPersonCam(bool enabled) {
    this->firstPerson = enabled;
    Vector offset = {0.0f, 150.0f, -800.0f};
    *reinterpret_cast<Vector *>(&this->fpOffsetX) = offset;
    this->shakeAccum = 0;
    this->shakeReference = 0;
}

// IDA 0x15b46c: _ZN18TargetFollowCamera18rotateAroundTargetEfff
void TargetFollowCamera::rotateAroundTarget(float x, float y, float z) {
    Vector r = {x, y, z};
    *getRotation() = r;
}

// IDA 0x15b5ec: _ZN18TargetFollowCamera3hitEv
void TargetFollowCamera::hit() {
    if (this->rumbleActive != 0) return;
    this->rumbleTimer = 1000;
    this->rumbleActive = 1;
    this->rumbleStrength = 6;
    this->smallRumble = 0;
}

// IDA 0x15b60e: _ZN18TargetFollowCamera8hitSmallEv
void TargetFollowCamera::hitSmall() {
    if (this->rumbleActive != 0) return;
    this->rumbleTimer = 50;
    this->rumbleActive = 1;
    this->rumbleStrength = 2;
    this->smallRumble = 1;
}

void TargetFollowCamera::setLocked(bool locked) {
    this->locked = locked;
    if (!locked) return;
    Matrix m = this->target->getMatrix();
    *getUp() = MatrixGetUp(m);
    *getPosition() = MatrixTransformVector(m, *getCamOffset());
    update(50);
}

void TargetFollowCamera::translateNoUpdate(float dx, float dy, float dz) {
    this->posX += dx;
    this->posY += dy;
    this->posZ += dz;
    this->targetX += dx;
    this->targetY += dy;
    this->targetZ += dz;
}

void TargetFollowCamera::translate(float dx, float dy, float dz) {
    translateNoUpdate(dx, dy, dz);
    update(1000);
}

void TargetFollowCamera::setShipHandling(float handling) {
    float s = handling * 0.01f;
    this->shipHandling = handling;
    this->handlingDampingA = 0.003f + (1.0f - s) * 0.015f;
    this->handlingDampingB = 0.001f + s * 0.011f;
    update(1);
}

void TargetFollowCamera::resetShipHandling() {
    this->handlingDampingA = bitsToFloat(0x3ba3d70a);
    this->handlingDampingB = bitsToFloat(0x3bc49ba6);
    update(1);
}

// IDA 0x15b4a4: _ZN18TargetFollowCamera20calculateCoefficentsEf
void TargetFollowCamera::calculateCoefficents(float t) {
    aproximateCooefficientsForAproximationOfDampingFunktion(
        this->handlingDampingA * t,
        dampCoeffA[0], dampCoeffA[1], dampCoeffA[2], dampCoeffA[3], dampCoeffA[4]);
    aproximateCooefficientsForAproximationOfDampingFunktion(
        this->handlingDampingB * t,
        dampCoeffB[0], dampCoeffB[1], dampCoeffB[2], dampCoeffB[3], dampCoeffB[4]);
}

void TargetFollowCamera::setFastForwardMode(bool enabled) {
    if (enabled == (this->fastForward != 0)) return;
    {
        float s = this->shipHandling * 0.01f;
        this->handlingDampingA = 0.003f + (1.0f - s) * 0.015f;
        this->handlingDampingB = 0.001f + s * 0.011f;
    }
    aproximateCooefficientsForAproximationOfDampingFunktion(
        this->handlingDampingA,
        dampCoeffA[0], dampCoeffA[1], dampCoeffA[2], dampCoeffA[3], dampCoeffA[4]);
    aproximateCooefficientsForAproximationOfDampingFunktion(
        this->handlingDampingB,
        dampCoeffB[0], dampCoeffB[1], dampCoeffB[2], dampCoeffB[3], dampCoeffB[4]);
    this->fastForward = enabled;
}

void TargetFollowCamera::aproximateCooefficientsForAproximationOfDampingFunktion(
    float t, double &outB, double &outA, double &outC, double &outD, double &outE) {
    // IDA 0x15a688: _ZN18TargetFollowCamera55aproximateCooefficientsForAproximationOfDampingFunktionEfRdS0_S0_S0_S0_
    const double x = static_cast<double>(t);
    const double x3 = x * x * x;
    const double x5 = x3 * x * x;
    const double x7 = x5 * x * x;

    outB = x7 * x * -103298.314
         + x7 * 15119.0744
         + x5 * x * -969.134662
         + x5 * 43.7588838
         + x3 * x * -1.71338562
         + x3 * 0.0422027858
         + x * x * -0.00000123348984
         + x * 0.00000000121021113;
    outA = -(x7 * x * -12480406.3
           + x7 * 1803725.98
           + x5 * x * -111015.303
           + x5 * 4448.09753
           + x3 * x * -128.529173
           + x3 * 0.148842358
           + x * x * 0.166515974
           + x * 0.000000147818764
           + -4.57169049e-11);
    outC = x7 * x * -149924392.0
         + x7 * 26011080.9
         + x5 * x * -2122499.17
         + x5 * 112294.223
         + x3 * x * -3466.19911
         + x3 * 0.377812139
         + x * x * -0.000964730718
         + x * 0.500000935
         + -2.8527534e-10;
    outD = x7 * -448847785.0
         + x7 * x * 2837989980.0
         + x5 * x * 31896097.6
         + x5 * -1451302.48
         + x3 * x * 39522.4566
         + x3 * -11.2522201
         + x * x * 0.192802034
         + x * 0.499974387
         + 0.00000000791336895;
    outE = -(x7 * -609933690.0
           + x7 * x * 3091464100.0
           + x5 * x * 56126838.4
           + x5 * -3103835.92
           + x3 * x * 89689.5018
           + x3 * -1.65895547
           + x * x * 0.00422802002
           + x * -0.00000437549158
           + 0.00000000149698935);
}

// IDA 0x15aa48: _ZN18TargetFollowCamera6updateEi
void TargetFollowCamera::update(int dt) {
    if (this->fixed != 0) {
        *getPosition() = MatrixGetPosition(this->localMatrix);
        PaintCanvas::gCanvas->CameraSetLocal(this->id, this->localMatrix);
        if (this->target != 0) {
            Matrix m = this->target->getMatrix();
            *getUp() = MatrixGetUp(m);
            *getTargetPos() = MatrixGetPosition(m);
        }
        return;
    }

    if (dt <= 0 || this->active == 0) return;

    Matrix m = this->target->getMatrix();

    if (this->lookAtCam == 0) {
        if (this->locked == 0) {
            if (this->firstPerson == 0) {
                Vector savedPos = *getPosition();
                Vector savedTarget = *getTargetPos();
                *getUp() = MatrixGetUp(m);

                if (this->rotateAroundTargetEnabled != 0) {
                    Matrix rot;
                    MatrixSetRotation(rot, this->rotX, this->rotY, this->rotZ,
                                      AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
                    m *= rot;
                    float curLen = VectorLength(*getCamOffset());
                    if (curLen != 0.0f && curLen != this->zoom)
                        *getCamOffset() *= (this->zoom / curLen);
                }

                Vector desiredPos = MatrixTransformVector(m, *getCamOffset());
                Vector desiredTarget = MatrixTransformVector(m, *getTargetOffset());

                Vector diff = desiredTarget - savedTarget;
                float kA = tfcDampingFactor(dampCoeffA, dt);
                diff *= kA;
                diff += savedTarget;
                *getTargetPos() = diff;

                Vector diff2 = desiredPos - savedPos;
                float kB = tfcDampingFactor(dampCoeffB, dt);
                diff2 *= kB;

                if (this->hideShip != 0) {
                    float l = VectorLength(diff2) + this->shakeAccum;
                    this->shakeAccum = l;
                    this->hideShip = (l < g_TFC_hiddenShipDistance) ? 1 : 0;
                }

                diff2 += savedPos;
                *getPosition() = diff2;
            } else {
                Vector cur = *getPosition();
                *getPosition() = MatrixTransformVector(
                    this->firstPersonMatrix,
                    *reinterpret_cast<Vector *>(&this->fpOffsetX));

                if (this->shakeReference == 0.0f ||
                    this->shakeAccum < this->shakeReference * 1.5f) {
                    Vector d = *getPosition() - cur;
                    if (this->shakeReference == 0.0f)
                        this->shakeReference = VectorLength(d);

                    float k = tfcDampingFactor(dampCoeffB, dt);
                    d *= k;
                    float l = VectorLength(d) + this->shakeAccum;
                    this->shakeAccum = l;
                    this->hideShip = (l >= this->shakeReference * 0.75f) ? 1 : 0;
                    cur += d;
                    *getPosition() = cur;
                }

                *getUp() = MatrixGetUp(this->firstPersonMatrix);
                *getTargetPos() = MatrixGetPosition(this->firstPersonMatrix);
            }
        } else {
            this->hideShip = 0;
            *getTargetPos() = MatrixGetPosition(m);
            this->targetX -= this->posX;
            this->targetY -= this->posY;
            this->targetZ -= this->posZ;
        }
    } else {
        Matrix fp = m;
        if (this->useTargetsUpVec == 0)
            fp = AbyssEngine::AEMath::Matrix();
        *getUp() = MatrixGetUp(fp);
        *getTargetPos() = MatrixGetPosition(fp);
        this->hideShip = 0;
    }

    if (this->firstPerson != 0) {
        Matrix fpm = this->firstPersonMatrix;
        *getPosition() = MatrixGetPosition(fpm);
        *getUp() = MatrixGetUp(fpm);
        Vector dir = MatrixGetDir(fpm);
        dir -= *getPosition();
        *getTargetPos() = dir;
    }

    if (this->rumbleActive != 0) {
        this->rumbleTimer -= dt;
        if (this->rumbleTimer < 1)
            this->rumbleActive = 0;
        float scale = (this->firstPerson == 0) ? 1.0f : g_TFC_firstPersonRumbleScale;
        int amt = this->rumbleStrength;
        this->posX += scale * (float) (tfcRandom(amt << 1) - amt);
        this->posY += scale * (float) (tfcRandom(amt << 1) - amt);
        this->posZ += scale * (float) (tfcRandom(amt << 1) - amt);
    }

    float shake = this->shakeAmount;
    if (shake > 0.0f) {
        int freq = this->shakeFrequency;
        float scale = (this->firstPerson == 0) ? 1.0f : g_TFC_firstPersonShakeScale;
        int b = freq << 1;
        this->targetX += scale * shake * (float) (tfcRandom(b) - freq);
        this->targetY += scale * shake * (float) (tfcRandom(b) - freq);
        this->targetZ += scale * shake * (float) (tfcRandom(b) - freq);
    }

    Matrix look = AbyssEngine::AEMath::MatrixGetLookAt(*getPosition(), *getTargetPos(), *getUp());
    Matrix rollMat;
    MatrixSetRotation(rollMat, 0.0f, 0.0f, this->rollAngle);
    look *= rollMat;
    PaintCanvas::gCanvas->CameraSetLocal(this->id, look);
    this->localMatrix = look;
}
