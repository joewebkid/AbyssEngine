#ifndef GOF2_SENTRYGUN_H
#define GOF2_SENTRYGUN_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/weapons/ObjectGun.h"

class Gun;
class Level;


class SentryGun : public ObjectGun {
public:
    // Android body stores `gun->itemIndex * 3 - 633`: a base index into the
    // Level-owned array of nine reusable sentry KIPlayer objects.
    int spawnPoolStartIndex;

    SentryGun(Gun *gun, int meshId, int objectGunArgument, int familyTag, Level *level);

    ~SentryGun();

    void update(int dt);

    void render();
};
#endif
