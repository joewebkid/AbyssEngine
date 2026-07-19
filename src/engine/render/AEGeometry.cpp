#include "engine/render/AEGeometry.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/LodMeshMerger.h"
#include "engine/math/Transform.h"


void _ae_geom_render(uint32_t canvas, uint32_t tf, int z);

void _ae_MatrixSetRotation(void *out, uint32_t loc, float x, float y, float z, int order);

void _ae_MatrixSetScaling(void *out, uint32_t loc, float sx, float sy, float sz);

void _ae_setPosition3(void *self, float x, float y, float z);

uint32_t Transform_GetTransform(uint32_t tf);

void VectorCross(Vector *out, const Vector *b);

Vector AEGeometry::getPosition() {
    Matrix &loc = *(Matrix *) AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    return MatrixGetPosition(loc);
}

AEGeometry::~AEGeometry() {
    delete[] this->lodTransforms;
    this->lodTransforms = nullptr;
    delete[] this->lodMeshes;
    this->lodMeshes = nullptr;
    delete[] this->lodChildTransforms;
    this->lodChildTransforms = nullptr;
    delete[] this->lodChildMeshes;
    this->lodChildMeshes = nullptr;
    delete[] this->lodDistancesSq;
    this->lodDistancesSq = nullptr;
}

bool AEGeometry::hasLod() { return this->lodTransforms != nullptr; }

uint16_t AEGeometry::getID() { return this->mesh; }

uint8_t AEGeometry::isVisible() { return (uint8_t) this->visibility; }

void AEGeometry::setVisible(bool v) {
    this->visibility = v ? 0x0101 : 0;
}

void AEGeometry::DEBUG_setMeshMergerIndex(int a, LodMeshMerger *b) {
    this->mergerIndex = (uint32_t) a;
    this->merger = b;
}

void AEGeometry::addChild(uint32_t child) {
    AEGeomCanvas::TransformAddChild(this->canvas, this->transform, child);
    uint32_t old = this->childTransform;
    if (old != 0xffffffffu)
        this->parentTransform = old;
    this->childTransform = child;
}

void AEGeometry::render() {
    if ((uint8_t) this->visibility == 0)
        return;
    _ae_geom_render((uint32_t)(uintptr_t)this->canvas, this->transform, 0);
}

void AEGeometry::translate(const Vector &v) {
    translate(v.x, v.y, v.z);
}

void AEGeometry::setScaling(float s) {
    setScaling(s, s, s);
}

Vector AEGeometry::getRotation() {
    return this->rotation;
}

void AEGeometry::setScaling(const Vector &v) {
    setScaling(v.x, v.y, v.z);
}

void AEGeometry::setLodMeshesWithMeshIds(uint16_t *meshes, uint32_t *meshIds, int *dists, int count) {
    this->lodMeshes = new uint16_t[count];
    this->lodDistancesSq = new unsigned long long[count];
    this->lodCount = count;
    this->lodTransforms = new uint32_t[count];
    for (int i = 0; i < count; i++) {
        this->lodMeshes[i] = meshes[i];
        this->lodDistancesSq[i] = (unsigned long long) (long long) dists[i];
        AEGeomCanvas::TransformCreate(this->canvas, &this->lodTransforms[i]);
        this->canvas->TransformAddMeshId(this->lodTransforms[i], meshIds[i]);
        unsigned long long v = this->lodDistancesSq[i];
        this->lodDistancesSq[i] = v * v;
        if (this->childTransform != 0xffffffffu)
            AEGeomCanvas::TransformAddChild(this->canvas, this->lodTransforms[i], this->childTransform);
    }
}

void AEGeometry::setLodLastVisibleDistance(uint64_t d) {
    this->lastVisibleDistSq = d * d;
}

Vector AEGeometry::getRightVector() {
    Matrix &loc = *(Matrix *) AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    return MatrixGetRight(loc);
}

Vector AEGeometry::getUpVector() {
    Matrix &loc = *(Matrix *) AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    return MatrixGetUp(loc);
}

Matrix &AEGeometry::getMatrix() {
    return *(Matrix *) AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
}

