#include "game/world/Route.h"
#include "game/world/Waypoint.h"
#include "game/ship/KIPlayer.h"
#include "game/core/RadioMessage.h"
#include "game/weapons/AbstractGun.h"
#include "engine/core/Array.h"

typedef void (*GetPosFn)(void *out);

static inline void getPos(void *kip, void *out) {
    (*(*(GetPosFn **) kip + 0xa))(out);
}

bool Route::waypointDefined() {
    return this->waypoints != nullptr;
}

int Route::length() {
    return (int) this->waypoints->size();
}

void Route::reset() {
    for (uint32_t i = 0; i < this->waypoints->size(); i++)
        (*this->waypoints)[i]->reset();
    this->currentIndex = 0;
}

Waypoint *Route::getWaypoint() {
    return getWaypoint(this->currentIndex);
}

Route *Route::getExactClone() {
    Route *result = clone();
    for (uint32_t i = 0; i < result->waypoints->size(); i++) {
        if ((*this->waypoints)[i]->state != 0)
            (*result->waypoints)[i]->reached();
    }
    result->currentIndex = this->currentIndex;
    return result;
}

KIPlayer *Route::getDockingTarget() {
    Array<KIPlayer *> *targets = this->dockingTargets;
    int index = this->currentIndex;
    if (targets != nullptr && (int) targets->size() > index)
        return (*targets)[index];
    return nullptr;
}

KIPlayer *Route::getDockingTarget(int index) {
    Array<KIPlayer *> *targets = this->dockingTargets;
    if (targets != nullptr && index < (int) targets->size())
        return (*targets)[index];
    return nullptr;
}

int Route::getDockingTime(int index) {
    if (this->dockingTargets != nullptr) {
        Array<int> *times = this->dockingTimes;
        if (index < (int) times->size())
            return (*times)[index];
    }
    return 0;
}

Waypoint *Route::getWaypoint(int index) {
    char pos[12];
    Waypoint *wp = nullptr;
    if ((int) this->waypoints->size() > index &&
        (wp = (*this->waypoints)[index]) != nullptr) {
        void *k = (void *) (*this->dockingTargets)[index];
        if (k != nullptr) {
            getPos(k, pos);
            wp->x = (int) *(float *) (pos + 0);
            getPos((void *) (*this->dockingTargets)[index], pos);
            wp->y = (int) *(float *) (pos + 4);
            getPos((void *) (*this->dockingTargets)[index], pos);
            wp->z = (int) *(float *) (pos + 8);
        }
    }
    return wp;
}

void Route::setNewCoords(Vector v) {
    Waypoint *wp = (*this->waypoints)[0];
    wp->x = (int) v.x;
    wp->y = (int) v.y;
    wp->z = (int) v.z;
}

void Route::update(const Vector &v) {
    update(v.x, v.y, v.z);
}

Waypoint *Route::getLastWaypoint() {
    return getWaypoint((int) this->waypoints->size() - 1);
}

void Route::translate(const Vector &v) {
    Array<Waypoint *> *wps = this->waypoints;
    float vx = v.x, vy = v.y, vz = v.z;
    for (uint32_t i = 0; i != wps->size(); i++) {
        Waypoint *wp = (*wps)[i];
        wp->x = (int) ((float) wp->x + vx);
        wp->y = (int) ((float) wp->y + vy);
        wp->z = (int) ((float) wp->z + vz);
    }
}

void Route::reachWaypoint(int index) {
    Array<Waypoint *> *wps = this->waypoints;
    uint32_t len = wps->size();
    if (this->currentIndex < (int) (len - 1)) {
        this->currentIndex = index + 1;
    } else if (this->loop != 0) {
        this->currentIndex = 0;
        for (uint32_t i = 0; i < len; i++) {
            (*this->waypoints)[i]->reset();
            len = this->waypoints->size();
        }
        (*this->waypoints)[0]->setActive(true);
    }
    (*this->waypoints)[index]->setActive(false);
    (*this->waypoints)[index]->reached();
}

