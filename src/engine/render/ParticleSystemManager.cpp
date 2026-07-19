

#include "engine/core/Array.h"

#include "engine/render/ParticleSystemMesh.h"
#define GOF2_ENUM_BlendMode

#include "engine/render/ParticleSystemManager.h"
#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSystemSprite.h"
#include "engine/render/PaintCanvas.h"

void _psm_ArrayReleaseSprites(void *arr);

void _psm_ReleaseSpriteSystemResource(void *canvas, unsigned res);

void _psm_renderSpritesExt(void *self);

void _psm_constructAfterCamera(void *self);

void _ips_emitManual(void *sys, float x, float y, float z);

void _psm_spriteRender4(void *canvas, unsigned a, unsigned b, unsigned c);

void _psm_spriteRender2(void *canvas, unsigned a);

void _psm_arraySpriteCtor(void *arr);

void _psm_arraySpriteDtor(void *arr);

void _ips_enableUpdate(void *sys, bool enable);

short _ips_getParticleCount16(void *sys);

int _psm_addSpriteSystem(void *self, const void *matrix, unsigned int set, bool flag);

int _psm_firstUpdate(void *self, int a, int b, int c);

void _ips_reset(void *sys);

void _psm_meshRender4(void *canvas, unsigned a, unsigned b, unsigned c);

void _psm_meshRender2(void *canvas, unsigned a);

void *_psmesh_ctor(void *self, void *canvas, const void *matrix, const void *sets,
                   bool b4, bool b5);

void *_pss_ctor(void *self, void *canvas, const void *matrix, const void *sets,
                bool b4, bool b5);

int _ips_getParticleCount(void *sys);


static char *g_activeParticleSet = nullptr;

ParticleSystemManager::ParticleSystemManager(
    PaintCanvas *canvas, ParticleSettings::CameraSet cameraSet, unsigned short spriteTex,
    bool spriteFlag, unsigned short meshTex, bool meshFlag) {
    this->cameraSet = cameraSet;
    this->canvas = canvas;

    this->spriteUvId = 0xffff;
    this->spriteTextureId = spriteTex;
    this->spriteBlendMode = 0;
    this->spriteUsesExtra = spriteFlag ? 1 : 0;

    new(&meshArray()) Array<ParticleSystemMesh *>();
    this->meshUvId = 0xffff;
    this->meshTextureId = meshTex;
    this->meshBlendMode = 0;
    this->meshUsesExtra = meshFlag ? 1 : 0;

    construct();
}

ParticleSystemManager::ParticleSystemManager(
    PaintCanvas *canvas, ParticleSettings::CameraSet cameraSet, unsigned short spriteTex,
    AbyssEngine::BlendMode spriteBlend, bool spriteFlag,
    unsigned short meshTex, AbyssEngine::BlendMode meshBlend, bool meshFlag) {
    this->cameraSet = cameraSet;
    this->canvas = canvas;

    this->spriteUvId = spriteTex;
    this->spriteTextureId = 0xffff;
    this->spriteBlendMode = spriteBlend;
    this->spriteUsesExtra = spriteFlag ? 1 : 0;

    new(&meshArray()) Array<ParticleSystemMesh *>();
    this->meshUvId = meshTex;
    this->meshTextureId = 0xffff;
    this->meshBlendMode = meshBlend;
    this->meshUsesExtra = meshFlag ? 1 : 0;

    construct();
}

void ParticleSystemManager::update(long long dt) {
    int d = (int) dt;
    if (this->enabled == 0)
        return;
    int accum = this->accumulatedDt + d;
    this->accumulatedDt = accum;

    IParticleSystem **sprites = (IParticleSystem **) this->spriteSystems.data_;
    for (unsigned i = 0; i < this->spriteSystems.count; i++) {
        IParticleSystem *p = sprites[i];
        if (p != nullptr) {
            p->update(d);
            p = sprites[i];
            if (p->canvas == nullptr) {
                if (accum > 9 || p->emitterVelocityDirty != 0) {
                    p->calcEmitterVelocity(this->accumulatedDt);
                    p = sprites[i];
                }
                p->emit(d);
            } else {
                p->resetEmitterVelocity();
            }
        }
    }

    IParticleSystem **meshes = (IParticleSystem **) this->meshSystems;
    for (unsigned i = 0; i < this->meshSystemCount; i++) {
        IParticleSystem *p = meshes[i];
        if (p != nullptr) {
            p->update(d);
            p = meshes[i];
            if (p->canvas == nullptr) {
                if (accum > 9 || p->emitterVelocityDirty != 0) {
                    p->calcEmitterVelocity(this->accumulatedDt);
                    p = meshes[i];
                }
                p->emit(d);
            } else {
                p->resetEmitterVelocity();
            }
        }
    }

    if (accum > 9)
        this->accumulatedDt = 0;
}