Vector AEGeometry::getDirection() {
    Matrix &loc = *(Matrix *) AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    return MatrixGetDir(loc);
}

Matrix &AEGeometry::getReferenceMatrix() {
    return this->referenceMatrix;
}

void AEGeometry::setMatrix(const Matrix &m) {
    AEGeomCanvas::TransformSetLocal(this->canvas, this->transform, (Matrix *) &m);
}

void AEGeometry::setRotationOrder(AbyssEngine::AEMath::RotationOrder order) {
    this->rotationOrder = order;
}

void AEGeometry::setPosition(float x, float y, float z) {
    _ae_setPosition3(this, x, y, z);
}

Vector AEGeometry::getScaling() {
    Vector v;
    v.x = this->scalingX;
    v.y = this->scalingY;
    v.z = this->scalingZ;
    return v;
}

void AEGeometry::updateReferenceMatrix() {
    Matrix *loc = (Matrix *) AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    this->referenceMatrix = *loc;
}

void AEGeometry::setLodMeshes(uint16_t *meshes, int *dists, int count) {
    this->lodMeshes = new uint16_t[count];
    this->lodDistancesSq = new unsigned long long[count];
    this->lodCount = count;
    this->lodTransforms = new uint32_t[count];
    for (int i = 0; i < count; i++) {
        this->lodMeshes[i] = meshes[i];
        this->lodDistancesSq[i] = (unsigned long long) (long long) dists[i];
        AEGeomCanvas::TransformCreate(this->canvas, &this->lodTransforms[i]);
        this->canvas->TransformAddMesh(this->lodTransforms[i], meshes[i], false);
        unsigned long long v = this->lodDistancesSq[i];
        this->lodDistancesSq[i] = v * v;
        if (this->childTransform != 0xffffffffu)
            AEGeomCanvas::TransformAddChild(this->canvas, this->lodTransforms[i], this->childTransform);
    }
}

Vector AEGeometry::getParentPosition() {
    uint32_t canvas = (uint32_t)(uintptr_t)
    this->canvas;
    uint32_t tf = this->altTransform;
    if (tf == 0xffffffffu)
        tf = this->transform;
    Matrix &loc = *(Matrix *) AEGeomCanvas::TransformGetLocal(canvas, tf);
    return MatrixGetPosition(loc);
}

void AEGeometry::setRotation(float x, float y, float z) {
    char buf[60];
    uint32_t loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    _ae_MatrixSetRotation(buf, loc, x, y, z, this->rotationOrder);
    loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    _ae_MatrixSetScaling(buf, loc, this->scalingX, this->scalingY, this->scalingZ);
    this->rotation.x = x;
    this->rotation.y = y;
    this->rotation.z = z;
}

void AEGeometry::setRotation(const Vector &v) {
    setRotation(v.x, v.y, v.z);
}

void AEGeometry::setPosition(const Vector &v) {
    setPosition(v.x, v.y, v.z);
}

void AEGeometry::setLodChildMeshes(uint16_t *meshes) {
    int count = this->lodCount;
    if (count > 0) {
        this->lodChildMeshes = new uint16_t[count];
        this->lodChildTransforms = new uint32_t[count];
        for (int i = 0; i < count; i++) {
            this->lodChildMeshes[i] = meshes[i];
            AEGeomCanvas::TransformCreate(this->canvas, &this->lodChildTransforms[i]);
            this->canvas->TransformAddMesh(this->lodChildTransforms[i], meshes[i], false);
            AEGeomCanvas::TransformAddChild(this->canvas, this->lodTransforms[i], this->lodChildTransforms[i]);
            this->canvas->TransformRemoveChild(this->lodTransforms[i], this->childTransform);
            AEGeomCanvas::TransformAddChild(this->canvas, this->lodTransforms[i], this->childTransform);
            count = this->lodCount;
        }
    }
}

void AEGeometry::rotate(float x, float y, float z) {
    char buf[60];
    this->rotation.x += x;
    this->rotation.y += y;
    this->rotation.z += z;
    uint32_t loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    _ae_MatrixSetRotation(buf, loc, this->rotation.x, this->rotation.y, this->rotation.z,
                          this->rotationOrder);
    loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    _ae_MatrixSetScaling(buf, loc, this->scalingX, this->scalingY, this->scalingZ);
}

