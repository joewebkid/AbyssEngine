#include "engine/render/Sparks.h"
#include "engine/core/AbyssEngine.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/AERandom.h"

static int Sparks_nextInt(void *rng, int bound) {
    return ((AbyssEngine::AERandom *) rng)->nextInt(bound);
}


static void **g_Sparks_canvas_ctor = (void **) &PaintCanvas::gCanvas;

static void **g_Sparks_random_ctor = (void **) &AERandom::gRandom;

Sparks::Sparks(int kind) {
    uint32_t count = 5;
    if (kind == 0)
        count = 1;
    void **canvas = g_Sparks_canvas_ctor;
    uint32_t lifetime = 0x514;
    if (kind == 0)
        lifetime = 0x1f4;

    this->kind = kind;
    this->count = count;
    this->lifetime = lifetime;

    ((PaintCanvas *) *canvas)->SpriteSystemCreate((uint16_t) count, false, this->spriteSystem);
    ((PaintCanvas *) *canvas)->SpriteSystemSetAllUv(this->spriteSystem, 0.626953125f,
                                                    0.001953125f, 0.748046875f, 0.123046875f);

    uint32_t n = this->count;
    this->active = 0;
    this->lifetimeThresholds = new int[n];
    this->totalThreshold = 0;

    if (this->kind == 1) {
        void **rng = g_Sparks_random_ctor;
        for (uint32_t i = 0; i < n; i++) {
            ((PaintCanvas *) *canvas)->SpriteSystemSetSize(this->spriteSystem, (uint16_t) i, 1);
            int value = Sparks_nextInt(*rng, 0x1f4);
            this->lifetimeThresholds[i] = value;
            n = this->count;
            this->totalThreshold = value + this->totalThreshold;
        }
    } else {
        this->lifetimeThresholds[0] = 0;
        this->totalThreshold = 0;
    }

    this->elapsed = 0;
}


static void **g_Sparks_canvas_translate = (void **) &PaintCanvas::gCanvas;

void Sparks::translate(Vector const &v) {
    void **canvas = g_Sparks_canvas_translate;
    for (uint32_t i = 0; i < this->count; i++)
        ((PaintCanvas *) *canvas)->SpriteSystemAddPosition(this->spriteSystem, (uint16_t) i,
                                                           v.x, v.y, v.z);
}

Sparks::~Sparks() {
    delete[] this->lifetimeThresholds;
    this->lifetimeThresholds = nullptr;
}

void Sparks::explode(Vector const &v) {
    return this->explode((int) v.x, (int) v.y, (int) v.z);
}

bool Sparks::isRocket() {
    return this->kind == 1;
}


static void **g_Sparks_canvas_update = (void **) &PaintCanvas::gCanvas;

void Sparks::update(int step) {
    if (this->active == 0)
        return;

    int elapsed0 = this->elapsed;
    uint32_t i = 0;
    elapsed0 += step;
    this->elapsed = elapsed0;
    for (; i < this->count; i++) {
        int *thresholds = this->lifetimeThresholds;
        int elapsed = this->elapsed;
        if (elapsed <= thresholds[i])
            continue;
        int size = this->lifetime - (elapsed << 1);
        ((PaintCanvas *) *g_Sparks_canvas_update)->SpriteSystemSetSize(this->spriteSystem, (uint16_t) i,
                                                                       (int16_t) size);
    }

    int elapsed = this->elapsed;
    int kind = this->kind;
    if (kind == 1) {
        if (elapsed <= 0x1f4) {
            int limit = this->lifetime;
            elapsed += elapsed;
            if (elapsed <= limit)
                return;
        }
    } else {
        if (elapsed <= 0x1f4)
            return;
    }

    this->active = 0;
    this->elapsed = 0;
}


static void **g_Sparks_canvas_explode_rocket = (void **) &PaintCanvas::gCanvas;

static void **g_Sparks_canvas_explode_single = (void **) &PaintCanvas::gCanvas;

static void **g_Sparks_random_explode = (void **) &AERandom::gRandom;

static int (*g_Sparks_nextInt_explode)(void *rng, int bound) = Sparks_nextInt;

void Sparks::explode(int x, int y, int z) {
    int x0 = x;
    int y0 = y;
    int z0 = z;

    if (this->active != 0)
        return;

    if (this->kind == 1) {
        uint32_t i = 0;
        void **canvas = g_Sparks_canvas_explode_rocket;
        void **rng = g_Sparks_random_explode;
        int (*nextInt)(void *, int) = g_Sparks_nextInt_explode;
        for (; i < this->count; i++) {
            uint32_t sprite = this->spriteSystem;
            void *canvasObj = *canvas;
            float px = (float) (nextInt(*rng, 0x190) + x0);
            float py = (float) (nextInt(*rng, 0x190) + y0);
            float pz = (float) (nextInt(*rng, 0x190) + z0);
            ((PaintCanvas *) canvasObj)->SpriteSystemSetPosition(sprite, (uint16_t) i,
                                                                 px, py, pz);
        }
    } else {
        void **canvas = g_Sparks_canvas_explode_single;
        ((PaintCanvas *) *canvas)->SpriteSystemSetPosition(this->spriteSystem, 0,
                                                           (float) x0, (float) y0, (float) z0);
    }

    this->active = 1;
}


static void **g_Sparks_canvas_render = (void **) &PaintCanvas::gCanvas;

void Sparks::render() {
    Matrix matrix;

    if (this->active != 0) {
        void **canvas = g_Sparks_canvas_render;
        ((PaintCanvas *) *canvas)->SetTexture(this->texture, 0xffffffff);
        ((PaintCanvas *) *canvas)->SetBlendMode(AbyssEngine::BlendMode_2);

        void *canvasObj = *canvas;
        matrix.m[0] = 1.0f;
        matrix.m[1] = 0.0f;
        matrix.m[2] = 0.0f;
        matrix.m[3] = 0.0f;
        matrix.m[4] = 0.0f;
        matrix.m[5] = 1.0f;
        matrix.m[6] = 0.0f;
        matrix.m[7] = 0.0f;
        matrix.m[8] = 0.0f;
        matrix.m[9] = 0.0f;
        matrix.m[10] = 1.0f;
        matrix.m[11] = 0.0f;
        matrix.m[12] = 1.0f;
        matrix.m[13] = 1.0f;
        matrix.m[14] = 1.0f;

        ((PaintCanvas *) canvasObj)->SetWorldViewMatrix(matrix);
        ((PaintCanvas *) *canvas)->DrawSpriteSystem(this->spriteSystem);
    }
}
