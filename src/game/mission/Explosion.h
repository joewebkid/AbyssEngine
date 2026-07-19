#ifndef GOF2_EXPLOSION_H
#define GOF2_EXPLOSION_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/core/AbyssEngine.h"
#include "engine/render/AEGeometry.h"
#include "game/ship/TargetFollowCamera.h"

#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
class AEGeometry;
class TargetFollowCamera;


class Explosion {
public:
    int type;
    AEGeometry *primaryMesh;
    AEGeometry *secondaryMesh;
    Array<AEGeometry *> *fireStreaks;
    long long duration;
    long long elapsed;
    uint8_t playing;
    float scale;
    int weaponIndex;
    AbyssEngine::Matrix rotation;

    explicit Explosion(int type);

    ~Explosion();

    void addFireStreaks();

    uint8_t isPlaying();

    bool peakReached();

    void playSound(AbyssEngine::Vector *pos);

    void render();

    void reset();

    void setScaling(float scale);

    void setWeaponIndex(int index);

    void start(const AbyssEngine::Vector &position, const AbyssEngine::Vector &direction);

    void start(const AbyssEngine::Matrix &matrix);

    void translate(const AbyssEngine::Vector &v);

    void update(int dt, const AbyssEngine::Vector &position);

    void update(int dt, TargetFollowCamera *camera);
};

#endif
