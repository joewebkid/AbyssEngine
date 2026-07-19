#include "game/ship/Ship.h"
#include "game/world/Standing.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/world/SolarSystem.h"

// These untyped ship-data / shop globals are referenced only from this
// translation unit, so they are kept as file-local definitions (parity loss
// accepted: the original linked them as shared symbols).
static int **gShipDataRoot = nullptr;
static int *gRaceTable      = nullptr;
static int *gDifficultyPtr  = nullptr;
static int *gShopRoot       = nullptr;

namespace {

// Named models for the untyped ship-data / shop globals. These mirror the exact
// byte offsets observed in the original 32-bit binary (see Ship::adjustPrice /
// Ship::priceDecline / Ship::getFirstEquipmentOfSort in the reference build).

// A single ship-data record. Only the fields touched here are named; the rest of
// the record is left as opaque storage so the offsets stay byte-faithful.
struct ShipDataEntry {
    int category;            // 0x00 : race/category index into gRaceTable
    unsigned char _pad04[0x14 - 0x04];
    int basePrice;           // 0x14 : base price (interpreted as float for math)
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(ShipDataEntry, category) == 0x00, "");
static_assert(offsetof(ShipDataEntry, basePrice) == 0x14, "");
#endif

// The object reached via *gShipDataRoot. At offset 4 it holds the entry table.
struct ShipDataObj {
    int _field00;            // 0x00
    ShipDataEntry **table;   // 0x04 : array of entry pointers, indexed by ship index
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(ShipDataObj, table) == 0x04, "");
#endif

// The object reached via gShopRoot index 1 (the second pointer slot). At
// offset 0x17c it holds the integrated-cloak equipment item pointer.
struct ShopEntry {
    unsigned char _pad000[0x17c];
    Item *cloakItem;         // 0x17c
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(ShopEntry, cloakItem) == 0x17c, "");
#endif

} // namespace

Ship::Ship(int index, int baseHP, int baseLoad, int value,
           int slot0, int slot1, int slot2, int slot3, float handling) {
    this->index = index;
    this->baseHP = baseHP;
    this->value = value;
    this->baseLoad = baseLoad;
    this->currentLoad = 0;
    this->price = value;

    this->slots = new int[4];
    this->slots[0] = slot0;
    this->slots[1] = slot1;
    this->slots[2] = slot2;
    this->slots[3] = slot3;

    this->handling = handling / 100.0f;

    this->equipment = new Array<Item *>();
    ArraySetLength<Item *>(slot0 + slot1 + slot2 + slot3, *this->equipment);

    this->race = 0;
    this->cargo = 0;
    this->mods = 0;
    this->numAddedDeviceSlots = 0;

    refreshValue();
}

Ship::~Ship() {
    if (this->equipment != 0) {
        if (this->equipment->size() != 0) {
            ArrayReleaseClasses<Item *>(*this->equipment);
        }
        delete this->equipment;
    }
    this->equipment = 0;

    if (this->cargo != 0) {
        if (this->cargo->size() != 0) {
            ArrayReleaseClasses<Item *>(*this->cargo);
        }
        delete this->cargo;
    }
    this->cargo = 0;

    delete[] this->slots;
    this->slots = 0;

    if (this->mods != 0) {
        delete this->mods;
    }
    this->mods = 0;
}

void Ship::addCargo(Array<Item *> *items) {
    setCargo(Item::combineItems(this->cargo, items));
}

void Ship::addCargo(Item *item) {
    Array<Item *> *a = new Array<Item *>();
    ArrayAdd<Item *>(item, *a);
    setCargo(a);
}

int Ship::removeCargo(int index, int amount) {
    Array<Item *> *c = this->cargo;
    if (c == 0) {
        return 0;
    }
    for (unsigned int i = 0; i < c->size(); i = i + 1) {
        if (c->data()[i]->getIndex() == index) {
            this->cargo->data()[i]->changeAmount(-amount);
            c = this->cargo;
            if (this->cargo->data()[i]->getAmount() < 1) {
                setCargo(Item::extractItems(c, true));
                return 1;
            }
            break;
        }
        c = this->cargo;
    }
    setCargo(c);
    return 0;
}