AEGeometry::AEGeometry(PaintCanvas *canvas) {
    this->cameraDelta = Vector{0, 0, 0};
    this->referenceMatrix = Matrix();
    this->mesh = 0;
    this->canvas = canvas;
    AEGeomCanvas::TransformCreate(canvas, &this->baseTransform);

    this->rotation = Vector{0.0f, 0.0f, 0.0f};
    this->scalingX = 1.0f;
    this->scalingY = 1.0f;
    this->scalingZ = 1.0f;
    this->lodTransforms = nullptr;
    this->lodChildTransforms = nullptr;
    this->lodMeshes = nullptr;
    this->lodChildMeshes = nullptr;
    this->lodDistancesSq = nullptr;
    this->visibility = 0x101;
    this->rotationOrder = 0;
    this->meshId = 0xffffffff;
    this->meshHandle = 0xffffffff;
    this->altTransform = 0xffffffff;
    this->currentLod = 0;
    this->merger = nullptr;
    this->transform = this->baseTransform;
    this->parentTransform = 0xffffffff;
    this->childTransform = 0xffffffff;
}

void AEGeometry::setMesh(uint16_t mesh) {
    if (this->transform == 0)
        this->canvas->TransformCreate(this->transform);
    this->canvas->TransformAddMesh(this->transform, mesh, false);
}

void AEGeometry::translate(float x, float y, float z) {
    uint32_t loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    Matrix matrix = *(Matrix *) (uintptr_t) loc;
    MatrixSetTranslation(matrix, matrix.m[3] + z, matrix.m[7], matrix.m[11] + y);
    this->canvas->TransformSetLocal(this->transform, matrix);
    (void) x;
}

AEGeometry::AEGeometry(uint16_t mesh, PaintCanvas *canvas, bool flag) {
    this->cameraDelta = Vector{0, 0, 0};
    this->referenceMatrix = Matrix();
    this->canvas = canvas;
    this->mesh = mesh;
    this->transform = 0;
    this->baseTransform = 0;
    AEGeomCanvas::TransformCreate(canvas, &this->baseTransform);
    canvas->MeshCreate(mesh, this->meshId, flag);
    canvas->TransformAddMeshId(this->baseTransform, this->meshId);

    this->rotation = Vector{0.0f, 0.0f, 0.0f};
    this->scalingX = 1.0f;
    this->scalingY = 1.0f;
    this->scalingZ = 1.0f;
    this->lodTransforms = nullptr;
    this->lodChildTransforms = nullptr;
    this->lodMeshes = nullptr;
    this->lodChildMeshes = nullptr;
    this->lodDistancesSq = nullptr;
    this->visibility = 0x101;
    this->rotationOrder = 0;
    this->meshHandle = 0xffffffff;
    this->altTransform = 0xffffffff;
    this->currentLod = 0;
    this->transform = this->baseTransform;
    this->parentTransform = 0xffffffff;
    this->childTransform = 0xffffffff;
    uint32_t loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)canvas, 0);
    this->referenceMatrix = *(Matrix *) loc;
    this->merger = nullptr;
}

void AEGeometry::setScaling(float x, float y, float z) {
    char buf[60];
    uint32_t loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    _ae_MatrixSetRotation(buf, loc, this->rotation.x, this->rotation.y, this->rotation.z,
                          this->rotationOrder);
    loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    _ae_MatrixSetScaling(buf, loc, x, y, z);
    this->scalingX = x;
    this->scalingY = y;
    this->scalingZ = z;
}

void AEGeometry::moveForward(float dist) {
    Vector n;
    Vector pos = getDirection();
    VectorNormalize(&n, &pos);
    pos = getPosition();
    pos.x = pos.x + n.x * dist;
    pos.y = pos.y + n.y * dist;
    pos.z = pos.z + n.z * dist;
    this->setPosition(pos);
}

