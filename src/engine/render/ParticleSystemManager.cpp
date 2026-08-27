

#include "engine/core/Array.h"

#include "engine/render/ParticleSystemMesh.h"
#define GOF2_ENUM_BlendMode

#include "engine/render/ParticleSystemManager.h"
#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSystemSprite.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/ParticleSettingsRef.h"

void _psm_constructAfterCamera(void *self);

void _ips_enableUpdate(void *sys, bool enable);

int _psm_firstUpdate(void *self, int a, int b, int c);

void _ips_reset(void *sys);

int _ips_getParticleCount(void *sys);


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
            if (p->resetEmitterVelocityPending != 0) {
                p->resetEmitterVelocity();
            } else {
                if (accum > 9 || p->emitterVelocityDirty != 0) {
                    p->calcEmitterVelocity(this->accumulatedDt);
                    p = sprites[i];
                }
                p->emit(d);
            }
        }
    }

    IParticleSystem **meshes = (IParticleSystem **) this->meshSystems;
    for (unsigned i = 0; i < this->meshSystemCount; i++) {
        IParticleSystem *p = meshes[i];
        if (p != nullptr) {
            p->update(d);
            p = meshes[i];
            if (p->resetEmitterVelocityPending != 0) {
                p->resetEmitterVelocity();
            } else {
                if (accum > 9 || p->emitterVelocityDirty != 0) {
                    p->calcEmitterVelocity(this->accumulatedDt);
                    p = meshes[i];
                }
                p->emit(d);
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
        this->canvas->ReleaseSpriteSystemResource(this->spriteSystemId);
        this->spriteSystemId = 0xffffffff;
    }
}

void ParticleSystemManager::construct() {
    this->accumulatedDt = 0;
    this->enabled = 0;
    this->spriteGeneratedTextureId = 0xffffffff;
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
    if (this->flagsHigh != 0)
        renderMeshes();
    if (this->flagsLow != 0)
        renderSprites();
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
    ParticleSystemMesh *sys = new ParticleSystemMesh(static_cast<PaintCanvas *>(this->canvas), matrix, sets,
                                                     flag, this->meshUsesExtra != 0);
    ArrayAdd<ParticleSystemMesh *>(sys, meshArray());

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
        ParticleSystemSprite::render(this->canvas, this->spriteSystemId);
    else if (this->spriteUvId != -1)
        ParticleSystemSprite::render(this->canvas, this->spriteSystemId, this->spriteGeneratedTextureId,
                                     static_cast<BlendMode>(this->spriteBlendMode));
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

    if ((unsigned short) this->spriteTextureId == 0xffff) {
        if (this->spriteUvId != -1) {
            this->canvas->SpriteSystemCreate((unsigned short) this->spriteParticleCount, false,
                                             this->spriteSystemId);
            this->canvas->TextureCreate((unsigned short) this->spriteUvId,
                                        this->spriteGeneratedTextureId, false);
        }
    } else {
        this->canvas->SpriteSystemCreate((unsigned short) this->spriteParticleCount, false,
                                         (unsigned short) this->spriteTextureId, this->spriteSystemId);
    }

    uint16_t particleOffset = 0;
    this->canvas->SpriteSystemSetAllSize(this->spriteSystemId, 0);

    // Android HD initSprites @ 0x1936c8 reads the default live definition
    // at ParticleSettingsRef::cur + 0x88..0x94.
    const ParticleSettings::SetDefinition &defaultSet = ParticleSettingsRef::cur.sets[0];
    this->canvas->SpriteSystemSetAllUv(this->spriteSystemId,
                                       defaultSet.uvU0, defaultSet.uvV0,
                                       defaultSet.uvU1, defaultSet.uvV1);

    IParticleSystem **sprites = (IParticleSystem **) this->spriteSystems.data_;
    for (unsigned i = 0; i < this->spriteSystems.count; ++i) {
        IParticleSystem *sys = sprites[i];
        sys->init(this->spriteSystemId, particleOffset);
        particleOffset = static_cast<uint16_t>(particleOffset + sys->getParticleCount());
    }
}

int ParticleSystemManager::addSpriteSystem(AbyssEngine::AEMath::Matrix const *matrix,
                                           Array<ParticleSettings::ParticleSet> const &sets, bool flag) {
    ParticleSystemSprite *sys = new ParticleSystemSprite(static_cast<PaintCanvas *>(this->canvas), matrix, sets,
                                                         flag, this->spriteUsesExtra != 0);
    ArrayAdd<ParticleSystemSprite *>(sys, spriteArray());
    this->spriteParticleCount += _ips_getParticleCount(sys);
    return this->spriteSystems.count - 1;
}

void ParticleSystemManager::initMesh() {
    if (this->meshSystemCount == 0)
        return;

    this->meshId = 0xffffffff;
    this->transformId = 0xffffffff;

    const uint16_t particleCount = static_cast<uint16_t>(this->meshParticleCount);
    const uint16_t vertexCount = static_cast<uint16_t>(particleCount << 2);
    const uint16_t triangleCount = static_cast<uint16_t>(particleCount << 1);

    if (this->meshTextureId == -1) {
        if (this->meshUvId != -1) {
            this->canvas->MeshCreate(vertexCount, triangleCount, 0x1b, this->meshId);
            this->canvas->TextureCreate((unsigned short) this->meshUvId,
                                        this->meshGeneratedTextureId, false);
        }
    } else {
        this->canvas->MeshCreate(vertexCount, triangleCount, 0x1b,
                                 (unsigned short) this->meshTextureId, this->meshId);
    }

    this->canvas->TransformCreate(this->transformId);
    this->canvas->TransformAddMeshId(this->transformId, this->meshId);

    uint16_t particleOffset = 0;
    IParticleSystem **meshes = (IParticleSystem **) this->meshSystems;
    for (unsigned i = 0; i < this->meshSystemCount; ++i) {
        IParticleSystem *sys = meshes[i];
        sys->init(this->meshId, particleOffset);
        particleOffset = static_cast<uint16_t>(particleOffset + sys->getParticleCount() * 4);
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
    Array<ParticleSettings::ParticleSet> sets;
    ArrayAdd(set, sets);

    const uint8_t *systemFlags = reinterpret_cast<const uint8_t *>(
        &ParticleSettingsRef::cur.sets[static_cast<int>(set)].flags);
    const uint32_t flags = *reinterpret_cast<const uint32_t *>(systemFlags);
    int handle = -1;
    if ((flags & 0x1u) != 0) {
        handle = addSpriteSystem(matrix, sets, flag);
    } else if ((flags & 0x2u) != 0) {
        handle = static_cast<int>(addMeshSystem(matrix, sets, flag));
    } else {
        return handle;
    }

    if ((systemFlags[3] & 0x1u) != 0)
        enableSystemUpdate(handle, false);
    return handle;
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
        ParticleSystemMesh::render(this->canvas, this->transformId);
    // Android 1.1.19 checks the shared sprite UV selector here, not meshUvId.
    // Keep the observed binary behavior even though the field choice looks asymmetric.
    else if (this->spriteUvId != -1)
        ParticleSystemMesh::render(this->canvas, this->transformId, this->meshGeneratedTextureId,
                                   static_cast<BlendMode>(this->meshBlendMode));
}

void ParticleSystemManager::renderPost3d() {
}

ParticleSystemManager::~ParticleSystemManager() {
    release();
    meshArray().~Array();
}