void Ship::removeCargo(int index) {
    removeCargo(index, 9999999);
}

void Ship::removeCargo(Item *item) {
    removeCargo(item->getIndex(), item->getAmount());
}

void Ship::removeAllCargo() {
    if (this->cargo != 0) {
        delete this->cargo;
    }
    this->cargo = 0;
}

void Ship::setCargo(Array<Item *> *cargo) {
    this->currentLoad = 0;
    this->cargo = cargo;
    if (cargo != 0) {
        for (unsigned int i = 0; i < cargo->size(); i = i + 1) {
            this->currentLoad += cargo->data()[i]->getAmount();
        }
    }
    refreshValue();
    int freeSpace = (this->baseLoad + this->cargoPlus) - this->currentLoad;
    if (Status::gStatus->field_dc < freeSpace) {
        Status::gStatus->field_dc = freeSpace;
    }
}

void Ship::replaceCargo(Array<Item *> *cargo) {
    this->currentLoad = 0;
    this->cargo = cargo;
    for (unsigned int i = 0; i < cargo->size(); i = i + 1) {
        this->currentLoad += cargo->data()[i]->getAmount();
    }
    refreshValue();
    int freeSpace = (this->baseLoad + this->cargoPlus) - this->currentLoad;
    if (Status::gStatus->field_dc < freeSpace) {
        Status::gStatus->field_dc = freeSpace;
    }
}

Array<Item *> *Ship::getCargo() {
    return this->cargo;
}

Item *Ship::getCargo(int index) {
    Array<Item *> *c = this->cargo;
    if (c == 0) {
        return 0;
    }
    for (unsigned int i = 0; i < c->size(); i = i + 1) {
        Item *it = c->data()[i];
        if (it != 0 && it->getIndex() == index) {
            return this->cargo->data()[i];
        }
    }
    return 0;
}

bool Ship::hasCargo(int index, int amount) {
    Array<Item *> *c = this->cargo;
    if (c == 0) {
        return false;
    }
    for (unsigned int i = 0; i < c->size(); i = i + 1) {
        Item *it = c->data()[i];
        if (it != 0 && it->getIndex() == index && it->getAmount() >= amount) {
            return true;
        }
    }
    return false;
}

bool Ship::hasCargoType(int type) {
    Array<Item *> *c = this->cargo;
    if (c == 0) {
        return false;
    }
    for (unsigned int i = 0; i < c->size(); i = i + 1) {
        Item *it = c->data()[i];
        if (it != 0 && it->getType() == type) {
            return true;
        }
    }
    return false;
}

bool Ship::hasVolatileGoods() {
    return hasCargo(0xd1, 1) || hasCargo(0xcc, 1);
}

int Ship::getCargoValue() {
    int total = 0;
    Array<Item *> *c = this->cargo;
    if (c != 0) {
        for (unsigned int i = 0; i < c->size(); i = i + 1) {
            Item *it = c->data()[i];
            if (it != 0) {
                total += it->getTotalPrice();
            }
        }
    }
    return total;
}

bool Ship::spaceAvailable(int n) {
    return n + this->currentLoad <= this->baseLoad + this->cargoPlus;
}

int Ship::getFreeSpace() {
    return (this->cargoPlus + this->baseLoad) - this->currentLoad;
}

void Ship::changeLoad(int delta) {
    this->currentLoad += delta;
    int freeSpace = (this->baseLoad - this->currentLoad) + this->cargoPlus;
    if (Status::gStatus->field_dc < freeSpace) {
        Status::gStatus->field_dc = freeSpace;
    }
}

void Ship::setEquipment(Item *item) {
    Array<Item *> *eq = this->equipment;
    for (unsigned int i = 0; i < eq->size(); i = i + 1) {
        if (eq->data()[i] == 0) {
            eq->data()[i] = item;
            break;
        }
    }
    refreshValue();
}

