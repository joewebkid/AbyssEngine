#ifndef GOF2_BEAMGUN_H
#define GOF2_BEAMGUN_H
#include <cstdint>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "Gun.h"
#include "AbstractGun.h"
#include "game/world/Level.h"

#include "engine/math/Vector.h"

class Player;

class AEGeometry;
class Gun;
class Level;


class BeamGun : public AbstractGun {
public:
    int32_t field_0x4;
    Gun *gun;
    Level *level;
    int32_t owner;
    int32_t meshKind;
    AEGeometry *primaryGeometry;
    AEGeometry *secondaryGeometry;
    uint8_t hasSecondary;
    uint8_t secondaryVisible;

    BeamGun(int owner, Gun *gun, int meshKind, Level *level);

    ~BeamGun() override;

    void setEnemies(Array<Player *> *enemies) override;

    void setEnemy(Player *enemy) override;

    void update(int elapsed) override;

    void render() override;

    void translate(const AbyssEngine::AEMath::Vector &v) override;

    void replaceGun(unsigned int mesh, int unused) override;
};
#endif
