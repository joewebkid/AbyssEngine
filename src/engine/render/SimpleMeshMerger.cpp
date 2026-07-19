#include "engine/render/SimpleMeshMerger.h"
#include "engine/render/Mesh.h"
#include "game/core/Vector.h"
#include "engine/math/AEMath.h"
#include "engine/render/PaintCanvas.h"

uint16_t aeabi_uidiv16(uint16_t a, uint16_t b);

SimpleMeshMerger::SimpleMeshMerger(const Array<unsigned short> &meshIds,
                                   Array<Matrix> transforms,
                                   PaintCanvas *canvas, unsigned short flags) {
    this->mergeFactor = (short) flags;
    this->canvas = canvas;
    this->matrixCount = (int) transforms.size();
    ArraySetLength(meshIds.size(), this->meshes);

    int16_t totalV = 0;
    int16_t totalI = 0;
    for (uint32_t i = 0; i < meshIds.size(); i++) {
        uint32_t localId;
        canvas->MeshCreate(meshIds.data()[i], localId, false);
        this->meshes[i] = (AbyssEngine::Mesh *) canvas->MeshGetPointer(localId);
        AbyssEngine::Mesh *m = this->meshes[i];
        totalV = (int16_t)(totalV + m->vertexCount);
        totalI = (int16_t)(totalI + aeabi_uidiv16(m->indexCount, 3));
    }

    AbyssEngine::Mesh *m0 = this->meshes[0];
    canvas->MeshCreate((uint16_t) totalV, (uint16_t) totalI, (signed char) m0->vertexFormat,
                       flags, this->mergedMeshId);

    int16_t vtxBase = 0;
    int16_t triBase = 0;
    for (uint32_t i = 0; i < meshIds.size(); i++) {
        AbyssEngine::Mesh *m = this->meshes[i];
        Matrix *xf = &transforms.data()[i];
        int uvOff = 0;
        int colOff = 0;
        uint16_t nv = m->vertexCount;
        for (uint16_t v = 0; v < nv; v++) {
            m = this->meshes[i];
            uint8_t fl = m->vertexFormat;
            Vector tmp;
            if (fl & 1) {
                tmp = AbyssEngine::AEMath::MatrixTransformVector(
                    *xf, *(const Vector *) ((const char *) m->positions + v * 0xc));
                canvas->MeshSetPoint(this->mergedMeshId, (uint16_t)(vtxBase + v), tmp.x, tmp.y, tmp.z);
                fl = m->vertexFormat;
            }
            if (fl & 4) {
                tmp = AbyssEngine::AEMath::MatrixRotateVector(
                    *xf, *(const Vector *) ((const char *) m->normals + v * 0xc));
                canvas->MeshSetNormal(this->mergedMeshId, (uint16_t)(vtxBase + v), tmp);
                fl = m->vertexFormat;
            }
            if (fl & 2) {
                float *uv = (float *) ((char *) m->texCoords + uvOff);
                canvas->MeshSetUv(this->mergedMeshId, (uint16_t)(vtxBase + v), uv[0], uv[1]);
                fl = m->vertexFormat;
            }
            if (fl & 8) {
                float *col = (float *) ((char *) (uintptr_t) m->colors + colOff);
                canvas->MeshSetColor(this->mergedMeshId, (uint16_t)(vtxBase + v),
                                     col[0], col[1], col[2], col[3]);
            }
            uvOff += 8;
            colOff += 0x10;
        }

        uint16_t tris = aeabi_uidiv16(m->indexCount, 3);
        int triOff = 0;
        for (uint16_t t = 0; t < tris; t++) {
            if (m->vertexFormat & 0x10) {
                int16_t *ix = (int16_t *) ((char *) m->indices + triOff);
                canvas->MeshSetTriangle(this->mergedMeshId, (uint16_t)(triBase + t),
                                        (uint16_t)(ix[0] + vtxBase), (uint16_t)(ix[1] + vtxBase),
                                        (uint16_t)(ix[2] + vtxBase));
                m = this->meshes[i];
            }
            triOff += 6;
        }
        triBase = (int16_t)(triBase + aeabi_uidiv16(m->indexCount, 3));
        vtxBase = (int16_t)(vtxBase + m->vertexCount);
    }

    canvas->TransformCreate(this->transformId);
    canvas->TransformAddMeshId(this->transformId, this->mergedMeshId);
    this->valid = 1;
}