void Ship::setEquipment(Item *item, int slot) {
    unsigned int idx = (unsigned int) slot;
    for (int i = 0; i < item->getType(); i = i + 1) {
        idx += this->slots[i];
    }
    if (idx >= this->equipment->size()) {
        return;
    }
    Item *old = this->equipment->data()[idx];
    if (old != 0) {
        delete old;
    }
    this->equipment->data()[idx] = item;
    refreshValue();
}

void Ship::setEquipment(Array<Item *> *items) {
    for (unsigned int i = 0; i < items->size(); i = i + 1) {
        setEquipment(items->data()[i]);
    }
}

void Ship::replaceEquipment(Array<Item *> *equipment) {
    if (equipment != 0) {
        int *slots = this->slots;
        unsigned int total = slots[0] + slots[1] + slots[2] + slots[3];
        if (total < equipment->size()) {
            int extra = equipment->size() - total;
            this->numAddedDeviceSlots = extra;
            slots[3] += extra;
        }
    }
    this->equipment = equipment;
    refreshValue();
}

int Ship::addEquipment(Item *item) {
    int type = item->getType();
    int cap = this->slots[type];
    if (cap < 1) {
        return 0;
    }
    int base = 0;
    for (int k = 0; k < type; k = k + 1) {
        base += this->slots[k];
    }
    for (int i = base; i < base + cap; i = i + 1) {
        if (this->equipment->data()[i] == 0) {
            this->equipment->data()[i] = item;
            return 1;
        }
    }
    return 0;
}

void Ship::removeEquipment(Item *item) {
    Array<Item *> *eq = this->equipment;
    if (eq == 0) {
        return;
    }
    for (unsigned int i = 0; i < eq->size(); i = i + 1) {
        Item *it = eq->data()[i];
        if (it != 0 && it->equals(item)) {
            this->equipment->data()[i] = 0;
            return;
        }
    }
}

Item *Ship::getFirstEquipmentOfSort(int sort) {
    Array<Item *> *eq = this->equipment;
    if (eq != 0) {
        for (unsigned int i = 0; i < eq->size(); i = i + 1) {
            Item *it = eq->data()[i];
            if (it != 0 && it->getSort() == sort) {
                return this->equipment->data()[i];
            }
        }
        if (sort == 0x15 && (this->index == 0x31 || this->index == 0x2c)) {
            ShopEntry *shop = ((ShopEntry **) gShopRoot)[1];
            return shop->cloakItem;
        }
    }
    return 0;
}

int Ship::getEquipmentValue() {
    int total = 0;
    for (unsigned int i = 0; i < this->equipment->size(); i = i + 1) {
        Item *it = this->equipment->data()[i];
        if (it != 0) {
            total += it->getTotalPrice();
        }
    }
    return total;
}

int Ship::getUsedSlots(int type) {
    int n = 0;
    for (unsigned int i = 0; i < this->equipment->size(); i = i + 1) {
        Item *it = this->equipment->data()[i];
        if (it != 0 && it->getType() == type) {
            n = n + 1;
        }
    }
    return n;
}

int Ship::getFreeSlots(int type) {
    return this->slots[type] - getUsedSlots(type);
}

int Ship::getSlots(int i) {
    return this->slots[i];
}

int Ship::getSlotTypes() {
    int n = 0;
    for (int i = 0; i < 4; i = i + 1) {
        if (this->slots[i] > 0) {
            n = n + 1;
        }
    }
    return n;
}

unsigned int Ship::getSlotPos(Item *item) {
    if (item == 0) {
        return 0xffffffffu;
    }
    Array<Item *> *eq = this->equipment;
    unsigned int pos = 0xffffffffu;
    for (unsigned int i = 0; i < eq->size(); i = i + 1) {
        if (eq->data()[i] == item) {
            pos = i;
            break;
        }
    }
    for (int j = 0; j < item->getType(); j = j + 1) {
        pos -= this->slots[j];
    }
    return pos;
}

