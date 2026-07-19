#include "engine/render/ParticleSystemMesh.h"


void _psm_emitTrail(ParticleSystemMesh *self, int id);

void _psm_emitUsual(ParticleSystemMesh *self, int id);

void _psm_meshSetPoint(PaintCanvas *canvas, uint32_t mesh, uint16_t point, float x, float y, float z);

void _psm_mesh_base_dtor(ParticleSystemMesh * self);

void _psm_render2(PaintCanvas *canvas, uint32_t texture, uint32_t camera);

void _psm_vectorMinus(Vector *out, const Vector *a, const Vector *b);

void _psm_vectorPlus(void *out, const Vector *a, const Vector *b);

static char g_ParticleSetData[1];

void _psm_matrixGetRight(Vector *out, const Matrix *m);

void _psm_matrixGetUp(Vector *out, const Matrix *m);

void _psm_matrixGetDir(Vector *out, const Matrix *m);

void _psm_vectorScale(Vector *out, const Vector *v, float scale);

void _psm_vectorAssign(Vector *dst, const Vector *src);

void _psm_meshSetColorWord(PaintCanvas *canvas, uint32_t mesh, uint16_t point, uint32_t color);

void _psm_meshSetUV2(PaintCanvas *canvas, uint32_t mesh, uint16_t point, float u, float v);

void _psm_meshSetUV(PaintCanvas *canvas, uint32_t mesh, uint16_t point, float u, float v);

void _psm_meshSetZero(PaintCanvas *canvas, uint32_t mesh, uint16_t point, uint32_t value);

void _psm_meshTranslatePoint(PaintCanvas *canvas, uint32_t mesh, uint16_t point, float x, float y, float z);

void _psm_interpolateColor(ParticleSystemMesh *self, int id, float *b, float *g, float *a, float *r);

void _psm_canvasSetTexture(PaintCanvas *canvas, uint32_t texture, uint32_t fallback);

void _psm_canvasSetBlendMode(PaintCanvas *canvas, BlendMode mode);

uint32_t _psm_cameraGetCurrent(PaintCanvas * canvas);

uint32_t _psm_cameraGetLocal(PaintCanvas *canvas, uint32_t current);

void _psm_meshSetPointIndirect(PaintCanvas *canvas, uint32_t mesh, uint16_t point, float x, float y,
                               float z);

void _psm_meshSetTriangle(PaintCanvas *canvas, uint32_t mesh, uint16_t triangle, uint16_t a, uint16_t b,
                          uint16_t c);

void _psm_meshSetColor(PaintCanvas *canvas, uint32_t mesh, uint16_t point, float a, float r, float g,
                       float b);

void _psm_finishCurrentTrailParticle(ParticleSystemMesh *self, ParticleSet set, int id, const Vector *a,
                                     const Vector *b);

void _psm_base_ctor(ParticleSystemMesh *self, PaintCanvas *canvas, const Matrix *matrix, const void *sets,
                    bool a, bool b);

int ParticleSystemMesh::getPrevId(int id) {
    if (id == 0)
        id = (int) this->particleCount;
    return id - 1;
}

void ParticleSystemMesh::setParticle(const Vector &pos, float scale, uint32_t color, float u0, float u1,
                                     float v0, float v1, bool useMaskedColor, float upScale, float dirScale,
                                     const Vector &delta) {
    return setParticle(pos, scale, color, u0, u1, v0, v1, useMaskedColor, upScale, dirScale, delta, false);
}

void ParticleSystemMesh::emit(int id) {
    if (this->emitEnabled == 0 || this->visible == 0) {
        this->newSectionStarted = 1;
        return;
    }

    uint32_t flags = this->flags;
    if ((flags & 0x80) != 0)
        return;
    if ((int) (flags << 16) < 0)
        return _psm_emitTrail(this, id);
    return _psm_emitUsual(this, id);
}

void ParticleSystemMesh::finishCurrentTrailParticle(ParticleSet set, int id, const Vector &first,
                                                    const Vector &second) {
    this->setIds[id] = (int8_t) set;
    this->ages[id] = 0;

    uint32_t flags = this->flags;
    uint32_t offset = (this->edgeCount * id * 2) | 1;
    Vector *dst = this->positions + offset;

    if ((flags & 0x1000) != 0) {
        *dst = first;
        flags = this->flags;
        dst += 2;
    }
    if ((int) (flags << 18) < 0)
        *dst = second;
}

