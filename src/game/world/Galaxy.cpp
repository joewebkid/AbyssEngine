#include "game/world/Galaxy.h"
Galaxy *Galaxy::gGalaxy = nullptr;

#include "engine/file/FileRead.h"
#include "game/core/Globals.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/world/SolarSystem.h"
#include "game/world/Station.h"

static float g_galaxyDistanceScale = 0.0f;

static const int kStationCount = 0x87;

Galaxy::Galaxy() {
    this->visited = new uint8_t[kStationCount];
    for (int i = 0; i < kStationCount; ++i)
        this->visited[i] = 0;

    FileRead loader;
    this->systems = loader.loadSystemsBinary();
}

Galaxy::~Galaxy() {
    delete[] this->visited;
    this->visited = 0;

    ArrayReleaseClasses<SolarSystem *>(*this->systems);
    delete this->systems;
    this->systems = 0;
}

void Galaxy::reset() {
    for (int i = 0; i < kStationCount; ++i)
        this->visited[i] = 0;
}

int Galaxy::distancePercent(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    float sum = (float) (dy * dy + dx * dx);
    return (int) Globals::gGlobals->sqrt(sum);
}

int Galaxy::invDistancePercent(int x1, int y1, int x2, int y2) {
    return 100 - distancePercent(x1, y1, x2, y2);
}

void Galaxy::visitStation(int index) {
    this->visited[index] = 1;
}

void Galaxy::setVisited(bool *src, int count) {
    for (int i = 0; i < count; ++i)
        this->visited[i] = (uint8_t) src[i];
    for (int i = count; i < kStationCount; ++i)
        this->visited[i] = 0;
}

int Galaxy::getSystem(int index) {
    if (index < 0)
        return 0;
    return (int) (intptr_t)(*this->systems)[index];
}

float Galaxy::distance(SolarSystem *a, SolarSystem *b) {
    if (a->getIndex() == b->getIndex())
        return 0.0f;

    Vector pa;
    Vector pb;
    pa.x = (float) a->getX();
    pa.y = (float) a->getY();
    pa.z = (float) (a->getZ() / 10);
    pb.x = (float) b->getX();
    pb.y = (float) b->getY();
    pb.z = (float) (b->getZ() / 10);

    pa -= pb;
    float sq = pa.x * pa.x + pa.y * pa.y + pa.z * pa.z;
    return Globals::gGlobals->sqrt(sq) * g_galaxyDistanceScale;
}

void *Galaxy::getPlasmaProbabilities(Station *station) {
    int alien = Status::gStatus->inAlienOrbit() ? 1 : 0;
    Array<SolarSystem *> *systems = alien == 0 ? this->systems : 0;
    Array<Item *> *itemTable = Item::g_items;

    int *probs = new int[4];
    int *ids = new int[4];

    int slot = 0;
    for (int id = 0xc9; id != 0xcd; ++id) {
        ids[slot] = id;
        int prob;
        int next;
        if (alien == 0) {
            int sys = station->getSystem();
            int sysX = (*systems)[sys]->getX();
            sys = station->getSystem();
            int sysY = (*systems)[sys]->getY();
            int it = (*itemTable)[id]->getMinPriceSystem();
            int itX = (*systems)[it]->getX();
            it = (*itemTable)[id]->getMinPriceSystem();
            int itY = (*systems)[it]->getY();
            prob = invDistancePercent(sysX, sysY, itX, itY);
            next = slot + 1;
            if (prob < 0x32)
                prob = 0;
        } else {
            prob = 0;
            next = slot;
        }
        probs[slot] = prob;
        slot = next;
    }

    bool sorted = true;
    int i = 1;
    do {
        for (; i != 4; ++i) {
            int prev = i - 1;
            int a = probs[i];
            int b = probs[prev];
            if (b < a) {
                probs[prev] = a;
                int idPrev = ids[prev];
                int idCur = ids[i];
                probs[i] = b;
                ids[prev] = idCur;
                ids[i] = idPrev;
                sorted = false;
            }
        }
        bool again = !sorted;
        sorted = true;
        i = 1;
        if (!again)
            break;
    } while (true);

    int sub = 0;
    for (int k = 0; k != 4; ++k) {
        int v = probs[k];
        if (0 < v)
            probs[k] = v + sub;
        sub = sub - 2;
    }

    int *out = new int[8];
    for (int j = 0; j < 8; j += 2) {
        out[j] = ids[j / 2];
        out[j + 1] = probs[j / 2];
    }

    delete[] probs;
    delete[] ids;
    return out;
}

void *Galaxy::getAsteroidProbabilities(Station *station) {
    int alien = Status::gStatus->inAlienOrbit() ? 1 : 0;
    int supernova = Status::gStatus->inSupernovaOrbit() ? 1 : 0;
    Array<SolarSystem *> *systems = alien == 0 ? this->systems : 0;
    Array<Item *> *itemTable = Item::g_items;

    int *probs = new int[11];
    int *ids = new int[11];

    int slot = 0;
    for (int id = 0x9a; id != 0xa4; ++id) {
        ids[slot] = id;
        int prob;
        int next;
        if (alien == 0) {
            int sys = station->getSystem();
            int sysX = (*systems)[sys]->getX();
            sys = station->getSystem();
            int sysY = (*systems)[sys]->getY();
            int it = (*itemTable)[id]->getMinPriceSystem();
            int itX = (*systems)[it]->getX();
            it = (*itemTable)[id]->getMinPriceSystem();
            int itY = (*systems)[it]->getY();
            prob = invDistancePercent(sysX, sysY, itX, itY);
            next = slot + 1;
            if (prob < 0x32)
                prob = 0;
        } else {
            prob = 0;
            next = slot;
        }
        probs[slot] = prob;
        slot = next;
    }

    ids[10] = 0xa4;
    probs[10] = alien != 0 ? 100 : 0;

    bool sorted = true;
    int i = 1;
    do {
        for (; i != 0xb; ++i) {
            int prev = i - 1;
            int a = probs[i];
            int b = probs[prev];
            if (b < a) {
                probs[prev] = a;
                int idPrev = ids[prev];
                int idCur = ids[i];
                probs[i] = b;
                ids[prev] = idCur;
                ids[i] = idPrev;
                sorted = false;
            }
        }
        bool again = !sorted;
        sorted = true;
        i = 1;
        if (!again)
            break;
    } while (true);

    int sub = 0;
    for (int k = 0; k != 0xb; ++k) {
        int v = probs[k];
        if (0 < v)
            probs[k] = v + sub;
        sub = sub - 2;
    }

    int *out = new int[22];
    for (int j = 0; j < 22; j += 2) {
        out[j] = ids[j / 2];
        out[j + 1] = probs[j / 2];
        if (supernova != 0 && Status::gStatus->getCurrentCampaignMission() > 0x59)
            out[j] = 0xd9;
    }

    delete[] probs;
    delete[] ids;
    return out;
}

int Galaxy::getStation(int index) {
    if (index < 0)
        return (int) (intptr_t) Status::gStatus->playerStation;

    FileRead loader;
    return loader.loadStation(index);
}

Array<SolarSystem *> *Galaxy::getSystems() {
    return this->systems;
}

uint8_t *Galaxy::getVisited() {
    return this->visited;
}
