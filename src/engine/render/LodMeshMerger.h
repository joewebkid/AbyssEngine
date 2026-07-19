#ifndef GOF2_LODMESHMERGER_H
#define GOF2_LODMESHMERGER_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "PaintCanvas.h"

#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
#include "engine/render/Mesh.h"


namespace AbyssEngine {
    class PaintCanvas;
    class Mesh;
}
using ::AbyssEngine::PaintCanvas;
using ::AbyssEngine::Mesh;

class LodMeshMerger {
public:



    int rows;
    uint16_t flags;
    uint8_t initialized;
    Array<Mesh *> sourceMeshes;
    PaintCanvas *canvas;
    uint32_t mergedMeshId;
    uint32_t transformId;
    void *mergedMesh;
    void **transformedMeshes;
    AbyssEngine::Matrix *transforms;
    int8_t *lodLevels;
    uint8_t *enabled;
    uint8_t *visible;
    int cols;
    uint8_t dirty;

    LodMeshMerger(int rows, int cols, PaintCanvas *canvas, uint16_t flags);

    ~LodMeshMerger();

    void setEnabled(int index, bool enabled);

    void setLod(int index, signed char lod);

    void setMatrix(int index, const AbyssEngine::Matrix &m);

    void setMesh(int index, signed char lod, uint16_t meshId);

    void update();

    int init();

    void *transformMesh(Mesh *src, const AbyssEngine::Matrix &m);
};

#endif
