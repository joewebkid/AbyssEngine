#include "engine/render/Trail.h"
#include "engine/render/PaintCanvas.h"

namespace AEMath = AbyssEngine::AEMath;

static AbyssEngine::PaintCanvas **gTrailCanvasRender = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasType1 = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasType2 = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasType3 = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasTypeDefault = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasType5 = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasType8 = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasCtor = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasUpdate = nullptr;
static AbyssEngine::PaintCanvas **gTrailCanvasSetWidth = nullptr;

void Trail::update(const Vector &a, const Vector &b) {
    update(a.x, a.y, a.z, b.x, b.y, b.z);
}

void Trail::render() {
    Trail_renderTransform(*gTrailCanvasRender, this->transformId, 0);
}

void Trail::translate(const Vector &delta) {
    float dx = delta.x;
    float dy = delta.y;
    float dz = delta.z;

    for (int i = 0; i < this->pointCount; i += 3) {
        int *point = this->points + i;
        point[0] = (int) (dx + (float) point[0]);
        point[1] = (int) (dy + (float) point[1]);
        point[2] = (int) (dz + (float) point[2]);
    }
}

Trail::~Trail() {
    delete[] this->points;
    this->points = nullptr;

    delete[] this->relativePoints;
    this->relativePoints = nullptr;
}

void Trail::update(float ax, float ay, float az, float bx, float by, float bz) {
    int *points = this->points;
    float width = (float) this->width;

    points[0] = (int) (ax - width);
    points[1] = (int) ay;
    points[2] = (int) az;

    width = (float) this->width;
    points[3] = (int) (width + ax);
    points[4] = (int) ay;
    points[5] = (int) az;

    width = (float) this->width;
    points[6] = (int) (bx - width);
    points[7] = (int) by;
    points[8] = (int) bz;

    width = (float) this->width;
    points[9] = (int) (width + bx);
    points[10] = (int) by;
    points[11] = (int) bz;

    int *copy = points + this->pointCount;
    for (int i = this->pointCount - 1; 10 < i; i -= 6) {
        copy[-2] = copy[-8];
        copy[-1] = copy[-7];
        *(uint64_t *) (copy - 6) = *(uint64_t *) (copy - 12);
        *(uint64_t *) (copy - 4) = *(uint64_t *) (copy - 10);
        copy -= 6;
    }

    for (int i = 0; i < this->pointCount; i += 3) {
        int *source = points + i;
        int *relative = this->relativePoints + i;
        relative[0] = (int) ((float) source[0] - ax);
        relative[1] = (int) ((float) source[1] - ay);
        relative[2] = (int) ((float) source[2] - az);
    }

    PaintCanvas *canvas = *gTrailCanvasUpdate;
    for (int vertex = 0; vertex < (this->segments + 1) * 2; ++vertex) {
        int *point = this->relativePoints + vertex * 3;
        float x = (float) point[0];
        float y = (float) point[1];
        float z = (float) point[2];
        canvas->MeshSetPoint(this->meshId, (uint16_t) vertex, x, y, z);
    }

    Matrix *local = static_cast<Matrix *>(canvas->TransformGetLocal(this->transformId));
    AEMath::MatrixSetTranslation(*local, ax, ay, az);
}

Trail::Trail(int type, int segments) {
    this->sourceX = 0;
    this->sourceY = 0;
    this->sourceZ = 0;
    this->segments = segments;

    float uvSide = 0.0f;
    if ((type & ~3) != 4) {
        uvSide = 0.0f;
        if ((uint32_t)(type - 9) < 3) {
            uvSide = 1.0f;
        }
    }

    PaintCanvas *canvas = *gTrailCanvasCtor;
    this->width = 0x3c;
    canvas->MeshCreate((uint16_t)(segments * 2 + 2),
                       (uint16_t)(segments * 2),
                       0x13,
                       0x4e77,
                       this->meshId);

    for (int i = 2; i - 2 < segments * 2 - 2; i += 2) {
        uint16_t tri = (uint16_t)(i - 2);
        uint16_t odd = (uint16_t)(tri | 1);
        uint16_t current = (uint16_t) i;
        canvas->MeshSetTriangle(this->meshId, tri, tri, odd, current);
        canvas->MeshSetTriangle(this->meshId, odd, (uint16_t)(tri + 3), current, odd);
    }

    float uvScale = 0.0f;
    int point = 0;
    for (int i = 0; i < segments * 2 + 2; i += 2) {
        float u = (float) -(int) (((float) point / (float) (segments + 1)) * uvScale);
        canvas->MeshSetUv(this->meshId, (uint16_t) i, uvSide, u);
        canvas->MeshSetUv(this->meshId, (uint16_t)(i | 1), uvSide, u);
        point++;
        segments = this->segments;
    }

    int count = (segments * 2 + 2) * 3;
    this->pointCount = count;

    this->points = new int[count];
    this->relativePoints = new int[count];
    canvas->TransformCreate(this->transformId);
    canvas->TransformAddMeshId(this->transformId, this->meshId);
    changeType(type);
}

void Trail::update(const Matrix &a, const Matrix &b, const Vector &v) {
    Vector va = AEMath::MatrixTransformVector(a, v);
    Vector vb = AEMath::MatrixTransformVector(b, v);
    update(va.x, va.y, va.z, vb.x, vb.y, vb.z);
}

void Trail::setWidth(int width) {
    int delta = this->width - width;
    PaintCanvas *canvas = *gTrailCanvasSetWidth;
    for (int vertex = 0; vertex < (this->segments + 1) * 2; ++vertex) {
        int *point = this->points + vertex * 3;
        int x = point[0] + delta;
        point[0] = x;
        canvas->MeshSetPoint(this->meshId, (uint16_t) vertex, (float) x, (float) point[1], (float) point[2]);
    }

    this->width = width;
}

void Trail::changeType(int type) {
    switch (type) {
        case 1:
        case 7:
        case 11:
            Trail_transformSetColor(*gTrailCanvasType1, this->transformId, 0xff0000ffu);
            break;
        case 2:
        case 9:
            Trail_transformSetColor(*gTrailCanvasType2, this->transformId, 0x00ff00ffu);
            break;
        case 3:
        case 6:
        case 10:
            Trail_transformSetColor(*gTrailCanvasType3, this->transformId, 0xffff00ffu);
            break;
        case 5:
            Trail_transformSetColor(*gTrailCanvasType5, this->transformId, 0x00ff0000u);
            break;
        case 8:
            Trail_transformSetColor(*gTrailCanvasType8, this->transformId, 0xff4000ffu);
            break;
        default:
            Trail_transformSetColor(*gTrailCanvasTypeDefault, this->transformId, 0xffffffffu);
            break;
    }
}

void Trail::reset(Vector value) {
    int x = (int) value.x;
    int y = (int) value.y;
    int z = (int) value.z;

    for (int i = 0; i < this->pointCount; i += 3) {
        int *point = this->points + i;
        point[0] = x;
        point[1] = y;
        point[2] = z;
    }

    update(value, value);
}