void ParticleSystemMesh::incId() {
    int id = this->currentId + 1;
    if (id >= (int) this->particleCount)
        id = 0;
    this->currentId = id;
}

void ParticleSystemMesh::reset() {
    for (int i = 0; i < (int) this->pointCount; i++) {
        _psm_meshSetPoint(this->canvas, this->mesh,
                          (uint16_t)(this->firstPoint + i), 0.0f, 0.0f, 0.0f);
    }

    for (int i = 0; i < (int) this->particleCount; i++)
        this->ages[i] = -1;

    this->frameCounter = 0;
    this->currentId = 0;
    this->newSectionStarted = 1;
    this->emitCounter = 0;
    this->dirty = 1;
}

ParticleSystemMesh::~ParticleSystemMesh() {
    _psm_mesh_base_dtor(this);
}

void ParticleSystemMesh::release() {
}

void ParticleSystemMesh::render(PaintCanvas *canvas, uint32_t texture) {
    if (texture != 0xffffffff)
        return _psm_render2(canvas, texture, 0);
}

void ParticleSystemMesh::startNewSection() {
    this->newSectionStarted = 1;
}

uint8_t ParticleSystemMesh::wasNewSectionStarted() {
    return this->newSectionStarted;
}

int ParticleSystemMesh::getQuadCount() {
    return (int) this->pointCount >> 2;
}

void ParticleSystemMesh::setQuadEdge(const Vector &edge, int point, const Vector &delta) {
    Vector pos;
    volatile char tmpStorage[sizeof(Vector)];

    _psm_vectorMinus(&pos, &edge, &delta);
    _psm_meshSetPoint(this->canvas, this->mesh, (uint16_t) point, pos.x, pos.y, pos.z);

    _psm_vectorPlus((void *) tmpStorage, &edge, &delta);
    pos = *(Vector *) (void *) tmpStorage;

    uint8_t wide = this->wide;
    int next = point + 1;
    PaintCanvas *canvas = this->canvas;
    uint32_t mesh = this->mesh;
    if (wide == 0) {
        _psm_meshSetPoint(canvas, mesh, (uint16_t) next, pos.x, pos.y, pos.z);
    } else {
        void(*setPoint)(PaintCanvas *, uint32_t, uint16_t, float, float, float) = _psm_meshSetPointIndirect;
        setPoint(canvas, mesh, (uint16_t) next, edge.x, edge.y, edge.z);
        setPoint(this->canvas, this->mesh, (uint16_t)(point + 5), edge.x, edge.y, edge.z);
        setPoint(this->canvas, this->mesh, (uint16_t)(point + 4), pos.x, pos.y, pos.z);
    }
}

ParticleSystemMesh::ParticleSystemMesh(PaintCanvas *canvas, const Matrix *matrix, const Array<ParticleSet> &sets,
                                       bool a, bool b) {
    _psm_base_ctor(this, canvas, matrix, &sets, a, b);

    uint32_t flags = this->flags;
    uint32_t particleCount = this->particleCount;
    this->field_0x80 = 0;
    this->field_0x84 = 0;
    this->field_0x88 = 0;

    uint32_t edgeCount = ((flags >> 13) & 1) + ((flags >> 12) & 1);
    if ((flags & 0x4000) != 0)
        edgeCount++;

    uint32_t wide = (flags >> 16) & 1;
    uint32_t stride = edgeCount << wide;
    this->wide = (uint8_t) wide;
    uint32_t quads = particleCount * stride;
    this->edgeCount = edgeCount;
    this->stride = stride;
    this->pointCount = quads << 2;

    uint32_t vectorCount;
    if ((flags & 0x8000) == 0) {
        vectorCount = particleCount;
    } else {
        if (sets.size() != 0) {
            int set = (int) sets.data()[0];
            if (set != -1) {
                char *data = g_ParticleSetData + (set * 160);
                if (*(float *) (data + 0x40) > 0.0f)
                    this->pointCount = (quads << 2) + (stride << 2);
            }
        }

        vectorCount = particleCount * edgeCount * 2;
    }

    this->positions = new Vector[vectorCount]();
    this->field_0x78 = 0;
    this->field_0x7c = 0;
}