void Ship::freeSlot(Item *item) {
    for (unsigned int i = 0; i < this->equipment->size(); i = i + 1) {
        Item *it = this->equipment->data()[i];
        if (it != 0 && it->equals(item)) {
            this->equipment->data()[i] = 0;
            break;
        }
    }
    refreshValue();
}

void Ship::freeSlot(Item *item, int slot) {
    for (unsigned int i = 0; i < this->equipment->size(); i = i + 1) {
        Item *it = this->equipment->data()[i];
        if (it != 0 && (unsigned int) slot == i && it->equals(item)) {
            this->equipment->data()[slot] = 0;
            break;
        }
    }
    refreshValue();
}

void Ship::freeAllSlots() {
    for (unsigned int i = 0; i < this->equipment->size(); i = i + 1) {
        Item **data = this->equipment->data();
        if (data[i] != 0) {
            data[i] = 0;
        }
    }
    refreshValue();
}

int Ship::slotAvailable(int sort) {
    if (sort != 0 && sort != 0xc) {
        for (unsigned int i = 0; i < this->equipment->size(); i = i + 1) {
            Item *it = this->equipment->data()[i];
            if (it != 0 && it->getSort() == sort) {
                return 0;
            }
        }
    }
    return 1;
}

bool Ship::hasEquipment(int index, int amount) {
    Array<Item *> *e = this->equipment;
    if (e == 0) {
        return false;
    }
    for (unsigned int i = 0; i < e->size(); i = i + 1) {
        Item *it = e->data()[i];
        if (it != 0 && it->getIndex() == index && it->getAmount() >= amount) {
            return true;
        }
    }
    return false;
}

bool Ship::hasSecondaryWeapons() {
    if (this->slots[1] == 0 || this->equipment == 0) {
        return false;
    }
    for (unsigned int i = 0; i < this->equipment->size(); i = i + 1) {
        Item *it = this->equipment->data()[i];
        if (it != 0 && it->getType() == 1) {
            return true;
        }
    }
    return false;
}

Array<Item *> *Ship::getEquipment() {
    return this->equipment;
}

Array<Item *> *Ship::getEquipment(int type) {
    if (type >= 4 || this->slots[type] == 0) {
        return 0;
    }
    Array<Item *> *result = new Array<Item *>();
    ArraySetLength<Item *>(this->slots[type], *result);
    int base = 0;
    for (int i = 0; i < type; i = i + 1) {
        base += this->slots[i];
    }
    unsigned int j = 0;
    for (int i = base; i < this->slots[type] + base; i = i + 1) {
        if (j < result->size()) {
            result->data()[j] = this->equipment->data()[i];
            j = j + 1;
        }
    }
    return result;
}

Ship *Ship::clone() {
    Ship * s = new Ship(this->index, this->baseHP, this->baseLoad, this->price,
                        this->slots[0], this->slots[1], this->slots[2],
                        this->slots[3] - this->numAddedDeviceSlots,
                        this->handling * 1.6f);
    Array<int> *m = this->mods;
    if (m != 0) {
        for (unsigned int i = 0; i < m->size(); i = i + 1) {
            s->addMod(m->data()[i]);
            m = this->mods;
        }
    }
    return s;
}

bool Ship::equals(Ship *other) {
    return this->index == other->index;
}

