#include "engine/render/AEGeometry.h"
#include "engine/render/PaintCanvas.h"

void AEGeometry::setPosition(float x, float y, float z) {
    Matrix &matrix = *(Matrix *) this->canvas->TransformGetLocal(this->transform);
    MatrixSetTranslation(matrix, x, y, z);
}