void ParticleSystemMesh::setParticle(const Vector &pos, float scale, uint32_t color, float u0, float u1, float v0,
                                     float v1, bool useMaskedColor, float upScale, float dirScale,
                                     const Vector &delta, bool finish) {
    Vector right;
    Vector rightScaled;
    Vector up;
    Vector upScaled;
    Vector dir;
    Vector dirScaled;
    Vector tmpA;
    Vector tmpB;

    _psm_matrixGetRight(&right, this->matrix);
    _psm_vectorScale(&rightScaled, &right, scale);
    if (this->flipRight != 0) {
        _psm_vectorScale(&tmpA, &rightScaled, -1.0f);
        _psm_vectorAssign(&rightScaled, &tmpA);
    }

    _psm_matrixGetUp(&up, this->matrix);
    _psm_vectorScale(&upScaled, &up, upScale == 0.0f ? (float) useMaskedColor : upScale);
    _psm_matrixGetDir(&dir, this->matrix);
    _psm_vectorScale(&dirScaled, &dir, dirScale == 0.0f ? scale : dirScale);

    uint32_t flags = this->flags;
    if ((flags & 0x20000) != 0) {
        _psm_vectorMinus(&tmpA, &upScaled, &rightScaled);
        _psm_vectorScale(&dirScaled, &tmpA, 0.70710677f);
        _psm_vectorPlus(&tmpB, &rightScaled, &upScaled);
        _psm_vectorScale(&rightScaled, &tmpB, 0.70710677f);
        flags = this->flags;
    }

    int point = (int) this->firstPoint + (int) this->stride * this->currentId * 4;
    if ((int) (flags << 19) < 0) {
        _psm_vectorMinus(&tmpA, &pos, &upScaled);
        _psm_vectorMinus(&tmpB, &tmpA, &delta);
        setQuadEdge(tmpB, point, rightScaled);
        _psm_vectorPlus(&tmpA, &pos, &upScaled);
        _psm_vectorPlus(&tmpB, &tmpA, &delta);
        setQuadEdge(tmpB, point + 2, rightScaled);
        point += this->wide == 0 ? 4 : 8;
        flags = this->flags;
    }
    if ((int) (flags << 18) < 0) {
        _psm_vectorMinus(&tmpA, &pos, &upScaled);
        _psm_vectorPlus(&tmpB, &tmpA, &delta);
        setQuadEdge(tmpB, point, dirScaled);
        _psm_vectorPlus(&tmpA, &pos, &upScaled);
        _psm_vectorMinus(&tmpB, &tmpA, &delta);
        setQuadEdge(tmpB, point + 2, dirScaled);
        point += this->wide == 0 ? 4 : 8;
        flags = this->flags;
    }
    if ((int) (flags << 17) < 0) {
        _psm_vectorPlus(&tmpB, &pos, &rightScaled);
        setQuadEdge(tmpB, point, upScaled);
        _psm_vectorMinus(&tmpB, &pos, &rightScaled);
        setQuadEdge(tmpB, point + 2, upScaled);
    }

    uint32_t frontColor = color;
    uint32_t backColor = color;
    uint32_t mask = this->colorMask == 0 ? 0xffffff00u : 0xffu;
    if (useMaskedColor && !finish)
        frontColor &= mask;
    if (finish)
        backColor &= mask;

    int base = (int) this->firstPoint + (int) this->stride * this->currentId * 4;
    for (int i = 0; i < (int) this->stride; i++) {
        _psm_meshSetColorWord(this->canvas, this->mesh, (uint16_t) base, backColor);
        _psm_meshSetColorWord(this->canvas, this->mesh, (uint16_t)(base + 1), backColor);
        _psm_meshSetColorWord(this->canvas, this->mesh, (uint16_t)(base + 2), frontColor);
        _psm_meshSetColorWord(this->canvas, this->mesh, (uint16_t)(base + 3), frontColor);
        _psm_meshSetUV2(this->canvas, this->mesh, (uint16_t) base, u0, v0);
        _psm_meshSetUV2(this->canvas, this->mesh, (uint16_t)(base + 1), u1, v0);
        _psm_meshSetUV2(this->canvas, this->mesh, (uint16_t)(base + 2), u0, v1);
        _psm_meshSetUV2(this->canvas, this->mesh, (uint16_t)(base + 3), u1, v1);
        base += 4;
    }
}

