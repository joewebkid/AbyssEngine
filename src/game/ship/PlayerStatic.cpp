#include "game/ship/PlayerStatic.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "engine/render/AEGeometry.h"

PlayerStatic::PlayerStatic(int playerId, AEGeometry *geometry, float x, float y, float z)
    : KIPlayer(playerId, -1, new Player(2000, 0, 0, 0, 0), geometry, x, y, z, true) {
    positionX = (int) x;
    positionY = (int) y;
    positionZ = (int) z;
}

PlayerStatic::~PlayerStatic() {
}

void PlayerStatic::render() {
    geometry->render();
}

Vector PlayerStatic::getPosition() {
    if (geometry != nullptr) {
        return geometry->getPosition();
    }

    Vector result;
    result.x = (float) positionX;
    result.y = (float) positionY;
    result.z = (float) positionZ;
    return result;
}

void PlayerStatic::update(int dt) {
}

void PlayerStatic::translate(const Vector &v) {
}

int PlayerStatic::collide(float x, float y, float z) {
    return 0;
}

int PlayerStatic::outerCollide(float x, float y, float z) {
    return 0;
}
