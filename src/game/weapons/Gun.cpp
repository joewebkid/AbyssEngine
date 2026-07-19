#include "game/weapons/Gun.h"
#include <cstdint>
#include "game/ship/Player.h"
#include "game/mission/Item.h"
#include "engine/render/Sparks.h"
#include "engine/render/AEGeometry.h"
#include "engine/core/Array.h"
#include "engine/math/Vector.h"

// Minimal byte-faithful models for the untyped game-module handles that Gun.cpp
// reaches into. Each field sits at exactly the byte offset the original
// pointer-arithmetic accessed; padding members keep the layout exact so the
// named-member access compiles to the same load/store in the 32-bit MATCH build.
namespace {

// Pointed to by the global item/status registry (gSI_items, gIG_status).
struct GunItemRegistry {
    int32_t field_0x0;          // 0x00
    int32_t *itemTable;         // 0x04 -> table of Item* values (stored as int)
    uint8_t pad_0x08[0xc8 - 0x08];
    int32_t nukeDetonations;    // 0xc8 (incremented when a nuke ignites)
};

// Pointed to by the per-frame update globals (gUP_globals).
struct GunUpdateGlobals {
    uint8_t pad_0x00[0x12c];
    int32_t field_0x12c;        // 0x12c
};

// The Level handle (Gun::level is stored as an int address).
struct GunLevelHandle {
    uint8_t pad_0x00[0x69];
    uint8_t field_0x69;         // 0x69 (cleared when an EMP/nuke ignites)
};

// A PaintCanvas transform handle (returned by TransformGetTransform).
struct GunTransformHandle {
    uint8_t pad_0x00[0xed];
    uint8_t visible_0xed;       // 0xed (non-zero when the transform is drawable)
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(GunItemRegistry, itemTable) == 0x04,
              "GunItemRegistry::itemTable offset");
static_assert(__builtin_offsetof(GunItemRegistry, nukeDetonations) == 0xc8,
              "GunItemRegistry::nukeDetonations offset");
static_assert(__builtin_offsetof(GunUpdateGlobals, field_0x12c) == 0x12c,
              "GunUpdateGlobals::field_0x12c offset");
static_assert(__builtin_offsetof(GunLevelHandle, field_0x69) == 0x69,
              "GunLevelHandle::field_0x69 offset");
static_assert(__builtin_offsetof(GunTransformHandle, visible_0xed) == 0xed,
              "GunTransformHandle::visible_0xed offset");
#endif

} // anonymous namespace

typedef Array<Vector> VecArray;


void Gun_VecArray_ctor(void *a);

void Gun_VecPtrArray_ctor(void *a);

void Gun_VecArray_setLength(int n, void *a);

void Gun_VecPtrArray_setLength(int n, void *a);

void Gun_ArrayReleaseClasses(VecArray * a);
void *Gun_ArrayDtor(VecArray * a);

typedef void (*dtor_fn)(void *);
static dtor_fn const gGunStringDtor = nullptr;

Gun::~Gun() noexcept(false) {
    delete[] this->lifetimes;
    this->lifetimes = 0;

    delete[] this->hitFlags;
    this->hitFlags = 0;

    delete[] this->geometries;
    this->geometries = 0;

    delete[] this->randomFlags;
    this->randomFlags = 0;

    VecArray *arr = reinterpret_cast<VecArray *>(this->wobbleOffsets);
    if (arr != 0) {
        Gun_ArrayReleaseClasses(arr);
        VecArray *arr2 = reinterpret_cast<VecArray *>(this->wobbleOffsets);
        if (arr2 != 0) {
            void *p = Gun_ArrayDtor(arr2);
            ::operator delete(p);
        }
    }
    this->wobbleOffsets = 0;

    dtor_fn d = gGunStringDtor;
    d(&this->field_0x2c);
    d(&this->field_0x20);
    d(&this->directionCount);
    d(&this->count);
}

void Gun::setFriendGun(bool v) {
    this->friendGun = v;
}

int Gun::getMagnitude() {
    return this->magnitude;
}

void *Gun::getEnemies() {
    return this->enemies;
}

void Gun::setEnemies(Array<Player *> *enemies) {
    this->enemies = enemies;
}

void Gun::setMagnitude(int v) {
    this->magnitude = v;
}

void Gun::setErrorMagnitudePercentage(int v) {
    this->errorMagnitudePercentage = (float) v;
}

void Gun::setImpact(Sparks *impact) {
    this->impact = impact;
}

void Gun::setPlayerGun(bool v) {
    this->playerGun = v;
}

uint8_t Gun::isPlayerGun() {
    return this->playerGun;
}