void Ship::refreshValue() {
    this->repairType = -1;
    this->hasJumpDriveFlag = 0;
    this->hasCloakFlag = 0;
    this->hasEmergency = 0;
    this->fireRateFactor = 1.0f;
    this->damageFactor = 1.0f;
    this->maxShieldHP = 0;
    this->maxArmorHP = 0;
    this->shieldRegen = 0;
    this->cargoPlus = 0;
    this->boostTime = 0;
    this->agility = 0;
    this->maxPassengers = 0;
    this->firePower = 0;
    this->signatureRace = -1;
    this->radarType = 0;
    this->boostSpeed = 0;
    this->boostDelay = 0;
    this->boostTime = 0;
    if (Status::gStatus != 0 && Status::gStatus->getStanding() != 0) {
        ((Standing *) ((void *) (intptr_t) Status::gStatus->getStanding()))->setPlayerSignatureRace(-1);
    }
    this->value = this->price;

    unsigned int i = 0;
    unsigned int n = 0;
    Array<Item *> *eq;
    for (;;) {
        eq = this->equipment;
        n = eq->size();
        if (n <= i) {
            break;
        }
        Item *cur = eq->data()[i];
        if (cur != 0) {
            switch (cur->getSort()) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 8:
                case 0x19: {
                    float a = (float) cur->getAttribute(9);
                    float b = (float) cur->getAttribute(0xb);
                    this->firePower = (int) ((float) this->firePower + (a / b) * 1.0f);
                    break;
                }
                case 9:
                    this->maxShieldHP = cur->getAttribute(0x12);
                    this->shieldRegen = cur->getAttribute(0x13);
                    break;
                case 10:
                    this->maxArmorHP = cur->getAttribute(0x14);
                    break;
                case 0xc:
                    this->cargoPlus = cur->getAttribute(0x16) + this->cargoPlus;
                    break;
                case 0xe:
                    this->boostSpeed = cur->getAttribute(0x19);
                    this->boostTime = cur->getAttribute(0x1b);
                    this->boostDelay = cur->getAttribute(0x1a);
                    break;
                case 0xf: {
                    int idx = cur->getIndex();
                    this->repairType = idx != 0x4b;
                    break;
                }
                case 0x10:
                    this->agility = cur->getAttribute(0x1c);
                    break;
                case 0x11:
                    this->radarType = cur->getAttribute(0x1f);
                    break;
                case 0x12:
                    this->hasJumpDriveFlag = 1;
                    break;
                case 0x14:
                    this->firePower = cur->getAttribute(0x22) + this->firePower;
                    break;
                case 0x15:
                    this->hasCloakFlag = 1;
                    break;
                case 0x1b:
                    this->hasEmergency = 1;
                    break;
                case 0x1c: {
                    float r = (float) cur->getAttribute(0x27);
                    r = 1.0f - r / 100.0f;
                    if (r == 0.0f) {
                        r = 1.0f;
                    }
                    this->fireRateFactor = r;
                    float d = (float) cur->getAttribute(0x28);
                    d = d / 100.0f + 1.0f;
                    this->damageFactor = d;
                    if (d == 0.0f) {
                        this->damageFactor = 1.0f;
                    }
                    break;
                }
                case 0x1d: {
                    int idx = cur->getIndex();
                    this->signatureRace = idx - 0xbd;
                    if (Status::gStatus != 0 && Status::gStatus->getStanding() != 0) {
                        ((Standing *) ((void *) (intptr_t) Status::gStatus->getStanding()))->setPlayerSignatureRace(
                            this->signatureRace);
                    }
                    break;
                }
            }
            this->value = cur->getTotalPrice() + this->value;
        }
        i = i + 1;
    }

    this->firePower = 0;
    for (i = 0; i < n; i = i + 1) {
        Item *cur = eq->data()[i];
        unsigned int sort;
        if (cur != 0 && (sort = cur->getSort()) < 0x1a && (1 << (sort & 0xff) & 0x4000) != 0) {
            float dmg = this->damageFactor;
            float a = (float) cur->getAttribute(9);
            float b = (float) cur->getAttribute(0xb);
            this->firePower = (int) ((float) this->firePower + ((dmg * a) / (this->fireRateFactor * b)) * 1.0f);
        }
        eq = this->equipment;
        n = eq->size();
    }

    Array<int> *m = this->mods;
    int loadBonus = 0;
    if (m != 0) {
        for (unsigned int j = 0; j < m->size(); j = j + 1) {
            if (m->data()[j] == 1) {
                loadBonus += 0x1e;
            }
        }
    }
    int cargoPlus = this->cargoPlus;
    float cp = (float) cargoPlus;
    float bl = (float) (this->baseLoad + loadBonus);
    this->cargoPlus = loadBonus + (int) ((cp * bl) / 100.0f);
    this->compression = cargoPlus;
    this->value = getCargoValue() + this->value;
}