void ParticleSystemManager::reset() {
    IParticleSystem **sprites = (IParticleSystem **) this->spriteSystems.data_;
    for (unsigned i = 0; i < this->spriteSystems.count; i++) {
        IParticleSystem *p = sprites[i];
        if (p != nullptr)
            _ips_reset(p);
    }
    IParticleSystem **meshes = (IParticleSystem **) this->meshSystems;
    for (unsigned i = 0; i < this->meshSystemCount; i++) {
        IParticleSystem *p = meshes[i];
        if (p != nullptr)
            _ips_reset(p);
    }
}

void ParticleSystemManager::releaseSprites() {
    ArrayReleaseClasses(this->spriteSystems);
    if (this->spriteSystemId != 0xffffffff) {
        _psm_ReleaseSpriteSystemResource(this->canvas, this->spriteSystemId);
        this->spriteSystemId = 0xffffffff;
    }
}

void ParticleSystemManager::construct() {
    this->accumulatedDt = 0;
    this->enabled = 0;
    this->spriteMeshId = 0xffffffff;
    this->spriteSystemId = 0xffffffff;
    this->spriteParticleCount = 0;
    this->meshId = 0xffffffff;
    this->transformId = 0xffffffff;
    this->meshParticleCount = 0;
    this->flags = 0x101;
}

void ParticleSystemManager::render3d() {
    if (this->enabled == 0)
        return;
    bool meshActive = (this->flags & 0xff00) != 0;
    bool spriteActive = (this->flags & 0x00ff) != 0;
    if (meshActive)
        renderMeshes();
    if (spriteActive)
        _psm_renderSpritesExt(this);
}

void ParticleSystemManager::setParticleSetByIndex(int handle, unsigned char setIndex) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys != nullptr)
        sys->setParticleSetIndex(setIndex);
}

void ParticleSystemManager::enableSystemRender(int handle, bool enable) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys != nullptr)
        sys->enableRender(enable);
}

void ParticleSystemManager::release() {
    releaseSprites();
    this->canvas = nullptr;
    ArrayReleaseClasses<ParticleSystemMesh *>(meshArray());
}

void ParticleSystemManager::cameraToggle(ParticleSettings::CameraSet cam) {
    if (this->cameraSet == cam)
        return;
    this->cameraSet = cam;
    releaseSprites();
    _psm_constructAfterCamera(this);
}

unsigned int ParticleSystemManager::addMeshSystem(AbyssEngine::AEMath::Matrix const *matrix,
                                                  Array<ParticleSettings::ParticleSet> const &sets,
                                                  bool flag) {
    void *sys = ::operator new(0xa0);
    _psmesh_ctor(sys, this->canvas, matrix, &sets, flag, this->meshUsesExtra != 0);
    ArrayAdd<ParticleSystemMesh *>(static_cast<ParticleSystemMesh *>(sys), meshArray());

    this->meshParticleCount += _ips_getParticleCount(sys);

    return (this->meshSystemCount - 1) | 0x4000;
}

unsigned long long ParticleSystemManager::emitManual(int handle, AbyssEngine::AEMath::Vector const &pos,
                                                     int ret, float p4) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys == nullptr)
        return ((unsigned long long) 0xffffffffu << 32) | (unsigned int) (unsigned long) this;

    sys->emitManual(pos, ret, nullptr, p4);
    return (unsigned int) ret;
}

unsigned long long ParticleSystemManager::emitManual(int handle, AbyssEngine::AEMath::Vector const &pos,
                                                     int ret, AbyssEngine::AEMath::Vector const &velocity,
                                                     float p5) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys == nullptr)
        return ((unsigned long long) 0xffffffffu << 32) | (unsigned int) (unsigned long) this;

    sys->emitManual(pos, ret, &velocity, p5);
    return (unsigned int) ret;
}

void ParticleSystemManager::renderSprites() {
    if (this->spriteTextureId != -1)
        _psm_spriteRender2(this->canvas, this->spriteSystemId);
    else if (this->spriteUvId != -1)
        _psm_spriteRender4(this->canvas, this->spriteSystemId, this->spriteMeshId, this->spriteBlendMode);
}

void ParticleSystemManager::systemSetMatrix(int handle, AbyssEngine::AEMath::Matrix const *matrix) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys != nullptr)
        sys->setMatrix(matrix);
}

void ParticleSystemManager::setParticleSetBySet(int handle, ParticleSettings::ParticleSet set) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys != nullptr)
        sys->setParticleSet(set);
}

void ParticleSystemManager::enableSystemUpdate(int handle, bool enable) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys != nullptr)
        _ips_enableUpdate(sys, enable);
}