void Gun::setLevelCollision(bool v) {
    this->levelCollision = v;
}

void Gun::setLevel(Level *lvl) {
    this->level = (int) (intptr_t) lvl;
}

void Gun::removeAllEnemies() {
    this->enemies = 0;
}

namespace AbyssEngine {
    namespace AERandom {
        int nextInt(int rng);
    }

    namespace Transform {
        void SetAnimationState(unsigned tf, int a, int b);
    }

    class PaintCanvas {
    public:
        static unsigned TransformGetTransform(unsigned canvas);

        static unsigned CameraGetCurrent();

        static unsigned CameraGetLocal(unsigned canvas);

        static unsigned TransformGetLocal(unsigned canvas);

        static void TransformSetLocal(unsigned canvas, AEMath::Matrix *m);

        static void DrawTransform(unsigned canvas, AEMath::Matrix *m);
    };
}

static int *const gSI_items = nullptr;
static int *const gSI_table = nullptr;
static unsigned *const gSI_canvas = nullptr;
static int *const gSI_rng = nullptr;

void Gun::setIndex(int index) {
    this->itemIndex = index;
    int *items = gSI_items;
    this->homing = (index == 0xe4) || ((unsigned) (index - 9) < 3);
    GunItemRegistry *reg = reinterpret_cast<GunItemRegistry *>(*items);
    this->empDamage = ((Item *) reg->itemTable[index])->getAttribute(0xa);
    int g = gSI_table[index];
    if (g >= 0) {
        unsigned count = this->count;
        this->geometries = new int[count];
        this->randomFlags = new uint8_t[count];
        unsigned *canvasHolder = gSI_canvas;
        int *rngHolder = gSI_rng;
        for (unsigned i = 0; i < count; i = i + 1) {
            AEGeometry *geom = new AEGeometry((uint16_t) g, (PaintCanvas *) *canvasHolder, false);
            this->geometries[i] = geom->transform;
            int r = AbyssEngine::AERandom::nextInt(*rngHolder);
            this->randomFlags[i] = (r == 0);
            unsigned tf = AbyssEngine::PaintCanvas::TransformGetTransform(*canvasHolder);
            AbyssEngine::Transform::SetAnimationState(tf, 0, 0);
            delete geom;
            count = this->count;
        }
    }
}

static int *const gSO_holder = nullptr;
static short *const gSO_table = nullptr;

void Gun::setOffset(int a, int b) {
    short *row = &gSO_table[b * 0x1e + a * 3];
    Vector local;
    local.x = (float) (int) row[0];
    local.y = (float) (int) row[1];
    local.z = (float) (int) row[2];
    local.x = this->offset.x + local.x;
    local.y = this->offset.y + local.y;
    local.z = this->offset.z + local.z;
    this->offset = local;
}

namespace AbyssEngine {
    namespace AEMath {
        float VectorLength(const Vector *v);
    }
}

static int *const gIG_status = nullptr;

void Gun::ignite() {
    if (this->weaponType == ITEM_SORT_EMP_BOMB || this->weaponType == ITEM_SORT_NUKE) {
        if (this->weaponType == ITEM_SORT_NUKE)
            reinterpret_cast<GunItemRegistry *>(*gIG_status)->nukeDetonations += 1;
        reinterpret_cast<GunLevelHandle *>(this->level)->field_0x69 = 0;
    }

    Array<Player *> *enemies = this->enemies;
    this->ignited = 1;
    if (enemies == 0)
        return;

    Vector *posOut = &this->targetDir;
    Vector *base = &this->basePos;
    this->lastHitKIPlayer = 0;

    for (unsigned ei = 0; ei < enemies->size(); ei = ei + 1) {
        Player *target = enemies->data()[ei];
        this->target = target;
        if (this->weaponType == ITEM_SORT_EMP_BOMB && target->isAsteroid() != 0)
            continue;
        if (target->isActive() == 0)
            continue;

        Vector *positions = reinterpret_cast<Vector *>(this->positions);
        Vector *hitPositions = reinterpret_cast<Vector *>(this->hitPositions);
        for (unsigned i = 0; i < this->count; i = i + 1) {
            Vector v = positions[i];
            *base = v;
            *posOut = v;
            *posOut -= *base;
            int dist = (int) AbyssEngine::AEMath::VectorLength(posOut);
            if (dist < this->magnitude) {
                this->hitFlags[i] = 1;
                hitPositions[i] = *base;
                GunItemRegistry *reg = reinterpret_cast<GunItemRegistry *>(*gIG_status);
                ((Item *) reg->itemTable[this->itemIndex])->getAttribute(0);
            }
        }
    }
}

