#include "engine/render/LodMeshMerger.h"
#include "engine/math/AEMath.h"
#include "engine/render/PaintCanvas.h"
#include <cstring>


void LodMeshMerger::setEnabled(int index, bool value) {
    if (enabled[index] != value) {
        enabled[index] = value;
        if (visible[index] != 0) {
            dirty = 1;
        }
    }
}

void LodMeshMerger::setLod(int index, signed char lod) {
    if (lodLevels[index] != lod) {
        lodLevels[index] = lod;
        if (enabled[index] != 0) {
            dirty = 1;
        }
    }
}

void LodMeshMerger::setMatrix(int index, const Matrix &m) {
    transforms[index] = m;
}

void LodMeshMerger::setMesh(int index, signed char lod, uint16_t meshId) {
    uint32_t id;
    canvas->MeshCreate(meshId, id, false);
    void *ptr = canvas->MeshGetPointer(id);
    sourceMeshes.data()[rows * lod + index] = (Mesh *) ptr;
}

void LodMeshMerger::update() {
    for (int i = 0; i < rows; i++) {
        Mesh *sph = (Mesh *) transformedMeshes[i];
        Vector sphCenter = {sph->boundsCenterX, sph->boundsCenterY, sph->boundsCenterZ};
        uint8_t vis = (uint8_t) canvas->CameraIsSphereinViewFrustum(
            sphCenter, sph->boundsRadius);
        if (vis != visible[i]) {
            visible[i] = vis;
            if (enabled[i] != 0) {
                dirty = 1;
            }
        }
    }

    if (dirty == 0) {
        return;
    }

    int indexBudget = 0;
    for (int j = 0; j < rows; j++) {
        if (enabled[j] != 0 && visible[j] != 0) {
            signed char lod = lodLevels[j];
            Mesh *src = (Mesh *) transformedMeshes[rows * lod + j];
            indexBudget += src->indexCount;
        }
    }

    for (int j = 0; indexBudget >= 0x10000 && j < rows; j++) {
        if (enabled[j] != 0 && visible[j] != 0) {
            signed char lod = lodLevels[j];
            if (lod < cols - 1) {
                Mesh *prev = (Mesh *) transformedMeshes[rows * lod + j];
                setLod(j, (signed char) (lod + 1));
                signed char newLod = lodLevels[j];
                Mesh *cur = (Mesh *) transformedMeshes[rows * newLod + j];
                indexBudget += cur->indexCount - prev->indexCount;
            }
        }
    }

    Mesh *out = (Mesh *) mergedMesh;
    int vtxOffset = 0;
    int idxOffset = 0;
    for (int j = 0; j < rows; j++) {
        if (enabled[j] != 0 && visible[j] != 0) {
            uint8_t mask = out->vertexFormat;
            signed char lod = lodLevels[j];
            Mesh *src = (Mesh *) transformedMeshes[rows * lod + j];

            if (mask & 1) {
                memcpy((char *) out->positions + vtxOffset * 0xc,
                       src->positions, src->vertexCount * 0xc);
            }
            if (mask & 4) {
                memcpy((char *) out->normals + vtxOffset * 0xc,
                       src->normals, src->vertexCount * 0xc);
            }
            if (mask & 8) {
                memcpy((char *) out->colors + vtxOffset * 0x10,
                       src->colors, (uint32_t) src->vertexCount << 4);
            }
            if (mask & 2) {
                memcpy((char *) out->texCoords + vtxOffset * 8,
                       src->texCoords, (uint32_t) src->vertexCount << 3);
            }
            if (mask & 0x10) {
                const int16_t *si = (const int16_t *) src->indices;
                int16_t *di = (int16_t *) out->indices + idxOffset;
                for (uint16_t k = 0; k < src->indexCount; k++) {
                    di[k] = (int16_t)(si[k] + (int16_t) vtxOffset);
                }
            }
            idxOffset += src->indexCount;
            vtxOffset += src->vertexCount;
        }
    }
    out->indexCount = (uint16_t) idxOffset;
    out->vertexCount = (uint16_t) vtxOffset;
    dirty = 0;
}

LodMeshMerger::LodMeshMerger(int rows_, int cols_, PaintCanvas *canvas_, uint16_t flags_)
    : rows(rows_),
      flags(flags_),
      initialized(0),
      canvas(canvas_),
      mergedMesh(nullptr),
      transformedMeshes(nullptr),
      transforms(nullptr),
      lodLevels(nullptr),
      cols(cols_),
      dirty(0) {
    ArraySetLength((uint32_t)(cols * rows), sourceMeshes);

    uint32_t n = (uint32_t) rows;
    transformedMeshes = new void *[n * cols]();

    lodLevels = new int8_t[n]();

    transforms = new Matrix[n];
    for (uint32_t i = 0; i < n; i++) {
        Matrix tmp;
        tmp.m[0] = 1.0f;
        tmp.m[1] = 0.0f;
        tmp.m[2] = 0.0f;
        tmp.m[3] = 0.0f;
        tmp.m[4] = 1.0f;
        tmp.m[5] = 0.0f;
        tmp.m[6] = 0.0f;
        tmp.m[7] = 0.0f;
        tmp.m[8] = 0.0f;
        tmp.m[9] = 0.0f;
        tmp.m[10] = 1.0f;
        tmp.m[11] = 0.0f;
        tmp.m[12] = 0.0f;
        tmp.m[13] = 0.0f;
        tmp.m[14] = 1.0f;
        transforms[i] = tmp;
    }

    enabled = new uint8_t[n];
    for (uint32_t i = 0; i < n; i++) enabled[i] = 1;

    visible = new uint8_t[n];
    for (uint32_t i = 0; i < n; i++) visible[i] = 1;
}