Route *Route::clone() {
    Array<Waypoint *> *wps = this->waypoints;
    int n = (int) wps->size();
    int *coords = new int[n * 3];
    int *p = coords;
    for (int i = 0; i != n; i++) {
        Waypoint *wp = (*wps)[i];
        p[0] = wp->x;
        p[1] = wp->y;
        p[2] = wp->z;
        p += 3;
    }
    Array<KIPlayer *> *tgt = this->dockingTargets;
    if (tgt != nullptr) {
        bool any = false;
        for (uint32_t k = 0; k < tgt->size(); k++)
            if ((*tgt)[k] != nullptr)
                any = true;
        if (any) {
            Array<int> *times = this->dockingTimes;
            int *timesCopy = new int[times->size()];
            for (uint32_t k = 0; k < times->size(); k++)
                timesCopy[k] = (*times)[k];
            Array<KIPlayer *> *targetsArr = new Array<KIPlayer *>();
            ArraySetLength(this->dockingTargets->size(), *targetsArr);
            for (uint32_t k = 0; k < this->dockingTargets->size(); k++)
                (*targetsArr)[k] = (*this->dockingTargets)[k];
            Route *r = new Route(coords, targetsArr, timesCopy, (int) this->waypoints->size() * 3);
            r->loop = this->loop;
            delete[] timesCopy;
            return r;
        }
    }
    Route *r = new Route(coords, (int) wps->size() * 3);
    r->loop = this->loop;
    return r;
}

float Route::update(float x, float y, float z) {
    int idx = this->currentIndex;
    Array<Waypoint *> *wps = this->waypoints;
    if (idx >= (int) wps->size())
        return x;
    if ((*this->dockingTargets)[idx] != nullptr)
        return x;
    Waypoint *wp = (*wps)[idx];
    float dx = x - (float) wp->x;
    if (!(dx < 2000.0f) || !(dx > -2000.0f))
        return x;
    float dy = y - (float) wp->y;
    if (!(dy < 2000.0f) || !(dy > -2000.0f))
        return x;
    float dz = z - (float) wp->z;
    if (!(dz < 2000.0f) || !(dz > -2000.0f))
        return x;

    wp->setActive(false);
    (*this->waypoints)[this->currentIndex]->reached();
    int cur = this->currentIndex;
    Array<Waypoint *> *w = this->waypoints;
    int next = cur + 1;
    this->currentIndex = next;
    uint32_t len = w->size();
    if (this->loop != 0 && (int) (len - 1) <= cur) {
        this->currentIndex = 0;
        for (uint32_t i = 0; i < len; i++) {
            (*w)[i]->reset();
            w = this->waypoints;
            len = w->size();
        }
        next = this->currentIndex;
    }
    if (next < (int) len) {
        (*w)[next]->setActive(true);
        return 0.0f;
    }
    return x;
}

int Route::getDockingTime() {
    return getDockingTime(this->currentIndex);
}

Route::Route(int *coords, int count) {
    this->loop = 0;
    this->currentIndex = 0;
    this->waypoints = new Array<Waypoint *>();
    this->dockingTargets = new Array<KIPlayer *>();
    this->dockingTimes = new Array<int>();
    uint32_t n = count / 3;
    ArraySetLength(n, *(this->dockingTargets));
    ArraySetLength(n, *(this->dockingTimes));
    for (int i = 0; i < count; i += 3)
        ArrayAdd(new Waypoint(coords[i], coords[i + 1], coords[i + 2], this), *(this->waypoints));
}

Route::Route(int *coords, Array<KIPlayer *> *targets, int *times, int count) {
    this->loop = 0;
    this->currentIndex = 0;
    this->waypoints = new Array<Waypoint *>();
    this->dockingTargets = targets;
    this->dockingTimes = new Array<int>();
    for (int i = 0; i < count; i += 3)
        ArrayAdd(times[i / 3], *(this->dockingTimes));
    for (int i = 0; i < count; i += 3)
        ArrayAdd(new Waypoint(coords[i], coords[i + 1], coords[i + 2], this), *(this->waypoints));
}

Route::~Route() {
    if (this->waypoints != nullptr) {
        ArrayReleaseClasses(*this->waypoints); ArrayRemoveAll(*(this->waypoints));
        delete this->waypoints;
    }
    this->waypoints = nullptr;
    delete this->dockingTargets;
    this->dockingTargets = nullptr;
    delete this->dockingTimes;
    this->dockingTimes = nullptr;
}

int Route::getCurrent() {
    return this->currentIndex;
}

void Route::setLoop(bool loop) {
    this->loop = loop;
}