namespace AbyssEngine {
    namespace AEMath {
        void MatrixGetPosition(Matrix *out, const Matrix *m);

        void MatrixSetTranslation(Matrix *m, float x, float y, float z);
    }
}

static unsigned *const gGunRenderCanvas = nullptr;

void Gun::render() {
    Matrix local;
    char camBuf[64];

    Sparks *impact = this->impact;
    if (impact != 0)
        impact->render();

    if (this->geometries != 0) {
        unsigned canvas = *gGunRenderCanvas;
        for (unsigned i = 0; i < this->count; i = i + 1) {
            int tf = AbyssEngine::PaintCanvas::TransformGetTransform(canvas);
            if (reinterpret_cast<GunTransformHandle *>(tf)->visible_0xed != 0) {
                unsigned c = canvas;
                AbyssEngine::PaintCanvas::CameraGetCurrent();
                unsigned camLocal = AbyssEngine::PaintCanvas::CameraGetLocal(c);
                memcpy(camBuf, (const void *) camLocal, 0x3c);
                unsigned tl = AbyssEngine::PaintCanvas::TransformGetLocal(canvas);
                AbyssEngine::AEMath::MatrixGetPosition(&local, (const Matrix *) tl);
                this->targetDir = *(const Vector *) ((Vector *) &local);
                AbyssEngine::AEMath::MatrixSetTranslation(&local, this->targetDir.z, 0, 0);
                Matrix *m = ((Matrix **) this->geometries)[i];
                AbyssEngine::PaintCanvas::TransformSetLocal(canvas, m);
                AbyssEngine::PaintCanvas::DrawTransform(canvas, m);
            }
        }
    }
}

Gun::Gun(int kind, int p2, int count, int p4, int p5, int p6, float p7, Vector dir, Vector vel) {
    Gun_VecArray_ctor(&this->count);
    Gun_VecArray_ctor(&this->directionCount);
    Gun_VecArray_ctor(&this->field_0x20);
    Gun_VecArray_ctor(&this->field_0x2c);
    this->offset.x = 0;
    this->offset.y = 0;
    this->offset.z = 0;
    this->field_0x90 = 0;
    this->field_0x94 = 0;
    this->field_0x98 = 0;
    this->level = 0;
    this->lifetimes = 0;
    this->enemies = 0;
    this->impact = 0;
    this->geometries = 0;
    this->randomFlags = 0;
    this->slotIndex = kind;
    this->damage = p2;
    this->field_0x50 = p7;
    this->targetDir.z = 0;
    this->velocity.x = 0;
    this->velocity.y = 0;
    this->velocity.z = 0;
    this->field_0xd0 = 0;
    this->field_0xd4 = 0;
    this->targetDir.x = 0;
    this->targetDir.y = 0;
    this->basePos.x = 0;
    this->basePos.y = 0;
    this->basePos.z = 0;
    this->field_0xcc = 0;
    this->ammoCount = 0;
    this->field_0x78 = 0;
    this->playerGun = 0;
    this->field_0xa8 = 0;

    this->offset = dir;
    vel *= p7;
    this->velocity = vel;
    this->initialLifetime = p5;
    this->fireDelay = p6;
    this->timer = p6;
    this->fireIndex = 0;
    this->ignited = 0;
    this->delayActive = 0;
    this->ammoCount = p4;
    this->field_0x78 = p4 << 1;
    this->lifetimes = new int[count];
    this->hitFlags = new uint8_t[count];
    void *arr = ::operator new(0xc);
    Gun_VecPtrArray_ctor(arr);
    this->wobbleOffsets = static_cast<Array<int> *>(arr);
    Gun_VecArray_setLength(count, &this->count);
    Gun_VecArray_setLength(count, &this->directionCount);
    Gun_VecArray_setLength(count, &this->field_0x20);
    Gun_VecArray_setLength(count, &this->field_0x2c);
    Gun_VecPtrArray_setLength(count, this->wobbleOffsets);
    Vector *positions = reinterpret_cast<Vector *>(this->positions);
    for (int i = 0; i < (int) count; i = i + 1) {
        positions[i].x = 0;
        this->lifetimes[i] = 0;
        this->hitFlags[i] = 0;
        this->wobbleOffsets->data_[i] = 0;
    }
    this->impact = 0;
    this->field_0x54 = 0;
    this->active = 0;
    this->enemies = 0;
    this->itemIndex = -1;
    this->weaponType = static_cast<ItemSort>(-1);
    this->useCustomRadius = 0;
    this->levelCollision = 1;
    this->errorMagnitudePercentage = 0;
    this->field_0x89 = 0;
    this->owner = 0;
    this->field_0xb0 = 0;
    this->empDamage = 0;
    this->field_0xa4 = 0;
}