int ParticleSystemMesh::init(uint32_t mesh, uint16_t firstPoint) {
    this->mesh = mesh;
    this->firstPoint = firstPoint;

    void(*setUV)(PaintCanvas *, uint32_t, uint16_t, float, float) = _psm_meshSetUV;
    void(*setZero)(PaintCanvas *, uint32_t, uint16_t, uint32_t) = _psm_meshSetZero;

    for (int i = 0; i < (int) this->pointCount; i += 4) {
        setUV(this->canvas, this->mesh, (uint16_t)(this->firstPoint + i), 0.0f, 0.0f);
        setUV(this->canvas, this->mesh, (uint16_t)(this->firstPoint + i + 1), 1.0f, 0.0f);
        setUV(this->canvas, this->mesh, (uint16_t)(this->firstPoint + i + 2), 0.0f, 1.0f);
        setUV(this->canvas, this->mesh, (uint16_t)(this->firstPoint + i + 3), 1.0f, 1.0f);
        setZero(this->canvas, this->mesh, (uint16_t)(this->firstPoint + i), 0);
        setZero(this->canvas, this->mesh, (uint16_t)(this->firstPoint + i + 1), 0);
        setZero(this->canvas, this->mesh, (uint16_t)(this->firstPoint + i + 2), 0);
        setZero(this->canvas, this->mesh, (uint16_t)(this->firstPoint + i + 3), 0);
    }

    int point = (int) this->firstPoint;
    for (int tri = 0; tri < ((int) this->pointCount >> 1); tri += 2) {
        _psm_meshSetTriangle(this->canvas, this->mesh,
                             (uint16_t)(tri + (this->firstPoint >> 1)), (uint16_t)(point + 2),
                             (uint16_t)(point + 1), (uint16_t) point);
        _psm_meshSetTriangle(this->canvas, this->mesh,
                             (uint16_t)(tri + (this->firstPoint >> 1) + 1), (uint16_t)(point + 1),
                             (uint16_t)(point + 2), (uint16_t)(point + 3));
        point += 4;
    }

    this->initialized = 1;

    this->reset();
    return 0;
}

void ParticleSystemMesh::updateUsualEdges(int id, int delta) {
    Vector move;
    Vector tmp;
    float scale = (float) delta * 0.001f;
    const Vector *src;
    if ((int) (this->edgeFlags << 28) < 0) {
        const Vector *trail = this->positions + id;
        src = &this->motion;
        scale *= trail->y;
    } else {
        src = this->positions + id;
    }

    _psm_vectorScale(&tmp, src, scale);
    move = tmp;

    int point = (int) this->firstPoint + (int) this->stride * id * 4;
    for (int i = 0; i < (int) this->stride * 4; i++)
        _psm_meshTranslatePoint(this->canvas, this->mesh, (uint16_t)(point + i), move.x,
                                move.y, move.z);
}

void ParticleSystemMesh::updateSingleColor(int id) {
    float b;
    float g;
    float r;
    float a;

    int start = (int) this->firstPoint;
    int stride = (int) this->stride;
    if ((int) (this->trailFlags << 24) < 0) {
        int prev = id == 0 ? (int) this->particleCount : id;
        if (this->ages[prev - 1] == -1) {
            int set = this->setIds[id];
            uint32_t color = *(uint32_t *) (g_ParticleSetData + set * 160 + 0x38);
            uint32_t mask = this->colorMask == 0 ? 0xffffff00u : 0xffu;
            color &= mask;
            r = (float) ((color >> 16) & 0xff) * 0.0039215689f;
            a = (float) (color >> 24) * 0.0039215689f;
            g = (float) ((color >> 8) & 0xff) * 0.0039215689f;
            b = (float) (color & 0xff) * 0.0039215689f;
        } else {
            _psm_interpolateColor(this, id, &b, &g, &a, &r);
        }
    } else {
        _psm_interpolateColor(this, id, &b, &g, &a, &r);
    }

    int point = start + stride * id * 4;
    for (int i = 0; i < (int) this->stride; i++) {
        _psm_meshSetColor(this->canvas, this->mesh, (uint16_t)(point + 2), a, r, g, b);
        _psm_meshSetColor(this->canvas, this->mesh, (uint16_t)(point + 3), a, r, g, b);
        point += 4;
    }

    if ((int) (this->trailFlags << 24) < 0) {
        int next = (id == (int) this->particleCount - 1) ? 0 : id + 1;
        if (this->ages[next] == -1)
            return;
        point = (int) this->firstPoint + (int) this->stride * next * 4;
        for (int i = 0; i < (int) this->stride; i++) {
            _psm_meshSetColor(this->canvas, this->mesh, (uint16_t) point, a, r, g, b);
            _psm_meshSetColor(this->canvas, this->mesh, (uint16_t)(point + 1), a, r, g, b);
            point += 4;
        }
    } else {
        point = start + stride * id * 4;
        for (int i = 0; i < (int) this->stride; i++) {
            _psm_meshSetColor(this->canvas, this->mesh, (uint16_t) point, a, r, g, b);
            _psm_meshSetColor(this->canvas, this->mesh, (uint16_t)(point + 1), a, r, g, b);
            point += 4;
        }
    }
}

