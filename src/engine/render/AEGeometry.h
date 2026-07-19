#ifndef GOF2_AEGEOMETRY_H
#define GOF2_AEGEOMETRY_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/AEMath.h"
#include "engine/math/Matrix.h"

#include "engine/math/Vector.h"

#include "engine/render/AEGeomCanvas.h"
namespace AbyssEngine { class PaintCanvas; }
using ::AbyssEngine::PaintCanvas;


class LodMeshMerger;

namespace AbyssEngine {
    namespace AEMath {
        void VectorNormalize(Vector *out, const Vector *v);

        Vector operator-(const Vector &a, const Vector &b);

        Vector MatrixGetUp(const Matrix &m);

        Vector MatrixGetRight(const Matrix &m);

        Vector MatrixGetPosition(const Matrix &m);

        Vector MatrixGetDir(const Matrix &m);
    }
}


class AEGeometry {
public:
    uint32_t mergerIndex;
    LodMeshMerger *merger;
    uint16_t mesh;
    uint32_t transform;
    uint32_t parentTransform;
    uint32_t childTransform;
    uint32_t baseTransform;
    uint32_t meshId;
    uint32_t meshHandle;
    uint32_t altTransform;
    int32_t currentLod;
    PaintCanvas *canvas;

    Vector rotation;
    float scalingX;
    float scalingY;
    float scalingZ;

    uint16_t visibility;
    int32_t rotationOrder;
    int32_t lodCount;

    uint32_t *lodTransforms;
    uint32_t *lodChildTransforms;
    uint16_t *lodMeshes;
    uint16_t *lodChildMeshes;
    unsigned long long *lodDistancesSq;

    unsigned long long distSq;
    unsigned long long lastVisibleDistSq;
    Vector cameraDelta;
    Matrix referenceMatrix;

    AEGeometry(PaintCanvas *canvas);

    AEGeometry(uint16_t mesh, PaintCanvas *canvas, bool flag);

    ~AEGeometry();

    Vector getPosition();

    Vector getRotation();

    Vector getScaling();

    Vector getRightVector();

    Vector getUpVector();

    Vector getParentPosition();

    Vector getDirection();

    Matrix &getMatrix();

    Matrix &getReferenceMatrix();

    void setMatrix(const Matrix &m);

    void setRotationOrder(AbyssEngine::AEMath::RotationOrder order);

    bool hasLod();

    uint16_t getID();

    uint8_t isVisible();

    void setVisible(bool v);

    void render();

    void updateReferenceMatrix();

    void addChild(uint32_t child);

    void setMesh(uint16_t mesh);

    void translate(float x, float y, float z);

    void translate(const Vector &v);

    void setScaling(float x, float y, float z);

    void setScaling(float s);

    void setScaling(const Vector &v);

    void setRotation(float x, float y, float z);

    void setRotation(const Vector &v);

    void setPosition(const Vector &v);

    void setPosition(float x, float y, float z);

    void rotate(float x, float y, float z);

    void rotate(const Vector &v);

    void moveForward(float dist);

    void setDirection(const Vector &dir, const Vector &up);

    void setLodMeshes(uint16_t *meshes, int *dists, int count);

    void setLodMeshesWithMeshIds(uint16_t *meshes, uint32_t *meshIds, int *dists, int count);

    void setLodChildMeshes(uint16_t *meshes);

    void setLodChildTransform(uint32_t param);

    void setLodLastVisibleDistance(uint64_t d);

    void updateLod(const Vector &camPos, float screenScale);

    void DEBUG_setMeshMergerIndex(int a, LodMeshMerger *b);
};
#endif