void ParticleSystemManager::initSprites() {
    if (this->spriteSystems.count == 0)
        return;

    this->spriteSystemId = 0xffffffff;
    if (this->cameraSet == 0)
        return;

    PaintCanvas *canvas = (PaintCanvas *) this->canvas;
    if ((unsigned short) this->spriteTextureId == 0xffff) {
        if (this->spriteUvId != -1) {
            canvas->SpriteSystemCreate((unsigned short) this->spriteParticleCount, false,
                                       this->spriteSystemId);
            canvas->TextureCreate((unsigned short) this->spriteUvId, this->spriteSystemId,
                                  (((char) (unsigned long) this + ',') != 0));
        }
    } else {
        canvas->SpriteSystemCreate((unsigned short) this->spriteParticleCount, false,
                                   (unsigned short) this->spriteTextureId, this->spriteSystemId);
    }

    short offset = 0;
    canvas->SpriteSystemSetAllSize((unsigned int) (short) this->spriteSystemId, 0);

    float u = *(float *) (g_activeParticleSet + 0x90);
    float w = *(float *) (g_activeParticleSet + 0x94);
    canvas->SpriteSystemSetAllUv(this->spriteSystemId, u, 0.0f, w, 0.0f);

    IParticleSystem **sprites = (IParticleSystem **) this->spriteSystems.data_;
    for (unsigned i = 0; i < this->spriteSystems.count; ++i) {
        IParticleSystem *sys = sprites[i];
        sys->init(this->spriteSystemId, (uint16_t) offset);
        offset += _ips_getParticleCount16(sprites[i]);
    }
}

int ParticleSystemManager::addSpriteSystem(AbyssEngine::AEMath::Matrix const *matrix,
                                           Array<ParticleSettings::ParticleSet> const &sets, bool flag) {
    void *sys = ::operator new(0x78);
    _pss_ctor(sys, this->canvas, matrix, &sets, flag, this->spriteUsesExtra != 0);
    ArrayAdd<ParticleSystemSprite *>(static_cast<ParticleSystemSprite *>(sys), spriteArray());
    this->spriteParticleCount += _ips_getParticleCount(sys);
    return this->spriteSystems.count - 1;
}

void ParticleSystemManager::initMesh() {
    if (this->meshSystemCount == 0)
        return;

    this->meshId = 0xffffffff;
    this->transformId = 0xffffffff;

    PaintCanvas *canvas = (PaintCanvas *) this->canvas;
    int verts = (int) ((this->meshParticleCount & 0x3fff) << 2);
    int indices = (int) ((this->meshParticleCount & 0x7fff) << 1);

    if (this->meshTextureId == -1) {
        if (this->meshUvId != -1) {
            canvas->MeshCreate((unsigned short) verts, (unsigned short) indices, (signed char) 0x1b,
                               this->meshId);
            canvas->TextureCreate((unsigned short) this->meshUvId, this->meshId,
                                  (((char) (unsigned long) this + 'P') != 0));
        }
    } else {
        canvas->MeshCreate((unsigned short) verts, (unsigned short) indices, (signed char) 0x1b,
                           (unsigned short) this->meshTextureId, this->meshId);
    }

    canvas->TransformCreate(this->transformId);
    canvas->TransformAddMeshId(this->transformId, this->meshId);

    short offset = 0;
    IParticleSystem **meshes = (IParticleSystem **) this->meshSystems;
    for (unsigned i = 0; i < this->meshSystemCount; ++i) {
        IParticleSystem *sys = meshes[i];
        sys->init(this->meshId, (uint16_t) offset);

        short count = _ips_getParticleCount16(meshes[i]);
        offset += (short) (count * 4);
    }
}

void ParticleSystemManager::enableSystemEmit(int handle, bool enable) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys != nullptr)
        sys->enableEmit(enable);
}

int ParticleSystemManager::addSystem(AbyssEngine::AEMath::Matrix const *matrix,
                                     ParticleSettings::ParticleSet set, bool flag) {
    return _psm_addSpriteSystem(this, matrix, set, flag);
}

int ParticleSystemManager::init() {
    initSprites();
    initMesh();
    this->enabled = 1;
    return _psm_firstUpdate(this, 0, 0, 0);
}

void ParticleSystemManager::resetSystem(int handle) {
    IParticleSystem *sys;
    if (handle == -1) {
        sys = nullptr;
    } else {
        IParticleSystem **arr;
        int idx;
        if (handle << 0x11 < 0) {
            arr = reinterpret_cast<IParticleSystem **>(this->meshSystems);
            idx = handle & 0x3fffffff;
        } else {
            arr = reinterpret_cast<IParticleSystem **>(this->spriteSystems.data_);
            idx = handle;
        }
        sys = arr[idx];
    }
    if (sys != nullptr)
        _ips_reset(sys);
}

void ParticleSystemManager::renderMeshes() {
    if (this->meshTextureId != -1)
        _psm_meshRender2(this->canvas, this->transformId);
    else if (this->spriteUvId != -1)
        _psm_meshRender4(this->canvas, this->transformId, this->meshExtraId, this->meshBlendMode);
}

void ParticleSystemManager::renderPost3d() {
}

ParticleSystemManager::~ParticleSystemManager() {
    release();
    meshArray().~Array();
}