void Gun::shoot(Matrix m, int n, bool b) {
    this->shootAt(m, n, 0, b);
}

void Gun::setEnemy(Player *enemy) {
    this->enemies = reinterpret_cast<Array<Player *> *>(enemy);
}

static const float kZOffset = 0.1f;

void Gun::setOffset(Vector *v) {
    Vector local;
    local.x = v->x;
    local.y = v->y;
    local.z = v->z + kZOffset;
    this->offset = local;
}

namespace AbyssEngine {
    namespace AEMath {
        void operator_mul(Vector *out, float s);
    }

    namespace Transform {
        void Update(long long tf, char b);
    }
}

static int *const gUP_canvas = nullptr;
static int *const gUP_globals = nullptr;

void Gun::update(int dt) {
    this->timer += dt;
    if (this->delayActive != 0) {
        int t = this->delayTimer + dt;
        this->delayTimer = t;
        if (this->fireDelay <= t)
            this->delayActive = 0;
    }
    Sparks *impact = this->impact;
    if (impact != 0)
        impact->update(dt);

    if (this->geometries != 0) {
        int canvas = *gUP_canvas;
        for (unsigned i = 0; i < this->count; i = i + 1) {
            long long tf = AbyssEngine::PaintCanvas::TransformGetTransform(canvas);
            AbyssEngine::Transform::Update(tf, (char) dt);
        }
    }

    if (this->active != 0 && this->weaponType != ITEM_SORT_SENTRY_GUN) {
        this->calcCharacterCollision();
        float fdt = (float) dt;
        Vector *positions = reinterpret_cast<Vector *>(this->positions);
        Vector *velocities = reinterpret_cast<Vector *>(this->velocities);
        for (unsigned i = 0; i < this->count; i = i + 1) {
            int amt = this->lifetimes[i];
            int thr = 5;

            if ((unsigned) (this->weaponType - ITEM_SORT_ROCKET) > 1 && this->weaponType != ITEM_SORT_CLUSTER_MISSILE)
                thr = 0;
            if (thr < amt) {
                this->lifetimes[i] = amt - dt;
                Vector scaled;
                long long velBits = (long long) this->velocities;
                if (this->weaponType == ITEM_SORT_MINE) {
                    Vector tmp;
                    memcpy(&tmp.x, &velBits, sizeof(long long));
                    AbyssEngine::AEMath::operator_mul(&tmp, fdt);
                    int rem = this->initialLifetime - this->lifetimes[i];
                    float f = (float) rem / 1.0f + 1.0f;
                    scaled = tmp;
                    AbyssEngine::AEMath::operator_mul(&scaled, f);
                } else {
                    memcpy(&scaled.x, &velBits, sizeof(long long));
                    AbyssEngine::AEMath::operator_mul(&scaled, fdt);
                }
                positions[i] += scaled;
                int v = this->lifetimes[i];
                if (v < 1) {
                    unsigned k = this->weaponType - 6;
                    if (k < 0x1d && ((1 << (k & 0xff)) & 0x12345678) != 0) {
                        this->ignite();
                        v = this->lifetimes[i];
                    }
                    if (v <= -2000) {
                        int s = this->weaponType;
                        if ((unsigned) (s - ITEM_SORT_ROCKET) < 2 || s == ITEM_SORT_CLUSTER_MISSILE)
                            reinterpret_cast<GunUpdateGlobals *>(*gUP_globals)->field_0x12c = 0;
                    }
                }
                if (this->weaponType == ITEM_SORT_SHOCK_BLAST)
                    this->ignite();
            } else {
                positions[i].x = 0;
                positions[i].y = 0;
                positions[i].z = 0;
                velocities[i].x = 0;
                velocities[i].y = 0;
                velocities[i].z = 0;
            }
        }
    }
}

void Gun::translate(const Vector &v) {
    Vector *positions = reinterpret_cast<Vector *>(this->positions);
    for (unsigned i = 0; i < this->count; i = i + 1) {
        positions[i] += v;
    }
}

void Gun::shootAt(Matrix m, int n, Player *p, bool b) {
    (void) m;
    this->damage = n;
    this->target = p;
    this->ignited = b;
}

static int *const gCC_status = nullptr;

void Gun::calcCharacterCollision() {
    Array<Player *> *enemies = this->enemies;
    if (enemies == 0)
        return;

    for (unsigned ei = 0; ei < enemies->size(); ei = ei + 1) {
        Player *target = enemies->data()[ei];
        this->target = target;
    }
}

void Gun::calcLevelCollision() {
}
