#include "game/world/Waypoint.h"
#include "game/ship/Player.h"

void Waypoint::setActive(bool active) {
    ((Player *) this->player)->setActive(active);
}

void Waypoint::reached() {
    this->state = 0x101;
}

Vector Waypoint::getPosition() {
    Vector result;
    result.x = (float) this->x;
    result.y = (float) this->y;
    result.z = (float) this->z;
    return result;
}

void Waypoint::reset() {
    this->state = 0;
    ((Player *) this->player)->setActive(false);
}

Waypoint::Waypoint(int x, int y, int z, Route *route)
    : KIPlayer(0, -1, new Player(2000, 0, 0, 0, 0),
               nullptr, (float) x, (float) y, (float) z, false) {
    this->route = route;
    this->player->setActive(false);

    this->x = x;
    this->y = y;
    this->z = z;
    this->posX = (float) x;
    this->posY = (float) y;
    this->posZ = (float) z;
    this->state = 0;
    this->field_0x72 = 1;
    this->hasCargo = 0;
}

Waypoint::~Waypoint() {
}