int LodMeshMerger::init() {
    if (initialized != 0) {
        return initialized;
    }

    for (int i = 0; i < rows; i++) {
        int lod = lodLevels[i];
        if (lod >= -1 && cols <= lod) {
            lodLevels[i] = 0;
        }
        for (int c = 0; c < cols; c++) {
            Mesh *mesh = sourceMeshes.data()[rows * c + i];
            if (mesh != nullptr) {
                transformedMeshes[rows * c + i] = transformMesh(mesh, transforms[i]);
            }
        }
    }

    uint32_t nv = 0;
    uint32_t ni = 0;
    for (int i = 0; i < rows; i++) {
        Mesh *m0 = sourceMeshes.data()[i];
        nv = nv + m0->vertexCount;
        ni = ni + (uint16_t)(m0->indexCount / 3);
    }

    uint16_t cappedNv = nv >= 0xffff ? 0xffff : (uint16_t) nv;
    uint16_t cappedNi = ni < 0xffff ? (uint16_t) ni : 0xffff;
    canvas->MeshCreate(cappedNv, cappedNi,
                       (signed char) sourceMeshes.data()[0]->vertexFormat,
                       flags, mergedMeshId);
    mergedMesh = canvas->MeshGetPointer(mergedMeshId);
    canvas->TransformCreate(transformId);
    canvas->TransformAddMeshId(transformId, mergedMeshId);
    dirty = 1;

    initialized = 1;
    update();
    return initialized;
}

void *LodMeshMerger::transformMesh(Mesh *src, const Matrix &m) {
    Mesh *out = (Mesh *) ::operator new(sizeof(Mesh));

    memset(out, 0, sizeof(Mesh));
    out->boundsRadiusSq = 1.0f;

    uint32_t vcount = src->vertexCount;
    out->vertexCount = src->vertexCount;
    out->indexCount = src->indexCount;
    uint8_t f = src->vertexFormat;
    out->vertexFormat = f;

    if (f & 0x2)
        out->texCoords = src->texCoords;
    if (f & 0x8)
        out->colors = src->colors;
    if (f & 0x10)
        out->indices = src->indices;

    if (f & 0x1) {
        out->positions = new char[vcount * 0xc];
        int o = 0;
        for (uint32_t i = 0; i < vcount; i++) {
            *(Vector *) ((char *) out->positions + o) =
                    AbyssEngine::AEMath::MatrixTransformVector(m, *(Vector *) ((char *) src->positions + o));
            o += 0xc;
            vcount = src->vertexCount;
        }
        f = src->vertexFormat;
    }

    if (f & 0x4) {
        out->normals = new char[vcount * 0xc];
        int o = 0;
        for (uint32_t i = 0; i < vcount; i++) {
            Vector rot = AbyssEngine::AEMath::MatrixRotateVector(m, *(Vector *) ((char *) src->normals + o));
            *(Vector *) ((char *) out->normals + o) = AbyssEngine::AEMath::VectorNormalize(rot);
            o += 0xc;
            vcount = src->vertexCount;
        }
    }

    float r = src->boundsRadius;
    Vector ext = {r, r, r};
    Vector tExt = AbyssEngine::AEMath::MatrixRotateVector(m, ext);
    Vector center = AbyssEngine::AEMath::MatrixTransformVector(m, *(Vector *) &src->boundsCenterX);

    float ax = tExt.x < 0.0f ? -tExt.x : tExt.x;
    float ay = tExt.y < 0.0f ? -tExt.y : tExt.y;
    float az = tExt.z < 0.0f ? -tExt.z : tExt.z;
    float rad = ax;
    if (ay > rad) rad = ay;
    if (az > rad) rad = az;

    out->boundsCenterX = center.x;
    out->boundsCenterY = center.y;
    out->boundsCenterZ = center.z;
    out->boundsRadius = rad;

    return out;
}

LodMeshMerger::~LodMeshMerger() {
    int count = rows * cols;
    for (int i = 0; i < count; i++) {
        Mesh *cell = (Mesh *) transformedMeshes[i];
        if (cell != nullptr) {
            if (cell->positions != nullptr) {
                delete[] (char *) cell->positions;
                cell->positions = nullptr;
            }
            if (cell->normals != nullptr) {
                delete[] (char *) cell->normals;
                cell->normals = nullptr;
            }
            ::operator delete(cell);
            transformedMeshes[i] = nullptr;
        }
    }
    delete[] transformedMeshes;
    transformedMeshes = nullptr;

    delete[] lodLevels;
    lodLevels = nullptr;
    delete[] enabled;
    enabled = nullptr;
    delete[] visible;
    visible = nullptr;

    delete[] transforms;
    transforms = nullptr;

    ArrayRemoveAll(sourceMeshes);
    sourceMeshes.shrink_to_fit();
}