void AEGeometry::updateLod(const Vector &camPos, float screenScale) {
    this->visibility = (this->visibility & 0xff00) | (this->visibility >> 8);

    char matrixCopy[60];
    uint32_t loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    memcpy(matrixCopy, (void *) loc, 0x3c);

    loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    Vector pos = MatrixGetPosition(*(Matrix *) loc);
    this->cameraDelta = camPos - pos;

    float dx = this->cameraDelta.x, dy = this->cameraDelta.y, dz = this->cameraDelta.z;
    this->distSq = (unsigned long long) (dy * dy + dx * dx + dz * dz);

    unsigned long long lastVis = this->lastVisibleDistSq;
    bool visible;
    if (lastVis == 0) {
        visible = true;
    } else {
        visible = this->distSq < lastVis;
        this->visibility = (this->visibility & 0xff00) | (visible ? 1 : 0);
    }

    if (!visible) {
        this->currentLod = -1;
        return;
    }

    Transform_GetTransform((uint32_t)(uintptr_t)this->canvas);

    float factor = (screenScale <= 0.0625f) ? 0.75f : 1.0f;
    float detail = (0.03125f < screenScale) ? factor : 0.5f;

    int level = this->lodCount;

    while (level >= 1) {
        int idx = level - 1;
        float thresh = (float) this->lodDistancesSq[idx];
        float d = (float) this->distSq;
        if (!(detail * thresh < d)) {
            level = idx;
            continue;
        }

        uint32_t lodTf = this->lodTransforms[idx];
        if (lodTf != this->transform) {
            AEGeomCanvas::TransformSetLocal(this->canvas, this->transform, (Matrix *) (uintptr_t) lodTf);
            this->transform = this->lodTransforms[idx];
            uint32_t t = Transform_GetTransform((uint32_t)(uintptr_t)this->canvas);
            ((AbyssEngine::Transform *) (uintptr_t) t)->SetCurrentAnimationTime(0);
            t = Transform_GetTransform((uint32_t)(uintptr_t)this->canvas);
            ((AbyssEngine::Transform *) (uintptr_t) t)->SetCurrentAnimationTime(0);
            this->currentLod = level;
            this->referenceMatrix = *(Matrix *) matrixCopy;
            if (this->merger != nullptr)
                this->merger->setLod(this->mergerIndex, (signed char) level);
        }
        return;
    }

    AEGeomCanvas::TransformSetLocal(this->canvas, this->transform, (Matrix *) (uintptr_t) this->baseTransform);
    this->currentLod = 0;
    this->transform = this->baseTransform;
    if (this->merger != nullptr)
        this->merger->setLod(this->mergerIndex, 0);
}

void AEGeometry::setDirection(const Vector &dir, const Vector &up) {
    char local[60];
    uint32_t loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    memcpy(local, (void *) loc, 0x3c);

    Vector right = up;
    VectorCross(&right, &dir);
    Vector tmp;
    VectorNormalize(&tmp, &right);
    right = tmp;

    Vector rUp;
    VectorCross(&tmp, &dir);
    rUp = tmp;
    VectorNormalize(&tmp, &rUp);
    rUp = tmp;

    Matrix &m = *(Matrix *) local;
    m.m[0] = right.x;
    m.m[1] = right.y;
    m.m[2] = right.z;
    m.m[3] = rUp.x;
    m.m[4] = rUp.y;
    m.m[5] = rUp.z;
    m.m[6] = dir.x;
    m.m[7] = dir.y;
    m.m[8] = dir.z;

    AEGeomCanvas::TransformSetLocal(this->canvas, this->transform, &m);
    loc = AEGeomCanvas::TransformGetLocal((uint32_t)(uintptr_t)this->canvas, this->transform);
    _ae_MatrixSetScaling((void *) local, loc, this->scalingX, this->scalingY, this->scalingZ);
}

void AEGeometry::rotate(const Vector &v) {
    rotate(v.x, v.y, v.z);
}

void AEGeometry::setLodChildTransform(uint32_t param) {
    int count = this->lodCount;
    if (count > 0) {
        this->lodChildTransforms = new uint32_t[count];
        for (int i = 0; i < count; i++) {
            AEGeomCanvas::TransformAddChild(this->canvas, this->lodTransforms[i], param);
            count = this->lodCount;
        }
    }
}
