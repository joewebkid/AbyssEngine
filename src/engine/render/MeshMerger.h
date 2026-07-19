#ifndef GOF2_MESHMERGER_H
#define GOF2_MESHMERGER_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"

namespace AbyssEngine { 
    class Mesh;
 }



namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;

class MeshMerger {
public:
    using Mesh = AbyssEngine::Mesh;



    int rows;
    uint16_t flags;
    uint8_t initialized;
    void *sourceMeshes;
    PaintCanvas *canvas;
    uint32_t mergedMeshId;
    uint32_t transformId;
    void **transformedMeshes;
    char *matrices;
    void *mergedMesh;
    int8_t *lods;
    uint8_t *enabledFlags;
    uint8_t *visibleFlags;
    int cols;
    uint8_t dirty;

    MeshMerger(const Array<uint16_t> &meshIds, Array<Matrix> transforms,
               PaintCanvas *canvas, uint16_t flags);

    MeshMerger(int rows, int cols, PaintCanvas *canvas, uint16_t flags);

    ~MeshMerger();

    void setMatrix(int index, const Matrix &m);

    void setLod(int index, signed char lod);

    void setMesh(int index, signed char lod, uint16_t meshId);

    void setEnabled(int index, bool enabled);

    void render();

    void update();

    int init();

    void *transformMesh(Mesh *mesh, const Matrix &m);
};

#endif
