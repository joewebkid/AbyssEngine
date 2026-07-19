#ifndef GOF2_AEGEOMCANVAS_H
#define GOF2_AEGEOMCANVAS_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/AEMath.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"

namespace AbyssEngine { class PaintCanvas; }
using ::AbyssEngine::PaintCanvas;

struct AEGeomCanvas {
    static uint32_t TransformGetLocal(uint32_t canvas, uint32_t tf);

    static void TransformCreate(PaintCanvas *canvas, uint32_t *out);

    static void TransformSetLocal(PaintCanvas *canvas, uint32_t tf, Matrix *m);

    static void TransformAddChild(PaintCanvas *canvas, uint32_t tf, uint32_t child);
};
#endif
