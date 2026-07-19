#include "game/world/SpacePoint.h"

SpacePoint::SpacePoint(int type, const Vector &position, const Vector &direction, int param)
    : position(position), direction(direction), type(type), free(1), param(param) {
}

void SpacePoint::giveFree() {
    this->free = 1;
}

void SpacePoint::take() {
    this->free = 0;
}

SpacePoint::~SpacePoint() {
}

uint8_t SpacePoint::isFree() {
    return this->free;
}

int SpacePoint::getIndex() {
    return this->param;
}
