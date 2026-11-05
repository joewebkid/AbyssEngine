#include "engine/render/PaintCanvas.h"

void paintcanvas_ext_mat_intern(AbyssEngine::PaintCanvas *, AbyssEngine::Transform *, AbyssEngine::Material *);

namespace AbyssEngine {

void PaintCanvas::MeshChangeMaterialIntern(Mesh *mesh, Material *mat) {
    if (mesh && mat) {
        mesh->field_0x30 = mat;
        return paintcanvas_ext_mat_intern(this, mesh->animation, mat);
    }
}

}