void ParticleSystemMesh::render(PaintCanvas *canvas, uint32_t mesh, uint32_t texture, BlendMode blend) {
    if (mesh == 0xffffffff)
        return;
    _psm_canvasSetTexture(canvas, texture, 0xffffffff);
    _psm_canvasSetBlendMode(canvas, blend);
    uint32_t current = _psm_cameraGetCurrent(canvas);
    uint32_t local = _psm_cameraGetLocal(canvas, current);
    return _psm_render2(canvas, mesh, local);
}

void ParticleSystemMesh::emitTrail(int) {
}

void ParticleSystemMesh::updateSingle(int id, float delta) {
    int intDelta = (int) delta;
    int set = this->setIds[id];
    if ((int) (this->trailFlags << 24) < 0) {
        updateTrailEdges(id, intDelta);
        if (this->ages[id] == -2 && this->newSectionStarted != 0) {
            Vector right;
            Vector scaledRight;
            Vector up;
            Vector scaledUp;
            _psm_matrixGetRight(&right, this->matrix);
            _psm_vectorScale(&scaledRight, &right, this->flipRight == 0 ? 1.0f : -1.0f);
            _psm_matrixGetUp(&up, this->matrix);
            float s = (float) *(int32_t *) (g_ParticleSetData + set * 160 + 0x44);
            _psm_vectorScale(&scaledRight, &scaledRight, s);
            _psm_vectorScale(&scaledUp, &up, s);
            _psm_finishCurrentTrailParticle(this, (ParticleSet) set, id, &scaledRight, &scaledUp);
        }
    } else {
        updateUsualEdges(id, intDelta);
    }

    int age = this->ages[id];
    age = (int) ((float) age + delta);
    this->ages[id] = age;
    updateSingleColor(id);

    int lifetime = *(int32_t *) (g_ParticleSetData + set * 160 + 0x28);
    if (age > lifetime) {
        this->ages[id] = -1;
        int point = (int) this->firstPoint + (int) this->stride * id * 4;
        for (int i = 0; i < (int) this->stride * 4; i++)
            _psm_meshSetPoint(this->canvas, this->mesh, (uint16_t)(point + i), 0.0f, 0.0f,
                              0.0f);
    }
}

void ParticleSystemMesh::updateTrailEdges(int id, int delta) {
    int edgeCount = (int) this->edgeCount;
    int stride = (int) this->stride;
    int point = (int) this->firstPoint + id * stride * 4;
    Vector *edge = this->positions + id * edgeCount * 2;
    float scale = (float) delta * 0.001f;

    for (int i = 0; i < edgeCount; i++) {
        Vector move;
        _psm_vectorScale(&move, edge, scale);
        _psm_meshTranslatePoint(this->canvas, this->mesh, (uint16_t) point, -move.x, move.y,
                                -move.z);
        int span = this->wide == 0 ? 1 : 4;
        _psm_meshTranslatePoint(this->canvas, this->mesh, (uint16_t)(point + span), move.x,
                                move.y, move.z);

        if (this->ages[id] != -2 || (int) (this->trailFlags << 24) >= 0) {
            Vector move2;
            _psm_vectorScale(&move2, edge + 1, scale);
            _psm_meshTranslatePoint(this->canvas, this->mesh, (uint16_t)(point + 2),
                                    -move2.x, move2.y, -move2.z);
            _psm_meshTranslatePoint(this->canvas, this->mesh, (uint16_t)(point + span + 2),
                                    move2.x, move2.y, move2.z);
            edge += 2;
            point += this->wide == 0 ? 4 : 8;
        } else {
            edge += 1;
        }
    }
}
