#ifndef GOF2_SIMPLEMESHMERGER_H
#define GOF2_SIMPLEMESHMERGER_H
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

class SimpleMeshMerger {
public:



    int matrixCount;
    short mergeFactor;
    unsigned char valid;
    Array<AbyssEngine::Mesh *> meshes;
    PaintCanvas *canvas;
    unsigned mergedMeshId;
    unsigned transformId;

    SimpleMeshMerger(const Array<unsigned short> &meshIds, Array<Matrix> transforms,
                     PaintCanvas *canvas, unsigned short flags);
};

#endif