void Ship::adjustPrice() {
    if (Status::gStatus->getStation() != 0 && this->price > 0) {
        ShipDataObj *root = *(ShipDataObj **) gShipDataRoot;
        ShipDataEntry *entry = root->table[this->index];
        int cat = entry->category;
        SolarSystem *system = (SolarSystem *) (intptr_t) Status::gStatus->getSystem();
        int race = system->getRace();
        ShipDataEntry *entry2 = root->table[this->index];
        float base = (float) entry2->basePrice;
        float bonus = 0.1f;
        if (gRaceTable[cat] == race) {
            bonus = base * 0.3f;
        }
        float acc = bonus + base;
        float diff = 0.1f;
        int dv = *gDifficultyPtr;
        if (dv != 0) {
            diff = base * (float) dv * 0.7f;
        }
        this->price = (int) (acc + diff);
    }
}

void Ship::priceDecline() {
    ShipDataObj *root = *(ShipDataObj **) gShipDataRoot;
    ShipDataEntry *entry = root->table[this->index];
    this->price = (int) ((float) entry->basePrice / 1.25f);
}

Ship *Ship::makeShip(int price) {
    Ship * s = clone();
    if (price >= 0) {
        s->price = price;
    }
    return s;
}

void Ship::addMod(int mod) {
    Array<int> *m = this->mods;
    if (m == 0) {
        m = new Array<int>();
        this->mods = m;
    }
    unsigned int sz = m->size();
    int found = 0;
    for (unsigned int i = 0; !found && i < sz; i = i + 1) {
        found = m->data()[i] == mod;
    }
    if (found) {
        return;
    }
    ArrayAdd<int>(mod, *m);
    int dev = 0;
    if (mod == 2) {
        dev = this->numAddedDeviceSlots;
    }
    if (mod == 2 && dev == 0) {
        this->slots[3] += 1;
        this->numAddedDeviceSlots += 1;
        Item *it = new Item(nullptr, nullptr, nullptr);
        ArrayAdd<Item *>(it, *this->equipment);
        Item **slot = &this->equipment->data()[this->equipment->size() - 1];
        if (*slot != 0) {
            delete *slot;
            slot = &this->equipment->data()[this->equipment->size() - 1];
        }
        *slot = 0;
    }
    refreshValue();
}

void Ship::setMods(Array<int> *mods) {
    this->mods = mods;
    if (mods != 0) {
        for (unsigned int i = 0; i < mods->size(); i = i + 1) {
            int v = mods->data()[i];
            bool isDev = v == 2;
            if (isDev) {
                v = this->numAddedDeviceSlots;
            }
            if (isDev && v == 0) {
                this->slots[3] += 1;
                this->numAddedDeviceSlots += 1;
                Item *it = new Item(nullptr, nullptr, nullptr);
                ArrayAdd<Item *>(it, *this->equipment);
                Item **slot = &this->equipment->data()[this->equipment->size() - 1];
                if (*slot != 0) {
                    delete *slot;
                    slot = &this->equipment->data()[this->equipment->size() - 1];
                }
                *slot = 0;
            }
        }
    }
    refreshValue();
}

Array<int> *Ship::getMods() {
    return this->mods;
}

bool Ship::hasModInstalled(int mod) {
    Array<int> *m = this->mods;
    if (m != 0) {
        for (unsigned int i = 0; i < m->size(); i = i + 1) {
            if (m->data()[i] == mod) {
                return true;
            }
        }
    }
    return false;
}

int Ship::getModdedLoad() {
    int load = this->baseLoad;
    Array<int> *m = this->mods;
    if (m != 0) {
        for (unsigned int i = 0; i < m->size(); i = i + 1) {
            if (m->data()[i] == 1) {
                load += 0x1e;
            }
        }
    }
    return load;
}

