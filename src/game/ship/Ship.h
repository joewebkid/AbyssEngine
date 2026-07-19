#ifndef GOF2_SHIP_H
#define GOF2_SHIP_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/mission/Item.h"

class Item;


class Ship {
public:
    Ship(int index, int baseHP, int baseLoad, int value,
         int slot0, int slot1, int slot2, int slot3, float handling);

    ~Ship();

    void addCargo(Array<Item *> *items);

    void addCargo(Item *item);

    int removeCargo(int index, int amount);

    void removeCargo(int index);

    void removeCargo(Item *item);

    void removeAllCargo();

    void setCargo(Array<Item *> *cargo);

    void replaceCargo(Array<Item *> *cargo);

    Array<Item *> *getCargo();

    Item *getCargo(int index);

    bool hasCargo(int index, int amount);

    bool hasCargoType(int type);

    bool hasVolatileGoods();

    int getCargoValue();

    bool spaceAvailable(int n);

    int getFreeSpace();

    void changeLoad(int delta);

    void setEquipment(Item *item);

    void setEquipment(Item *item, int slot);

    void setEquipment(Array<Item *> *items);

    void replaceEquipment(Array<Item *> *equipment);

    int addEquipment(Item *item);

    void removeEquipment(Item *item);

    Item *getFirstEquipmentOfSort(int sort);

    int getEquipmentValue();

    int getUsedSlots(int type);

    int getFreeSlots(int type);

    int getSlots(int i);

    int getSlotTypes();

    unsigned int getSlotPos(Item *item);

    void freeSlot(Item *item);

    void freeSlot(Item *item, int slot);

    void freeAllSlots();

    int slotAvailable(int sort);

    bool hasEquipment(int index, int amount);

    bool hasSecondaryWeapons();

    Array<Item *> *getEquipment();

    Array<Item *> *getEquipment(int type);

    Ship *clone();

    bool equals(Ship *other);

    void refreshValue();

    void adjustPrice();

    void priceDecline();

    Ship *makeShip(int price);

    void addMod(int mod);

    void setMods(Array<int> *mods);

    Array<int> *getMods();

    bool hasModInstalled(int mod);

    int getModdedLoad();

    int getIndex();

    int getRace();

    void setRace(int race);

    int getSignatureRace();

    int getBaseHP();

    int getMaxHP();

    int getCombinedHP();

    int getMaxShieldHP();

    int getMaxArmorHP();

    int getShieldRegen();

    int getBaseLoad();

    int getMaxLoad();

    int getCurrentLoad();

    int getCargoPlus();

    int getCompression();

    int getValue();

    int getPrice();

    void setPrice(int price);

    float getHandling();

    float getHandlingForShop();

    int getUnmoddedHandling();

    int getFireRateFactor();

    int getDamageFactor();

    int getFirePower();

    int getAgility();

    int getRadarType();

    int getRepairType();

    int getMaxPassengers();

    int getBoostSpeed();

    int getBoostDelay();

    int getBoostTime();

    bool hasBooster();

    int getCurrentWeaponSlot();

    void setCurrentWeaponSlot(int slot);

    int getNumAddedDeviceSlots();

    unsigned char hasEmergencySystem();

    bool hasCloak();

    bool hasCloakIntegrated();

    unsigned int hasJumpDrive();

    unsigned int hasJumpDriveIntegrated();

    int index;
    int baseHP;
    int value;
    int baseLoad;
    int currentLoad;
    int price;
    float handling;
    int maxShieldHP;
    int maxArmorHP;
    int shieldRegen;
    int cargoPlus;
    int compression;
    int radarType;
    int boostSpeed;
    int boostDelay;
    int boostTime;
    int agility;
    int maxPassengers;
    int firePower;
    int repairType;
    unsigned char hasJumpDriveFlag;
    unsigned char hasCloakFlag;
    unsigned char pad52[2];
    float fireRateFactor;
    float damageFactor;
    unsigned char hasEmergency;
    unsigned char pad5d[3];
    int signatureRace;
    int race;
    int *slots;
    Array<Item *> *equipment;
    Array<Item *> *cargo;
    int currentWeaponSlot;
    Array<int> *mods;
    int numAddedDeviceSlots;
};
#endif
