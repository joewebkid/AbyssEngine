#ifndef GOF2_PARTICLESYSTEMMANAGER_H
#define GOF2_PARTICLESYSTEMMANAGER_H
#include "engine/core/Array.h"
#include "engine/render/RenderEnums.h"
#include "../core/AEString.h"
#include "engine/render/ParticleSettings.h"

#include "engine/math/AEMath.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"


class IParticleSystem;
class ParticleSystemMesh;
class ParticleSystemSprite;


namespace AbyssEngine {
#ifndef GOF2_ENUM_BlendMode
#define GOF2_ENUM_BlendMode

#endif
}


namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;

class ParticleSystemManager {
public:
    union {
        uint16_t flags;
        uint8_t flagsLow;   // low byte; some callers write only this byte
    };
    void *canvas;
    int32_t cameraSet;
    int32_t accumulatedDt;
    uint8_t enabled;

    ::Array<ParticleSystemSprite *> spriteSystems;
    int16_t spriteTextureId;
    int16_t spriteUvId;
    uint32_t spriteBlendMode;
    uint32_t spriteMeshId;
    uint32_t spriteSystemId;
    uint32_t spriteParticleCount;
    uint8_t spriteUsesExtra;

    uint32_t meshSystemCount;
    void *meshSystems;
    uint32_t meshSystemCapacity;
    int16_t meshTextureId;
    int16_t meshUvId;
    uint32_t meshBlendMode;
    uint32_t meshExtraId;
    uint32_t meshId;
    uint32_t transformId;
    uint32_t meshParticleCount;
    uint8_t meshUsesExtra;

    ParticleSystemManager(PaintCanvas *canvas, ParticleSettings::CameraSet cameraSet,
                          unsigned short spriteTex, bool spriteFlag,
                          unsigned short meshTex, bool meshFlag);

    ParticleSystemManager(PaintCanvas *canvas, ParticleSettings::CameraSet cameraSet,
                          unsigned short spriteTex, AbyssEngine::BlendMode spriteBlend,
                          bool spriteFlag, unsigned short meshTex,
                          AbyssEngine::BlendMode meshBlend, bool meshFlag);

    ~ParticleSystemManager();

    void update(long long dt);

    void renderPost3d();

    void reset();

    void releaseSprites();

    void construct();

    void render3d();

    void setParticleSetByIndex(int handle, unsigned char setIndex);

    void enableSystemRender(int handle, bool enable);

    void release();

    void cameraToggle(ParticleSettings::CameraSet cam);

    unsigned int addMeshSystem(AbyssEngine::AEMath::Matrix const *matrix,
                               Array<ParticleSettings::ParticleSet> const &sets, bool flag);

    unsigned long long emitManual(int handle, AbyssEngine::AEMath::Vector const &pos, int ret, float p4);

    unsigned long long emitManual(int handle, AbyssEngine::AEMath::Vector const &pos, int ret,
                                  AbyssEngine::AEMath::Vector const &velocity, float p5);

    void renderSprites();

    void systemSetMatrix(int handle, AbyssEngine::AEMath::Matrix const *matrix);

    void setParticleSetBySet(int handle, ParticleSettings::ParticleSet set);

    void enableSystemUpdate(int handle, bool enable);

    void initSprites();

    int addSpriteSystem(AbyssEngine::AEMath::Matrix const *matrix,
                        Array<ParticleSettings::ParticleSet> const &sets, bool flag);

    void initMesh();

    void enableSystemEmit(int handle, bool enable);

    int addSystem(AbyssEngine::AEMath::Matrix const *matrix, ParticleSettings::ParticleSet set, bool flag);

    int init();

    void resetSystem(int handle);

    void renderMeshes();

private:
    Array<ParticleSystemSprite *> &spriteArray() {
        return spriteSystems;
    }

    Array<ParticleSystemMesh *> &meshArray() {
        return *reinterpret_cast<Array<ParticleSystemMesh *> *>(&meshSystemCount);
    }

};
#endif