int Ship::getIndex() { return this->index; }
int Ship::getRace() { return this->race; }
void Ship::setRace(int race) { this->race = race; }
int Ship::getSignatureRace() { return this->signatureRace; }
int Ship::getBaseHP() { return this->baseHP; }
int Ship::getMaxShieldHP() { return this->maxShieldHP; }
int Ship::getMaxArmorHP() { return this->maxArmorHP; }
int Ship::getShieldRegen() { return this->shieldRegen; }
int Ship::getBaseLoad() { return this->baseLoad; }
int Ship::getMaxLoad() { return this->cargoPlus + this->baseLoad; }
int Ship::getCurrentLoad() { return this->currentLoad; }
int Ship::getCargoPlus() { return this->cargoPlus; }
int Ship::getCompression() { return this->compression; }
int Ship::getValue() { return this->value; }
int Ship::getPrice() { return this->price; }
void Ship::setPrice(int price) { this->price = price; }
int Ship::getUnmoddedHandling() { return *(int *) &this->handling; }
int Ship::getFireRateFactor() { return *(int *) &this->fireRateFactor; }
int Ship::getDamageFactor() { return *(int *) &this->damageFactor; }
int Ship::getFirePower() { return this->firePower; }
int Ship::getAgility() { return this->agility; }
int Ship::getRadarType() { return this->radarType; }
int Ship::getRepairType() { return this->repairType; }
int Ship::getMaxPassengers() { return this->maxPassengers; }
int Ship::getBoostSpeed() { return this->boostSpeed; }
int Ship::getBoostDelay() { return this->boostDelay; }
int Ship::getBoostTime() { return this->boostTime; }
bool Ship::hasBooster() { return this->boostSpeed > 0; }
int Ship::getCurrentWeaponSlot() { return this->currentWeaponSlot; }
void Ship::setCurrentWeaponSlot(int slot) { this->currentWeaponSlot = slot; }
int Ship::getNumAddedDeviceSlots() { return this->numAddedDeviceSlots; }
unsigned char Ship::hasEmergencySystem() { return this->hasEmergency; }

int Ship::getMaxHP() {
    int bonus = 0;
    Array<int> *m = this->mods;
    if (m != 0) {
        for (unsigned int i = 0; i < m->size(); i = i + 1) {
            if (m->data()[i] == 0) {
                bonus += 0x28;
            }
        }
    }
    return this->baseHP + bonus;
}

int Ship::getCombinedHP() {
    int bonus = 0;
    Array<int> *m = this->mods;
    if (m != 0) {
        for (unsigned int i = 0; i < m->size(); i = i + 1) {
            if (m->data()[i] == 0) {
                bonus += 0x28;
            }
        }
    }
    return bonus + this->baseHP + this->maxShieldHP + this->maxArmorHP;
}

float Ship::getHandling() {
    float h = 1.3f;
    Array<int> *m = this->mods;
    if (m != 0) {
        for (unsigned int i = 0; i < m->size(); i = i + 1) {
            if (m->data()[i] == 3) {
                h += 0.1f;
            }
        }
    }
    return h + this->handling;
}

float Ship::getHandlingForShop() {
    float h = 1.3f;
    Array<int> *m = this->mods;
    if (m != 0) {
        for (unsigned int i = 0; i < m->size(); i = i + 1) {
            if (m->data()[i] == 3) {
                h += 0.1f;
            }
        }
    }
    return h + this->handling;
}

bool Ship::hasCloak() {
    return this->hasCloakFlag != 0 || this->index == 0x31 || this->index == 0x2c;
}

bool Ship::hasCloakIntegrated() {
    return this->index == 0x2c || this->index == 0x31;
}

unsigned int Ship::hasJumpDrive() {
    if (this->hasJumpDriveFlag != 0) {
        return 1;
    }
    unsigned int v = (unsigned int) this->index - 0x25u;
    if (v < 4) {
        return 0xbu >> (v & 0xf) & 1;
    }
    return 0;
}

unsigned int Ship::hasJumpDriveIntegrated() {
    unsigned int v = (unsigned int) this->index - 0x25u;
    if (v < 4) {
        return 0xbu >> (v & 0xf) & 1;
    }
    return 0;
}
