#ifndef GOF2_PLAYER_H
#define GOF2_PLAYER_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/math/Vector.h"

#include "engine/math/Matrix.h"

class Gun;
class KIPlayer;
namespace FMOD { class Event; }


class Player {
public:
    Array<Array<Gun *> *> *guns;
    union {
        float transform[15];
        AbyssEngine::AEMath::Matrix transformMatrix;
    };
    int32_t radius;
    union {
        uint16_t destroyed;
        struct {
            uint8_t destroyedByte;
            uint8_t spawnedFlag;
        };
    };
    uint8_t pad_46[2];
    int32_t mirrorPosX;
    int32_t mirrorPosY;
    int32_t mirrorPosZ;
    uint16_t field_54;
    uint8_t pad_56[2];
    int32_t field_58;
    union {
        uint16_t enemyFlags;
        struct {
            uint8_t enemyFlagsLo;
            uint8_t carriesFriendCargoFlag;
        };
    };
    uint8_t field_5e;
    uint8_t pad_5f;
    float flShake;
    uint8_t shieldHit;
    uint8_t armorHit;
    uint8_t hullHit;
    uint8_t gammaHit;
    union {
        uint16_t empDisabled;
        uint8_t empDisabledByte;
    };
    uint8_t pad_6a[2];
    int32_t damageDoneByPlayer;
    uint8_t playShootSoundFlag;
    uint8_t pad_71[3];
    Array<Player *> *enemies;
    int32_t hitpoints;
    union {
        int32_t empPoints;
        float empPointsF;
    };
    union {
        int32_t maxEmpPoints;
        float maxEmpPointsF;
    };
    int32_t maxHitpoints;
    float shieldHP;
    int32_t armorHP;
    int32_t maxArmorHP;
    int32_t maxShieldHP;
    int32_t damageRate;
    int32_t numPrimaryGuns;
    int32_t numSecondaryGuns;
    int32_t numTertiaryGuns;
    int32_t shieldDamageRate;
    int32_t armorDamageRate;
    int32_t empDamageRate;
    int32_t damageTimer;
    float gammaHP;
    uint8_t pad_bc[4];
    uint8_t active;
    uint8_t damaged;
    uint8_t vulnerable;
    uint8_t shootingEnabled;
    float hitVector[3];
    KIPlayer *kiPlayer;
    float bombForce;
    float empForce;
    uint8_t field_dc;
    uint8_t pad_dd[3];
    uint8_t turnedEnemyFlag;
    uint8_t pad_e1[3];
    int32_t empData;
    uint8_t pad_e8[4];
    uint8_t alwaysEnemy;
    uint8_t alwaysFriend;
    uint8_t neverAttack;
    uint8_t pad_ef;
    FMOD::Event *engineEvent;
    void *enginePositionVec;
    uint8_t enginePaused;
    uint8_t pad_f9[3];
    float position[3];
    uint8_t engineSoundPlaying;
    uint8_t pad_109[3];
    int32_t playShootSoundId;
    float healAccumulator;

    Player(int radius, int hitpoints, int numPrimary, int numSecondary, int numTertiary);

    ~Player();

    int GetEngineEvent();

    void calcWeaponSounds(int count);

    void damage(int amount);

    void damage(int amount, bool flag, int missionId);

    void damageEmp(int amount, bool flag);

    void damageShip(int damage);

    unsigned char doesNeverAttack();

    int getArmorDamageRate();

    int getArmorHP();

    float getBombForce();

    int getCombinedHP();

    int getDamageRate();

    int getEmpDamageRate();

    float getEmpForce();

    int getEmpPoints();

    Player *getEnemy(int index);

    int getGammaHP();

    int getGunRegenRate(int slot);

    int getGunSlots();

    int getHitpoints();

    KIPlayer *getKIPlayer();

    int getMaxArmorHP();

    int getMaxEmpPoints();

    int getMaxHitpoints();

    int getMaxShieldHP();

    int getRadius();

    int getShieldDamageRate();

    int getShieldHP();

    unsigned char isActive();

    unsigned char isAlwaysEnemy();

    unsigned char isAlwaysFriend();

    unsigned char isDamaged();

    bool isDead();

    void regenerateArmor();

    void regenerateHull();

    void regenerateShield();

    void regenerateShield(float amount);

    void removeAllGuns();

    void resetDamageDoneByPlayer();

    void setActive(bool value);

    void setArmorHP(int value);

    void setBombForce(float value);

    void setEmpData(int points, int data);

    void setEmpForce(float value);

    float *setHitVector(float x, float y, float z);

    void setHitpoints(int value);

    void setKIPlayer(KIPlayer *value);

    void setMaxArmorHP(int value);

    void setMaxEmpPoints(int value);

    void setMaxHitpoints(int value);

    void setMaxShieldHP(int value);

    void setNeverAttack(bool value);

    void playShootSound(int type, int channel, Vector *pos, float volume);

    void setPlayShootSound(bool play, int id);

    void setRadius(int value);

    void setShootingEnabled(bool value);

    void setVulnerable(bool value);

    void shoot(int a, int b, long long pos, bool flag);

    void shoot(int a, int b, long long pos, bool flag, Matrix mat);

    void shoot(int a, long long pos, bool flag);

    void shoot(int a, long long pos, bool flag, Matrix mat);

    void turnEnemy();

    unsigned char turnedEnemy();

    Vector *update(int dt, bool doSound);

    void updateDamageRate();

    void pitchAllPrimaryGuns(float pitch);

    void damageHull(int damage);

    void damageShield(int damage);

    float damageGamma(float amount);

    Array<Player *> *getEnemies();

    Vector getPosition();

    Vector getHitVector();

    int replaceGuns(int a, int b, int c, int d, int e, bool f);

    bool isAsteroid();

    bool isGasCloud();

    bool gunAvailable(int slot);

    void heal(float amount);

    void setShieldHP(int value);

    void setGammaHP(int value);

    void refillGunDelay(int slot);

    void resetGunDelay(int slot);

    void setAlwaysEnemy(bool value);

    void setAlwaysFriend(bool value);

    void reset();

    void setEnemies(Array<Player *> *enemies);

    void addEnemies(Array<Player *> *enemies);

    void setEnemy(Player *enemy);

    void addEnemy(Player *enemy);

    void addGun(Array<Gun *> *gunsIn, int slot);

    void addGun(Gun *gun, int slot);

    void stopShooting(int slot);

    void stopShooting(int slot, int channel);

    void stopShootSound(int index, int channel);

    void PlayEngineSound(int unused, Vector *vec);

    void PauseEngineSound();

    void ResumeEngineSound(bool force);

    void StopEngineSound();

    // Static data members present in the original binary (defined for symbol parity).
    static void *velocity;
};

#endif
