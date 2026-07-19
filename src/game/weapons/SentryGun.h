#ifndef GOF2_SENTRYGUN_H
#define GOF2_SENTRYGUN_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/weapons/ObjectGun.h"

class Gun;
class Level;


class SentryGun : public ObjectGun {
public:
    int cooldown;

    SentryGun(Gun *gun, int mesh, int unused, int p4, Level *level);

    ~SentryGun();

    void update(int dt);

    void render();
};
#endif
