#ifndef GOF2_TARGETFOLLOWCAMERA_H
#define GOF2_TARGETFOLLOWCAMERA_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/core/AbyssEngine.h"
#include "engine/render/AEGeometry.h"

#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
class AEGeometry;


using AbyssEngine::Matrix;
using AbyssEngine::Vector;

class TargetFollowCamera {
public:
    unsigned id;
    AEGeometry *target;
    float posX;
    float posY;
    float posZ;
    float targetX;
    float targetY;
    float targetZ;
    float upX;
    float upY;
    float upZ;
    float targetOffsetX;
    float targetOffsetY;
    float targetOffsetZ;
    float camOffsetX;
    float camOffsetY;
    float camOffsetZ;
    uint8_t locked;
    uint8_t lookAtCam;
    uint8_t active;
    uint8_t rumbleActive;
    int rumbleTimer;
    uint8_t rotateAroundTargetEnabled;
    uint8_t fastForward;
    float rotX;
    float rotY;
    float rotZ;
    double dampCoeffA[5];
    double dampCoeffB[5];
    float zoom;
    AbyssEngine::Matrix firstPersonMatrix;
    char firstPerson;
    float fpOffsetX;
    float fpOffsetY;
    float fpOffsetZ;
    uint8_t hideShip;
    float shakeAccum;
    float shakeReference;
    uint8_t useTargetsUpVec;
    float shakeAmount;
    int shakeFrequency;
    int rumbleStrength;
    uint8_t smallRumble;
    float handlingDampingA;
    float handlingDampingB;
    float rollAngle;
    float shipHandling;
    uint8_t fixed;
    AbyssEngine::Matrix localMatrix;

    TargetFollowCamera(unsigned id, AEGeometry *target,
                       AbyssEngine::Vector camOffset, AbyssEngine::Vector targetOffset);

    ~TargetFollowCamera();

    AEGeometry *getTarget();

    Vector *getPosition();

    Vector *getTargetPos() { return reinterpret_cast<Vector *>(&this->targetX); }
    Vector *getUp() { return reinterpret_cast<Vector *>(&this->upX); }

    Vector *getTargetOffset();

    Vector *getCamOffset();

    Vector *getRotation() { return reinterpret_cast<Vector *>(&this->rotX); }

    Matrix getLocal();

    void setLocal(Matrix m);

    void setTarget(AEGeometry *target);

    void setCamOffset(const Vector &offset);

    void setTargetOffset(const Vector &offset);

    void setPosition(const Vector &position);

    void setPosition(float x, float y, float z);

    void setFirstPersonMatrix(Matrix &m);

    void zoomTarget(float zoom);

    void setRoll(float roll);

    void roll(float delta);

    void setLookAtCam(bool enabled);

    void setActive(bool enabled);

    void setLocked(bool locked);

    void setFixed(bool enabled);

    void setRotationAroundTarget(bool enabled);

    void rotateAroundTarget(float x, float y, float z);

    void enableFirstPersonCam(bool enabled);

    void setFastForwardMode(bool enabled);

    void useTargetsUpVector(bool enabled);

    void setRumblePercentage(float pct, int frequency);

    void setBoostPercentage(float pct, int frequency);

    void setShipHandling(float handling);

    void resetShipHandling();

    void calculateCoefficents(float t);

    void aproximateCooefficientsForAproximationOfDampingFunktion(
        float t, double &outB, double &outA, double &outC,
        double &outD, double &outE);

    void hit();

    void hitSmall();

    void translate(float dx, float dy, float dz);

    void translateNoUpdate(float dx, float dy, float dz);

    bool isInLookAtMode();

    bool isInFastForwardMode();

    bool hideShipForFirstPersonCam();

    void update(int dt);
};
#endif
