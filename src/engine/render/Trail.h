#ifndef GOF2_TRAIL_H
#define GOF2_TRAIL_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"




namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;

class Trail {
public:
    int sourceX;
    int sourceY;
    int sourceZ;
    int width;
    uint32_t meshId;
    uint32_t transformId;
    int *points;
    int *relativePoints;
    int pointCount;
    int segments;

    Trail(int type, int segments);

    ~Trail();

    void update(const AbyssEngine::AEMath::Vector &a, const AbyssEngine::AEMath::Vector &b);

    void update(float ax, float ay, float az, float bx, float by, float bz);

    void update(const AbyssEngine::AEMath::Matrix &a, const AbyssEngine::AEMath::Matrix &b,
                const AbyssEngine::AEMath::Vector &v);

    void render();

    void translate(const AbyssEngine::AEMath::Vector &delta);

    void setWidth(int width);

    void changeType(int type);

    void reset(AbyssEngine::AEMath::Vector value);
};

namespace AbyssEngine {
    using ::Trail;
}

void Trail_renderTransform(AbyssEngine::PaintCanvas *canvas, uint32_t transform, int mode);

void Trail_transformSetColor(AbyssEngine::PaintCanvas *canvas, uint32_t transform, uint32_t color);

#endif
